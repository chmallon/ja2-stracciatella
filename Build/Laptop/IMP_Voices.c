#include "CharProfile.h"
#include "Font.h"
#include "IMP_Voices.h"
#include "IMP_MainPage.h"
#include "IMPVideoObjects.h"
#include "WCheck.h"
#include "Input.h"
#include "Debug.h"
#include "Render_Dirty.h"
#include "Cursors.h"
#include "Laptop.h"
#include "Sound_Control.h"
#include "IMP_Text_System.h"
#include "Text.h"
#include "LaptopSave.h"
#include "Button_System.h"
#include "SoundMan.h"
#include "Font_Control.h"


//current and last pages
INT32 iCurrentVoices = 0;
INT32 iLastVoice = 2;

//INT32 iVoiceId = 0;


UINT32 uiVocVoiceSound = 0;
// buttons needed for the IMP Voices screen
INT32 giIMPVoicesButton[ 3 ];
static BUTTON_PICS* giIMPVoicesButtonImage[3];


// redraw protrait screen
BOOLEAN fReDrawVoicesScreenFlag = FALSE;

// the portrait region, for player to click on and re-hear voice
MOUSE_REGION gVoicePortraitRegion;


static void CreateIMPVoiceMouseRegions(void);
static void CreateIMPVoicesButtons(void);


void EnterIMPVoices( void )
{
		// create buttons
	CreateIMPVoicesButtons( );

	// create mouse regions
	CreateIMPVoiceMouseRegions( );

	// render background
	RenderIMPVoices( );

	// play voice once
	uiVocVoiceSound = PlayVoice( );
}


static void RenderVoiceIndex(void);


void RenderIMPVoices( void )
{


  // render background
	RenderProfileBackGround( );

	// the Voices frame
	RenderPortraitFrame( 191, 167 );

	// the sillouette
	RenderLargeSilhouette( 200, 176 );

	// indent for the text
	RenderAttrib1IndentFrame( 128, 65);

	// render voice index value
	RenderVoiceIndex( );

	// text
	PrintImpText( );
}


static void DestroyIMPVoiceMouseRegions(void);
static void DestroyIMPVoicesButtons(void);


void ExitIMPVoices( void )
{
	// destroy buttons for IMP Voices page
  DestroyIMPVoicesButtons( );

  // destroy mouse regions for this screen
	DestroyIMPVoiceMouseRegions( );
}

void HandleIMPVoices( void )
{

	// do we need to re write screen
	if ( fReDrawVoicesScreenFlag == TRUE )
	{
    RenderIMPVoices( );

		// reset redraw flag
		fReDrawVoicesScreenFlag = FALSE;
	}
}


static void IncrementVoice(void)
{
	// cycle to next voice

	iCurrentVoices++;

	// gone too far?
	if( iCurrentVoices > iLastVoice )
	{
		iCurrentVoices = 0;
	}
}


static void DecrementVoice(void)
{
  // cycle to previous voice

	iCurrentVoices--;

	// gone too far?
  if( iCurrentVoices < 0 )
	{
    iCurrentVoices = iLastVoice;
	}
}


static void MakeButton(UINT idx, const char* img_file, INT32 off_normal, INT32 on_normal, const wchar_t* text, INT16 x, INT16 y, GUI_CALLBACK click)
{
	BUTTON_PICS* const img = LoadButtonImage(img_file, -1, off_normal, -1, on_normal, -1);
	giIMPVoicesButtonImage[idx] = img;
	const INT16 text_col   = FONT_WHITE;
	const INT16 shadow_col = DEFAULT_SHADOW;
	const INT32 btn = CreateIconAndTextButton(img, text, FONT12ARIAL, text_col, shadow_col, text_col, shadow_col, x, y, MSYS_PRIORITY_HIGH, click);
	giIMPVoicesButton[idx] = btn;
	SetButtonCursor(btn, CURSOR_WWW);
}


static void BtnIMPVoicesDoneCallback(GUI_BUTTON* btn, INT32 reason);
static void BtnIMPVoicesNextCallback(GUI_BUTTON* btn, INT32 reason);
static void BtnIMPVoicesPreviousCallback(GUI_BUTTON* btn, INT32 reason);


static void CreateIMPVoicesButtons(void)
{
  // will create buttons need for the IMP Voices screen
	const INT16 dx = LAPTOP_SCREEN_UL_X;
	const INT16 dy = LAPTOP_SCREEN_WEB_UL_Y;
	MakeButton(0, "LAPTOP/voicearrows.sti", 1, 3, pImpButtonText[13], dx + 343, dy + 205, BtnIMPVoicesNextCallback);     // Next button
	MakeButton(1, "LAPTOP/voicearrows.sti", 0, 2, pImpButtonText[12], dx +  93, dy + 205, BtnIMPVoicesPreviousCallback); // Previous button
	MakeButton(2, "LAPTOP/button_5.sti",    0, 1, pImpButtonText[11], dx + 187, dy + 330, BtnIMPVoicesDoneCallback);     // Done button
}


static void DestroyIMPVoicesButtons(void)
{

	// will destroy buttons created for IMP Voices screen

	// the next button
  RemoveButton(giIMPVoicesButton[ 0 ] );
  UnloadButtonImage(giIMPVoicesButtonImage[ 0 ] );

	// the previous button
  RemoveButton(giIMPVoicesButton[ 1 ] );
  UnloadButtonImage(giIMPVoicesButtonImage[ 1 ] );

	// the done button
  RemoveButton(giIMPVoicesButton[ 2 ] );
  UnloadButtonImage(giIMPVoicesButtonImage[ 2 ] );
}


static void BtnIMPVoicesNextCallback(GUI_BUTTON *btn, INT32 reason)
{
	if (reason & MSYS_CALLBACK_REASON_LBUTTON_UP)
	{
		IncrementVoice();
		if (SoundIsPlaying(uiVocVoiceSound)) SoundStop(uiVocVoiceSound);
		uiVocVoiceSound = PlayVoice();
		fReDrawVoicesScreenFlag = TRUE;
	}
}


static void BtnIMPVoicesPreviousCallback(GUI_BUTTON *btn, INT32 reason)
{
	if (reason & MSYS_CALLBACK_REASON_LBUTTON_UP)
	{
		DecrementVoice();
		if (SoundIsPlaying(uiVocVoiceSound)) SoundStop(uiVocVoiceSound);
		uiVocVoiceSound = PlayVoice();
		fReDrawVoicesScreenFlag = TRUE;
	}
}


static void BtnIMPVoicesDoneCallback(GUI_BUTTON *btn, INT32 reason)
{
	if (reason & MSYS_CALLBACK_REASON_LBUTTON_UP)
	{
		iCurrentImpPage = IMP_MAIN_PAGE;

		// if we are already done, leave
		if (iCurrentProfileMode == 5)
		{
			iCurrentImpPage = IMP_FINISH;
		}
		else if (iCurrentProfileMode < 4)
		{
			// current mode now is voice
			iCurrentProfileMode = 4;
		}
		else if (iCurrentProfileMode == 4)
		{
			// all done profiling
			iCurrentProfileMode = 5;
			iCurrentImpPage = IMP_FINISH;
		}

		// set voice id, to grab character slot
		LaptopSaveInfo.iVoiceId = iCurrentVoices + (fCharacterIsMale ? 0 : 3);

		// set button up image pending
		fButtonPendingFlag = TRUE;
	}
}


UINT32 PlayVoice( void )
{
	// gender?
	if( fCharacterIsMale == TRUE )
	{
	  switch( iCurrentVoices )
		{
		  case( 0 ):
	     return PlayJA2SampleFromFile("Speech/051_001.wav", MIDVOLUME, 1, MIDDLEPAN);
			case( 1 ):
	     return PlayJA2SampleFromFile("Speech/052_001.wav", MIDVOLUME, 1, MIDDLEPAN);
			case( 2 ):
				return PlayJA2SampleFromFile("Speech/053_001.wav", MIDVOLUME, 1, MIDDLEPAN);
		}
	}
	else
	{
    switch( iCurrentVoices )
		{
		  case( 0 ):
				return PlayJA2SampleFromFile("Speech/054_001.wav", MIDVOLUME, 1, MIDDLEPAN);
			case( 1 ):
	    	return PlayJA2SampleFromFile("Speech/055_001.wav", MIDVOLUME, 1, MIDDLEPAN);
			case( 2 ):
				return PlayJA2SampleFromFile("Speech/056_001.wav", MIDVOLUME, 1, MIDDLEPAN);
		}

	}
	return 0;
}


static void IMPPortraitRegionButtonCallback(MOUSE_REGION* pRegion, INT32 iReason);


static void CreateIMPVoiceMouseRegions(void)
{
  // will create mouse regions needed for the IMP voices page
	MSYS_DefineRegion( &gVoicePortraitRegion, LAPTOP_SCREEN_UL_X + 200, LAPTOP_SCREEN_WEB_UL_Y + 176 ,LAPTOP_SCREEN_UL_X + 200 + 100, LAPTOP_SCREEN_WEB_UL_Y + 176 + 100,MSYS_PRIORITY_HIGH,
							 MSYS_NO_CURSOR, MSYS_NO_CALLBACK, IMPPortraitRegionButtonCallback );
}


static void DestroyIMPVoiceMouseRegions(void)
{
  // will destroy already created mouse reiogns for IMP voices page
  MSYS_RemoveRegion( &gVoicePortraitRegion );
}


static void IMPPortraitRegionButtonCallback(MOUSE_REGION* pRegion, INT32 iReason)
{
	// callback handler for imp portrait region button events
  if(iReason & MSYS_CALLBACK_REASON_LBUTTON_UP)
  {
    if( ! SoundIsPlaying( uiVocVoiceSound ) )
		{
       uiVocVoiceSound = PlayVoice( );
		}
  }
}


static void RenderVoiceIndex(void)
{

	CHAR16 sString[ 32 ];
	INT16 sX, sY;

	// render the voice index value on the the blank portrait
	swprintf(sString, lengthof(sString), L"%ls %d", pIMPVoicesStrings, iCurrentVoices + 1);

	FindFontCenterCoordinates( 290 + LAPTOP_UL_X, 0, 100, 0, sString, FONT12ARIAL, &sX, &sY );

	SetFont( FONT12ARIAL );
	SetFontForeground( FONT_WHITE );
	SetFontBackground( FONT_BLACK );

	mprintf( sX, 320, sString );
}
