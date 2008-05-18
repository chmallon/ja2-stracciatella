#include <stdexcept>

#include "Debug.h"
#include "HImage.h"
#include "MemMan.h"
#include "VObject.h"
#include "VObject_Blitters.h"
#include "VSurface.h"
#include "WCheck.h"


// ******************************************************************************
//
// Video Object SGP Module
//
// Video Objects are used to contain any imagery which requires blitting. The data
// is contained within a Direct Draw surface. Palette information is in both
// a Direct Draw Palette and a 16BPP palette structure for 8->16 BPP Blits.
// Blitting is done via Direct Draw as well as custum blitters. Regions are
// used to define local coordinates within the surface
//
// Second Revision: Dec 10, 1996, Andrew Emmons
//
// *******************************************************************************


SGPVObject::SGPVObject(SGPImage const* const img) :
	flags_(),
	palette16_(),
	pShades(),
	current_shade_(),
	ppZStripInfo()
{
	if (!(img->fFlags & IMAGE_TRLECOMPRESSED))
	{
		throw std::runtime_error("Image for video object creation must be TRLE compressed");
	}

	ETRLEData TempETRLEData;
	if (!GetETRLEImageData(img, &TempETRLEData))
	{
		throw std::runtime_error("Failed to get ETRLE data from image for video object creation");
	}

	subregion_count_ = TempETRLEData.usNumberOfObjects;
	pETRLEObject     = TempETRLEData.pETRLEObject;
	pPixData         = TempETRLEData.pPixData;
	pix_data_size_   = TempETRLEData.uiSizePixData;
	bit_depth_       = img->ubBitDepth;

	if (img->ubBitDepth == 8)
	{
		// create palette
		const SGPPaletteEntry* const src_pal = img->pPalette;
		Assert(src_pal != NULL);

		SGPPaletteEntry* const pal = palette_.Allocate(256);
		memcpy(pal, src_pal, sizeof(*pal) * 256);

		palette16_     = Create16BPPPalette(pal);
		current_shade_ = palette16_;
	}
}


SGPVObject::~SGPVObject()
{
	DestroyPalettes();

	if (pPixData     != NULL) MemFree(pPixData);
	if (pETRLEObject != NULL) MemFree(pETRLEObject);

	if (ppZStripInfo != NULL)
	{
		for (UINT32 usLoop = 0; usLoop < SubregionCount(); usLoop++)
		{
			if (ppZStripInfo[usLoop] != NULL)
			{
				MemFree(ppZStripInfo[usLoop]->pbZChange);
				MemFree(ppZStripInfo[usLoop]);
			}
		}
		MemFree(ppZStripInfo);
	}
}


void SGPVObject::CurrentShade(size_t const idx)
{
	if (idx >= lengthof(pShades) || !pShades[idx])
	{
		throw std::logic_error("Tried to set invalid video object shade");
	}
	current_shade_ = pShades[idx];
}


ETRLEObject const* SGPVObject::SubregionProperties(size_t const idx) const
{
	if (idx >= SubregionCount())
	{
		throw std::logic_error("Tried to access invalid subregion in video object");
	}
	return &pETRLEObject[idx];
}


/* Destroys the palette tables of a video object. All memory is deallocated, and
 * the pointers set to NULL. Be careful not to try and blit this object until
 * new tables are calculated, or things WILL go boom. */
void SGPVObject::DestroyPalettes()
{
	for (UINT32 x = 0; x < HVOBJECT_SHADE_TABLES; x++)
	{
		if (flags_ & SHADETABLE_SHARED) continue;

		if (pShades[x] != NULL)
		{
			if (palette16_ == pShades[x]) palette16_ = 0;

			MemFree(pShades[x]);
			pShades[x] = NULL;
		}
	}

	if (palette16_ != NULL)
	{
		MemFree(palette16_);
		palette16_ = 0;
	}

	current_shade_ = 0;
}


void SGPVObject::ShareShadetables(SGPVObject* const other)
{
	flags_ |= SHADETABLE_SHARED;
	for (size_t i = 0; i < lengthof(pShades); ++i)
	{
		pShades[i] = other->pShades[i];
	}
}


#define COMPRESS_TRANSPARENT				0x80
#define COMPRESS_RUN_MASK						0x7F


typedef struct VOBJECT_NODE
{
	HVOBJECT hVObject;
  struct VOBJECT_NODE* next;
#ifdef SGP_VIDEO_DEBUGGING
	char* pName;
	char* pCode;
#endif
} VOBJECT_NODE;

static VOBJECT_NODE* gpVObjectHead = NULL;
static VOBJECT_NODE* gpVObjectTail = NULL;
UINT32 guiVObjectSize = 0;


BOOLEAN InitializeVideoObjectManager(void)
{
	//Shouldn't be calling this if the video object manager already exists.
	//Call shutdown first...
	Assert(gpVObjectHead == NULL);
	Assert(gpVObjectTail == NULL);
	gpVObjectHead = NULL;
	gpVObjectTail = NULL;
	return TRUE;
}


BOOLEAN ShutdownVideoObjectManager(void)
{
	while (gpVObjectHead != NULL)
	{
		VOBJECT_NODE* curr = gpVObjectHead;
		gpVObjectHead = gpVObjectHead->next;
		delete curr->hVObject;
#ifdef SGP_VIDEO_DEBUGGING
		if (curr->pName != NULL) MemFree(curr->pName);
		if (curr->pCode != NULL) MemFree(curr->pCode);
#endif
		MemFree(curr);
	}
	gpVObjectHead = NULL;
	gpVObjectTail = NULL;
	guiVObjectSize = 0;
	return TRUE;
}


#ifdef JA2TESTVERSION
static UINT32 CountVideoObjectNodes(void)
{
	UINT32 i = 0;
	for (const VOBJECT_NODE* curr = gpVObjectHead; curr != NULL; curr = curr->next)
	{
		i++;
	}
	return i;
}
#endif


static void AddStandardVideoObject(HVOBJECT hVObject)
{
	if (hVObject == NULL) return;

	VOBJECT_NODE* const Node = MALLOC(VOBJECT_NODE);
	Assert(Node != NULL); // out of memory?
	Node->hVObject = hVObject;

	Node->next = NULL;
#ifdef SGP_VIDEO_DEBUGGING
	Node->pName = NULL;
	Node->pCode = NULL;
#endif

	if (gpVObjectTail == NULL)
	{
		gpVObjectHead = Node;
	}
	else
	{
		gpVObjectTail->next = Node;
	}
	gpVObjectTail = Node;

	guiVObjectSize++;
#ifdef JA2TESTVERSION
	if (CountVideoObjectNodes() != guiVObjectSize)
	{
		guiVObjectSize = guiVObjectSize;
	}
#endif
}


#ifdef SGP_VIDEO_DEBUGGING
static
#endif
SGPVObject* AddStandardVideoObjectFromHImage(HIMAGE hImage)
{
	SGPVObject* const vo = new SGPVObject(hImage);
	AddStandardVideoObject(vo);
	return vo;
}


#ifdef SGP_VIDEO_DEBUGGING
static
#endif
SGPVObject* AddStandardVideoObjectFromFile(const char* const ImageFile)
{
	AutoSGPImage hImage(CreateImage(ImageFile, IMAGE_ALLIMAGEDATA));
	return AddStandardVideoObjectFromHImage(hImage);
}


void DeleteVideoObject(SGPVObject* const vo)
{
	VOBJECT_NODE* prev = NULL;
	VOBJECT_NODE* curr = gpVObjectHead;
	while (curr != NULL)
	{
		if (curr->hVObject == vo)
		{ //Found the node, so detach it and delete it.
			delete vo;

			if (curr == gpVObjectHead) gpVObjectHead = gpVObjectHead->next;
			if (curr == gpVObjectTail) gpVObjectTail = prev;
			if (prev != NULL) prev->next = curr->next;

#ifdef SGP_VIDEO_DEBUGGING
			if (curr->pName != NULL) MemFree(curr->pName);
			if (curr->pCode != NULL) MemFree(curr->pCode);
#endif
			MemFree(curr);
			guiVObjectSize--;
#ifdef JA2TESTVERSION
			if (CountVideoObjectNodes() != guiVObjectSize)
			{
				guiVObjectSize = guiVObjectSize;
			}
#endif
			return;
		}
		prev = curr;
		curr = curr->next;
	}
}


BOOLEAN BltVideoObject(SGPVSurface* const dst, const SGPVObject* const src, const UINT16 usRegionIndex, const INT32 iDestX, const INT32 iDestY)
{
	Assert(src->BPP() ==  8);
	Assert(dst->BPP() == 16);

	SGPVSurface::Lock l(dst);
	UINT16* const pBuffer = l.Buffer<UINT16>();
	UINT32  const uiPitch = l.Pitch();

	if (BltIsClipped(src, iDestX, iDestY, usRegionIndex, &ClippingRect))
	{
		Blt8BPPDataTo16BPPBufferTransparentClip(pBuffer, uiPitch, src, iDestX, iDestY, usRegionIndex, &ClippingRect);
	}
	else
	{
		Blt8BPPDataTo16BPPBufferTransparent(pBuffer, uiPitch, src, iDestX, iDestY, usRegionIndex);
	}

	return TRUE;
}


/* Given a VOBJECT and ETRLE image index, retrieves the value of the pixel
 * located at the given image coordinates. The value returned is an 8-bit
 * palette index */
BOOLEAN GetETRLEPixelValue(UINT8* pDest, HVOBJECT hVObject, UINT16 usETRLEIndex, UINT16 usX, UINT16 usY)
{
	CHECKF(hVObject != NULL);

	ETRLEObject const* const pETRLEObject = hVObject->SubregionProperties(usETRLEIndex);

	CHECKF(usX < pETRLEObject->usWidth);
	CHECKF(usY < pETRLEObject->usHeight);

	// Assuming everything's okay, go ahead and look...
	UINT8* pCurrent = &((UINT8*)hVObject->pPixData)[pETRLEObject->uiDataOffset];

	// Skip past all uninteresting scanlines
	for (UINT16 usLoopY = 0; usLoopY < usY; usLoopY++)
	{
		while (*pCurrent != 0)
		{
			if (*pCurrent & COMPRESS_TRANSPARENT)
			{
				pCurrent++;
			}
			else
			{
				pCurrent += *pCurrent & COMPRESS_RUN_MASK;
			}
		}
	}

	// Now look in this scanline for the appropriate byte
	UINT16 usLoopX = 0;
	do
	{
		UINT16 ubRunLength = *pCurrent & COMPRESS_RUN_MASK;

		if (*pCurrent & COMPRESS_TRANSPARENT)
		{
			if (usLoopX + ubRunLength >= usX)
			{
				*pDest = 0;
				return TRUE;
			}
			else
			{
				pCurrent++;
			}
		}
		else
		{
			if (usLoopX + ubRunLength >= usX)
			{
				// skip to the correct byte; skip at least 1 to get past the byte defining the run
				pCurrent += (usX - usLoopX) + 1;
				*pDest = *pCurrent;
				return TRUE;
			}
			else
			{
				pCurrent += ubRunLength + 1;
			}
		}
		usLoopX += ubRunLength;
	}
	while (usLoopX < usX);
	// huh???
	return FALSE;
}


BOOLEAN BltVideoObjectOutline(SGPVSurface* const dst, const SGPVObject* const hSrcVObject, const UINT16 usIndex, const INT32 iDestX, const INT32 iDestY, const INT16 s16BPPColor)
{
	SGPVSurface::Lock l(dst);
	UINT16* const pBuffer = l.Buffer<UINT16>();
	UINT32  const uiPitch = l.Pitch();

	if (BltIsClipped(hSrcVObject, iDestX, iDestY, usIndex, &ClippingRect))
	{
		Blt8BPPDataTo16BPPBufferOutlineClip(pBuffer, uiPitch, hSrcVObject, iDestX, iDestY, usIndex, s16BPPColor, &ClippingRect);
	}
	else
	{
		Blt8BPPDataTo16BPPBufferOutline(pBuffer, uiPitch, hSrcVObject, iDestX, iDestY, usIndex, s16BPPColor);
	}

	return TRUE;
}


BOOLEAN BltVideoObjectOutlineShadow(SGPVSurface* const dst, const SGPVObject* const src, const UINT16 usIndex, const INT32 iDestX, const INT32 iDestY)
{
	SGPVSurface::Lock l(dst);
	UINT16* const pBuffer = l.Buffer<UINT16>();
	UINT32  const uiPitch = l.Pitch();

	if (BltIsClipped(src, iDestX, iDestY, usIndex, &ClippingRect))
	{
		Blt8BPPDataTo16BPPBufferOutlineShadowClip(pBuffer, uiPitch, src, iDestX, iDestY, usIndex, &ClippingRect);
	}
	else
	{
		Blt8BPPDataTo16BPPBufferOutlineShadow(pBuffer, uiPitch, src, iDestX, iDestY, usIndex);
	}

	return TRUE;
}


BOOLEAN BltVideoObjectOnce(SGPVSurface* const dst, const char* const filename, const UINT16 region, const INT32 x, const INT32 y)
try
{
	AutoSGPVObject vo(AddVideoObjectFromFile(filename));
	BltVideoObject(dst, vo, region, x, y);
	return TRUE;
}
catch (...) { return FALSE; }


#ifdef SGP_VIDEO_DEBUGGING

typedef struct DUMPINFO
{
	UINT32 Counter;
	char Name[256];
	char Code[256];
} DUMPINFO;


static void DumpVObjectInfoIntoFile(const char* filename, BOOLEAN fAppend)
{
	if (guiVObjectSize == 0) return;

	FILE* fp = fopen(filename, fAppend ? "a" : "w");
	Assert(fp != NULL);

	//Allocate enough strings and counters for each node.
	DUMPINFO* const Info = MALLOCNZ(DUMPINFO, guiVObjectSize);

	//Loop through the list and record every unique filename and count them
	UINT32 uiUniqueID = 0;
	for (const VOBJECT_NODE* curr = gpVObjectHead; curr != NULL; curr = curr->next)
	{
		const char* Name = curr->pName;
		const char* Code = curr->pCode;
		BOOLEAN fFound = FALSE;
		for (UINT32 i = 0; i < uiUniqueID; i++)
		{
			if (strcasecmp(Name, Info[i].Name) == 0 && strcasecmp(Code, Info[i].Code) == 0)
			{ //same string
				fFound = TRUE;
				Info[i].Counter++;
				break;
			}
		}
		if (!fFound)
		{
			strcpy(Info[uiUniqueID].Name, Name);
			strcpy(Info[uiUniqueID].Code, Code);
			Info[uiUniqueID].Counter++;
			uiUniqueID++;
		}
	}

	//Now dump the info.
	fprintf(fp, "-----------------------------------------------\n");
	fprintf(fp, "%d unique vObject names exist in %d VObjects\n", uiUniqueID, guiVObjectSize);
	fprintf(fp, "-----------------------------------------------\n\n");
	for (UINT32 i = 0; i < uiUniqueID; i++)
	{
		fprintf(fp, "%d occurrences of %s\n%s\n\n", Info[i].Counter, Info[i].Name, Info[i].Code);
	}
	fprintf(fp, "\n-----------------------------------------------\n\n");

	//Free all memory associated with this operation.
	MemFree(Info);
	fclose(fp);
}


//Debug wrapper for adding vObjects
static void RecordVObject(const char* Filename, UINT32 uiLineNum, const char* pSourceFile)
{
	//record the filename of the vObject (some are created via memory though)
	gpVObjectTail->pName = MALLOCN(char, strlen(Filename) + 1);
	strcpy(gpVObjectTail->pName, Filename);

	//record the code location of the calling creating function.
	char str[256];
	sprintf(str, "%s -- line(%d)", pSourceFile, uiLineNum);
	gpVObjectTail->pCode = MALLOCN(char, strlen(str) + 1);
	strcpy(gpVObjectTail->pCode, str);
}


SGPVObject* AddAndRecordVObjectFromHImage(HIMAGE hImage, UINT32 uiLineNum, const char* pSourceFile)
{
	SGPVObject* const vo = AddStandardVideoObjectFromHImage(hImage);
	RecordVObject("<IMAGE>", uiLineNum, pSourceFile);
	return vo;
}


SGPVObject* AddAndRecordVObjectFromFile(const char* ImageFile, UINT32 uiLineNum, const char* pSourceFile)
{
	SGPVObject* const vo = AddStandardVideoObjectFromFile(ImageFile);
	RecordVObject(ImageFile, uiLineNum, pSourceFile);
	return vo;
}


void PerformVideoInfoDumpIntoFile(const char* filename, BOOLEAN fAppend)
{
	DumpVObjectInfoIntoFile(filename, fAppend);
	DumpVSurfaceInfoIntoFile(filename, TRUE);
}

#endif
