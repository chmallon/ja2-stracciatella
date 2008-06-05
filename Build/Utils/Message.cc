#include "Debug.h"
#include "Local.h"
#include "Font.h"
#include "LoadSaveData.h"
#include "Types.h"
#include "Font_Control.h"
#include "Message.h"
#include "Timer_Control.h"
#include "Render_Dirty.h"
#include "RenderWorld.h"
#include "Interface.h"
#include "Map_Screen_Interface_Bottom.h"
#include "WordWrap.h"
#include "Sound_Control.h"
#include "SoundMan.h"
#include "Dialogue_Control.h"
#include "Game_Clock.h"
#include <stdarg.h>
#include "VSurface.h"
#include "MemMan.h"
#include "JAScreens.h"
#include "ScreenIDs.h"
#include "FileMan.h"


typedef struct ScrollStringSt ScrollStringSt;
struct ScrollStringSt
{
	wchar_t*        pString16;
	VIDEO_OVERLAY* video_overlay;
	UINT16  usColor;
	BOOLEAN fBeginningOfNewString;
	UINT32  uiTimeOfLastUpdate;
	ScrollStringSt* pNext;
};


#define MAX_LINE_COUNT 6
#define X_START 2
#define Y_START 330
#define MAX_AGE 10000
#define LINE_WIDTH 320
#define MAP_LINE_WIDTH 300
#define WIDTH_BETWEEN_NEW_STRINGS 5

#define BETAVERSION_COLOR FONT_ORANGE
#define TESTVERSION_COLOR FONT_GREEN
#define DEBUG_COLOR FONT_RED
#define DIALOGUE_COLOR FONT_WHITE
#define INTERFACE_COLOR FONT_YELLOW

#define MAP_SCREEN_MESSAGE_FONT TINYFONT1

UINT8 gubStartOfMapScreenMessageList = 0;
static UINT8 gubEndOfMapScreenMessageList = 0;

// index of the current string we are looking at
UINT8 gubCurrentMapMessageString = 0;

// are allowed to beep on message scroll?
BOOLEAN fOkToBeepNewMessage = TRUE;


static ScrollStringSt* gpDisplayList[MAX_LINE_COUNT];
static ScrollStringSt* gMapScreenMessageList[256];
static ScrollStringSt* pStringS = NULL;

BOOLEAN fDisableJustForIan = FALSE;

static BOOLEAN fScrollMessagesHidden = FALSE;
static UINT32  uiStartOfPauseTime = 0;

extern BOOLEAN gfFacePanelActive;

// region created and due to last quote box
extern BOOLEAN fTextBoxMouseRegionCreated;
extern BOOLEAN fDialogueBoxDueToLastMessage;


static ScrollStringSt* AddString(const wchar_t* pString, UINT16 usColor, BOOLEAN fStartOfNewString)
{
	ScrollStringSt* const i = MALLOC(ScrollStringSt);
	i->pString16             = MALLOCN(wchar_t, wcslen(pString) + 1);
	wcscpy(i->pString16, pString);
	i->video_overlay         = NULL;
	i->usColor               = usColor;
	i->fBeginningOfNewString = fStartOfNewString;
	i->pNext                 = NULL;
	return i;
}


static void BlitString(VIDEO_OVERLAY* pBlitter);


static void CreateStringVideoOverlay(ScrollStringSt* pStringSt, UINT16 usX, UINT16 usY)
{
	VIDEO_OVERLAY_DESC VideoOverlayDesc;
	VideoOverlayDesc.sLeft       = usX;
	VideoOverlayDesc.sTop        = usY;
	VideoOverlayDesc.uiFontID    = TINYFONT1;
	VideoOverlayDesc.ubFontBack  = FONT_MCOLOR_BLACK;
	VideoOverlayDesc.ubFontFore  = pStringSt->usColor;
	VideoOverlayDesc.BltCallback = BlitString;
	wcslcpy(VideoOverlayDesc.pzText, pStringSt->pString16, lengthof(VideoOverlayDesc.pzText));
	pStringSt->video_overlay = RegisterVideoOverlay(VOVERLAY_DIRTYBYTEXT, &VideoOverlayDesc);
}


static void RemoveStringVideoOverlay(ScrollStringSt* pStringSt)
{
	// error check, remove one not there
	if (pStringSt->video_overlay == NULL) return;
	RemoveVideoOverlay(pStringSt->video_overlay);
	pStringSt->video_overlay = NULL;
}


static void SetStringVideoOverlayPosition(ScrollStringSt* pStringSt, UINT16 usX, UINT16 usY)
{
	// Donot update if not allocated!
	if (pStringSt->video_overlay == NULL) return;
	SetVideoOverlayPos(pStringSt->video_overlay, usX, usY);
}


static void BlitString(VIDEO_OVERLAY* pBlitter)
{
	if (fScrollMessagesHidden) return;

	SetFont(pBlitter->uiFontID);
	SetFontBackground(pBlitter->ubFontBack);
	SetFontForeground(pBlitter->ubFontFore);
	SetFontShadow(DEFAULT_SHADOW);

	SGPVSurface::Lock l(pBlitter->uiDestBuff);
	mprintf_buffer_coded(l.Buffer<UINT16>(), l.Pitch(), pBlitter->sX, pBlitter->sY, pBlitter->zText);
}


// this function will go through list of display strings and clear them all out
void ClearDisplayedListOfTacticalStrings(void)
{
	for (UINT32 cnt = 0; cnt < MAX_LINE_COUNT; cnt++)
	{
		if (gpDisplayList[cnt] != NULL)
		{
			RemoveStringVideoOverlay(gpDisplayList[cnt]);
			MemFree(gpDisplayList[cnt]->pString16);
			MemFree(gpDisplayList[cnt]);
			gpDisplayList[cnt] = NULL;
		}
	}
}


static INT32 GetMessageQueueSize(void);
static void HandleLastQuotePopUpTimer(void);
static void PlayNewMessageSound(void);


void ScrollString(void)
{
	// UPDATE TIMER
	UINT32 suiTimer = GetJA2Clock();

	// might have pop up text timer
	HandleLastQuotePopUpTimer();

	if (guiCurrentScreen == MAP_SCREEN)
	{
		return;
	}

	// DONOT UPDATE IF WE ARE SCROLLING!
	if (gfScrollPending || gfScrollInertia)
	{
		return;
	}

	// messages hidden
	if (fScrollMessagesHidden)
	{
		return;
	}

	INT32 iNumberOfMessagesOnQueue = GetMessageQueueSize();
	INT32 iMaxAge = MAX_AGE;

	BOOLEAN fDitchLastMessage = (iNumberOfMessagesOnQueue > 0 && gpDisplayList[MAX_LINE_COUNT - 1] != NULL);

	if (iNumberOfMessagesOnQueue * 1000 >= iMaxAge)
	{
		iNumberOfMessagesOnQueue = iMaxAge / 1000;
	}
	else if (iNumberOfMessagesOnQueue < 0)
	{
		iNumberOfMessagesOnQueue = 0;
	}

	//AGE
	for (UINT32 cnt = 0; cnt < MAX_LINE_COUNT; cnt++)
	{
		if (gpDisplayList[cnt] != NULL)
		{
			if (fDitchLastMessage && cnt == MAX_LINE_COUNT - 1)
			{
				gpDisplayList[cnt]->uiTimeOfLastUpdate = iMaxAge;
			}
				// CHECK IF WE HAVE AGED
			if (suiTimer - gpDisplayList[cnt]->uiTimeOfLastUpdate > (UINT32)(iMaxAge - 1000 * iNumberOfMessagesOnQueue))
			{
				RemoveStringVideoOverlay(gpDisplayList[cnt]);
				MemFree(gpDisplayList[cnt]->pString16);
				MemFree(gpDisplayList[cnt]);
				gpDisplayList[cnt] = NULL;
			}
		}
	}


	// CHECK FOR FREE SPOTS AND ADD ANY STRINGS IF WE HAVE SOME TO ADD!

	// FIRST CHECK IF WE HAVE ANY IN OUR QUEUE
	if (pStringS != NULL)
	{
		// CHECK IF WE HAVE A SLOT!
		// CHECK OUR LAST SLOT!
		if (gpDisplayList[MAX_LINE_COUNT - 1] == NULL)
		{
			// MOVE ALL UP!

			// cpy, then move
			for (UINT32 cnt = MAX_LINE_COUNT - 1; cnt > 0; cnt--)
			{
				gpDisplayList[cnt] = gpDisplayList[cnt - 1];
			}

			INT32 iNumberOfNewStrings = 0; // the count of new strings, so we can update position by WIDTH_BETWEEN_NEW_STRINGS pixels in the y

			// now add in the new string
			gpDisplayList[0] = pStringS;
			CreateStringVideoOverlay(pStringS, X_START, Y_START);
			if (pStringS->fBeginningOfNewString)
			{
				iNumberOfNewStrings++;
			}

			// set up age
			pStringS->uiTimeOfLastUpdate = GetJA2Clock();

			// now move
			for (UINT32 cnt = 0; cnt <= MAX_LINE_COUNT - 1; cnt++)
			{
				// Adjust position!
				if (gpDisplayList[cnt] != NULL)
				{
					SetStringVideoOverlayPosition(gpDisplayList[cnt], X_START, Y_START - cnt * GetFontHeight(SMALLFONT1) - WIDTH_BETWEEN_NEW_STRINGS * iNumberOfNewStrings);

					// start of new string, increment count of new strings, for spacing purposes
					if (gpDisplayList[cnt]->fBeginningOfNewString)
					{
						iNumberOfNewStrings++;
					}
				}
			}

			// WE NOW HAVE A FREE SPACE, INSERT!

			// Adjust head!
			pStringS = pStringS->pNext;

			//check if new meesage we have not seen since mapscreen..if so, beep
			if (fOkToBeepNewMessage &&
					gpDisplayList[MAX_LINE_COUNT - 2] == NULL &&
					(guiCurrentScreen == GAME_SCREEN || guiCurrentScreen == MAP_SCREEN) &&
					!gfFacePanelActive)
			{
				PlayNewMessageSound();
			}
		}
	}
}


void DisableScrollMessages(void)
{
	// will stop the scroll of messages in tactical and hide them during an NPC's dialogue
	// disble video overlay for tatcitcal scroll messages
	EnableDisableScrollStringVideoOverlay(FALSE);
}


void EnableScrollMessages(void)
{
	EnableDisableScrollStringVideoOverlay(TRUE);
}


void HideMessagesDuringNPCDialogue(void)
{
	// will stop the scroll of messages in tactical and hide them during an NPC's dialogue
	fScrollMessagesHidden = TRUE;
	uiStartOfPauseTime = GetJA2Clock();

	for (INT32 cnt = 0; cnt < MAX_LINE_COUNT; cnt++)
	{
		const ScrollStringSt* const s = gpDisplayList[cnt];
		if (s != NULL)
		{
			RestoreExternBackgroundRectGivenID(s->video_overlay->background);
			EnableVideoOverlay(FALSE, s->video_overlay);
		}
	}
}


void UnHideMessagesDuringNPCDialogue(void)
{
	fScrollMessagesHidden = FALSE;

	for (INT32 cnt = 0; cnt < MAX_LINE_COUNT; cnt++)
	{
		ScrollStringSt* const s = gpDisplayList[cnt];
		if (s != NULL)
		{
			s->uiTimeOfLastUpdate += GetJA2Clock() - uiStartOfPauseTime;
			EnableVideoOverlay(TRUE, s->video_overlay);
		}
	}
}


static void TacticalScreenMsg(UINT16 usColor, UINT8 ubPriority, const wchar_t* pStringA, ...);


// new screen message
void ScreenMsg(UINT16 usColor, UINT8 ubPriority, const wchar_t* pStringA, ...)
{
	if (fDisableJustForIan)
	{
		switch (ubPriority)
		{
			case MSG_BETAVERSION:
			case MSG_TESTVERSION:
			case MSG_DEBUG:
				return;
		}
	}

	switch (ubPriority)
	{
		case MSG_DEBUG:
#if defined _DEBUG && !defined JA2DEMO
			usColor = DEBUG_COLOR;
			break;
#else
			return;
#endif

		case MSG_BETAVERSION:
#if defined JA2BETAVERSION || defined JA2TESTVERSION
			usColor = BETAVERSION_COLOR;
			break;
#else
			return;
#endif

		case MSG_TESTVERSION:
#if defined JA2TESTVERSION
			usColor = TESTVERSION_COLOR;
			break;
#else
			return;
#endif
	}

	va_list argptr;
	va_start(argptr, pStringA);
	wchar_t DestString[512];
	vswprintf(DestString, lengthof(DestString), pStringA, argptr);
	va_end(argptr);

	// pass onto tactical message and mapscreen message
	TacticalScreenMsg(usColor, ubPriority, L"%ls", DestString);
	MapScreenMessage(usColor, ubPriority, L"%ls", DestString);

	if (guiCurrentScreen == MAP_SCREEN)
	{
		PlayNewMessageSound( );
	}
	else
	{
		fOkToBeepNewMessage = TRUE;
	}
}


// clear up a linked list of wrapped strings
static void ClearWrappedStrings(WRAPPED_STRING* pStringWrapperHead)
{
	WRAPPED_STRING* i = pStringWrapperHead;
	while (i != NULL)
	{
		WRAPPED_STRING* del = i;
		i = i->pNextWrappedString;
		MemFree(del);
	}
}


static void WriteMessageToFile(const wchar_t* pString);


// this function sets up the string into several single line structures
static void TacticalScreenMsg(UINT16 usColor, UINT8 ubPriority, const wchar_t* pStringA, ...)
{
	if (giTimeCompressMode > TIME_COMPRESS_X1 ||
			fDisableJustForIan && ubPriority != MSG_ERROR && ubPriority != MSG_INTERFACE)
	{
		return;
	}

	va_list argptr;
	va_start(argptr, pStringA);
	wchar_t DestString[512];
	vswprintf(DestString, lengthof(DestString), pStringA, argptr);
	va_end(argptr);

	switch (ubPriority)
	{
		case MSG_BETAVERSION:
#if defined JA2BETAVERSION || defined JA2TESTVERSION
			WriteMessageToFile(DestString);
			usColor = BETAVERSION_COLOR;
			break;
#else
			return;
#endif

		case MSG_TESTVERSION:
#if defined JA2TESTVERSION
			WriteMessageToFile(DestString);
			usColor = TESTVERSION_COLOR;
			break;
#else
			return;
#endif

		case MSG_DEBUG:
#if defined _DEBUG && !defined JA2DEMO
		{
			wchar_t DestStringA[512];
			wcscpy(DestStringA, DestString);
			swprintf(DestString, lengthof(DestString), L"Debug: %ls", DestStringA);
			WriteMessageToFile(DestStringA);
			usColor = DEBUG_COLOR;
			break;
		}
#else
			return;
#endif

		case MSG_DIALOG:    usColor = DIALOGUE_COLOR;  break;
		case MSG_INTERFACE: usColor = INTERFACE_COLOR; break;
	}

	WRAPPED_STRING* pStringWrapperHead = LineWrap(TINYFONT1, LINE_WIDTH, NULL, DestString);
	WRAPPED_STRING* pStringWrapper = pStringWrapperHead;
	if (pStringWrapper == NULL) return;

	ScrollStringSt* tail = pStringS;
	if (tail != NULL)
	{
		while (tail->pNext != NULL) tail = tail->pNext;
	}

	BOOLEAN fNewString = TRUE;
	do
	{
		ScrollStringSt* pTempStringSt = AddString(pStringWrapper->sString, usColor, fNewString);
		if (tail == NULL)
		{
			pStringS = pTempStringSt;
		}
		else
		{
			tail->pNext = pTempStringSt;
		}
		tail = pTempStringSt;
		fNewString = FALSE;
		pStringWrapper = pStringWrapper->pNextWrappedString;
	}
	while (pStringWrapper != NULL);

	ClearWrappedStrings(pStringWrapperHead);
}


static void AddStringToMapScreenMessageList(const wchar_t* pString, UINT16 usColor, BOOLEAN fStartOfNewString);


// this function sets up the string into several single line structures
void MapScreenMessage(UINT16 usColor, UINT8 ubPriority, const wchar_t* pStringA, ...)
{
	if (fDisableJustForIan)
	{
		switch (ubPriority)
		{
			case MSG_BETAVERSION:
			case MSG_TESTVERSION:
			case MSG_DEBUG:
				return;
		}
	}

	wchar_t DestStringA[512];

	va_list argptr;
	va_start(argptr, pStringA);
	wchar_t DestString[512];
	vswprintf(DestString, lengthof(DestString), pStringA, argptr);
	va_end(argptr);

	switch (ubPriority)
	{
		case MSG_BETAVERSION:
#if defined JA2BETAVERSION || defined JA2TESTVERSION
			WriteMessageToFile(DestString);
			usColor = BETAVERSION_COLOR;
			break;
#else
			return;
#endif

		case MSG_TESTVERSION:
#if defined JA2TESTVERSION
			WriteMessageToFile(DestString);
			usColor = TESTVERSION_COLOR;
			break;
#else
			return;
#endif

		case MSG_UI_FEEDBACK:
			// An imeediate feedback message. Do something else!
			BeginUIMessage(FALSE, DestString);
			return;

		case MSG_SKULL_UI_FEEDBACK:
			BeginUIMessage(TRUE, DestString);
			return;

		case MSG_ERROR:
			swprintf(DestStringA, lengthof(DestStringA), L"DEBUG: %ls", DestString);
			BeginUIMessage(FALSE, DestStringA);
			WriteMessageToFile(DestStringA);
			return;

		case MSG_DEBUG:
#if defined _DEBUG && !defined JA2DEMO
			wcscpy(DestStringA, DestString);
			swprintf(DestString, lengthof(DestString), L"Debug: %ls", DestStringA);
			usColor = DEBUG_COLOR;
#else
			return;
#endif

		case MSG_DIALOG:    usColor = DIALOGUE_COLOR;  break;
		case MSG_INTERFACE: usColor = INTERFACE_COLOR; break;
	}

	WRAPPED_STRING* pStringWrapperHead = LineWrap(MAP_SCREEN_MESSAGE_FONT, MAP_LINE_WIDTH, NULL, DestString);
	WRAPPED_STRING* pStringWrapper = pStringWrapperHead;
	if (!pStringWrapper) return;

	BOOLEAN fNewString = TRUE;
	do
	{
		AddStringToMapScreenMessageList(pStringWrapper->sString, usColor, fNewString);
		fNewString = FALSE;
		pStringWrapper = pStringWrapper->pNextWrappedString;
	}
	while (pStringWrapper != NULL);

	ClearWrappedStrings(pStringWrapperHead);

	MoveToEndOfMapScreenMessageList();
}


// add string to the map screen message list
static void AddStringToMapScreenMessageList(const wchar_t* pString, UINT16 usColor, BOOLEAN fStartOfNewString)
{
	ScrollStringSt* const pStringSt = AddString(pString, usColor, fStartOfNewString);

	// Figure out which queue slot index we're going to use to store this
	// If queue isn't full, this is easy, if is is full, we'll re-use the oldest slot
	// Must always keep the wraparound in mind, although this is easy enough with a static, fixed-size queue.

	// always store the new message at the END index

	// check if slot is being used, if so, clear it up
	ScrollStringSt* const old = gMapScreenMessageList[gubEndOfMapScreenMessageList];
	if (old != NULL)
	{
		MemFree(old->pString16);
		MemFree(old);
	}

	// store the new message there
	gMapScreenMessageList[gubEndOfMapScreenMessageList] = pStringSt;

	// increment the end
	gubEndOfMapScreenMessageList = (gubEndOfMapScreenMessageList + 1) % 256;

	// if queue is full, end will now match the start
	if (gubEndOfMapScreenMessageList == gubStartOfMapScreenMessageList)
	{
		// if that's so, increment the start
		gubStartOfMapScreenMessageList = (gubStartOfMapScreenMessageList + 1) % 256;
	}
}


void DisplayStringsInMapScreenMessageList(void)
{
	SetFontDestBuffer(FRAME_BUFFER, 17, 360 + 6, 407, 360 + 101);

	SetFont(MAP_SCREEN_MESSAGE_FONT);
	SetFontBackground(FONT_BLACK);
	SetFontShadow(DEFAULT_SHADOW);

	UINT8 ubCurrentStringIndex = gubCurrentMapMessageString;

	INT16 sY = 377;
	UINT16 usSpacing = GetFontHeight(MAP_SCREEN_MESSAGE_FONT);

	for (UINT8 ubLinesPrinted = 0; ubLinesPrinted < MAX_MESSAGES_ON_MAP_BOTTOM; ubLinesPrinted++)
	{
		// reached the end of the list?
		if (ubCurrentStringIndex == gubEndOfMapScreenMessageList)
		{
			break;
		}

		const ScrollStringSt* const s = gMapScreenMessageList[ubCurrentStringIndex];
		if (s == NULL) break;

		SetFontForeground(s->usColor);
		mprintf_coded(20, sY, s->pString16);

		sY += usSpacing;

		// next message index to print (may wrap around)
		ubCurrentStringIndex = (ubCurrentStringIndex + 1) % 256;
	}

	SetFontDestBuffer(FRAME_BUFFER, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}


void EnableDisableScrollStringVideoOverlay(BOOLEAN fEnable)
{
	/* will go through the list of video overlays for the tactical scroll message
	 * system, and enable/disable video overlays depending on fEnable */
	for (INT8 bCounter = 0; bCounter < MAX_LINE_COUNT; bCounter++)
	{
		const ScrollStringSt* const s = gpDisplayList[bCounter];
		if (s != NULL)
		{
			EnableVideoOverlay(fEnable, s->video_overlay);
		}
	}
}


// play beep when new message is added
static void PlayNewMessageSound(void)
{
	// play a new message sound, if there is one playing, do nothing
	static UINT32 uiSoundId = NO_SAMPLE;

	// if a sound is playing, don't play new one
	if (uiSoundId != NO_SAMPLE && SoundIsPlaying(uiSoundId))
	{
		return;
	}

	uiSoundId = PlayJA2SampleFromFile("Sounds/newbeep.wav", MIDVOLUME, 1, MIDDLEPAN);
}


static BOOLEAN ExtractScrollStringFromFile(const HWFILE file, ScrollStringSt* const s)
try
{
	BYTE data[28];
	FileRead(file, data, sizeof(data));

	const BYTE* d = data;
	EXTR_SKIP(d, 4)
	EXTR_U32(d, s->uiTimeOfLastUpdate)
	EXTR_SKIP(d, 16)
	EXTR_U16(d, s->usColor)
	EXTR_BOOL(d, s->fBeginningOfNewString)
	EXTR_SKIP(d, 1)
	Assert(d == endof(data));

	return TRUE;
}
catch (...) { return FALSE; }


static BOOLEAN InjectScrollStringIntoFile(const HWFILE file, const ScrollStringSt* const s)
try
{
	BYTE data[28];
	BYTE* d = data;
	INJ_SKIP(d, 4)
	INJ_U32(d, s->uiTimeOfLastUpdate)
	INJ_SKIP(d, 16)
	INJ_U16(d, s->usColor)
	INJ_BOOL(d, s->fBeginningOfNewString)
	INJ_SKIP(d, 1)
	Assert(d == endof(data));

	FileWrite(file, data, sizeof(data));
	return TRUE;
}
catch (...) { return FALSE; }


BOOLEAN SaveMapScreenMessagesToSaveGameFile(HWFILE hFile)
try
{
	// write to the begining of the message list
	FileWrite(hFile, &gubEndOfMapScreenMessageList, sizeof(UINT8));

	FileWrite(hFile, &gubStartOfMapScreenMessageList, sizeof(UINT8));

	// write the current message string
	FileWrite(hFile, &gubCurrentMapMessageString, sizeof(UINT8));

	//Loopthrough all the messages
	for (ScrollStringSt*const *i = gMapScreenMessageList; i != endof(gMapScreenMessageList); ++i)
	{
		const ScrollStringSt* const s = *i;
		UINT32 uiSizeOfString;
		if (s != NULL)
		{
			uiSizeOfString = (wcslen(s->pString16) + 1) * sizeof(*s->pString16);
		}
		else
		{
			uiSizeOfString = 0;
		}

		// write to the file the size of the message
		FileWrite(hFile, &uiSizeOfString, sizeof(UINT32));

		//if there is a message
		if (uiSizeOfString)
		{
			// write the message to the file
			FileWrite(hFile, s->pString16, uiSizeOfString);

			if (!InjectScrollStringIntoFile(hFile, s)) return FALSE;
		}
	}

	return TRUE;
}
catch (...) { return FALSE; }


BOOLEAN LoadMapScreenMessagesFromSaveGameFile(HWFILE hFile)
try
{
	// clear tactical message queue
	ClearTacticalMessageQueue();

	gubEndOfMapScreenMessageList   = 0;
	gubStartOfMapScreenMessageList = 0;
	gubCurrentMapMessageString     = 0;

	// Read to the begining of the message list
	FileRead(hFile, &gubEndOfMapScreenMessageList, sizeof(UINT8));

	// Read the current message string
	FileRead(hFile, &gubStartOfMapScreenMessageList, sizeof(UINT8));

	// Read the current message string
	FileRead(hFile, &gubCurrentMapMessageString, sizeof(UINT8));

	//Loopthrough all the messages
	for (ScrollStringSt** i = gMapScreenMessageList; i != endof(gMapScreenMessageList); ++i)
	{
		// Read to the file the size of the message
		UINT32 uiSizeOfString;
		FileRead(hFile, &uiSizeOfString, sizeof(uiSizeOfString));

		//if there is a message
		if (uiSizeOfString != 0)
		{
			// Read the message from the file
			wchar_t SavedString[512];
			FileRead(hFile, SavedString, uiSizeOfString);

			//if there is an existing string,delete it
			ScrollStringSt* s = *i;
			if (s != NULL)
			{
				if (s->pString16)
				{
					MemFree(s->pString16);
					s->pString16 = NULL;
				}
			}
			else
			{
				// There is now message here, add one
				s = MALLOCZ(ScrollStringSt);
				*i = s;
			}

			//allocate space for the new string
			s->pString16 = MALLOCNZ(wchar_t, uiSizeOfString / sizeof(wchar_t));

			//copy the string over
			wcscpy(s->pString16, SavedString);

			if (!ExtractScrollStringFromFile(hFile, s)) return FALSE;
		}
		else
		{
			*i = NULL;
		}
	}

	// this will set a valid value for gubFirstMapscreenMessageIndex, which isn't being saved/restored
	MoveToEndOfMapScreenMessageList();

	return TRUE;
}
catch (...) { return FALSE; }


static void HandleLastQuotePopUpTimer(void)
{
	if (!fTextBoxMouseRegionCreated || !fDialogueBoxDueToLastMessage)
	{
		return;
	}

	// check if timed out
	if (GetJA2Clock() - guiDialogueLastQuoteTime >  guiDialogueLastQuoteDelay)
	{
		// done clear up
		ShutDownLastQuoteTacticalTextBox();
		guiDialogueLastQuoteTime = 0;
		guiDialogueLastQuoteDelay = 0;
	}
}


static INT32 GetMessageQueueSize(void)
{
	INT32 iCounter = 0;
	for (const ScrollStringSt* i = pStringS; i != NULL; i = i->pNext)
	{
		iCounter++;
	}
	return iCounter;
}


void ClearTacticalMessageQueue(void)
{
	ClearDisplayedListOfTacticalStrings();

	// now run through all the tactical messages
	for (ScrollStringSt* i = pStringS; i != NULL;)
	{
		ScrollStringSt* del = i;
		i = i->pNext;
		MemFree(del->pString16);
		MemFree(del);
	}

	pStringS = NULL;
}


static void WriteMessageToFile(const wchar_t* pString)
{
#ifdef JA2BETAVERSION
	FILE* fp = fopen("DebugMessage.txt", "a");
	if (fp == NULL) return;
	fprintf(fp, "%ls\n", pString);
	fclose(fp);
#endif
}


void FreeGlobalMessageList(void)
{
	for (ScrollStringSt** i = gMapScreenMessageList; i != endof(gMapScreenMessageList); ++i)
	{
		ScrollStringSt* const s = *i;
		if (s != NULL)
		{
			MemFree(s->pString16);
			MemFree(s);
			*i = NULL;
		}
	}

	gubEndOfMapScreenMessageList   = 0;
	gubStartOfMapScreenMessageList = 0;
	gubCurrentMapMessageString     = 0;
}


UINT8 GetRangeOfMapScreenMessages(void)
{
	// NOTE: End is non-inclusive, so start/end 0/0 means no messages, 0/1 means 1 message, etc.
	if (gubStartOfMapScreenMessageList <= gubEndOfMapScreenMessageList)
	{
		return gubEndOfMapScreenMessageList - gubStartOfMapScreenMessageList;
	}
	else
	{
		// this should always be 255 now, since this only happens when queue fills up, and we never remove any messages
		return 256 + gubEndOfMapScreenMessageList - gubStartOfMapScreenMessageList;
	}
}
