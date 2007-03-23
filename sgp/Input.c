#include "Types.h"
#include <stdio.h>
#include <memory.h>
#include "Debug.h"
#include "Input.h"
#include "MemMan.h"
#include "English.h"
#include "Video.h"
#include "Local.h"


// The gfKeyState table is used to track which of the keys is up or down at any one time. This is used while polling
// the interface.

BOOLEAN gfKeyState[SDLK_LAST]; // TRUE = Pressed, FALSE = Not Pressed
static BOOLEAN fCursorWasClipped = FALSE;
static SGPRect gCursorClipRect;


// These data structure are used to track the mouse while polling

static UINT32 guiSingleClickTimer;

static UINT32 guiLeftButtonRepeatTimer;
static UINT32 guiRightButtonRepeatTimer;

BOOLEAN gfLeftButtonState;  // TRUE = Pressed, FALSE = Not Pressed
BOOLEAN gfRightButtonState; // TRUE = Pressed, FALSE = Not Pressed
UINT16  gusMouseXPos;       // X position of the mouse on screen
UINT16  gusMouseYPos;       // y position of the mouse on screen

// The queue structures are used to track input events using queued events

static InputAtom gEventQueue[256];
static UINT16    gusQueueCount;
static UINT16    gusHeadIndex;
static UINT16    gusTailIndex;


BOOLEAN InitializeInputManager(void)
{
	// Link to debugger
	RegisterDebugTopic(TOPIC_INPUT, "Input Manager");
	memset(gfKeyState, FALSE, sizeof(gfKeyState));
	// Initialize the Event Queue
	gusQueueCount = 0;
	gusHeadIndex  = 0;
	gusTailIndex  = 0;

	// Initialize variables pertaining to DOUBLE CLIK stuff
	guiSingleClickTimer = 0;
	// Initialize variables pertaining to the button states
	gfLeftButtonState  = FALSE;
	gfRightButtonState = FALSE;
	// Initialize variables pertaining to the repeat mechanism
	guiLeftButtonRepeatTimer = 0;
	guiRightButtonRepeatTimer = 0;
	// Set the mouse to the center of the screen
	gusMouseXPos = 320;
	gusMouseYPos = 240;
	return TRUE;
}


void ShutdownInputManager(void)
{
	UnRegisterDebugTopic(TOPIC_INPUT, "Input Manager");
}


static void QueueMouseEvent(UINT16 ubInputEvent)
{
	// Can we queue up one more event, if not, the event is lost forever
	if (gusQueueCount == lengthof(gEventQueue)) return;

	gEventQueue[gusTailIndex].usEvent = ubInputEvent;

	gusQueueCount++;

	gusTailIndex = (gusTailIndex + 1) % lengthof(gEventQueue);
}


static void QueueKeyEvent(UINT16 ubInputEvent, SDLKey Key, SDLMod Mod, wchar_t Char)
{
	// Can we queue up one more event, if not, the event is lost forever
	if (gusQueueCount == lengthof(gEventQueue)) return;

	UINT16 ModifierState = 0;
	if (Mod & KMOD_SHIFT) ModifierState |= SHIFT_DOWN;
	if (Mod & KMOD_CTRL)  ModifierState |= CTRL_DOWN;
	if (Mod & KMOD_ALT)   ModifierState |= ALT_DOWN;
	gEventQueue[gusTailIndex].usKeyState = ModifierState;
	gEventQueue[gusTailIndex].usEvent = ubInputEvent;
	gEventQueue[gusTailIndex].usParam = Key;
	gEventQueue[gusTailIndex].Char    = Char;

	gusQueueCount++;

	gusTailIndex = (gusTailIndex + 1) % lengthof(gEventQueue);
}


BOOLEAN DequeueSpecificEvent(InputAtom* Event, UINT32 uiMaskFlags)
{
	// Is there an event to dequeue
	if (gusQueueCount > 0)
	{
		memcpy(Event, &gEventQueue[gusHeadIndex], sizeof(InputAtom));

		// Check if it has the masks!
		if (Event->usEvent & uiMaskFlags)
		{
			return DequeueEvent(Event);
		}
	}

	return FALSE;
}


static void HandleSingleClicksAndButtonRepeats(void);


BOOLEAN DequeueEvent(InputAtom *Event)
{
	HandleSingleClicksAndButtonRepeats();

	// Is there an event to dequeue
	if (gusQueueCount > 0)
	{
		// We have an event, so we dequeue it
		memcpy(Event, &gEventQueue[gusHeadIndex], sizeof(InputAtom));

		if (gusHeadIndex == 255)
		{
			gusHeadIndex = 0;
		}
		else
		{
			gusHeadIndex++;
		}

		// Decrement the number of items on the input queue
		gusQueueCount--;

		// dequeued an event, return TRUE
		return TRUE;
	}
	else
	{
		// No events to dequeue, return FALSE
		return FALSE;
	}
}


static void UpdateMousePos(const SDL_MouseButtonEvent* BtnEv)
{
	gusMouseXPos = BtnEv->x;
	gusMouseYPos = BtnEv->y;
}


void MouseButtonDown(const SDL_MouseButtonEvent* BtnEv)
{
	UpdateMousePos(BtnEv);
	switch (BtnEv->button)
	{
		case SDL_BUTTON_LEFT:
			guiLeftButtonRepeatTimer = GetTickCount() + BUTTON_REPEAT_TIMEOUT;
			gfLeftButtonState = TRUE;
			QueueMouseEvent(LEFT_BUTTON_DOWN);
			break;

		case SDL_BUTTON_RIGHT:
			guiRightButtonRepeatTimer = GetTickCount() + BUTTON_REPEAT_TIMEOUT;
			gfRightButtonState = TRUE;
			QueueMouseEvent(RIGHT_BUTTON_DOWN);
			break;
	}
}


void MouseButtonUp(const SDL_MouseButtonEvent* BtnEv)
{
	UpdateMousePos(BtnEv);
	switch (BtnEv->button)
	{
		case SDL_BUTTON_LEFT:
		{
			guiLeftButtonRepeatTimer = 0;
			gfLeftButtonState = FALSE;
			QueueMouseEvent(LEFT_BUTTON_UP);
			UINT32 uiTimer = GetTickCount();
			if (uiTimer - guiSingleClickTimer < DBL_CLK_TIME)
			{
				QueueMouseEvent(LEFT_BUTTON_DBL_CLK);
			}
			else
			{
				guiSingleClickTimer = uiTimer;
			}
			break;
		}

		case SDL_BUTTON_RIGHT:
			guiRightButtonRepeatTimer = 0;
			gfRightButtonState = FALSE;
			QueueMouseEvent(RIGHT_BUTTON_UP);
			break;
	}
}


static void KeyChange(const SDL_keysym* KeySym, BOOLEAN Pressed)
{
	UINT32 ubKey;
	UINT16 ubChar;

	SDLKey Key = KeySym->sym;
	if (Key >= SDLK_a && Key <= SDLK_z)
	{
		ubKey = KeySym->mod & KMOD_SHIFT ? Key - 32 : Key;
	}
	else if (Key >= SDLK_KP0 && Key <= SDLK_KP9)
	{
		if (KeySym->mod & KMOD_NUM)
		{
			ubKey = Key - SDLK_KP0 + SDLK_0;
		}
		else
		{
			switch (Key)
			{
				case SDLK_KP0: ubKey = SDLK_INSERT;   break;
				case SDLK_KP1: ubKey = SDLK_END;      break;
				case SDLK_KP2: ubKey = SDLK_DOWN;     break;
				case SDLK_KP3: ubKey = SDLK_PAGEDOWN; break;
				case SDLK_KP4: ubKey = SDLK_LEFT;     break;
				case SDLK_KP5: return;
				case SDLK_KP6: ubKey = SDLK_RIGHT;    break;
				case SDLK_KP7: ubKey = SDLK_HOME;     break;
				case SDLK_KP8: ubKey = SDLK_UP;       break;
				case SDLK_KP9: ubKey = SDLK_PAGEUP;   break;
			}
		}
	}
	else
	{
		switch (Key)
		{
			default:
				if (Key >= lengthof(gfKeyState)) return;
				ubKey = Key;
				break;
		}
	}
	ubChar = KeySym->unicode;

	if (Pressed)
	{ // Key has been PRESSED
		// Find out if the key is already pressed and if not, queue an event and update the gfKeyState array
		if (!gfKeyState[ubKey])
		{ // Well the key has just been pressed, therefore we queue up and event and update the gsKeyState
			gfKeyState[ubKey] = TRUE;
			QueueKeyEvent(KEY_DOWN, ubKey, KeySym->mod, ubChar);
		}
		else
		{ // Well the key gets repeated
			QueueKeyEvent(KEY_REPEAT, ubKey, KeySym->mod, ubChar);
		}
	}
	else
	{ // Key has been RELEASED
		// Find out if the key is already pressed and if so, queue an event and update the gfKeyState array
		if (gfKeyState[ubKey])
		{ // Well the key has just been pressed, therefore we queue up and event and update the gsKeyState
			gfKeyState[ubKey] = FALSE;
			QueueKeyEvent(KEY_UP, ubKey, KeySym->mod, ubChar);
		}
#if 0 // XXX TODO
		//else if the alt tab key was pressed
		else if (ubChar == SDLK_TAB && KeySym->mod & KMOD_ALT)
		{
			// therefore minimize the application
			ShowWindow(ghWindow, SW_MINIMIZE);
			gfKeyState[ALT] = FALSE;
		}
#endif
	}
}


void KeyDown(const SDL_keysym* KeySym)
{
	switch (KeySym->sym)
	{
		case SDLK_LSHIFT:
		case SDLK_RSHIFT:
			gfKeyState[SHIFT] = TRUE;
			break;

		case SDLK_LCTRL:
		case SDLK_RCTRL:
			gfKeyState[CTRL] = TRUE;
			break;

		case SDLK_LALT:
		case SDLK_RALT:
			gfKeyState[ALT] = TRUE;
			break;

		case SDLK_PRINT:
		case SDLK_SCROLLOCK:
			break;

		default:
			KeyChange(KeySym, TRUE);
			break;
	}
}


void KeyUp(const SDL_keysym* KeySym)
{
	switch (KeySym->sym)
	{
		case SDLK_LSHIFT:
		case SDLK_RSHIFT:
			gfKeyState[SHIFT] = FALSE;
			break;

		case SDLK_LCTRL:
		case SDLK_RCTRL:
			gfKeyState[CTRL] = FALSE;
			break;

		case SDLK_LALT:
		case SDLK_RALT:
			gfKeyState[ALT] = FALSE;
			break;

		case SDLK_PRINT:
			if (KeySym->mod & KMOD_CTRL) VideoCaptureToggle(); else PrintScreen();
			break;

		case SDLK_SCROLLOCK:
			SDL_WM_GrabInput
			(
				SDL_WM_GrabInput(SDL_GRAB_QUERY) == SDL_GRAB_OFF ?
					SDL_GRAB_ON : SDL_GRAB_OFF
			);
			break;

		default:
			KeyChange(KeySym, FALSE);
			break;
	}
}


void GetMousePos(SGPPoint* Point)
{
	POINT MousePos;
	GetCursorPos(&MousePos);
	Point->iX = MousePos.x;
	Point->iY = MousePos.y;
}


void RestrictMouseToXYXY(UINT16 usX1, UINT16 usY1, UINT16 usX2, UINT16 usY2)
{
	SGPRect TempRect;
	TempRect.iLeft   = usX1;
	TempRect.iTop    = usY1;
	TempRect.iRight  = usX2;
	TempRect.iBottom = usY2;
	RestrictMouseCursor(&TempRect);
}


void RestrictMouseCursor(SGPRect *pRectangle)
{
	// Make a copy of our rect....
	gCursorClipRect = *pRectangle;
#if 1 // XXX TODO0000 Should probably removed completly. Confining the mouse cursor is The Wrong Thing(tm)
#else
	ClipCursor((RECT*)pRectangle);
#endif
	fCursorWasClipped = TRUE;
}


void FreeMouseCursor(void)
{
#if 1 // XXX TODO0000
#else
	ClipCursor(NULL);
#endif
	fCursorWasClipped = FALSE;
}


void RestoreCursorClipRect(void)
{
#if 1 // XXX TODO0000
	UNIMPLEMENTED();
#else
	if (fCursorWasClipped)
	{
		ClipCursor(&gCursorClipRect);
	}
#endif
}


void GetRestrictedClipCursor(SGPRect* pRectangle)
{
#if 1 // XXX TODO0000
	pRectangle->iLeft   = 0;
	pRectangle->iTop    = 0;
	pRectangle->iRight  = SCREEN_WIDTH;
	pRectangle->iBottom = SCREEN_HEIGHT;
#else
	GetClipCursor((RECT *) pRectangle );
#endif
}


BOOLEAN IsCursorRestricted(void)
{
	return fCursorWasClipped;
}


void SimulateMouseMovement( UINT32 uiNewXPos, UINT32 uiNewYPos )
{
	// Wizardry NOTE: This function currently doesn't quite work right for in any Windows resolution other than 640x480.
	// mouse_event() uses your current Windows resolution to calculate the resulting x,y coordinates.  So in order to get
	// the right coordinates, you'd have to find out the current Windows resolution through a system call, and then do:
	//		uiNewXPos = uiNewXPos * SCREEN_WIDTH  / WinScreenResX;
	//		uiNewYPos = uiNewYPos * SCREEN_HEIGHT / WinScreenResY;
	//
	// JA2 doesn't have this problem, 'cause they use DirectDraw calls that change the Windows resolution properly.
	//
	// Alex Meduna, Dec. 3, 1997

#if 1
	FIXME
	SDL_WarpMouse(uiNewXPos, uiNewYPos);
#else
	// Adjust coords based on our resolution
	FLOAT flNewXPos = (FLOAT)uiNewXPos / SCREEN_WIDTH  * 65536;
	FLOAT flNewYPos = (FLOAT)uiNewYPos / SCREEN_HEIGHT * 65536;

	mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, (UINT32)flNewXPos, (UINT32)flNewYPos, 0, 0);
#endif
}


void DequeueAllKeyBoardEvents(void)
{
#if 1 // XXX TODO
	FIXME
#else
	//dequeue all the events waiting in the windows queue
	MSG KeyMessage;
	while (PeekMessage(&KeyMessage, ghWindow, WM_KEYFIRST, WM_KEYLAST, PM_REMOVE));

	//Deque all the events waiting in the SGP queue
	InputAtom InputEvent;
	while (DequeueEvent(&InputEvent))
  {
		//dont do anything
	}
#endif
}


static void HandleSingleClicksAndButtonRepeats(void)
{
	UINT32 uiTimer = GetTickCount();

	// Is there a LEFT mouse button repeat
	if (gfLeftButtonState)
	{
		if ((guiLeftButtonRepeatTimer > 0)&&(guiLeftButtonRepeatTimer <= uiTimer))
		{
			QueueMouseEvent(LEFT_BUTTON_REPEAT);
			guiLeftButtonRepeatTimer = uiTimer + BUTTON_REPEAT_TIME;
		}
	}
	else
	{
		guiLeftButtonRepeatTimer = 0;
	}


	// Is there a RIGHT mouse button repeat
	if (gfRightButtonState)
	{
		if ((guiRightButtonRepeatTimer > 0)&&(guiRightButtonRepeatTimer <= uiTimer))
		{
			QueueMouseEvent(RIGHT_BUTTON_REPEAT);
			guiRightButtonRepeatTimer = uiTimer + BUTTON_REPEAT_TIME;
		}
	}
	else
	{
		guiRightButtonRepeatTimer = 0;
	}
}
