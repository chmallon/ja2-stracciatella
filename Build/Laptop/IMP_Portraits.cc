#include "CharProfile.h"
#include "Font.h"
#include "IMP_Portraits.h"
#include "IMP_MainPage.h"
#include "IMPVideoObjects.h"
#include "Text.h"
#include "VObject.h"
#include "Input.h"
#include "Debug.h"
#include "Render_Dirty.h"
#include "Cursors.h"
#include "Laptop.h"
#include "IMP_Text_System.h"
#include "IMP_Compile_Character.h"
#include "Button_System.h"
#include "VSurface.h"
#include "Font_Control.h"


//current and last pages
INT32 iCurrentPortrait = 0;
INT32 iLastPicture = 7;

// buttons needed for the IMP portrait screen
GUIButtonRef giIMPPortraitButton[3];
static BUTTON_PICS* giIMPPortraitButtonImage[3];

// redraw protrait screen
BOOLEAN fReDrawPortraitScreenFlag = FALSE;

// face index
INT32 iPortraitNumber = 0;


static void CreateIMPPortraitButtons(void);


void EnterIMPPortraits( void )
{

		// create buttons
	CreateIMPPortraitButtons( );

	// render background
	RenderIMPPortraits( );
}


static BOOLEAN RenderPortrait(INT16 sX, INT16 sY);


void RenderIMPPortraits( void )
{


  // render background
	RenderProfileBackGround( );

		// the Voices frame
	RenderPortraitFrame( 191, 167 );

	// render the current portrait
	RenderPortrait( 200, 176 );

	// indent for the text
	RenderAttrib1IndentFrame( 128, 65);

	// text
	PrintImpText( );
}


static void DestroyIMPPortraitButtons(void);


void ExitIMPPortraits( void )
{
	// destroy buttons for IMP portrait page
  DestroyIMPPortraitButtons( );
}

void HandleIMPPortraits( void )
{
	// do we need to re write screen
	if (fReDrawPortraitScreenFlag)
	{
    RenderIMPPortraits( );

		// reset redraw flag
		fReDrawPortraitScreenFlag = FALSE;
	}
}


static BOOLEAN RenderPortrait(INT16 sX, INT16 sY)
try
{
  // render the portrait of the current picture
	const INT32 portrait = iCurrentPortrait + (fCharacterIsMale ? 0 : 8);
	BltVideoObjectOnce(FRAME_BUFFER, pPlayerSelectedBigFaceFileNames[portrait], 0, LAPTOP_SCREEN_UL_X + sX, LAPTOP_SCREEN_WEB_UL_Y + sY);
	return TRUE;
}
catch (...) { return FALSE; }


static void IncrementPictureIndex(void)
{
	// cycle to next picture

	iCurrentPortrait++;

	// gone too far?
	if( iCurrentPortrait > iLastPicture )
	{
		iCurrentPortrait = 0;
	}
}


static void DecrementPicture(void)
{
  // cycle to previous picture

	iCurrentPortrait--;

	// gone too far?
  if( iCurrentPortrait < 0 )
	{
    iCurrentPortrait = iLastPicture;
	}
}


static void MakeButton(UINT idx, const char* img_file, INT32 off_normal, INT32 on_normal, const wchar_t* text, INT16 x, INT16 y, GUI_CALLBACK click)
{
	BUTTON_PICS* const img = LoadButtonImage(img_file, -1, off_normal, -1, on_normal, -1);
	giIMPPortraitButtonImage[idx] = img;
	const INT16 text_col   = FONT_WHITE;
	const INT16 shadow_col = DEFAULT_SHADOW;
	GUIButtonRef const btn = CreateIconAndTextButton(img, text, FONT12ARIAL, text_col, shadow_col, text_col, shadow_col, x, y, MSYS_PRIORITY_HIGH, click);
	giIMPPortraitButton[idx] = btn;
	btn->SetCursor(CURSOR_WWW);
}


static void BtnIMPPortraitDoneCallback(GUI_BUTTON* btn, INT32 reason);
static void BtnIMPPortraitNextCallback(GUI_BUTTON* btn, INT32 reason);
static void BtnIMPPortraitPreviousCallback(GUI_BUTTON* btn, INT32 reason);


static void CreateIMPPortraitButtons(void)
{
  // will create buttons need for the IMP portrait screen
  const INT16 dx = LAPTOP_SCREEN_UL_X;
  const INT16 dy = LAPTOP_SCREEN_WEB_UL_Y;
	MakeButton(0, "LAPTOP/voicearrows.sti", 1, 3, pImpButtonText[13], dx + 343, dy + 205, BtnIMPPortraitNextCallback);     // Next button
	MakeButton(1, "LAPTOP/voicearrows.sti", 0, 2, pImpButtonText[12], dx +  93, dy + 205, BtnIMPPortraitPreviousCallback); // Previous button
	MakeButton(2, "LAPTOP/button_5.sti",    0, 1, pImpButtonText[11], dx + 187, dy + 330, BtnIMPPortraitDoneCallback);     // Done button
}


static void DestroyIMPPortraitButtons(void)
{

	// will destroy buttons created for IMP Portrait screen

	// the next button
  RemoveButton(giIMPPortraitButton[ 0 ] );
  UnloadButtonImage(giIMPPortraitButtonImage[ 0 ] );

	// the previous button
  RemoveButton(giIMPPortraitButton[ 1 ] );
  UnloadButtonImage(giIMPPortraitButtonImage[ 1 ] );

	// the done button
  RemoveButton(giIMPPortraitButton[ 2 ] );
  UnloadButtonImage(giIMPPortraitButtonImage[ 2 ] );
}


static void BtnIMPPortraitNextCallback(GUI_BUTTON *btn, INT32 reason)
{
	if (reason & MSYS_CALLBACK_REASON_LBUTTON_UP)
	{
		IncrementPictureIndex();
		fReDrawPortraitScreenFlag = TRUE;
	}
}


static void BtnIMPPortraitPreviousCallback(GUI_BUTTON *btn, INT32 reason)
{
	if (reason & MSYS_CALLBACK_REASON_LBUTTON_UP)
	{
		DecrementPicture();
		fReDrawPortraitScreenFlag = TRUE;
	}
}


static void BtnIMPPortraitDoneCallback(GUI_BUTTON *btn, INT32 reason)
{
	if (reason & MSYS_CALLBACK_REASON_LBUTTON_UP)
	{
		iCurrentImpPage = IMP_MAIN_PAGE;

		// current mode now is voice
		if (iCurrentProfileMode < 4) iCurrentProfileMode = 4;

		// if we are already done, leave
		if (iCurrentProfileMode == 5) iCurrentImpPage = IMP_FINISH;

		// grab picture number
		iPortraitNumber = iCurrentPortrait + (fCharacterIsMale ? 0 : 8);

		fButtonPendingFlag = TRUE;
	}
}
