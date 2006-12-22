#include "Debug.h"
#include "Local.h"
#include "MemMan.h"
#include "VObject_Blitters.h"
#include "VSurface.h"
#include "Video.h"
#include "WCheck.h"
#include <string.h>


///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Video Surface SGP Module
//
// Second Revision: Dec 10, 1996, Andrew Emmons
//
///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
//
// LOCAL functions
//
///////////////////////////////////////////////////////////////////////////////////////////////////

static BOOLEAN FillSurfaceRect(HVSURFACE hDestVSurface, SDL_Rect* Rect, UINT16 Color);
BOOLEAN BltVSurfaceUsingDD( HVSURFACE hDestVSurface, HVSURFACE hSrcVSurface, UINT32 fBltFlags, INT32 iDestX, INT32 iDestY, RECT *SrcRect );
static void DeletePrimaryVideoSurfaces(void);

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// LOCAL global variables
//
///////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct VSURFACE_NODE
{
	HVSURFACE hVSurface;
	UINT32 uiIndex;
  struct VSURFACE_NODE *next, *prev;

	#ifdef SGP_VIDEO_DEBUGGING
		UINT8									*pName;
		UINT8									*pCode;
	#endif

}VSURFACE_NODE;

static VSURFACE_NODE* gpVSurfaceHead = NULL;
static VSURFACE_NODE* gpVSurfaceTail = NULL;
static UINT32 guiVSurfaceIndex = 0;
UINT32				guiVSurfaceSize = 0;

#ifdef _DEBUG
enum
{
	DEBUGSTR_NONE,
	DEBUGSTR_SETVIDEOSURFACETRANSPARENCY,
	DEBUGSTR_BLTVIDEOSURFACE_DST,
	DEBUGSTR_BLTVIDEOSURFACE_SRC,
	DEBUGSTR_COLORFILLVIDEOSURFACEAREA,
	DEBUGSTR_SHADOWVIDEOSURFACERECT,
	DEBUGSTR_BLTSTRETCHVIDEOSURFACE_DST,
	DEBUGSTR_BLTSTRETCHVIDEOSURFACE_SRC,
	DEBUGSTR_DELETEVIDEOSURFACEFROMINDEX
};

static UINT8 gubVSDebugCode = 0;

static void CheckValidVSurfaceIndex(UINT32 uiIndex);
#endif

INT32				giMemUsedInSurfaces;


static HVSURFACE ghBackBuffer = NULL;
HVSURFACE   ghFrameBuffer = NULL;
static HVSURFACE ghMouseBuffer = NULL;


static BOOLEAN SetPrimaryVideoSurfaces(void);


BOOLEAN InitializeVideoSurfaceManager( )
{
	//Shouldn't be calling this if the video surface manager already exists.
	//Call shutdown first...
	Assert( !gpVSurfaceHead );
	Assert( !gpVSurfaceTail );
	RegisterDebugTopic(TOPIC_VIDEOSURFACE, "Video Surface Manager");
	gpVSurfaceHead = gpVSurfaceTail = NULL;

	giMemUsedInSurfaces = 0;

	// Create primary and backbuffer from globals
	if ( !SetPrimaryVideoSurfaces( ) )
	{
    DbgMessage(TOPIC_VIDEOSURFACE, DBG_LEVEL_1, String( "Could not create primary surfaces" ) );
		return FALSE;
	}

	return TRUE ;
}

BOOLEAN ShutdownVideoSurfaceManager( )
{
	VSURFACE_NODE *curr;

  DbgMessage(TOPIC_VIDEOSURFACE, DBG_LEVEL_0, "Shutting down the Video Surface manager");

	// Delete primary viedeo surfaces
	DeletePrimaryVideoSurfaces( );

	while( gpVSurfaceHead )
	{
		curr = gpVSurfaceHead;
		gpVSurfaceHead = gpVSurfaceHead->next;
		DeleteVideoSurface( curr->hVSurface );
		#ifdef SGP_VIDEO_DEBUGGING
			if( curr->pName )
				MemFree( curr->pName );
			if( curr->pCode )
				MemFree( curr->pCode );
		#endif
		MemFree( curr );
	}
	gpVSurfaceHead = NULL;
	gpVSurfaceTail = NULL;
	guiVSurfaceIndex = 0;
	guiVSurfaceSize = 0;
	UnRegisterDebugTopic(TOPIC_VIDEOSURFACE, "Video Objects");
	return TRUE;
}


static BOOLEAN AddStandardVideoSurface(HVSURFACE hVSurface, UINT32* puiIndex)
{
	Assert(puiIndex);

	if( !hVSurface )
	{
		// Video Object will set error condition.
		return FALSE ;
	}

	// Set into video object list
	if( gpVSurfaceHead )
	{ //Add node after tail
		gpVSurfaceTail->next = (VSURFACE_NODE*)MemAlloc( sizeof( VSURFACE_NODE ) );
		Assert( gpVSurfaceTail->next ); //out of memory?
		gpVSurfaceTail->next->prev = gpVSurfaceTail;
		gpVSurfaceTail->next->next = NULL;
		gpVSurfaceTail = gpVSurfaceTail->next;
	}
	else
	{ //new list
		gpVSurfaceHead = (VSURFACE_NODE*)MemAlloc( sizeof( VSURFACE_NODE ) );
		Assert( gpVSurfaceHead ); //out of memory?
		gpVSurfaceHead->prev = gpVSurfaceHead->next = NULL;
		gpVSurfaceTail = gpVSurfaceHead;
	}
	#ifdef SGP_VIDEO_DEBUGGING
		gpVSurfaceTail->pName = NULL;
		gpVSurfaceTail->pCode = NULL;
	#endif
	//Set the hVSurface into the node.
	gpVSurfaceTail->hVSurface = hVSurface;
	gpVSurfaceTail->uiIndex = guiVSurfaceIndex+=2;
	*puiIndex = gpVSurfaceTail->uiIndex;
	Assert( guiVSurfaceIndex < 0xfffffff0 ); //unlikely that we will ever use 2 billion VSurfaces!
	//We would have to create about 70 VSurfaces per second for 1 year straight to achieve this...
	guiVSurfaceSize++;

	return TRUE ;
}


static HVSURFACE CreateVideoSurface(VSURFACE_DESC* VSurfaceDesc);


BOOLEAN AddVideoSurface(VSURFACE_DESC* VSurfaceDesc, UINT32* Index)
{
	HVSURFACE hVSurface = CreateVideoSurface(VSurfaceDesc);
	return AddStandardVideoSurface(hVSurface, Index);
}


BOOLEAN AddVideoSurfaceFromFile(const char* Filename, UINT32* Index)
{
	HVSURFACE hVSurface = CreateVideoSurfaceFromFile(Filename);
	return AddStandardVideoSurface(hVSurface, Index);
}


static BYTE* LockVideoSurfaceBuffer(HVSURFACE hVSurface, UINT32* pPitch);
static void UnLockVideoSurfaceBuffer(HVSURFACE hVSurface);


BYTE *LockVideoSurface( UINT32 uiVSurface, UINT32 *puiPitch )
{
	VSURFACE_NODE *curr;

  //
  // Check if given backbuffer
  //
#ifdef JA2
  if ( uiVSurface == BACKBUFFER )
  {
    return LockBackBuffer( puiPitch );
  }
#endif

  if ( uiVSurface == FRAME_BUFFER )
  {
    return LockFrameBuffer( puiPitch );
  }

  if ( uiVSurface == MOUSE_BUFFER )
  {
    return LockMouseBuffer( puiPitch );
  }

  //
  // Otherwise, use list
  //

	curr = gpVSurfaceHead;
	while( curr )
	{
		if( curr->uiIndex == uiVSurface )
		{
			break;
		}
		curr = curr->next;
	}
	if( !curr )
	{
		return FALSE;
	}

  //
	// Lock buffer
  //

	return LockVideoSurfaceBuffer( curr->hVSurface, puiPitch );

}

void UnLockVideoSurface( UINT32 uiVSurface )
{
	VSURFACE_NODE *curr;

  //
  // Check if given backbuffer
  //
#ifdef JA2
  if ( uiVSurface == BACKBUFFER )
  {
    UnlockBackBuffer();
    return;
  }
#endif

  if ( uiVSurface == FRAME_BUFFER )
  {
    UnlockFrameBuffer();
    return;
  }

  if ( uiVSurface == MOUSE_BUFFER )
  {
    UnlockMouseBuffer();
    return;
  }

	curr = gpVSurfaceHead;
	while( curr )
	{
		if( curr->uiIndex == uiVSurface )
		{
			break;
		}
		curr = curr->next;
	}
	if( !curr )
	{
    return;
  }

  //
	// unlock buffer
  //

	UnLockVideoSurfaceBuffer( curr->hVSurface );
}


static BOOLEAN SetVideoSurfaceTransparencyColor(HVSURFACE hVSurface, COLORVAL TransColor);


BOOLEAN SetVideoSurfaceTransparency( UINT32 uiIndex, COLORVAL TransColor )
{
	HVSURFACE hVSurface;

  //
	// Get Video Surface
  //

	#ifdef _DEBUG
		gubVSDebugCode = DEBUGSTR_SETVIDEOSURFACETRANSPARENCY;
	#endif
	CHECKF( GetVideoSurface( &hVSurface, uiIndex ) );

  //
	// Set transparency
  //

	SetVideoSurfaceTransparencyColor( hVSurface, TransColor );

	return( TRUE );
}


BOOLEAN GetVideoSurface( HVSURFACE *hVSurface, UINT32 uiIndex )
{
	VSURFACE_NODE *curr;

	#ifdef _DEBUG
		CheckValidVSurfaceIndex( uiIndex );
	#endif

  if ( uiIndex == BACKBUFFER )
  {
    *hVSurface = ghBackBuffer;
		return TRUE;
  }

  if ( uiIndex == FRAME_BUFFER )
  {
    *hVSurface = ghFrameBuffer;
		return TRUE;
  }

  if ( uiIndex == MOUSE_BUFFER )
  {
    *hVSurface = ghMouseBuffer;
		return TRUE;
  }

	curr = gpVSurfaceHead;
	while( curr )
	{
		if( curr->uiIndex == uiIndex )
		{
			*hVSurface = curr->hVSurface;
			return TRUE;
		}
		curr = curr->next;
	}
  return FALSE;
}


static HVSURFACE CreateVideoSurfaceFromDDSurface(SDL_Surface* surface);


static BOOLEAN SetPrimaryVideoSurfaces(void)
{
	SDL_Surface* pSurface;

	// Delete surfaces if they exist
	DeletePrimaryVideoSurfaces( );

#ifdef JA2
  //
	// Get Backbuffer surface
  //

	pSurface = GetBackBufferObject( );
	CHECKF( pSurface != NULL );

	ghBackBuffer = CreateVideoSurfaceFromDDSurface( pSurface );
	CHECKF( ghBackBuffer != NULL );

  //
	// Get mouse buffer surface
  //
	pSurface = GetMouseBufferObject( );
	CHECKF( pSurface != NULL );

	ghMouseBuffer = CreateVideoSurfaceFromDDSurface( pSurface );
	CHECKF( ghMouseBuffer != NULL );

#endif

  //
	// Get frame buffer surface
  //

	pSurface = GetFrameBufferObject( );
	CHECKF( pSurface != NULL );

	ghFrameBuffer = CreateVideoSurfaceFromDDSurface( pSurface );
	CHECKF( ghFrameBuffer != NULL );

	return TRUE;
}


static void DeletePrimaryVideoSurfaces(void)
{
  //
	// If globals are not null, delete them
  //

	if ( ghBackBuffer != NULL )
	{
		DeleteVideoSurface( ghBackBuffer );
		ghBackBuffer = NULL;
	}

  if ( ghFrameBuffer != NULL )
	{
		DeleteVideoSurface( ghFrameBuffer );
		ghFrameBuffer = NULL;
	}

	if ( ghMouseBuffer != NULL )
	{
		DeleteVideoSurface( ghMouseBuffer );
		ghMouseBuffer = NULL;
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Given an index to the dest and src vobject contained in our private VSurface list
// Based on flags, blit accordingly
// There are two types, a BltFast and a Blt. BltFast is 10% faster, uses no
// clipping lists
//
///////////////////////////////////////////////////////////////////////////////////////////////////

BOOLEAN BltVideoSurface(UINT32 uiDestVSurface, UINT32 uiSrcVSurface, UINT16 usRegionIndex, INT32 iDestX, INT32 iDestY, UINT32 fBltFlags, const SGPRect* SrcRect)
{

	HVSURFACE	hDestVSurface;
	HVSURFACE	hSrcVSurface;

	#ifdef _DEBUG
		gubVSDebugCode = DEBUGSTR_BLTVIDEOSURFACE_DST;
	#endif
	if( !GetVideoSurface( &hDestVSurface, uiDestVSurface ) )
	{
		return FALSE;
	}
	#ifdef _DEBUG
		gubVSDebugCode = DEBUGSTR_BLTVIDEOSURFACE_SRC;
	#endif
	if( !GetVideoSurface( &hSrcVSurface, uiSrcVSurface ) )
	{
		return FALSE;
	}
	if (!BltVideoSurfaceToVideoSurface(hDestVSurface, hSrcVSurface, usRegionIndex, iDestX, iDestY, fBltFlags, SrcRect))
	{ // VO Blitter will set debug messages for error conditions
		return FALSE ;
	}
	return TRUE ;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Fills an rectangular area with a specified color value.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

BOOLEAN ColorFillVideoSurfaceArea(UINT32 uiDestVSurface, INT32 iDestX1, INT32 iDestY1, INT32 iDestX2, INT32 iDestY2, UINT16 Color16BPP)
{
	HVSURFACE	hDestVSurface;
	SGPRect Clip;

	#ifdef _DEBUG
		gubVSDebugCode = DEBUGSTR_COLORFILLVIDEOSURFACEAREA;
	#endif
	if( !GetVideoSurface( &hDestVSurface, uiDestVSurface ) )
	{
		return FALSE;
	}

  //
	// Clip fill region coords
  //

	GetClippingRect(&Clip);

	if(iDestX1 < Clip.iLeft)
		iDestX1 = Clip.iLeft;

	if(iDestX1 > Clip.iRight)
		return(FALSE);

	if(iDestX2 > Clip.iRight)
		iDestX2 = Clip.iRight;

	if(iDestX2 < Clip.iLeft)
		return(FALSE);

	if(iDestY1 < Clip.iTop)
		iDestY1 = Clip.iTop;

	if(iDestY1 > Clip.iBottom)
		return(FALSE);

	if(iDestY2 > Clip.iBottom)
		iDestY2 = Clip.iBottom;

	if(iDestY2 < Clip.iTop)
		return(FALSE);

	if((iDestX2 <= iDestX1) || (iDestY2 <= iDestY1))
		return(FALSE);

	SDL_Rect Rect;
	Rect.x = iDestX1;
	Rect.y = iDestY1;
	Rect.w = iDestX2 - iDestX1;
	Rect.h = iDestY2 - iDestY1;

	return FillSurfaceRect(hDestVSurface, &Rect, Color16BPP);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Video Surface Manipulation Functions
//
///////////////////////////////////////////////////////////////////////////////////////////////////


static HVSURFACE CreateVideoSurface(VSURFACE_DESC* VSurfaceDesc)
{
	HVSURFACE						hVSurface;
	HIMAGE							hImage;
	SGPRect							tempRect;
	UINT16							usHeight;
	UINT16							usWidth;
	UINT8								ubBitDepth;
	UINT32							uiRBitMask;
	UINT32							uiGBitMask;
	UINT32							uiBBitMask;
	SDL_Surface* surface;
	Uint32 surface_flags;

	usHeight   = VSurfaceDesc->usHeight;
	usWidth    = VSurfaceDesc->usWidth;
	ubBitDepth = VSurfaceDesc->ubBitDepth;

	Assert(usHeight > 0);
	Assert(usWidth  > 0);

	switch (ubBitDepth)
	{
		case 8:
			uiRBitMask = 0;
			uiGBitMask = 0;
			uiBBitMask = 0;
			break;

		case 16:
		{
			const SDL_Surface* screen = SDL_GetVideoSurface();
			uiRBitMask = screen->format->Rmask;
			uiGBitMask = screen->format->Gmask;
			uiBBitMask = screen->format->Bmask;
			break;
		}

		default:
			// If Here, an invalid format was given
			DbgMessage(TOPIC_VIDEOSURFACE, DBG_LEVEL_2, "Invalid BPP value, can only be 8 or 16.");
			return FALSE;
	}

	surface = SDL_CreateRGBSurface(
		SDL_SWSURFACE,
		usWidth, usHeight, ubBitDepth,
		uiRBitMask, uiGBitMask, uiBBitMask, 0
	);

	// Allocate memory for Video Surface data and initialize
	hVSurface = MemAlloc(sizeof(SGPVSurface));
	memset(hVSurface, 0, sizeof(SGPVSurface));
	CHECKF(hVSurface != NULL);

	hVSurface->surface       = surface;
	hVSurface->usHeight      = usHeight;
	hVSurface->usWidth       = usWidth;
	hVSurface->ubBitDepth    = ubBitDepth;
	hVSurface->pPalette      = NULL;
	hVSurface->p16BPPPalette = NULL;

	giMemUsedInSurfaces += ( hVSurface->usHeight * hVSurface->usWidth * ( hVSurface->ubBitDepth / 8 ) );

  DbgMessage( TOPIC_VIDEOSURFACE, DBG_LEVEL_3, String("Success in Creating Video Surface" ) );

	return( hVSurface );
}


static BOOLEAN SetVideoSurfaceDataFromHImage(HVSURFACE hVSurface, HIMAGE hImage, UINT16 usX, UINT16 usY, SGPRect* pSrcRect);


HVSURFACE CreateVideoSurfaceFromFile(const char* Filename)
{
	HIMAGE hImage = CreateImage(Filename, IMAGE_ALLIMAGEDATA);

	if (hImage == NULL)
	{
		DbgMessage(TOPIC_VIDEOSURFACE, DBG_LEVEL_2, "Invalid Image Filename given");
		return NULL;
	}

	VSURFACE_DESC VSDesc;
	VSDesc.usWidth      = hImage->usWidth;
	VSDesc.usHeight     = hImage->usHeight;
	VSDesc.ubBitDepth   = hImage->ubBitDepth;
	HVSURFACE hVSurface = CreateVideoSurface(&VSDesc);
	if (hVSurface != NULL)
	{
		SGPRect tempRect;
		tempRect.iLeft   = 0;
		tempRect.iTop    = 0;
		tempRect.iRight  = hImage->usWidth  - 1;
		tempRect.iBottom = hImage->usHeight - 1;
		SetVideoSurfaceDataFromHImage(hVSurface, hImage, 0, 0, &tempRect);

		if (hImage->ubBitDepth == 8)
		{
			SetVideoSurfacePalette(hVSurface, hImage->pPalette);
		}
	}

	DestroyImage(hImage);
	return hVSurface;
}


// Lock must be followed by release
// Pitch MUST be used for all width calculations ( Pitch is in bytes )
// The time between Locking and unlocking must be minimal
static BYTE* LockVideoSurfaceBuffer(HVSURFACE hVSurface, UINT32* pPitch)
{
	Assert( hVSurface != NULL );
	Assert( pPitch != NULL );

	SDL_LockSurface(hVSurface->surface); // XXX necessary?
	*pPitch = hVSurface->surface->pitch;
	return hVSurface->surface->pixels;
}


static void UnLockVideoSurfaceBuffer(HVSURFACE hVSurface)
{
	Assert( hVSurface != NULL );

	SDL_UnlockSurface(hVSurface->surface); // XXX necessary?
}


// Given an HIMAGE object, blit imagery into existing Video Surface. Can be from 8->16 BPP
static BOOLEAN SetVideoSurfaceDataFromHImage(HVSURFACE hVSurface, HIMAGE hImage, UINT16 usX, UINT16 usY, SGPRect* pSrcRect)
{
	BYTE		*pDest;
	UINT32	fBufferBPP = 0;
	UINT32  uiPitch;
	UINT16  usEffectiveWidth;
	SGPRect	aRect;

	// Assertions
	Assert( hVSurface != NULL );
	Assert( hImage != NULL );

	// Get Size of hImage and determine if it can fit
	CHECKF( hImage->usWidth  >= hVSurface->usWidth );
	CHECKF( hImage->usHeight >= hVSurface->usHeight );

	// Check BPP and see if they are the same
	if ( hImage->ubBitDepth != hVSurface->ubBitDepth )
	{
		// They are not the same, but we can go from 8->16 without much cost
		if ( hImage->ubBitDepth == 8 && hVSurface->ubBitDepth == 16 )
		{
			fBufferBPP = BUFFER_16BPP;
		}
	}
	else
	{
		// Set buffer BPP
		switch ( hImage->ubBitDepth )
		{
			case 8:

				fBufferBPP = BUFFER_8BPP;
				break;

			case 16:

				fBufferBPP = BUFFER_16BPP;
				break;
		}

	}

	Assert( fBufferBPP != 0 );

	// Get surface buffer data
	pDest = LockVideoSurfaceBuffer( hVSurface, &uiPitch );

	// Effective width ( in PIXELS ) is Pitch ( in bytes ) converted to pitch ( IN PIXELS )
	usEffectiveWidth = (UINT16)( uiPitch / ( hVSurface->ubBitDepth / 8 ) );

	CHECKF( pDest != NULL );

	// Blit Surface
	// If rect is NULL, use entrie image size
	if ( pSrcRect == NULL )
	{
		aRect.iLeft = 0;
		aRect.iTop = 0;
		aRect.iRight = hImage->usWidth;
		aRect.iBottom = hImage->usHeight;
	}
	else
	{
		aRect.iLeft = pSrcRect->iLeft;
		aRect.iTop = pSrcRect->iTop;
		aRect.iRight = pSrcRect->iRight;
		aRect.iBottom = pSrcRect->iBottom;
	}

	// This HIMAGE function will transparently copy buffer
	if ( !CopyImageToBuffer( hImage, fBufferBPP, pDest, usEffectiveWidth, hVSurface->usHeight, usX, usY, &aRect ) )
	{
    DbgMessage( TOPIC_VIDEOSURFACE, DBG_LEVEL_2, String( "Error Occured Copying HIMAGE to HVSURFACE" ));
		UnLockVideoSurfaceBuffer( hVSurface );
		return( FALSE );
	}

	// All is OK
	UnLockVideoSurfaceBuffer( hVSurface );

	return( TRUE );

}

// Palette setting is expensive, need to set both DDPalette and create 16BPP palette
BOOLEAN SetVideoSurfacePalette( HVSURFACE hVSurface, SGPPaletteEntry *pSrcPalette )
{
	UINT32 i;

	Assert( hVSurface != NULL );

	// Create palette object if not already done so
	if ( hVSurface->pPalette == NULL )
	{
		hVSurface->pPalette = MemAlloc(sizeof(*hVSurface->pPalette) * 256);
	}
	for (i = 0; i < 256; i++)
	{
		hVSurface->pPalette[i].r = pSrcPalette[i].peRed;
		hVSurface->pPalette[i].g = pSrcPalette[i].peGreen;
		hVSurface->pPalette[i].b = pSrcPalette[i].peBlue;
	}

	// Delete 16BPP Palette if one exists
	if ( hVSurface->p16BPPPalette != NULL )
	{
		MemFree( hVSurface->p16BPPPalette );
		hVSurface->p16BPPPalette = NULL;
	}

	// Create 16BPP Palette
	hVSurface->p16BPPPalette = Create16BPPPalette( pSrcPalette );

  DbgMessage(TOPIC_VIDEOSURFACE, DBG_LEVEL_3, String("Video Surface Palette change successfull" ));
	return( TRUE );
}


// Transparency needs to take RGB value and find best fit and place it into DD Surface
// colorkey value.
static BOOLEAN SetVideoSurfaceTransparencyColor(HVSURFACE hVSurface, COLORVAL TransColor)
{
	Uint32 ColorKey;
	SDL_Surface* Surface;

	// Assertions
	Assert( hVSurface != NULL );

	Surface = hVSurface->surface;
	CHECKF(Surface != NULL);

	// Get right pixel format, based on bit depth

	switch( hVSurface->ubBitDepth )
	{
		case  8: ColorKey = TransColor;                break;
		case 16: ColorKey = Get16BPPColor(TransColor); break;
	}

	SDL_SetColorKey(Surface, SDL_SRCCOLORKEY, ColorKey);

	return( TRUE );
}


BOOLEAN GetVSurfacePaletteEntries( HVSURFACE hVSurface, SGPPaletteEntry *pPalette )
{
	UINT32 i;

	CHECKF( hVSurface->pPalette != NULL );

	for (i = 0; i < 256; i++)
	{
		pPalette[i].peRed   = hVSurface->pPalette[i].r;
		pPalette[i].peGreen = hVSurface->pPalette[i].g;
		pPalette[i].peBlue  = hVSurface->pPalette[i].b;
	}

	return( TRUE );
}


BOOLEAN DeleteVideoSurfaceFromIndex( UINT32 uiIndex )
{
	VSURFACE_NODE *curr;

	#ifdef _DEBUG
		gubVSDebugCode = DEBUGSTR_DELETEVIDEOSURFACEFROMINDEX;
		CheckValidVSurfaceIndex( uiIndex );
	#endif

	curr = gpVSurfaceHead;
	while( curr )
	{
		if( curr->uiIndex == uiIndex )
		{ //Found the node, so detach it and delete it.

			//Deallocate the memory for the video surface
			DeleteVideoSurface( curr->hVSurface );

			if( curr == gpVSurfaceHead )
			{ //Advance the head, because we are going to remove the head node.
				gpVSurfaceHead = gpVSurfaceHead->next;
			}
			if( curr == gpVSurfaceTail )
			{ //Back up the tail, because we are going to remove the tail node.
				gpVSurfaceTail = gpVSurfaceTail->prev;
			}
			//Detach the node from the vsurface list
			if( curr->next )
			{ //Make the prev node point to the next
				curr->next->prev = curr->prev;
			}
			if( curr->prev )
			{ //Make the next node point to the prev
				curr->prev->next = curr->next;
			}
			//The node is now detached.  Now deallocate it.

			#ifdef SGP_VIDEO_DEBUGGING
				if( curr->pName )
				{
					MemFree( curr->pName );
				}
				if( curr->pCode )
				{
					MemFree( curr->pCode );
				}
			#endif

			MemFree( curr );
			guiVSurfaceSize--;
			return TRUE;
		}
		curr = curr->next;
	}
	return FALSE;
}


// Deletes all palettes, surfaces and region data
BOOLEAN DeleteVideoSurface( HVSURFACE hVSurface )
{
	// Assertions
	CHECKF( hVSurface != NULL );

	SDL_FreeSurface(hVSurface->surface);

	// Release palette
	if ( hVSurface->pPalette != NULL )
	{
		MemFree(hVSurface->pPalette);
		hVSurface->pPalette = NULL;
	}

	//If there is a 16bpp palette, free it
	if( hVSurface->p16BPPPalette != NULL )
	{
		MemFree( hVSurface->p16BPPPalette );
		hVSurface->p16BPPPalette = NULL;
	}

	giMemUsedInSurfaces -= ( hVSurface->usHeight * hVSurface->usWidth * ( hVSurface->ubBitDepth / 8 ) );

	// Release object
	MemFree( hVSurface );

	return( TRUE );
}


static BOOLEAN GetVSurfaceRect(HVSURFACE hVSurface, RECT* pRect)
{
	Assert( hVSurface != NULL );
	Assert( pRect != NULL );

	pRect->left=0;
	pRect->top=0;
	pRect->right=hVSurface->usWidth;
	pRect->bottom=hVSurface->usHeight;

	return( TRUE );
}


// *******************************************************************
//
// Blitting Functions
//
// *******************************************************************

// Blt  will use DD Blt or BltFast depending on flags.
// Will drop down into user-defined blitter if 8->16 BPP blitting is being done

BOOLEAN BltVideoSurfaceToVideoSurface( HVSURFACE hDestVSurface, HVSURFACE hSrcVSurface, UINT16 usIndex, INT32 iDestX, INT32 iDestY, INT32 fBltFlags, const SGPRect* SRect)
{
	RECT					 SrcRect, DestRect;
	UINT8					*pSrcSurface8, *pDestSurface8;
	UINT16				*pDestSurface16, *pSrcSurface16;
	UINT32				uiSrcPitch, uiDestPitch, uiWidth, uiHeight;

	// Assertions
	Assert( hDestVSurface != NULL );

	// Check for source coordinate options - specific rect or full src dimensions
	if (SRect != NULL)
	{
		SrcRect.top    = SRect->iTop;
		SrcRect.left   = SRect->iLeft;
		SrcRect.bottom = SRect->iBottom;
		SrcRect.right  = SRect->iRight;
	}
	else
	{
		// Here, use default, which is entire Video Surface
		// Check Sizes, SRC size MUST be <= DEST size
		if ( hDestVSurface->usHeight < hSrcVSurface->usHeight )
		{
				DbgMessage(TOPIC_VIDEOSURFACE, DBG_LEVEL_2, String( "Incompatible height size given in Video Surface blit" ));
				return( FALSE );
		}
		if ( hDestVSurface->usWidth < hSrcVSurface->usWidth )
		{
				DbgMessage(TOPIC_VIDEOSURFACE, DBG_LEVEL_2, String( "Incompatible height size given in Video Surface blit" ));
				return( FALSE );
		}

		SrcRect.top = (int)0;
		SrcRect.left = (int)0;
		SrcRect.bottom = (int)hSrcVSurface->usHeight;
		SrcRect.right = (int)hSrcVSurface->usWidth;
	}

	// Once here, assert valid Src
	Assert( hSrcVSurface != NULL );

	// clipping -- added by DB
	GetVSurfaceRect( hDestVSurface, &DestRect);
	uiWidth=SrcRect.right-SrcRect.left;
	uiHeight=SrcRect.bottom-SrcRect.top;

	// check for position entirely off the screen
	if(iDestX >= DestRect.right)
		return(FALSE);
	if(iDestY >= DestRect.bottom)
		return(FALSE);
	if((iDestX+(INT32)uiWidth) < (INT32)DestRect.left)
		return(FALSE);
	if((iDestY+(INT32)uiHeight) < (INT32)DestRect.top)
		return(FALSE);

	if((iDestX+(INT32)uiWidth) >= (INT32)DestRect.right)
	{
		SrcRect.right-=((iDestX+uiWidth)-DestRect.right);
		uiWidth-=((iDestX+uiWidth)-DestRect.right);
	}
	if((iDestY+(INT32)uiHeight) >= (INT32)DestRect.bottom)
	{
		SrcRect.bottom-=((iDestY+uiHeight)-DestRect.bottom);
		uiHeight-=((iDestY+uiHeight)-DestRect.bottom);
	}
	if(iDestX < DestRect.left)
	{
		SrcRect.left+=(DestRect.left-iDestX);
		uiWidth-=(DestRect.left-iDestX);
		iDestX=DestRect.left;
	}
	if(iDestY < (INT32)DestRect.top)
	{
		SrcRect.top+=(DestRect.top-iDestY);
		uiHeight-=(DestRect.top-iDestY);
		iDestY=DestRect.top;
	}

	// Send dest position, rectangle, etc to DD bltfast function
	// First check BPP values for compatibility
	if ( hDestVSurface->ubBitDepth == 16 && hSrcVSurface->ubBitDepth == 16 )
	{
// For testing with non-DDraw blitting, uncomment to test -- DB
#ifndef JA2
		if((pSrcSurface16=(UINT16 *)LockVideoSurfaceBuffer(hSrcVSurface, &uiSrcPitch))==NULL)
		{
			DbgMessage(TOPIC_VIDEOSURFACE, DBG_LEVEL_2, String( "Failed on lock of 16BPP surface for blitting" ));
			return(FALSE);
		}

		if((pDestSurface16=(UINT16 *)LockVideoSurfaceBuffer(hDestVSurface, &uiDestPitch))==NULL)
		{
			UnLockVideoSurfaceBuffer(hSrcVSurface);
			DbgMessage(TOPIC_VIDEOSURFACE, DBG_LEVEL_2, String( "Failed on lock of 16BPP dest surface for blitting" ));
			return(FALSE);
		}

		Blt16BPPTo16BPP(pDestSurface16, uiDestPitch, pSrcSurface16, uiSrcPitch, iDestX, iDestY, SrcRect.left, SrcRect.top, uiWidth, uiHeight);
		UnLockVideoSurfaceBuffer(hSrcVSurface);
		UnLockVideoSurfaceBuffer(hDestVSurface);
		return(TRUE);
#endif

		CHECKF( BltVSurfaceUsingDD( hDestVSurface, hSrcVSurface, fBltFlags, iDestX, iDestY, &SrcRect ) );

	}
	else if ( hDestVSurface->ubBitDepth == 8 && hSrcVSurface->ubBitDepth == 8 )
	{
		if((pSrcSurface8=(UINT8 *)LockVideoSurfaceBuffer(hSrcVSurface, &uiSrcPitch))==NULL)
		{
			DbgMessage(TOPIC_VIDEOSURFACE, DBG_LEVEL_2, String( "Failed on lock of 8BPP surface for blitting" ));
			return(FALSE);
		}

		if((pDestSurface8=(UINT8 *)LockVideoSurfaceBuffer(hDestVSurface, &uiDestPitch))==NULL)
		{
			UnLockVideoSurfaceBuffer(hSrcVSurface);
			DbgMessage(TOPIC_VIDEOSURFACE, DBG_LEVEL_2, String( "Failed on lock of 8BPP dest surface for blitting" ));
			return(FALSE);
		}

		Blt8BPPTo8BPP(pDestSurface8, uiDestPitch, pSrcSurface8, uiSrcPitch, iDestX, iDestY, SrcRect.left, SrcRect.top, uiWidth, uiHeight);
		UnLockVideoSurfaceBuffer(hSrcVSurface);
		UnLockVideoSurfaceBuffer(hDestVSurface);
		return(TRUE);
	}
	else
	{
		DbgMessage(TOPIC_VIDEOSURFACE, DBG_LEVEL_2, String( "Incompatible BPP values with src and dest Video Surfaces for blitting" ));
		return( FALSE );
	}

	return( TRUE );
}


static HVSURFACE CreateVideoSurfaceFromDDSurface(SDL_Surface* surface)
{
	// Create Video Surface
	HVSURFACE						hVSurface;
	SGPPaletteEntry			SGPPalette[ 256 ];

	hVSurface = MemAlloc(sizeof(*hVSurface));

	hVSurface->surface           = surface;
	hVSurface->usHeight          = surface->h;
	hVSurface->usWidth           = surface->w;
	hVSurface->ubBitDepth        = surface->format->BitsPerPixel;

	if (surface->format->palette != NULL) // XXX necessary?
	{
		UINT32 i;

		hVSurface->pPalette = MemAlloc(sizeof(*hVSurface->pPalette) * 256);
		for (i = 0; i < 256; i++)
		{
			hVSurface->pPalette[i] = surface->format->palette->colors[i];
			SGPPalette[i].peRed   = surface->format->palette->colors[i].r;
			SGPPalette[i].peGreen = surface->format->palette->colors[i].g;
			SGPPalette[i].peBlue  = surface->format->palette->colors[i].b;
		}
		hVSurface->p16BPPPalette = Create16BPPPalette( SGPPalette );
	}
	else
	{
		hVSurface->pPalette = NULL;
		hVSurface->p16BPPPalette = NULL;
	}

	// All is well
  DbgMessage( TOPIC_VIDEOSURFACE, DBG_LEVEL_0, String("Success in Creating Video Surface from DD Surface" ) );

	return hVSurface;
}


// UTILITY FUNCTIONS FOR BLITTING

static BOOLEAN ClipReleatedSrcAndDestRectangles(HVSURFACE hDestVSurface, HVSURFACE hSrcVSurface, RECT* DestRect, RECT* SrcRect)
{

	Assert( hDestVSurface != NULL );
	Assert( hSrcVSurface != NULL );

	// Check for invalid start positions and clip by ignoring blit
	if ( DestRect->left >= hDestVSurface->usWidth || DestRect->top >= hDestVSurface->usHeight )
	{
		return( FALSE );
	}

	if ( SrcRect->left >= hSrcVSurface->usWidth || SrcRect->top >= hSrcVSurface->usHeight )
	{
		return( FALSE );
	}

	// For overruns
	// Clip destination rectangles
	if ( DestRect->right > hDestVSurface->usWidth )
	{
		// Both have to be modified or by default streching occurs
		DestRect->right = hDestVSurface->usWidth;
		SrcRect->right = SrcRect->left + ( DestRect->right - DestRect->left );
	}
	if ( DestRect->bottom > hDestVSurface->usHeight )
	{
		// Both have to be modified or by default streching occurs
		DestRect->bottom = hDestVSurface->usHeight;
		SrcRect->bottom = SrcRect->top + ( DestRect->bottom - DestRect->top );
	}

	// Clip src rectangles
	if ( SrcRect->right > hSrcVSurface->usWidth )
	{
		// Both have to be modified or by default streching occurs
		SrcRect->right = hSrcVSurface->usWidth;
		DestRect->right = DestRect->left  + ( SrcRect->right - SrcRect->left );
	}
	if ( SrcRect->bottom > hSrcVSurface->usHeight )
	{
		// Both have to be modified or by default streching occurs
		SrcRect->bottom = hSrcVSurface->usHeight;
		DestRect->bottom = DestRect->top + ( SrcRect->bottom - SrcRect->top );
	}

	// For underruns
	// Clip destination rectangles
	if ( DestRect->left < 0 )
	{
		// Both have to be modified or by default streching occurs
		DestRect->left = 0;
		SrcRect->left = SrcRect->right - ( DestRect->right - DestRect->left );
	}
	if ( DestRect->top < 0 )
	{
		// Both have to be modified or by default streching occurs
		DestRect->top = 0;
		SrcRect->top = SrcRect->bottom - ( DestRect->bottom - DestRect->top );
	}

	// Clip src rectangles
	if ( SrcRect->left < 0 )
	{
		// Both have to be modified or by default streching occurs
		SrcRect->left = 0;
		DestRect->left = DestRect->right  - ( SrcRect->right - SrcRect->left );
	}
	if ( SrcRect->top < 0 )
	{
		// Both have to be modified or by default streching occurs
		SrcRect->top = 0;
		DestRect->top = DestRect->bottom - ( SrcRect->bottom - SrcRect->top );
	}

	return( TRUE );
}


static BOOLEAN FillSurfaceRect(HVSURFACE hDestVSurface, SDL_Rect* Rect, UINT16 Color)
{
	Assert( hDestVSurface != NULL );
	CHECKF(Rect != NULL);

	SDL_FillRect(hDestVSurface->surface, Rect, Color);

	return( TRUE );
}


BOOLEAN BltVSurfaceUsingDD( HVSURFACE hDestVSurface, HVSURFACE hSrcVSurface, UINT32 fBltFlags, INT32 iDestX, INT32 iDestY, RECT *SrcRect )
{
	SDL_Rect srcrect;
	SDL_Rect dstrect;

  // Blit using the correct blitter
	if ( fBltFlags & VS_BLT_FAST )
	{
		// Validations
		CHECKF( iDestX >= 0 );
		CHECKF( iDestY >= 0 );

		srcrect.x = SrcRect->left;
		srcrect.y = SrcRect->top;
		srcrect.w = SrcRect->right  - SrcRect->left;
		srcrect.h = SrcRect->bottom - SrcRect->top;
		dstrect.x = iDestX;
		dstrect.y = iDestY;
	}
	else
	{
		// Normal, specialized blit for clipping, etc

		// Setup dest rectangle
		RECT DestRect;
		DestRect.top =  (int)iDestY;
		DestRect.left = (int)iDestX;
		DestRect.bottom = (int)iDestY + ( SrcRect->bottom - SrcRect->top );
		DestRect.right = (int)iDestX + ( SrcRect->right - SrcRect->left );

		// Do Clipping of rectangles
		if ( !ClipReleatedSrcAndDestRectangles( hDestVSurface, hSrcVSurface, &DestRect, SrcRect ) )
		{
			// Returns false because dest start is > dest size
			return( TRUE );
		}

		// Check values for 0 size
		if ( DestRect.top == DestRect.bottom || DestRect.right == DestRect.left )
		{
			return( TRUE );
		}

		srcrect.x = SrcRect->left;
		srcrect.y = SrcRect->top;
		srcrect.w = SrcRect->right  - SrcRect->left;
		srcrect.h = SrcRect->bottom - SrcRect->top;
		dstrect.x = DestRect.left;
		dstrect.y = DestRect.top;
	}

	SDL_BlitSurface(hSrcVSurface->surface, &srcrect, hDestVSurface->surface, &dstrect);
	return TRUE;
}

BOOLEAN Blt16BPPBufferShadowRectAlternateTable(UINT16 *pBuffer, UINT32 uiDestPitchBYTES, SGPRect *area);


static BOOLEAN InternalShadowVideoSurfaceRect(UINT32 uiDestVSurface, INT32 X1, INT32 Y1, INT32 X2, INT32 Y2, BOOLEAN fLowPercentShadeTable)
{
	UINT16 *pBuffer;
	UINT32 uiPitch;
	SGPRect   area;
	HVSURFACE hVSurface;


	// CLIP IT!
	// FIRST GET SURFACE

  //
	// Get Video Surface
  //
	#ifdef _DEBUG
		gubVSDebugCode = DEBUGSTR_SHADOWVIDEOSURFACERECT;
	#endif
	CHECKF( GetVideoSurface( &hVSurface, uiDestVSurface ) );

	if ( X1 < 0 )
			X1 = 0;

	if ( X2 < 0 )
			return( FALSE );

	if ( Y2 < 0 )
			return( FALSE );

	if ( Y1 < 0 )
			Y1 = 0;

	if ( X2 >= hVSurface->usWidth )
			X2 = hVSurface->usWidth-1;

	if ( Y2 >= hVSurface->usHeight )
			Y2 = hVSurface->usHeight-1;

	if ( X1 >= hVSurface->usWidth )
			return( FALSE );

	if ( Y1 >= hVSurface->usHeight )
			return( FALSE );

	if (  ( X2 - X1 ) <= 0 )
			return( FALSE );

	if (  ( Y2 - Y1 ) <= 0 )
			return( FALSE );


	area.iTop=Y1;
	area.iBottom=Y2;
	area.iLeft=X1;
	area.iRight=X2;


	// Lock video surface
	pBuffer = (UINT16*)LockVideoSurface( uiDestVSurface, &uiPitch );
 	//UnLockVideoSurface( uiDestVSurface );

	if ( pBuffer == NULL )
	{
		return( FALSE );
	}

	if ( !fLowPercentShadeTable )
	{
		// Now we have the video object and surface, call the shadow function
		if(!Blt16BPPBufferShadowRect(pBuffer, uiPitch, &area))
		{
			// Blit has failed if false returned
			return( FALSE );
		}
	}
	else
	{
		// Now we have the video object and surface, call the shadow function
		if(!Blt16BPPBufferShadowRectAlternateTable(pBuffer, uiPitch, &area))
		{
			// Blit has failed if false returned
			return( FALSE );
		}
	}

	// Mark as dirty if it's the backbuffer
	//if ( uiDestVSurface == BACKBUFFER )
	//{
	//	InvalidateBackbuffer( );
	//}

	UnLockVideoSurface( uiDestVSurface );
	return( TRUE );
}


BOOLEAN ShadowVideoSurfaceRect(  UINT32	uiDestVSurface, INT32 X1, INT32 Y1, INT32 X2, INT32 Y2)
{
 return( InternalShadowVideoSurfaceRect( uiDestVSurface, X1, Y1, X2, Y2, FALSE ) );
}


BOOLEAN ShadowVideoSurfaceRectUsingLowPercentTable(  UINT32	uiDestVSurface, INT32 X1, INT32 Y1, INT32 X2, INT32 Y2)
{
 return( InternalShadowVideoSurfaceRect( uiDestVSurface, X1, Y1, X2, Y2, TRUE ) );
}


//
// This function will stretch the source image to the size of the dest rect.
//
// If the 2 images are not 16 Bpp, it returns false.
//
BOOLEAN BltStretchVideoSurface(UINT32 uiDestVSurface, UINT32 uiSrcVSurface, SGPRect* SrcRect, SGPRect* DestRect)
{
	HVSURFACE	hDestVSurface;
	HVSURFACE	hSrcVSurface;

	#ifdef _DEBUG
		gubVSDebugCode = DEBUGSTR_BLTSTRETCHVIDEOSURFACE_DST;
	#endif
	if( !GetVideoSurface( &hDestVSurface, uiDestVSurface ) )
	{
		return FALSE;
	}
	#ifdef _DEBUG
		gubVSDebugCode = DEBUGSTR_BLTSTRETCHVIDEOSURFACE_SRC;
	#endif
	if( !GetVideoSurface( &hSrcVSurface, uiSrcVSurface ) )
	{
		return FALSE;
	}

	//if the 2 images are not both 16bpp, return FALSE
	if( (hDestVSurface->ubBitDepth != 16) && (hSrcVSurface->ubBitDepth != 16) )
		return(FALSE);

	const UINT16* os = (const UINT16*)hSrcVSurface->surface->pixels + (hSrcVSurface->surface->pitch >> 1) * SrcRect->iTop + SrcRect->iLeft;
	UINT16* d = (UINT16*)hDestVSurface->surface->pixels + (hDestVSurface->surface->pitch >> 1) * DestRect->iTop + DestRect->iLeft;

	const UINT width  = DestRect->iRight  - DestRect->iLeft;
	const UINT height = DestRect->iBottom - DestRect->iTop;
	const UINT dx = SrcRect->iRight  - SrcRect->iLeft;
	const UINT dy = SrcRect->iBottom - SrcRect->iTop;
	UINT py = 0;
	for (UINT iy = 0; iy < height; ++iy)
	{
		const UINT16* s = os;
		UINT px = 0;
		for (UINT ix = 0; ix < width; ++ix)
		{
			*d++ = *s;
			px += dx;
			for (; px >= width; px -= width) ++s;
		}
		d += (hDestVSurface->surface->pitch >> 1) - width;
		py += dy;
		for (; py >= height; py -= height) os += hSrcVSurface->surface->pitch >> 1;
	}

	return TRUE;
}


BOOLEAN ShadowVideoSurfaceImage( UINT32	uiDestVSurface, HVOBJECT hImageHandle, INT32 iPosX, INT32 iPosY)
{
	//Horizontal shadow
	ShadowVideoSurfaceRect( uiDestVSurface, iPosX+3, iPosY+hImageHandle->pETRLEObject->usHeight, iPosX+hImageHandle->pETRLEObject->usWidth, iPosY+	hImageHandle->pETRLEObject->usHeight+3);

	//vertical shadow
	ShadowVideoSurfaceRect( uiDestVSurface, iPosX+hImageHandle->pETRLEObject->usWidth, iPosY+3, iPosX+hImageHandle->pETRLEObject->usWidth+3, iPosY+	hImageHandle->pETRLEObject->usHeight);
	return( TRUE );
}


#ifdef _DEBUG
void CheckValidVSurfaceIndex( UINT32 uiIndex )
{
	BOOLEAN fAssertError = FALSE;
	if( uiIndex == 0xffffffff )
	{ //-1 index -- deleted
		fAssertError = TRUE;
	}
	else if( uiIndex % 2 && uiIndex < 0xfffffff0  )
	{ //odd numbers are reserved for vobjects
		fAssertError = TRUE;
	}

	if( fAssertError )
	{
		UINT8 str[60];
		switch( gubVSDebugCode )
		{
			case DEBUGSTR_SETVIDEOSURFACETRANSPARENCY:
				sprintf( str, "SetVideoSurfaceTransparency" );
				break;
			case DEBUGSTR_BLTVIDEOSURFACE_DST:
				sprintf( str, "BltVideoSurface (dest)" );
				break;
			case DEBUGSTR_BLTVIDEOSURFACE_SRC:
				sprintf( str, "BltVideoSurface (src)" );
				break;
			case DEBUGSTR_COLORFILLVIDEOSURFACEAREA:
				sprintf( str, "ColorFillVideoSurfaceArea" );
				break;
			case DEBUGSTR_SHADOWVIDEOSURFACERECT:
				sprintf( str, "ShadowVideoSurfaceRect" );
				break;
			case DEBUGSTR_BLTSTRETCHVIDEOSURFACE_DST:
				sprintf( str, "BltStretchVideoSurface (dest)" );
				break;
			case DEBUGSTR_BLTSTRETCHVIDEOSURFACE_SRC:
				sprintf( str, "BltStretchVideoSurface (src)" );
				break;
			case DEBUGSTR_DELETEVIDEOSURFACEFROMINDEX:
				sprintf( str, "DeleteVideoSurfaceFromIndex" );
				break;
			case DEBUGSTR_NONE:
			default:
				sprintf( str, "GetVideoSurface" );
				break;
		}
		if( uiIndex == 0xffffffff )
		{
			AssertMsg( 0, String( "Trying to %s with deleted index -1." , str ) );
		}
		else
		{
			AssertMsg( 0, String( "Trying to %s using a VOBJECT ID %d!", str, uiIndex ) );
		}
	}
}
#endif

#ifdef SGP_VIDEO_DEBUGGING
typedef struct DUMPFILENAME
{
	UINT8 str[256];
}DUMPFILENAME;


void DumpVSurfaceInfoIntoFile(const char *filename, BOOLEAN fAppend)
{
	VSURFACE_NODE *curr;
	FILE *fp;
	DUMPFILENAME *pName, *pCode;
	UINT32 *puiCounter;
	UINT8 tempName[ 256 ];
	UINT8 tempCode[ 256 ];
	UINT32 i, uiUniqueID;
	BOOLEAN fFound;
	if( !guiVSurfaceSize )
	{
		return;
	}

	if( fAppend )
	{
		fp = fopen( filename, "a" );
	}
	else
	{
		fp = fopen( filename, "w" );
	}
	Assert( fp );

	//Allocate enough strings and counters for each node.
	pName = (DUMPFILENAME*)MemAlloc( sizeof( DUMPFILENAME ) * guiVSurfaceSize );
	pCode = (DUMPFILENAME*)MemAlloc( sizeof( DUMPFILENAME ) * guiVSurfaceSize );
	memset( pName, 0, sizeof( DUMPFILENAME ) * guiVSurfaceSize );
	memset( pCode, 0, sizeof( DUMPFILENAME ) * guiVSurfaceSize );
	puiCounter = (UINT32*)MemAlloc( 4 * guiVSurfaceSize );
	memset( puiCounter, 0, 4 * guiVSurfaceSize );

	//Loop through the list and record every unique filename and count them
	uiUniqueID = 0;
	curr = gpVSurfaceHead;
	while( curr )
	{
		strcpy( tempName, curr->pName );
		strcpy( tempCode, curr->pCode );
		fFound = FALSE;
		for( i = 0; i < uiUniqueID; i++ )
		{
			if (strcasecmp(tempName, pName[i].str) == 0 && strcasecmp(tempCode, pCode[i].str) == 0)
			{ //same string
				fFound = TRUE;
				(puiCounter[ i ])++;
				break;
			}
		}
		if( !fFound )
		{
			strcpy( pName[i].str, tempName );
			strcpy( pCode[i].str, tempCode );
			(puiCounter[ i ])++;
			uiUniqueID++;
		}
		curr = curr->next;
	}

	//Now dump the info.
	fprintf( fp, "-----------------------------------------------\n" );
	fprintf( fp, "%d unique vSurface names exist in %d VSurfaces\n", uiUniqueID, guiVSurfaceSize );
	fprintf( fp, "-----------------------------------------------\n\n" );
	for( i = 0; i < uiUniqueID; i++ )
	{
		fprintf( fp, "%d occurrences of %s\n", puiCounter[i], pName[i].str );
		fprintf( fp, "%s\n\n", pCode[i].str );
	}
	fprintf( fp, "\n-----------------------------------------------\n\n" );

	//Free all memory associated with this operation.
	MemFree( pName );
	MemFree( pCode );
	MemFree( puiCounter );
	fclose( fp );
}


static BOOLEAN RecordVSurface(const char* Filename, UINT32* Index, UINT32 LineNum, const char* SourceFile)
{
	//record the filename of the vsurface (some are created via memory though)
	gpVSurfaceTail->pName = MemAlloc(strlen(Filename) + 1);
	strcpy(gpVSurfaceTail->pName, Filename);

	//record the code location of the calling creating function.
	char str[256];
	sprintf(str, "%s -- line(%d)", SourceFile, LineNum);
	gpVSurfaceTail->pCode = MemAlloc(strlen(str) + 1);
	strcpy(gpVSurfaceTail->pCode, str);

	return TRUE;
}


#undef AddVideoSurface
#undef AddVideoSurfaceFromFile


BOOLEAN AddAndRecordVSurface(VSURFACE_DESC* VSurfaceDesc, UINT32* Index, UINT32 LineNum, const char* SourceFile)
{
	if (!AddVideoSurface(VSurfaceDesc, Index)) return FALSE;
	return RecordVSurface("<EMPTY>", Index, LineNum, SourceFile);
}


BOOLEAN AddAndRecordVSurfaceFromFile(const char* Filename, UINT32* Index, UINT32 LineNum, const char* SourceFile)
{
	if (!AddVideoSurfaceFromFile(Filename, Index)) return FALSE;
	return RecordVSurface(Filename, Index, LineNum, SourceFile);
}

#endif
