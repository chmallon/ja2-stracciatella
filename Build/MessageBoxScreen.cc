#include "Font.h"
#include "Input.h"
#include "Interface.h"
#include "Local.h"
#include "Timer_Control.h"
#include "Fade_Screen.h"
#include "SysUtil.h"
#include "MercTextBox.h"
#include "VSurface.h"
#include "Cursors.h"
#include "MessageBoxScreen.h"
#include "Font_Control.h"
#include "Game_Clock.h"
#include "Map_Screen_Interface.h"
#include "RenderWorld.h"
#include "GameLoop.h"
#include "English.h"
#include "GameSettings.h"
#include "Interface_Control.h"
#include "Cursor_Control.h"
#include "Laptop.h"
#include "Text.h"
#include "MapScreen.h"
#include "Overhead_Map.h"
#include "Button_System.h"
#include "JAScreens.h"
#include "Video.h"

#ifdef JA2BETAVERSION
#	include "Debug.h"
#endif


#define MSGBOX_DEFAULT_WIDTH      300

#define MSGBOX_BUTTON_WIDTH        61
#define MSGBOX_BUTTON_HEIGHT       20
#define MSGBOX_BUTTON_X_SEP        15

#define MSGBOX_SMALL_BUTTON_WIDTH  31
#define MSGBOX_SMALL_BUTTON_X_SEP   8

// old mouse x and y positions
static SGPPoint pOldMousePosition;
static SGPRect  MessageBoxRestrictedCursorRegion;

// if the cursor was locked to a region
static BOOLEAN fCursorLockedToArea = FALSE;
BOOLEAN        gfInMsgBox = FALSE;


static SGPRect gOldCursorLimitRectangle;


MESSAGE_BOX_STRUCT gMsgBox;
static BOOLEAN     gfNewMessageBox = FALSE;
static BOOLEAN     gfStartedFromGameScreen = FALSE;
BOOLEAN            gfStartedFromMapScreen = FALSE;
BOOLEAN            fRestoreBackgroundForMessageBox = FALSE;
BOOLEAN            gfDontOverRideSaveBuffer = TRUE;	//this variable can be unset if ur in a non gamescreen and DONT want the msg box to use the save buffer

wchar_t gzUserDefinedButton1[128];
wchar_t gzUserDefinedButton2[128];


static void ContractMsgBoxCallback(GUI_BUTTON* btn, INT32 reason);
static void LieMsgBoxCallback(GUI_BUTTON* btn, INT32 reason);
static void MsgBoxClickCallback(MOUSE_REGION* pRegion, INT32 iReason);
static void NOMsgBoxCallback(GUI_BUTTON* btn, INT32 reason);
static void NumberedMsgBoxCallback(GUI_BUTTON* btn, INT32 reason);
static void OKMsgBoxCallback(GUI_BUTTON* btn, INT32 reason);
static void YESMsgBoxCallback(GUI_BUTTON* btn, INT32 reason);


static GUIButtonRef MakeButton(const wchar_t* text, INT16 fore_colour, INT16 shadow_colour, INT16 x, INT16 y, GUI_CALLBACK click, UINT16 cursor)
{
	GUIButtonRef const btn = CreateIconAndTextButton(gMsgBox.iButtonImages, text, FONT12ARIAL, fore_colour, shadow_colour, fore_colour, shadow_colour, x, y, MSYS_PRIORITY_HIGHEST, click);
	btn->SetCursor(cursor);
	ForceButtonUnDirty(btn);
	return btn;
}


struct MessageBoxStyle
{
	MercPopUpBackground background;
	MercPopUpBorder     border;
	char const*         btn_image;
	INT32               btn_off;
	INT32               btn_on;
	UINT8               font_colour;
	UINT8               shadow_colour;
	UINT16              cursor;
};


static MessageBoxStyle const g_msg_box_style[] =
{
	{ DIALOG_MERC_POPUP_BACKGROUND, DIALOG_MERC_POPUP_BORDER, "INTERFACE/popupbuttons.sti",      0, 1, FONT_MCOLOR_WHITE, DEFAULT_SHADOW,    CURSOR_NORMAL        }, // MSG_BOX_BASIC_STYLE
	{ WHITE_MERC_POPUP_BACKGROUND,  RED_MERC_POPUP_BORDER,    "INTERFACE/msgboxRedButtons.sti",  0, 1, 2,                 NO_SHADOW,         CURSOR_LAPTOP_SCREEN }, // MSG_BOX_RED_ON_WHITE
	{ GREY_MERC_POPUP_BACKGROUND,   BLUE_MERC_POPUP_BORDER,   "INTERFACE/msgboxGreyButtons.sti", 0, 1, 2,                 FONT_MCOLOR_WHITE, CURSOR_LAPTOP_SCREEN }, // MSG_BOX_BLUE_ON_GREY
	{ DIALOG_MERC_POPUP_BACKGROUND, DIALOG_MERC_POPUP_BORDER, "INTERFACE/popupbuttons.sti",      2, 3, FONT_MCOLOR_WHITE, DEFAULT_SHADOW,    CURSOR_NORMAL        }, // MSG_BOX_BASIC_SMALL_BUTTONS
	{ IMP_POPUP_BACKGROUND,         DIALOG_MERC_POPUP_BORDER, "INTERFACE/msgboxGreyButtons.sti", 0, 1, 2,                 FONT_MCOLOR_WHITE, CURSOR_LAPTOP_SCREEN }, // MSG_BOX_IMP_STYLE
	{ LAPTOP_POPUP_BACKGROUND,      LAPTOP_POP_BORDER,        "INTERFACE/popupbuttons.sti",      0, 1, FONT_MCOLOR_WHITE, DEFAULT_SHADOW,    CURSOR_LAPTOP_SCREEN }  // MSG_BOX_LAPTOP_DEFAULT
};


static MessageBoxStyle const g_msg_box_style_default = { BASIC_MERC_POPUP_BACKGROUND, BASIC_MERC_POPUP_BORDER, "INTERFACE/msgboxbuttons.sti", 0, 1, FONT_MCOLOR_WHITE, DEFAULT_SHADOW, CURSOR_NORMAL };


void DoMessageBox(MessageBoxStyleID const ubStyle, wchar_t const* const zString, ScreenID const uiExitScreen, MessageBoxFlags const usFlags, MSGBOX_CALLBACK const ReturnCallback, SGPRect const* const pCenteringRect)
{
	GetMousePos(&pOldMousePosition);

	//this variable can be unset if ur in a non gamescreen and DONT want the msg box to use the save buffer
	gfDontOverRideSaveBuffer = TRUE;

	SetCurrentCursorFromDatabase(CURSOR_NORMAL);

	if (gMsgBox.BackRegion.uiFlags & MSYS_REGION_EXISTS) return;

	MessageBoxStyle const& style = ubStyle < lengthof(g_msg_box_style) ?
		g_msg_box_style[ubStyle] : g_msg_box_style_default;

	SGPRect	aRect;
	if (pCenteringRect != NULL)
	{
		aRect = *pCenteringRect;
	}
	else
	{
		// Use default!
		aRect.iLeft   = 0;
		aRect.iTop    = 0;
		aRect.iRight  = SCREEN_WIDTH;
		aRect.iBottom = SCREEN_HEIGHT;
	}

	// Set some values!
	gMsgBox.usFlags      = usFlags;
	gMsgBox.uiExitScreen = uiExitScreen;
	gMsgBox.ExitCallback = ReturnCallback;
	gMsgBox.fRenderBox   = TRUE;
	gMsgBox.bHandled     = MSG_BOX_RETURN_NONE;

	// Init message box
	UINT16 usTextBoxWidth;
	UINT16 usTextBoxHeight;
	gMsgBox.iBoxId = PrepareMercPopupBox(-1, style.background, style.border, zString, MSGBOX_DEFAULT_WIDTH, 40, 10, 30, &usTextBoxWidth, &usTextBoxHeight);

	if (gMsgBox.iBoxId == -1)
	{
#ifdef JA2BETAVERSION
		AssertMsg(0, "Failed in DoMessageBox().  Probable reason is because the string was too large to fit in max message box size.");
#endif
		return;
	}

	// Save height,width
	gMsgBox.usWidth  = usTextBoxWidth;
	gMsgBox.usHeight = usTextBoxHeight;

	// Determine position ( centered in rect )
	gMsgBox.sX = ((aRect.iRight  - aRect.iLeft) - usTextBoxWidth)  / 2 + aRect.iLeft;
	gMsgBox.sY = ((aRect.iBottom - aRect.iTop)  - usTextBoxHeight) / 2 + aRect.iTop;

	if (guiCurrentScreen == GAME_SCREEN)
	{
		gfStartedFromGameScreen = TRUE;
	}

	if (fInMapMode)
	{
		gfStartedFromMapScreen = TRUE;
		fMapPanelDirty         = TRUE;
	}


	// Set pending screen
	SetPendingNewScreen(MSG_BOX_SCREEN);

	// Init save buffer
	gMsgBox.uiSaveBuffer = AddVideoSurface(usTextBoxWidth, usTextBoxHeight, PIXEL_DEPTH);

  //Save what we have under here...
	const SGPRect r = { gMsgBox.sX, gMsgBox.sY, gMsgBox.sX + usTextBoxWidth, gMsgBox.sY + usTextBoxHeight };
	BltVideoSurface(gMsgBox.uiSaveBuffer, FRAME_BUFFER, 0, 0, &r);

	UINT16 const cursor = style.cursor;
	// Create top-level mouse region
	MSYS_DefineRegion(&gMsgBox.BackRegion, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, MSYS_PRIORITY_HIGHEST, cursor, MSYS_NO_CALLBACK, MsgBoxClickCallback);

	if (!gGameSettings.fOptions[TOPTION_DONT_MOVE_MOUSE])
	{
		UINT32 x = gMsgBox.sX + usTextBoxWidth / 2;
		UINT32 y = gMsgBox.sY + usTextBoxHeight - 4;
		if (usFlags == MSG_BOX_FLAG_OK)
		{
			x += 27;
			y -=  6;
		}
		SimulateMouseMovement(x, y);
	}

	// findout if cursor locked, if so, store old params and store, restore when done
	if (IsCursorRestricted())
	{
		fCursorLockedToArea = TRUE;
		GetRestrictedClipCursor(&MessageBoxRestrictedCursorRegion);
		FreeMouseCursor();
	}

	INT16       x = gMsgBox.sX;
	const INT16 y = gMsgBox.sY + usTextBoxHeight - MSGBOX_BUTTON_HEIGHT - 10;

	gMsgBox.iButtonImages = LoadButtonImage(style.btn_image, -1, style.btn_off, -1, style.btn_on, -1);

	INT16 const dx            = MSGBOX_BUTTON_WIDTH + MSGBOX_BUTTON_X_SEP;
	UINT8 const font_colour   = style.font_colour;
	UINT8 const shadow_colour = style.shadow_colour;
	switch (usFlags)
	{
		case MSG_BOX_FLAG_FOUR_NUMBERED_BUTTONS:
		{
			// This is exclusive of any other buttons... no ok, no cancel, no nothing
			const INT16 dx = MSGBOX_SMALL_BUTTON_WIDTH + MSGBOX_SMALL_BUTTON_X_SEP;
			x += (usTextBoxWidth - (MSGBOX_SMALL_BUTTON_WIDTH + dx * 3)) / 2;

			for (UINT i = 0; i < 4; ++i)
			{
				wchar_t text[] = { '1' + i, '\0' };
				GUIButtonRef const btn = MakeButton(text, font_colour, shadow_colour, x + dx * i, y, NumberedMsgBoxCallback, cursor);
				gMsgBox.uiButton[i] = btn;
				MSYS_SetBtnUserData(btn, i + 1);
			}
			break;
		}

		case MSG_BOX_FLAG_OK:
			x += (usTextBoxWidth - GetDimensionsOfButtonPic(gMsgBox.iButtonImages)->w) / 2;
			gMsgBox.uiOKButton = MakeButton(pMessageStrings[MSG_OK], font_colour, shadow_colour, x, y, OKMsgBoxCallback, cursor);
			break;

		case MSG_BOX_FLAG_YESNO:
			x += (usTextBoxWidth - (MSGBOX_BUTTON_WIDTH + dx)) / 2;
			gMsgBox.uiYESButton = MakeButton(pMessageStrings[MSG_YES], font_colour, shadow_colour, x,      y, YESMsgBoxCallback, cursor);
			gMsgBox.uiNOButton  = MakeButton(pMessageStrings[MSG_NO],  font_colour, shadow_colour, x + dx, y, NOMsgBoxCallback,  cursor);
			break;

		case MSG_BOX_FLAG_CONTINUESTOP:
			x += (usTextBoxWidth - (MSGBOX_BUTTON_WIDTH + dx)) / 2;
			gMsgBox.uiYESButton = MakeButton(pUpdatePanelButtons[0], font_colour, shadow_colour, x,      y, YESMsgBoxCallback, cursor);
			gMsgBox.uiNOButton  = MakeButton(pUpdatePanelButtons[1], font_colour, shadow_colour, x + dx, y, NOMsgBoxCallback,  cursor);
			break;

		case MSG_BOX_FLAG_OKCONTRACT:
			x += (usTextBoxWidth - (MSGBOX_BUTTON_WIDTH + dx)) / 2;
			gMsgBox.uiYESButton = MakeButton(pMessageStrings[MSG_OK],     font_colour, shadow_colour, x,      y, YESMsgBoxCallback,      cursor);
			gMsgBox.uiNOButton  = MakeButton(pMessageStrings[MSG_REHIRE], font_colour, shadow_colour, x + dx, y, ContractMsgBoxCallback, cursor);
			break;

		case MSG_BOX_FLAG_GENERICCONTRACT:
			x += (usTextBoxWidth - (MSGBOX_BUTTON_WIDTH + dx * 2)) / 2;
			gMsgBox.uiYESButton = MakeButton(gzUserDefinedButton1,        font_colour, shadow_colour, x,          y, YESMsgBoxCallback,      cursor);
			gMsgBox.uiNOButton  = MakeButton(gzUserDefinedButton2,        font_colour, shadow_colour, x + dx,     y, NOMsgBoxCallback,       cursor);
			gMsgBox.uiOKButton  = MakeButton(pMessageStrings[MSG_REHIRE], font_colour, shadow_colour, x + dx * 2, y, ContractMsgBoxCallback, cursor);
			break;

		case MSG_BOX_FLAG_GENERIC:
			x += (usTextBoxWidth - (MSGBOX_BUTTON_WIDTH + dx)) / 2;
			gMsgBox.uiYESButton = MakeButton(gzUserDefinedButton1, font_colour, shadow_colour, x,      y, YESMsgBoxCallback, cursor);
			gMsgBox.uiNOButton  = MakeButton(gzUserDefinedButton2, font_colour, shadow_colour, x + dx, y, NOMsgBoxCallback,  cursor);
			break;

		case MSG_BOX_FLAG_YESNOLIE:
			x += (usTextBoxWidth - (MSGBOX_BUTTON_WIDTH + dx * 2)) / 2;
			gMsgBox.uiYESButton = MakeButton(pMessageStrings[MSG_YES], font_colour, shadow_colour, x,          y, YESMsgBoxCallback, cursor);
			gMsgBox.uiNOButton  = MakeButton(pMessageStrings[MSG_NO],  font_colour, shadow_colour, x + dx,     y, NOMsgBoxCallback,  cursor);
			gMsgBox.uiOKButton  = MakeButton(pMessageStrings[MSG_LIE], font_colour, shadow_colour, x + dx * 2, y, LieMsgBoxCallback, cursor);
			break;

		case MSG_BOX_FLAG_OKSKIP:
			x += (usTextBoxWidth - (MSGBOX_BUTTON_WIDTH + dx)) / 2;
			gMsgBox.uiYESButton = MakeButton(pMessageStrings[MSG_OK],   font_colour, shadow_colour, x,      y, YESMsgBoxCallback, cursor);
			gMsgBox.uiNOButton  = MakeButton(pMessageStrings[MSG_SKIP], font_colour, shadow_colour, x + dx, y, NOMsgBoxCallback,  cursor);
			break;
	}

	InterruptTime();
	PauseGame();
	LockPauseState(1);
	// Pause timers as well....
	PauseTime(TRUE);

  // Save mouse restriction region...
  GetRestrictedClipCursor(&gOldCursorLimitRectangle);
  FreeMouseCursor();

	gfNewMessageBox = TRUE;
	gfInMsgBox     = TRUE;
}


static void MsgBoxClickCallback(MOUSE_REGION* pRegion, INT32 iReason)
{
}


static void OKMsgBoxCallback(GUI_BUTTON* btn, INT32 reason)
{
	if (reason & MSYS_CALLBACK_REASON_LBUTTON_UP)
	{
		gMsgBox.bHandled = MSG_BOX_RETURN_OK;
	}
}


static void YESMsgBoxCallback(GUI_BUTTON* btn, INT32 reason)
{
	if (reason & MSYS_CALLBACK_REASON_LBUTTON_UP)
	{
		gMsgBox.bHandled = MSG_BOX_RETURN_YES;
	}
}


static void NOMsgBoxCallback(GUI_BUTTON* btn, INT32 reason)
{
	if (reason & MSYS_CALLBACK_REASON_LBUTTON_UP)
	{
		gMsgBox.bHandled = MSG_BOX_RETURN_NO;
	}
}


static void ContractMsgBoxCallback(GUI_BUTTON* btn, INT32 reason)
{
	if (reason & MSYS_CALLBACK_REASON_LBUTTON_UP)
	{
		gMsgBox.bHandled = MSG_BOX_RETURN_CONTRACT;
	}
}


static void LieMsgBoxCallback(GUI_BUTTON* btn, INT32 reason)
{
	if (reason & MSYS_CALLBACK_REASON_LBUTTON_UP)
	{
		gMsgBox.bHandled = MSG_BOX_RETURN_LIE;
	}
}


static void NumberedMsgBoxCallback(GUI_BUTTON* btn, INT32 reason)
{
	if (reason & MSYS_CALLBACK_REASON_LBUTTON_UP)
	{
		gMsgBox.bHandled = static_cast<MessageBoxReturnValue>(MSYS_GetBtnUserData(btn));
	}
}


static ScreenID ExitMsgBox(MessageBoxReturnValue const ubExitCode)
{
	// Delete popup!
	RemoveMercPopupBoxFromIndex(gMsgBox.iBoxId);
	gMsgBox.iBoxId = -1;

	//Delete buttons!
	switch (gMsgBox.usFlags)
	{
		case MSG_BOX_FLAG_FOUR_NUMBERED_BUTTONS:
			RemoveButton(gMsgBox.uiButton[0]);
			RemoveButton(gMsgBox.uiButton[1]);
			RemoveButton(gMsgBox.uiButton[2]);
			RemoveButton(gMsgBox.uiButton[3]);
			break;

		case MSG_BOX_FLAG_OK:
			RemoveButton(gMsgBox.uiOKButton);
			break;

		case MSG_BOX_FLAG_YESNO:
		case MSG_BOX_FLAG_OKCONTRACT:
		case MSG_BOX_FLAG_GENERIC:
		case MSG_BOX_FLAG_CONTINUESTOP:
		case MSG_BOX_FLAG_OKSKIP:
			RemoveButton(gMsgBox.uiYESButton);
			RemoveButton(gMsgBox.uiNOButton);
			break;

		case MSG_BOX_FLAG_GENERICCONTRACT:
		case MSG_BOX_FLAG_YESNOLIE:
			RemoveButton(gMsgBox.uiYESButton);
			RemoveButton(gMsgBox.uiNOButton);
			RemoveButton(gMsgBox.uiOKButton);
			break;
	}

	// Delete button images
	UnloadButtonImage(gMsgBox.iButtonImages);

	// Unpause game....
	UnLockPauseState();
	UnPauseGame();
	// UnPause timers as well....
	PauseTime(FALSE);

  // Restore mouse restriction region...
  RestrictMouseCursor(&gOldCursorLimitRectangle);

	gfInMsgBox = FALSE;

	// Call done callback!
	if (gMsgBox.ExitCallback != NULL) gMsgBox.ExitCallback(ubExitCode);

	//if you are in a non gamescreen and DONT want the msg box to use the save buffer, unset gfDontOverRideSaveBuffer in your callback
	if ((gMsgBox.uiExitScreen != GAME_SCREEN || fRestoreBackgroundForMessageBox) && gfDontOverRideSaveBuffer)
	{
		// restore what we have under here...
		BltVideoSurface(FRAME_BUFFER, gMsgBox.uiSaveBuffer, gMsgBox.sX, gMsgBox.sY, NULL);
		InvalidateRegion(gMsgBox.sX, gMsgBox.sY, gMsgBox.sX + gMsgBox.usWidth, gMsgBox.sY + gMsgBox.usHeight);
	}

	fRestoreBackgroundForMessageBox = FALSE;
	gfDontOverRideSaveBuffer        = TRUE;

	if (fCursorLockedToArea)
	{
		SGPPoint pPosition;
		GetMousePos(&pPosition);

		if (pPosition.iX > MessageBoxRestrictedCursorRegion.iRight ||
				pPosition.iX > MessageBoxRestrictedCursorRegion.iLeft && pPosition.iY < MessageBoxRestrictedCursorRegion.iTop && pPosition.iY > MessageBoxRestrictedCursorRegion.iBottom)
		{
			SimulateMouseMovement(pOldMousePosition.iX, pOldMousePosition.iY);
		}

		fCursorLockedToArea = FALSE;
		RestrictMouseCursor(&MessageBoxRestrictedCursorRegion);
	}

	MSYS_RemoveRegion(&gMsgBox.BackRegion);
	DeleteVideoSurface(gMsgBox.uiSaveBuffer);

	switch (gMsgBox.uiExitScreen)
	{
		case GAME_SCREEN:
			if (InOverheadMap())
			{
				gfOverheadMapDirty = TRUE;
			}
			else
			{
				SetRenderFlags(RENDER_FLAG_FULL);
			}
			break;

		case MAP_SCREEN:
			fMapPanelDirty = TRUE;
			break;
	}

	if (gfFadeInitialized)
	{
		SetPendingNewScreen(FADE_SCREEN);
		return FADE_SCREEN;
	}

	return gMsgBox.uiExitScreen;
}


ScreenID MessageBoxScreenHandle(void)
{
	if (gfNewMessageBox)
	{
		// If in game screen....
		if (gfStartedFromGameScreen || gfStartedFromMapScreen)
		{
			if (gfStartedFromGameScreen)
			{
				HandleTacticalUILoseCursorFromOtherScreen();
			}
			else
			{
				HandleMAPUILoseCursorFromOtherScreen();
			}

			gfStartedFromGameScreen = FALSE;
			gfStartedFromMapScreen  = FALSE;
		}

		gfNewMessageBox = FALSE;

		return MSG_BOX_SCREEN;
	}

	UnmarkButtonsDirty();

	// Render the box!
	if (gMsgBox.fRenderBox)
	{
		switch (gMsgBox.usFlags)
		{
			case MSG_BOX_FLAG_FOUR_NUMBERED_BUTTONS:
				MarkAButtonDirty(gMsgBox.uiButton[0]);
				MarkAButtonDirty(gMsgBox.uiButton[1]);
				MarkAButtonDirty(gMsgBox.uiButton[2]);
				MarkAButtonDirty(gMsgBox.uiButton[3]);
				break;

			case MSG_BOX_FLAG_OK:
				MarkAButtonDirty(gMsgBox.uiOKButton);
				break;

			case MSG_BOX_FLAG_YESNO:
			case MSG_BOX_FLAG_OKCONTRACT:
			case MSG_BOX_FLAG_GENERIC:
			case MSG_BOX_FLAG_CONTINUESTOP:
			case MSG_BOX_FLAG_OKSKIP:
				MarkAButtonDirty(gMsgBox.uiYESButton);
				MarkAButtonDirty(gMsgBox.uiNOButton);
				break;

			case MSG_BOX_FLAG_GENERICCONTRACT:
			case MSG_BOX_FLAG_YESNOLIE:
				MarkAButtonDirty(gMsgBox.uiYESButton);
				MarkAButtonDirty(gMsgBox.uiNOButton);
				MarkAButtonDirty(gMsgBox.uiOKButton);
				break;
		}

		RenderMercPopUpBoxFromIndex(gMsgBox.iBoxId, gMsgBox.sX, gMsgBox.sY, FRAME_BUFFER);
		//gMsgBox.fRenderBox = FALSE;
		// ATE: Render each frame...
	}

	RenderButtons();
	EndFrameBufferRender();

	// carter, need key shortcuts for clearing up message boxes
	// Check for esc
	InputAtom InputEvent;
	while (DequeueEvent(&InputEvent))
	{
		if (InputEvent.usEvent != KEY_UP) continue;

		switch (gMsgBox.usFlags)
		{
			case MSG_BOX_FLAG_YESNO:
				switch (InputEvent.usParam)
				{
					case 'n':
					case SDLK_ESCAPE: gMsgBox.bHandled = MSG_BOX_RETURN_NO;  break;
					case 'y':
					case SDLK_RETURN: gMsgBox.bHandled = MSG_BOX_RETURN_YES; break;
				}
				break;

			case MSG_BOX_FLAG_OK:
				switch (InputEvent.usParam)
				{
					case 'o':
					case SDLK_RETURN: gMsgBox.bHandled = MSG_BOX_RETURN_OK; break;
				}
				break;

			case MSG_BOX_FLAG_CONTINUESTOP:
				switch (InputEvent.usParam)
				{
					case SDLK_RETURN: gMsgBox.bHandled = MSG_BOX_RETURN_OK; break;
				}
				break;

			case MSG_BOX_FLAG_FOUR_NUMBERED_BUTTONS:
				switch (InputEvent.usParam)
				{
					case '1': gMsgBox.bHandled = MSG_BOX_RETURN_1; break;
					case '2': gMsgBox.bHandled = MSG_BOX_RETURN_2; break;
					case '3': gMsgBox.bHandled = MSG_BOX_RETURN_3; break;
					case '4': gMsgBox.bHandled = MSG_BOX_RETURN_4; break;
				}
				break;
		}
	}

  if (gMsgBox.bHandled != MSG_BOX_RETURN_NONE)
	{
		SetRenderFlags(RENDER_FLAG_FULL);
		return ExitMsgBox(gMsgBox.bHandled);
	}

	return MSG_BOX_SCREEN;
}


static void DoScreenIndependantMessageBoxWithRect(wchar_t const* zString, MessageBoxFlags, MSGBOX_CALLBACK ReturnCallback, SGPRect const* pCenteringRect);


// a basic box that don't care what screen we came from
void DoScreenIndependantMessageBox(wchar_t const* const zString, MessageBoxFlags const usFlags, MSGBOX_CALLBACK const ReturnCallback)
{
	const SGPRect CenteringRect = {0, 0, SCREEN_WIDTH, INV_INTERFACE_START_Y };
	DoScreenIndependantMessageBoxWithRect(zString, usFlags, ReturnCallback, &CenteringRect);
}


// a basic box that don't care what screen we came from
void DoLowerScreenIndependantMessageBox(wchar_t const* const zString, MessageBoxFlags const usFlags, MSGBOX_CALLBACK const ReturnCallback)
{
	const SGPRect CenteringRect = {0, INV_INTERFACE_START_Y / 2, SCREEN_WIDTH, INV_INTERFACE_START_Y };
	DoScreenIndependantMessageBoxWithRect(zString, usFlags, ReturnCallback, &CenteringRect);
}


static void DoScreenIndependantMessageBoxWithRect(wchar_t const* const zString, MessageBoxFlags const usFlags, MSGBOX_CALLBACK const ReturnCallback, SGPRect const* const pCenteringRect)
{
	/// which screen are we in?

	// Map Screen (excluding AI Viewer)
#ifdef JA2BETAVERSION
	if (guiTacticalInterfaceFlags & INTERFACE_MAPSCREEN && guiCurrentScreen != AIVIEWER_SCREEN)
#else
	if (guiTacticalInterfaceFlags & INTERFACE_MAPSCREEN)
#endif
	{
		// auto resolve is a special case
		if (guiCurrentScreen == AUTORESOLVE_SCREEN)
		{
			DoMessageBox(MSG_BOX_BASIC_STYLE, zString, AUTORESOLVE_SCREEN, usFlags, ReturnCallback, pCenteringRect);
		}
		else
		{
			// set up for mapscreen
			DoMapMessageBoxWithRect(MSG_BOX_BASIC_STYLE, zString, MAP_SCREEN, usFlags, ReturnCallback, pCenteringRect);
		}
	}
	else
	{
		switch (guiCurrentScreen)
		{
			case LAPTOP_SCREEN:    DoLapTopSystemMessageBoxWithRect(MSG_BOX_LAPTOP_DEFAULT, zString, LAPTOP_SCREEN,    usFlags, ReturnCallback, pCenteringRect); break;
			case SAVE_LOAD_SCREEN: DoSaveLoadMessageBoxWithRect(                            zString, SAVE_LOAD_SCREEN, usFlags, ReturnCallback, pCenteringRect); break;
			case OPTIONS_SCREEN:   DoOptionsMessageBoxWithRect(                             zString, OPTIONS_SCREEN,   usFlags, ReturnCallback, pCenteringRect); break;
			case GAME_SCREEN:      DoMessageBox(                    MSG_BOX_BASIC_STYLE,    zString, guiCurrentScreen, usFlags, ReturnCallback, pCenteringRect); break;
		}
	}
}
