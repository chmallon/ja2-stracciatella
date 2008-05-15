#include "FileMan.h"
#include "Font.h"
#include "Font_Control.h"
#include "Input.h"
#include "Interface.h"
#include "Local.h"
#include "MessageBoxScreen.h"
#include "Timer.h"
#include "Timer_Control.h"
#include "VObject.h"
#include "VSurface.h"
#include "WCheck.h"
#include "Render_Dirty.h"
#include "SysUtil.h"
#include "Screens.h"
#include "Cursors.h"
#include "Event_Pump.h"
#include "Laptop.h"
#include "AIM.h"
#include "AIMMembers.h"
#include "AIMFacialIndex.h"
#include "AIMSort.h"
#include "AIMArchives.h"
#include "AIMHistory.h"
#include "AIMLinks.h"
#include "AIMPolicies.h"
#include "Mercs.h"
#include "Mercs_Files.h"
#include "Mercs_Account.h"
#include "Mercs_No_Account.h"
#include "BobbyR.h"
#include "BobbyRGuns.h"
#include "BobbyRAmmo.h"
#include "BobbyRArmour.h"
#include "BobbyRMisc.h"
#include "BobbyRUsed.h"
#include "BobbyRMailOrder.h"
#include "CharProfile.h"
#include "Florist.h"
#include "Florist_Cards.h"
#include "Florist_Gallery.h"
#include "Florist_Order_Form.h"
#include "Insurance.h"
#include "Insurance_Contract.h"
#include "Insurance_Info.h"
#include "Insurance_Comments.h"
#include "Funeral.h"
#include "Finances.h"
#include "Personnel.h"
#include "History.h"
#include "Files.h"
#include "EMail.h"
#include "Interface_Control.h"
#include "Game_Event_Hook.h"
#include "WordWrap.h"
#include "Game_Init.h"
#include "Game_Clock.h"
#include "Soldier_Control.h"
#include "Soldier_Profile.h"
#include "Overhead.h"
#include "Environment.h"
#include "Music_Control.h"
#include "LaptopSave.h"
#include "RenderWorld.h"
#include "GameLoop.h"
#include "English.h"
#include "Random.h"
#include "Merc_Hiring.h"
#include "Map_Screen_Interface.h"
#include "Ambient_Control.h"
#include "Sound_Control.h"
#include "Text.h"
#include "Message.h"
#include "Map_Screen_Interface_Bottom.h"
#include "Cursor_Control.h"
#include "Quests.h"
#include "Multi_Language_Graphic_Utils.h"
#include "BrokenLink.h"
#include "BobbyRShipments.h"
#include "Dialogue_Control.h"
#include "HelpScreen.h"
#include "Cheats.h"
#include "Video.h"
#include "MemMan.h"
#include "Debug.h"
#include "Button_System.h"
#include "JAScreens.h"

#ifdef JA2TESTVERSION
#	include "Arms_Dealer_Init.h"
#	include "Strategic_Status.h"
#endif


// laptop programs
enum
{
	LAPTOP_PROGRAM_MAILER,
	LAPTOP_PROGRAM_WEB_BROWSER,
	LAPTOP_PROGRAM_FILES,
	LAPTOP_PROGRAM_PERSONNEL,
	LAPTOP_PROGRAM_FINANCES,
	LAPTOP_PROGRAM_HISTORY
};

// laptop program states
enum
{
	LAPTOP_PROGRAM_MINIMIZED,
	LAPTOP_PROGRAM_OPEN
};

#define BOOK_FONT     FONT10ARIAL
#define DOWNLOAD_FONT FONT12ARIAL


#define BOOK_X      111
#define BOOK_TOP_Y   79
#define BOOK_HEIGHT  12
#define DOWN_HEIGHT  19
#define BOOK_WIDTH  100


#define LONG_UNIT_TIME        120
#define UNIT_TIME              40
#define FAST_UNIT_TIME          3
#define FASTEST_UNIT_TIME       2
#define ALMOST_FAST_UNIT_TIME  25
#define DOWNLOAD_X            300
#define DOWNLOAD_Y            200
#define LAPTOP_WINDOW_X       DOWNLOAD_X + 12
#define LAPTOP_WINDOW_Y       DOWNLOAD_Y + 25
#define LAPTOP_BAR_Y          LAPTOP_WINDOW_Y + 2
#define LAPTOP_BAR_X          LAPTOP_WINDOW_X + 1
#define UNIT_WIDTH              4
#define DOWN_STRING_X         DOWNLOAD_X + 47
#define DOWN_STRING_Y         DOWNLOAD_Y +  5
#define LAPTOP_TITLE_ICONS_X  113
#define LAPTOP_TITLE_ICONS_Y   27

// HD flicker times
#define HD_FLICKER_TIME 3000
#define FLICKER_TIME      50


#define NUMBER_OF_LAPTOP_TITLEBAR_ITERATIONS  18
#define LAPTOP_TITLE_BAR_WIDTH               500
#define LAPTOP_TITLE_BAR_HEIGHT               24

#define LAPTOP_TITLE_BAR_TOP_LEFT_X          111
#define LAPTOP_TITLE_BAR_TOP_LEFT_Y           25
#define LAPTOP_TITLE_BAR_TOP_RIGHT_X         610

#define LAPTOP_TITLE_BAR_ICON_OFFSET_X         5
#define LAPTOP_TITLE_BAR_ICON_OFFSET_Y         2
#define LAPTOP_TITLE_BAR_TEXT_OFFSET_X        29
#define LAPTOP_TITLE_BAR_TEXT_OFFSET_Y         8

#define LAPTOP_PROGRAM_ICON_X      LAPTOP_TITLE_BAR_TOP_LEFT_X
#define LAPTOP_PROGRAM_ICON_Y      LAPTOP_TITLE_BAR_TOP_LEFT_Y
#define LAPTOP_PROGRAM_ICON_WIDTH  20
#define LAPTOP_PROGRAM_ICON_HEIGHT 20
#define DISPLAY_TIME_FOR_WEB_BOOKMARK_NOTIFY 2000

// the wait time for closing of laptop animation/delay
#define EXIT_LAPTOP_DELAY_TIME 100

static SGPVSurface* guiTitleBarSurface;
static BOOLEAN gfTitleBarSurfaceAlreadyActive = FALSE;

#define LAPTOP__NEW_FILE_ICON_X  83
#define LAPTOP__NEW_FILE_ICON_Y 412

#define LAPTOP__NEW_EMAIL_ICON_X (83 - 16)
#define LAPTOP__NEW_EMAIL_ICON_Y LAPTOP__NEW_FILE_ICON_Y


// Mode values
UINT32 guiCurrentLaptopMode;
UINT32 guiPreviousLaptopMode;
static UINT32 guiCurrentWWWMode = LAPTOP_MODE_NONE;
INT32  giCurrentSubPage;


static INT32 iHighLightBookLine = -1;
BOOLEAN fFastLoadFlag = FALSE;
static BOOLEAN gfEnterLapTop=TRUE;
BOOLEAN gfShowBookmarks=FALSE;

// in progress of loading a page?
BOOLEAN fLoadPendingFlag=FALSE;

// mark buttons dirty?
static BOOLEAN fMarkButtonsDirtyFlag = TRUE;

// redraw afer rendering buttons?
BOOLEAN fReDrawPostButtonRender = FALSE;

// in laptop right now?
BOOLEAN fCurrentlyInLaptop = FALSE;

// exit due to a message box pop up?..don't really leave LAPTOP
BOOLEAN fExitDueToMessageBox = FALSE;

// exit laptop during a load?
static BOOLEAN fExitDuringLoad = FALSE;

// done loading?
BOOLEAN fDoneLoadPending = FALSE;

// going a subpage of a web page?..faster access time
BOOLEAN fConnectingToSubPage = FALSE;

// is this our first time in laptop?
static BOOLEAN fFirstTimeInLaptop = TRUE;

// redraw the book mark info panel .. for blitting on top of animations
BOOLEAN fReDrawBookMarkInfo = FALSE;

// show the 2 second info about bookmarks being accessed by clicking on web
BOOLEAN fShowBookmarkInfo = FALSE;


//TEMP! Disables the loadpending delay when switching b/n www pages
static BOOLEAN gfTemporaryDisablingOfLoadPendingFlag = FALSE;


//GLOBAL FOR WHICH SCREEN TO EXIT TO FOR LAPTOP
static UINT32 guiExitScreen = MAP_SCREEN;

// Laptop screen graphic handle
static SGPVObject* guiLAPTOP;

static BOOLEAN fNewWWW = TRUE;

//Used to store the site to go to after the 'rain delay' message
extern UINT32 guiRainLoop;


static INT32 giRainDelayInternetSite = -1;


// the laptop icons
static SGPVObject* guiBOOKHIGH;
static SGPVObject* guiBOOKMARK;
static SGPVObject* guiGRAPHWINDOW;
static SGPVObject* guiGRAPHBAR;
SGPVObject* guiLaptopBACKGROUND;
static SGPVObject* guiDOWNLOADTOP;
static SGPVObject* guiDOWNLOADMID;
static SGPVObject* guiDOWNLOADBOT;
static SGPVObject* guiTITLEBARLAPTOP;
static SGPVObject* guiLIGHTS;
SGPVObject* guiTITLEBARICONS;
static SGPVSurface* guiDESKTOP;

// enter new laptop mode due to sliding bars
static BOOLEAN fEnteredNewLapTopDueToHandleSlidingBars = FALSE;


// whether or not we are initing the slide in title bar
static BOOLEAN fInitTitle = TRUE;

// tab handled
static BOOLEAN fTabHandled = FALSE;

static INT32 gLaptopButton[7];

// minimize button
static INT32 gLaptopMinButton;


static INT32 gLaptopProgramStates[LAPTOP_PROGRAM_HISTORY + 1];

// process of mazimizing
static BOOLEAN fMaximizingProgram = FALSE;

// program we are maximizing
static INT8 bProgramBeingMaximized = -1;

// are we minimizing
static BOOLEAN fMinizingProgram = FALSE;


// process openned queue
static INT32 gLaptopProgramQueueList[6];


// state of createion of minimize button
static BOOLEAN fCreateMinimizeButton = FALSE;

BOOLEAN fExitingLaptopFlag = FALSE;

// HD and power lights on
static BOOLEAN fPowerLightOn = TRUE;
static BOOLEAN fHardDriveLightOn = FALSE;

// HD flicker
static BOOLEAN fFlickerHD = FALSE;

// the screens limiting rect
static const SGPRect LaptopScreenRect = { LAPTOP_UL_X, LAPTOP_UL_Y - 5, LAPTOP_SCREEN_LR_X + 2, LAPTOP_SCREEN_LR_Y + 5 + 19 };


// the sub pages vistsed or not status within the web browser
static BOOLEAN gfWWWaitSubSitesVisitedFlags[LAPTOP_MODE_FUNERAL - LAPTOP_MODE_WWW];

// mouse regions
static MOUSE_REGION gLapTopScreenRegion;
static MOUSE_REGION gBookmarkMouseRegions[MAX_BOOKMARKS];
static MOUSE_REGION gLapTopProgramMinIcon;
static MOUSE_REGION gNewMailIconRegion;
static MOUSE_REGION gNewFileIconRegion;


//used for global variables that need to be saved
LaptopSaveInfoStruct LaptopSaveInfo;


BOOLEAN fReDrawScreenFlag=FALSE;
BOOLEAN fPausedReDrawScreenFlag=FALSE; //used in the handler functions to redraw the screen, after the current frame
void PrintBalance(void);


void PrintDate(void);
void PrintNumberOnTeam(void);

extern void ClearHistoryList(void);


void SetLaptopExitScreen(UINT32 uiExitScreen)
{
	guiExitScreen = uiExitScreen;
}


void SetLaptopNewGameFlag(void)
{
	LaptopSaveInfo.gfNewGameLaptop = TRUE;
}


static void GetLaptopKeyboardInput(void)
{
	SGPPoint MousePos;
	GetMousePos(&MousePos);

	fTabHandled = FALSE;

	InputAtom InputEvent;
	while (DequeueEvent(&InputEvent))
	{
		MouseSystemHook(InputEvent.usEvent, MousePos.iX, MousePos.iY);
		HandleKeyBoardShortCutsForLapTop(InputEvent.usEvent, InputEvent.usParam, InputEvent.usKeyState);
	}
}


static void InitBookMarkList(void);


//This is called only once at game initialization.
UINT32 LaptopScreenInit(void)
{
	//Memset the whole structure, to make sure of no 'JUNK'
	memset(&LaptopSaveInfo, 0, sizeof(LaptopSaveInfoStruct));

	LaptopSaveInfo.gfNewGameLaptop = TRUE;

	InitializeNumDaysMercArrive();

	//reset the id of the last hired merc
	LaptopSaveInfo.sLastHiredMerc.iIdOfMerc = -1;

	//reset the flag that enables the 'just hired merc' popup
	LaptopSaveInfo.sLastHiredMerc.fHaveDisplayedPopUpInLaptop = FALSE;

	//Initialize all vars
	guiCurrentLaptopMode  = LAPTOP_MODE_EMAIL;
	guiPreviousLaptopMode = LAPTOP_MODE_NONE;
	guiCurrentWWWMode     = LAPTOP_MODE_NONE;

	gfShowBookmarks = FALSE;
	InitBookMarkList();
	GameInitAIM();
	GameInitAimSort();
	GameInitMercs();
	GameInitBobbyRGuns();
	GameInitBobbyRMailOrder();
	GameInitEmail();
	GameInitCharProfile();
	GameInitFiles();
	GameInitPersonnel();

	// init program states
	memset(&gLaptopProgramStates, LAPTOP_PROGRAM_MINIMIZED, sizeof(gLaptopProgramStates));

	gfAtLeastOneMercWasHired = FALSE;

	//No longer inits the laptop screens, now InitLaptopAndLaptopScreens() does

	return 1;
}


BOOLEAN InitLaptopAndLaptopScreens(void)
{
	GameInitFinances();
	GameInitHistory();

	//Reset the flag so we can create a new IMP character
	LaptopSaveInfo.fIMPCompletedFlag = FALSE;

	//Reset the flag so that BOBBYR's isnt available at the begining of the game
	LaptopSaveInfo.fBobbyRSiteCanBeAccessed = FALSE;

	return TRUE;
}


//This is only called once at game shutdown.
void LaptopScreenShutdown(void)
{
	InsuranceContractEndGameShutDown();
	BobbyRayMailOrderEndGameShutDown();
	ShutDownEmailList();
	ClearHistoryList();
}


static void CreateDestroyMouseRegionForNewMailIcon(void);
static void CreateLapTopMouseRegions(void);
static void DrawDeskTopBackground(void);
static void EnterLaptopInitLaptopPages(void);
static void InitLaptopOpenQueue(void);
static void InitalizeSubSitesList(void);
static BOOLEAN IsItRaining(void);
static void LoadBookmark(void);
static void LoadDesktopBackground(void);
static void LoadLoadPending(void);
static void RenderLapTopImage(void);


static INT32 EnterLaptop(void)
try
{
	//Create, load, initialize data -- just entered the laptop.

	// we are re entering due to message box, leave NOW!
	if (fExitDueToMessageBox) return TRUE;

	//if the radar map mouse region is still active, disable it.
	if (gRadarRegion.uiFlags & MSYS_REGION_ENABLED)
	{
		MSYS_DisableRegion(&gRadarRegion);
/*
		#ifdef JA2BETAVERSION
			DoLapTopMessageBox( MSG_BOX_LAPTOP_DEFAULT, L"Mapscreen's radar region is still active, please tell Dave how you entered Laptop.", LAPTOP_SCREEN, MSG_BOX_FLAG_OK, NULL );
		#endif
*/
	}

	gfDontStartTransitionFromLaptop = FALSE;

	//Since we are coming in from MapScreen, uncheck the flag
	guiTacticalInterfaceFlags &= ~INTERFACE_MAPSCREEN;

	// ATE: Disable messages....
	DisableScrollMessages();

	// Stop any person from saying anything
	StopAnyCurrentlyTalkingSpeech();

	// Don't play music....
	SetMusicMode(MUSIC_LAPTOP);

	// Stop ambients...
	StopAmbients();

	//if its raining, start the rain showers
	if (IsItRaining())
	{
		//Enable the rain delay warning
		giRainDelayInternetSite = -1;

		//lower the volume
		guiRainLoop = PlayJA2Ambient(RAIN_1, LOWVOLUME, 0);
	}

	//pause the game because we dont want time to advance in the laptop
	PauseGame();

	// set the fact we are currently in laptop, for rendering purposes
	fCurrentlyInLaptop = TRUE;

	// reset redraw flag and redraw new mail
	fReDrawScreenFlag  = FALSE;
	fReDrawNewMailFlag = TRUE;

	// sub page
	giCurrentSubPage = 0;

	// load the laptop graphic and add it
	guiLAPTOP = AddVideoObjectFromFile("LAPTOP/laptop3.sti");

	// background for panel
	guiLaptopBACKGROUND = AddVideoObjectFromFile("LAPTOP/taskbar.sti");

	// background for panel
	guiTITLEBARLAPTOP = AddVideoObjectFromFile("LAPTOP/programtitlebar.sti");

	// lights for power and HD
	guiLIGHTS = AddVideoObjectFromFile("LAPTOP/lights.sti");

	// icons for title bars
	guiTITLEBARICONS = AddVideoObjectFromFile("LAPTOP/ICONS.sti");

	// load, blt and delete graphics
	guiEmailWarning = AddVideoObjectFromFile("LAPTOP/NewMailWarning.sti");
	// load background
	LoadDesktopBackground();

	guiCurrentLaptopMode  = LAPTOP_MODE_NONE;
	guiPreviousLaptopMode = LAPTOP_MODE_NONE;
	guiCurrentWWWMode     = LAPTOP_MODE_NONE;
	CreateLapTopMouseRegions();
	RenderLapTopImage();

	// reset bookmarks flags
	fFirstTimeInLaptop = TRUE;

	// reset all bookmark visits
	memset(&LaptopSaveInfo.fVisitedBookmarkAlready, 0, sizeof(LaptopSaveInfo.fVisitedBookmarkAlready));

	// init program states
	memset(&gLaptopProgramStates, LAPTOP_PROGRAM_MINIMIZED, sizeof(gLaptopProgramStates));

	// turn the power on
	fPowerLightOn = TRUE;

	// we are not exiting laptop right now, we just got here
	fExitingLaptopFlag = FALSE;

	// reset program we are maximizing
	bProgramBeingMaximized = -1;

	// reset fact we are maximizing/ mining
	fMaximizingProgram = FALSE;
	fMinizingProgram = FALSE;


	// initialize open queue
	InitLaptopOpenQueue();


	gfShowBookmarks = FALSE;
	LoadBookmark();
	SetBookMark(AIM_BOOKMARK);
	LoadLoadPending();

	DrawDeskTopBackground();

	// create region for new mail icon
	CreateDestroyMouseRegionForNewMailIcon();

	//DEF: Added to Init things in various laptop pages
	EnterLaptopInitLaptopPages();
	InitalizeSubSitesList();

	InvalidateScreen();

	return TRUE;
}
catch (...) { return FALSE; }


static void CreateDestoryBookMarkRegions(void);
static void CreateDestroyMinimizeButtonForCurrentMode(void);
static void DeleteBookmark(void);
static void DeleteDesktopBackground(void);
static void DeleteLapTopButtons(void);
static void DeleteLapTopMouseRegions(void);
static void DeleteLoadPending(void);
static void ExitLaptopMode(UINT32 uiMode);


void ExitLaptop(void)
{
	// exit is called due to message box, leave
	if (fExitDueToMessageBox)
	{
		fExitDueToMessageBox = FALSE;
		return;
	}

	if (DidGameJustStart())
	{
		SetMusicMode(MUSIC_LAPTOP);
	}
	else
	{
		// Restore to old stuff...
		SetMusicMode(MUSIC_RESTORE);
	}

	// Start ambients...
	BuildDayAmbientSounds();

	//if its raining, start the rain showers
	if (IsItRaining())
	{
		//Raise the volume to where it was
		guiRainLoop = PlayJA2Ambient(RAIN_1, MIDVOLUME, 0);
	}

	// release cursor
	FreeMouseCursor();

	// set the fact we are currently not in laptop, for rendering purposes
	fCurrentlyInLaptop = FALSE;

	//Deallocate, save data -- leaving laptop.
	SetRenderFlags(RENDER_FLAG_FULL);

	if (!fExitDuringLoad) ExitLaptopMode(guiCurrentLaptopMode);

	fExitDuringLoad  = FALSE;
	fLoadPendingFlag = FALSE;


	DeleteVideoObject(guiLAPTOP);
	DeleteVideoObject(guiLaptopBACKGROUND);
	DeleteVideoObject(guiTITLEBARLAPTOP);
	DeleteVideoObject(guiLIGHTS);
	DeleteVideoObject(guiTITLEBARICONS);
	DeleteVideoObject(guiEmailWarning);

	// destroy region for new mail icon
	CreateDestroyMouseRegionForNewMailIcon();

	// get rid of desktop
	DeleteDesktopBackground();

	if (MailToDelete != NULL)
	{
		MailToDelete = NULL;
		CreateDestroyDeleteNoticeMailButton();
	}
	if (fNewMailFlag)
	{
		fNewMailFlag = FALSE;
		CreateDestroyNewMailButton();
	}

	// get rid of minize button
	CreateDestroyMinimizeButtonForCurrentMode();

	//MSYS_SetCurrentCursor(CURSOR_NORMAL);
	gfEnterLapTop=TRUE;
	DeleteLapTopButtons();
	DeleteLapTopMouseRegions();
	gfShowBookmarks = FALSE;
	CreateDestoryBookMarkRegions();

	fNewWWW = TRUE;
	DeleteBookmark();
	DeleteLoadPending();
	fReDrawNewMailFlag = FALSE;

	//Since we are going to MapScreen, check the flag
	guiTacticalInterfaceFlags |= INTERFACE_MAPSCREEN;

	//pause the game because we dont want time to advance in the laptop
	UnPauseGame();
}


static void RenderLapTopImage(void)
{
	if (fMaximizingProgram || fMinizingProgram) return;

	BltVideoObject(FRAME_BUFFER, guiLAPTOP,           0, LAPTOP_X, LAPTOP_Y);
	BltVideoObject(FRAME_BUFFER, guiLaptopBACKGROUND, 1, 25,       23);

	MarkButtonsDirty();
}


static void RenderLaptop(void)
{
	if (fMaximizingProgram || fMinizingProgram)
	{
		gfShowBookmarks = FALSE;
		return;
	}

	UINT32 uiTempMode = 0;
	if (fLoadPendingFlag)
	{
		uiTempMode           = guiCurrentLaptopMode;
		guiCurrentLaptopMode = guiPreviousLaptopMode;
	}

	switch (guiCurrentLaptopMode)
	{
		case LAPTOP_MODE_NONE:                     DrawDeskTopBackground();   break;

		case LAPTOP_MODE_AIM:                      RenderAIM();               break;
		case LAPTOP_MODE_AIM_MEMBERS:              RenderAIMMembers();        break;
		case LAPTOP_MODE_AIM_MEMBERS_FACIAL_INDEX: RenderAimFacialIndex();    break;
		case LAPTOP_MODE_AIM_MEMBERS_SORTED_FILES: RenderAimSort();           break;
		case LAPTOP_MODE_AIM_MEMBERS_ARCHIVES:     RenderAimArchives();       break;
		case LAPTOP_MODE_AIM_POLICIES:             RenderAimPolicies();       break;
		case LAPTOP_MODE_AIM_LINKS:                RenderAimLinks();          break;
		case LAPTOP_MODE_AIM_HISTORY:              RenderAimHistory();        break;

		case LAPTOP_MODE_MERC:                     RenderMercs();             break;
		case LAPTOP_MODE_MERC_FILES:               RenderMercsFiles();        break;
		case LAPTOP_MODE_MERC_ACCOUNT:             RenderMercsAccount();      break;
		case LAPTOP_MODE_MERC_NO_ACCOUNT:          RenderMercsNoAccount();    break;

		case LAPTOP_MODE_BOBBY_R:                  RenderBobbyR();            break;
		case LAPTOP_MODE_BOBBY_R_GUNS:             RenderBobbyRGuns();        break;
		case LAPTOP_MODE_BOBBY_R_AMMO:             RenderBobbyRAmmo();        break;
		case LAPTOP_MODE_BOBBY_R_ARMOR:            RenderBobbyRArmour();      break;
		case LAPTOP_MODE_BOBBY_R_MISC:             RenderBobbyRMisc();        break;
		case LAPTOP_MODE_BOBBY_R_USED:             RenderBobbyRUsed();        break;
		case LAPTOP_MODE_BOBBY_R_MAILORDER:        RenderBobbyRMailOrder();   break;
		case LAPTOP_MODE_BOBBYR_SHIPMENTS:         RenderBobbyRShipments();   break;

		case LAPTOP_MODE_CHAR_PROFILE:             RenderCharProfile();       break;

		case LAPTOP_MODE_FLORIST:                  RenderFlorist();           break;
		case LAPTOP_MODE_FLORIST_FLOWER_GALLERY:   RenderFloristGallery();    break;
		case LAPTOP_MODE_FLORIST_ORDERFORM:        RenderFloristOrderForm();  break;
		case LAPTOP_MODE_FLORIST_CARD_GALLERY:     RenderFloristCards();      break;

		case LAPTOP_MODE_INSURANCE:                RenderInsurance();         break;
		case LAPTOP_MODE_INSURANCE_INFO:           RenderInsuranceInfo();     break;
		case LAPTOP_MODE_INSURANCE_CONTRACT:       RenderInsuranceContract(); break;
		case LAPTOP_MODE_INSURANCE_COMMENTS:       RenderInsuranceComments(); break;

		case LAPTOP_MODE_FUNERAL:                  RenderFuneral();           break;

		case LAPTOP_MODE_FINANCES:                 RenderFinances();          break;
		case LAPTOP_MODE_PERSONNEL:                RenderPersonnel();         break;
		case LAPTOP_MODE_HISTORY:                  RenderHistory();           break;
		case LAPTOP_MODE_FILES:                    RenderFiles();             break;
		case LAPTOP_MODE_EMAIL:                    RenderEmail();             break;

		case LAPTOP_MODE_WWW:                      DrawDeskTopBackground();   break;

		case LAPTOP_MODE_BROKEN_LINK:              RenderBrokenLink();        break;
	}

	if (guiCurrentLaptopMode >= LAPTOP_MODE_WWW)
	{
		// render program bar for www program
		RenderWWWProgramTitleBar();
	}

	if (fLoadPendingFlag)
	{
		guiCurrentLaptopMode = uiTempMode;
		return;
	}

	DisplayProgramBoundingBox(FALSE);

	// mark the buttons dirty at this point
	MarkButtonsDirty();
}


static void InitTitleBarMaximizeGraphics(const SGPVObject* uiBackgroundGraphic, const wchar_t* pTitle, const SGPVObject* uiIconGraphic, UINT16 usIconGraphicIndex);
static void SetSubSiteAsVisted(void);


static void EnterNewLaptopMode(void)
{
	static BOOLEAN fOldLoadFlag = FALSE;

	if (fExitingLaptopFlag) return;
	// cause flicker, as we are going to a new program/WEB page
	fFlickerHD = TRUE;

	// handle maximizing of programs
	UINT           prog;
	const wchar_t* title;
	UINT16         gfx_idx;
	switch (guiCurrentLaptopMode)
	{
		case LAPTOP_MODE_EMAIL:
			prog    = LAPTOP_PROGRAM_MAILER;
			title   = pLaptopTitles[0];
			gfx_idx = 0;
			break;

		case LAPTOP_MODE_FILES:
			prog    = LAPTOP_PROGRAM_FILES;
			title   = pLaptopTitles[1];
			gfx_idx = 2;
			break;

		case LAPTOP_MODE_PERSONNEL:
			prog    = LAPTOP_PROGRAM_PERSONNEL;
			title   = pLaptopTitles[2];
			gfx_idx = 3;
			break;

		case LAPTOP_MODE_FINANCES:
			prog    = LAPTOP_PROGRAM_FINANCES;
			title   = pLaptopTitles[3];
			gfx_idx = 5;
			break;

		case LAPTOP_MODE_HISTORY:
			prog    = LAPTOP_PROGRAM_HISTORY;
			title   = pLaptopTitles[4];
			gfx_idx = 4;
			break;

		case LAPTOP_MODE_NONE: goto do_nothing;

		default:
			prog    = LAPTOP_PROGRAM_WEB_BROWSER;
			title   = pWebTitle;
			gfx_idx = 1;
			break;
	}

	if (gLaptopProgramStates[prog] == LAPTOP_PROGRAM_MINIMIZED)
	{
		if (!fMaximizingProgram)
		{
			fInitTitle = TRUE;
			InitTitleBarMaximizeGraphics(guiTITLEBARLAPTOP, title, guiTITLEBARICONS, gfx_idx);
			ExitLaptopMode(guiPreviousLaptopMode);
		}
		fMaximizingProgram         = TRUE;
		bProgramBeingMaximized     = prog;
		gLaptopProgramStates[prog] = LAPTOP_PROGRAM_OPEN;
		return;
	}

do_nothing:
	if (fMaximizingProgram || fMinizingProgram) return;

	if (fOldLoadFlag && !fLoadPendingFlag)
	{
		fOldLoadFlag = FALSE;
	}
	else if (fLoadPendingFlag && !fOldLoadFlag)
	{
		ExitLaptopMode(guiPreviousLaptopMode);
		fOldLoadFlag = TRUE;
		return;
	}
	else if (fOldLoadFlag && fLoadPendingFlag)
	{
		return;
	}
	else
	{
		// do not exit previous mode if coming from sliding bar handler
		if (!fEnteredNewLapTopDueToHandleSlidingBars)
		{
			ExitLaptopMode(guiPreviousLaptopMode);
		}
	}

	if (guiCurrentWWWMode == LAPTOP_MODE_NONE && guiCurrentLaptopMode >= LAPTOP_MODE_WWW)
	{
		RenderLapTopImage();
		guiCurrentLaptopMode = LAPTOP_MODE_WWW;
	}
	else if (guiCurrentLaptopMode > LAPTOP_MODE_WWW)
	{
		if (guiPreviousLaptopMode < LAPTOP_MODE_WWW)
		{
			guiCurrentLaptopMode = guiCurrentWWWMode;
		}
		else
		{
			guiCurrentWWWMode = guiCurrentLaptopMode;
			giCurrentSubPage  = 0;
		}
	}

	if (guiCurrentLaptopMode >= LAPTOP_MODE_WWW)
	{
		RenderWWWProgramTitleBar();
		if (guiPreviousLaptopMode >= LAPTOP_MODE_WWW) gfShowBookmarks = FALSE;
	}

	//Initialize the new mode.
	switch (guiCurrentLaptopMode)
	{
		case LAPTOP_MODE_AIM:                      EnterAIM();               break;
		case LAPTOP_MODE_AIM_MEMBERS:              EnterAIMMembers();        break;
		case LAPTOP_MODE_AIM_MEMBERS_FACIAL_INDEX: EnterAimFacialIndex();    break;
		case LAPTOP_MODE_AIM_MEMBERS_SORTED_FILES: EnterAimSort();           break;
		case LAPTOP_MODE_AIM_MEMBERS_ARCHIVES:     EnterAimArchives();       break;
		case LAPTOP_MODE_AIM_POLICIES:             EnterAimPolicies();       break;
		case LAPTOP_MODE_AIM_LINKS:                EnterAimLinks();          break;
		case LAPTOP_MODE_AIM_HISTORY:              EnterAimHistory();        break;

		case LAPTOP_MODE_MERC:                     EnterMercs();             break;
		case LAPTOP_MODE_MERC_FILES:               EnterMercsFiles();        break;
		case LAPTOP_MODE_MERC_ACCOUNT:             EnterMercsAccount();      break;
		case LAPTOP_MODE_MERC_NO_ACCOUNT:          EnterMercsNoAccount();    break;

		case LAPTOP_MODE_BOBBY_R:                  EnterBobbyR();            break;
		case LAPTOP_MODE_BOBBY_R_GUNS:             EnterBobbyRGuns();        break;
		case LAPTOP_MODE_BOBBY_R_AMMO:             EnterBobbyRAmmo();        break;
		case LAPTOP_MODE_BOBBY_R_ARMOR:            EnterBobbyRArmour();      break;
		case LAPTOP_MODE_BOBBY_R_MISC:             EnterBobbyRMisc();        break;
		case LAPTOP_MODE_BOBBY_R_USED:             EnterBobbyRUsed();        break;
		case LAPTOP_MODE_BOBBY_R_MAILORDER:        EnterBobbyRMailOrder();   break;
		case LAPTOP_MODE_BOBBYR_SHIPMENTS:         EnterBobbyRShipments();   break;

		case LAPTOP_MODE_CHAR_PROFILE:             EnterCharProfile();       break;

		case LAPTOP_MODE_FLORIST:                  EnterFlorist();           break;
		case LAPTOP_MODE_FLORIST_FLOWER_GALLERY:   EnterFloristGallery();    break;
		case LAPTOP_MODE_FLORIST_ORDERFORM:        EnterFloristOrderForm();  break;
		case LAPTOP_MODE_FLORIST_CARD_GALLERY:     EnterFloristCards();      break;

		case LAPTOP_MODE_INSURANCE:                EnterInsurance();         break;
		case LAPTOP_MODE_INSURANCE_INFO:           EnterInsuranceInfo();     break;
		case LAPTOP_MODE_INSURANCE_CONTRACT:       EnterInsuranceContract(); break;
		case LAPTOP_MODE_INSURANCE_COMMENTS:       EnterInsuranceComments(); break;

		case LAPTOP_MODE_FUNERAL:                  EnterFuneral();           break;

		case LAPTOP_MODE_FINANCES:                 EnterFinances();          break;
		case LAPTOP_MODE_PERSONNEL:                EnterPersonnel();         break;
		case LAPTOP_MODE_HISTORY:                  EnterHistory();           break;
		case LAPTOP_MODE_FILES:                    EnterFiles();             break;
		case LAPTOP_MODE_EMAIL:                    EnterEmail();             break;

		case LAPTOP_MODE_BROKEN_LINK:              EnterBrokenLink();        break;
	}

	// first time using webbrowser in this laptop session
	if (fFirstTimeInLaptop && guiCurrentLaptopMode >= LAPTOP_MODE_WWW)
	{
		gfShowBookmarks    = TRUE;
		fFirstTimeInLaptop = FALSE;
	}

	if (!fLoadPendingFlag)
	{
		CreateDestroyMinimizeButtonForCurrentMode();
		guiPreviousLaptopMode = guiCurrentLaptopMode;
		SetSubSiteAsVisted();
	}

	DisplayProgramBoundingBox(TRUE);
}


static void HandleLapTopHandles(void)
{
	if (fLoadPendingFlag) return;

	if (fMaximizingProgram || fMinizingProgram) return;

	switch (guiCurrentLaptopMode)
	{
		case LAPTOP_MODE_AIM:                      HandleAIM();               break;
		case LAPTOP_MODE_AIM_MEMBERS:              HandleAIMMembers();        break;
		case LAPTOP_MODE_AIM_MEMBERS_FACIAL_INDEX: HandleAimFacialIndex();    break;
		case LAPTOP_MODE_AIM_MEMBERS_SORTED_FILES: HandleAimSort();           break;
		case LAPTOP_MODE_AIM_MEMBERS_ARCHIVES:     HandleAimArchives();       break;
		case LAPTOP_MODE_AIM_POLICIES:             HandleAimPolicies();       break;
		case LAPTOP_MODE_AIM_LINKS:                HandleAimLinks();          break;
		case LAPTOP_MODE_AIM_HISTORY:              HandleAimHistory();        break;

		case LAPTOP_MODE_MERC:                     HandleMercs();             break;
		case LAPTOP_MODE_MERC_FILES:               HandleMercsFiles();        break;
		case LAPTOP_MODE_MERC_ACCOUNT:             HandleMercsAccount();      break;
		case LAPTOP_MODE_MERC_NO_ACCOUNT:          HandleMercsNoAccount();    break;

		case LAPTOP_MODE_BOBBY_R:                  HandleBobbyR();            break;
		case LAPTOP_MODE_BOBBY_R_GUNS:             HandleBobbyRGuns();        break;
		case LAPTOP_MODE_BOBBY_R_AMMO:             HandleBobbyRAmmo();        break;
		case LAPTOP_MODE_BOBBY_R_ARMOR:            HandleBobbyRArmour();      break;
		case LAPTOP_MODE_BOBBY_R_MISC:             HandleBobbyRMisc();        break;
		case LAPTOP_MODE_BOBBY_R_USED:             HandleBobbyRUsed();        break;
		case LAPTOP_MODE_BOBBY_R_MAILORDER:        HandleBobbyRMailOrder();   break;
		case LAPTOP_MODE_BOBBYR_SHIPMENTS:         HandleBobbyRShipments();   break;

		case LAPTOP_MODE_CHAR_PROFILE:             HandleCharProfile();       break;

		case LAPTOP_MODE_FLORIST:                  HandleFlorist();           break;
		case LAPTOP_MODE_FLORIST_FLOWER_GALLERY:   HandleFloristGallery();    break;
		case LAPTOP_MODE_FLORIST_ORDERFORM:        HandleFloristOrderForm();  break;
		case LAPTOP_MODE_FLORIST_CARD_GALLERY:     HandleFloristCards();      break;

		case LAPTOP_MODE_INSURANCE:                HandleInsurance();         break;
		case LAPTOP_MODE_INSURANCE_INFO:           HandleInsuranceInfo();     break;
		case LAPTOP_MODE_INSURANCE_CONTRACT:       HandleInsuranceContract(); break;
		case LAPTOP_MODE_INSURANCE_COMMENTS:       HandleInsuranceComments(); break;

		case LAPTOP_MODE_FUNERAL:                  HandleFuneral();           break;

		case LAPTOP_MODE_FINANCES:                 HandleFinances();          break;
		case LAPTOP_MODE_PERSONNEL:                HandlePersonnel();         break;
		case LAPTOP_MODE_HISTORY:                  HandleHistory();           break;
		case LAPTOP_MODE_FILES:                    HandleFiles();             break;
		case LAPTOP_MODE_EMAIL:                    HandleEmail();             break;

		case LAPTOP_MODE_BROKEN_LINK:              HandleBrokenLink();        break;
	}
}


static void CheckIfNewWWWW(void);
static void CheckMarkButtonsDirtyFlag(void);
static void CreateLaptopButtons(void);
static void DisplayBookMarks(void);
static void DisplayLoadPending(void);
static void DisplayTaskBarIcons(void);
static void DisplayWebBookMarkNotify(void);
static void FlickerHDLight(void);
static void HandleSlidingTitleBar(void);
static void HandleWWWSubSites(void);
static void PostButtonRendering(void);
static void ShouldNewMailBeDisplayed(void);
static void ShowLights(void);
static void UpdateStatusOfDisplayingBookMarks(void);


UINT32 LaptopScreenHandle()
{
	//User just changed modes.  This is determined by the button callbacks
	//created in LaptopScreenInit()

	// just entered
	if (gfEnterLapTop)
	{
		EnterLaptop();
		CreateLaptopButtons();
		gfEnterLapTop = FALSE;
	}

	if (gfStartMapScreenToLaptopTransition)
	{ //Everything is set up to start the transition animation.
		SetCurrentCursorFromDatabase(VIDEO_NO_CURSOR);
		//Step 1:  Build the laptop image into the save buffer.
		gfStartMapScreenToLaptopTransition = FALSE;
		RestoreBackgroundRects();
		RenderLapTopImage();
		RenderLaptop();
		RenderButtons();
		PrintDate();
		PrintBalance();
		PrintNumberOnTeam();
		ShowLights();

		//Step 2:  The mapscreen image is in the EXTRABUFFER, and laptop is in the SAVEBUFFER
		//         Start transitioning the screen.
		SGPRect DstRect;
		DstRect.iLeft   = 0;
		DstRect.iTop    = 0;
		DstRect.iRight  = SCREEN_WIDTH;
		DstRect.iBottom = SCREEN_HEIGHT;
		const UINT32 uiTimeRange = 1000;
		INT32 iPercentage     = 0;
		INT32 iRealPercentage = 0;
		const UINT32 uiStartTime = GetClock();
		BltVideoSurface(guiSAVEBUFFER, FRAME_BUFFER,   0, 0, NULL);
		BltVideoSurface(FRAME_BUFFER,  guiEXTRABUFFER, 0, 0, NULL);
		PlayJA2SampleFromFile("SOUNDS/Laptop power up (8-11).wav", HIGHVOLUME, 1, MIDDLEPAN);
		while (iRealPercentage < 100)
		{
			const UINT32 uiCurrTime = GetClock();
			iPercentage = (uiCurrTime-uiStartTime) * 100 / uiTimeRange;
			iPercentage = min(iPercentage, 100);

			iRealPercentage = iPercentage;

			//Factor the percentage so that it is modified by a gravity falling acceleration effect.
			const INT32 iFactor = (iPercentage - 50) * 2;
			if (iPercentage < 50)
			{
				iPercentage += iPercentage         * iFactor * 0.01 + 0.5;
			}
			else
			{
				iPercentage += (100 - iPercentage) * iFactor * 0.01 + 0.5;
			}

			//Mapscreen source rect
			SGPRect SrcRect1;
			SrcRect1.iLeft   =                 464 * iPercentage / 100;
			SrcRect1.iRight  = SCREEN_WIDTH  - 163 * iPercentage / 100;
			SrcRect1.iTop    =                 417 * iPercentage / 100;
			SrcRect1.iBottom = SCREEN_HEIGHT -  55 * iPercentage / 100;
			//Laptop source rect
			INT32 iScalePercentage;
			if (iPercentage < 99)
			{
				iScalePercentage = 10000 / (100 - iPercentage);
			}
			else
			{
				iScalePercentage = 5333;
			}
			const INT32 iWidth  = 12 * iScalePercentage / 100;
			const INT32 iHeight =  9 * iScalePercentage / 100;
			const INT32 iX      = 472 - (472 - 320) * iScalePercentage / 5333;
			const INT32 iY      = 424 - (424 - 240) * iScalePercentage / 5333;

			SGPRect SrcRect2;
			SrcRect2.iLeft   = iX             - iWidth / 2;
			SrcRect2.iRight  = SrcRect2.iLeft + iWidth;
			SrcRect2.iTop    = iY             - iHeight / 2;
			SrcRect2.iBottom = SrcRect2.iTop  + iHeight;
			//SrcRect2.iLeft = 464 - 464 * iScalePercentage / 100;
			//SrcRect2.iRight = 477 + 163 * iScalePercentage / 100;
			//SrcRect2.iTop = 417 - 417 * iScalePercentage / 100;
			//SrcRect2.iBottom = 425 + 55 * iScalePercentage / 100;

			//BltStretchVideoSurface(FRAME_BUFFER, guiEXTRABUFFER, &SrcRect1, &DstRect);

			//SetFont( FONT10ARIAL );
			//SetFontForeground( FONT_YELLOW );
			//SetFontShadow( FONT_NEARBLACK );
			//mprintf( 10, 10, L"%d -> %d", iRealPercentage, iPercentage );
			//{ SGPVSurface::Lock(FRAME_BUFFER);
			//	SetClippingRegionAndImageWidth(l.Pitch(), 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
			//	UINT8* const pDestBuf = l.Buffer<UINT8>();
			//	RectangleDraw(TRUE, SrcRect1.iLeft, SrcRect1.iTop, SrcRect1.iRight, SrcRect1.iBottom, Get16BPPColor(FROMRGB(255, 100, 0)), pDestBuf);
			//	RectangleDraw(TRUE, SrcRect2.iLeft, SrcRect2.iTop, SrcRect2.iRight, SrcRect2.iBottom, Get16BPPColor(FROMRGB(100, 255, 0)), pDestBuf);
			//}

			BltStretchVideoSurface(FRAME_BUFFER, guiSAVEBUFFER, &DstRect, &SrcRect2);
			InvalidateScreen();
			RefreshScreen();
		}
		fReDrawScreenFlag = TRUE;
	}

	//DO NOT MOVE THIS FUNCTION CALL!!!

	//This determines if the help screen should be active
	if (ShouldTheHelpScreenComeUp(HELP_SCREEN_LAPTOP, FALSE))
	{
		// handle the help screen
		HelpScreenHandler();
		return LAPTOP_SCREEN;
	}

	RestoreBackgroundRects();

	// lock cursor to screen
	RestrictMouseCursor(&LaptopScreenRect);

	// handle animated cursors
	HandleAnimatedCursors();
	// Deque all game events
	DequeAllGameEvents();

	// handle sub sites..like BR Guns, BR Ammo, Armour, Misc...for WW Wait..since they are not true sub pages
	// and are not individual sites
	HandleWWWSubSites();
	UpdateStatusOfDisplayingBookMarks();

	// check if we need to reset new WWW mode
	CheckIfNewWWWW();

	if (guiCurrentLaptopMode != guiPreviousLaptopMode)
	{
		if (guiCurrentLaptopMode <=LAPTOP_MODE_WWW) fLoadPendingFlag = FALSE;

		if (!fMaximizingProgram && !fMinizingProgram)
		{
			if (guiCurrentLaptopMode <= LAPTOP_MODE_WWW)
			{
				EnterNewLaptopMode();
				if (!fMaximizingProgram && !fMinizingProgram)
				{
					guiPreviousLaptopMode = guiCurrentLaptopMode;
				}
			}
			else
			{
				if (!fLoadPendingFlag)
				{
					EnterNewLaptopMode();
					guiPreviousLaptopMode = guiCurrentLaptopMode;
				}
			}
		}
	}
	if (fPausedReDrawScreenFlag)
	{
		fReDrawScreenFlag       = TRUE;
		fPausedReDrawScreenFlag = FALSE;
	}

	if (fReDrawScreenFlag)
	{
		RenderLapTopImage();
		RenderLaptop();
	}

	// are we about to leave laptop
	if (fExitingLaptopFlag)
	{
		if (fLoadPendingFlag)
		{
			fLoadPendingFlag = FALSE;
			fExitDuringLoad  = TRUE;
		}
		LeaveLapTopScreen();
	}

	if (!fExitingLaptopFlag)
	{
		// handle handles for laptop input stream
		HandleLapTopHandles();
	}

	// get keyboard input, handle it
	GetLaptopKeyboardInput();

	// check to see if new mail box needs to be displayed
	DisplayNewMailBox();
	CreateDestroyNewMailButton();

	// create various mouse regions that are global to laptop system
	CreateDestoryBookMarkRegions();

	// check to see if buttons marked dirty
	CheckMarkButtonsDirtyFlag();

	// check to see if new mail box needs to be displayed
	ShouldNewMailBeDisplayed();

	// check to see if new mail box needs to be displayed
	ReDrawNewMailBox();

	// look for unread email
	LookForUnread();
	//Handle keyboard shortcuts...

	if (!fLoadPendingFlag || fNewMailFlag)
	{
		// render buttons marked dirty
		RenderButtons();
	}

	// check to see if bookmarks need to be displayed
	if (gfShowBookmarks)
	{
		if (fExitingLaptopFlag)
		{
			gfShowBookmarks = FALSE;
		}
		else
		{
			DisplayBookMarks();
		}
	}

	// check to see if laod pending flag is set
	DisplayLoadPending();

	// check if we are showing message?
	DisplayWebBookMarkNotify();

	if (fReDrawPostButtonRender)
	{
		// rendering AFTER buttons and button text
		if (!fMaximizingProgram && !fMinizingProgram)
		{
			PostButtonRendering();
		}
	}

	PrintDate();
	PrintBalance();
	PrintNumberOnTeam();
	DisplayTaskBarIcons();

	// handle if we are maximizing a program from a minimized state or vice versa
	HandleSlidingTitleBar();

	// flicker HD light as nessacary
	FlickerHDLight();

	// display power and HD lights
	ShowLights();

	// render frame rate
	DisplayFrameRate();

	// invalidate screen if redrawn
	if (fReDrawScreenFlag)
	{
		InvalidateScreen();
		fReDrawScreenFlag = FALSE;
	}

	ExecuteVideoOverlays();

	SaveBackgroundRects();
	RenderFastHelp();

	// ex SAVEBUFFER queue
	ExecuteBaseDirtyRectQueue();
	ResetInterface();
	EndFrameBufferRender();
	return (LAPTOP_SCREEN);
}


static void ExitLaptopMode(UINT32 uiMode)
{
	// Deallocate the previous mode that you were in.
	switch (uiMode)
	{
		case LAPTOP_MODE_AIM:                      ExitAIM();               break;
		case LAPTOP_MODE_AIM_MEMBERS:              ExitAIMMembers();        break;
		case LAPTOP_MODE_AIM_MEMBERS_FACIAL_INDEX: ExitAimFacialIndex();    break;
		case LAPTOP_MODE_AIM_MEMBERS_SORTED_FILES: ExitAimSort();           break;
		case LAPTOP_MODE_AIM_MEMBERS_ARCHIVES:     ExitAimArchives();       break;
		case LAPTOP_MODE_AIM_POLICIES:             ExitAimPolicies();       break;
		case LAPTOP_MODE_AIM_LINKS:                ExitAimLinks();          break;
		case LAPTOP_MODE_AIM_HISTORY:              ExitAimHistory();        break;

		case LAPTOP_MODE_MERC:                     ExitMercs();             break;
		case LAPTOP_MODE_MERC_FILES:               ExitMercsFiles();        break;
		case LAPTOP_MODE_MERC_ACCOUNT:             ExitMercsAccount();      break;
		case LAPTOP_MODE_MERC_NO_ACCOUNT:          ExitMercsNoAccount();    break;

		case LAPTOP_MODE_BOBBY_R:                  ExitBobbyR();            break;
		case LAPTOP_MODE_BOBBY_R_GUNS:             ExitBobbyRGuns();        break;
		case LAPTOP_MODE_BOBBY_R_AMMO:             ExitBobbyRAmmo();        break;
		case LAPTOP_MODE_BOBBY_R_ARMOR:            ExitBobbyRArmour();      break;
		case LAPTOP_MODE_BOBBY_R_MISC:             ExitBobbyRMisc();        break;
		case LAPTOP_MODE_BOBBY_R_USED:             ExitBobbyRUsed();        break;
		case LAPTOP_MODE_BOBBY_R_MAILORDER:        ExitBobbyRMailOrder();   break;
		case LAPTOP_MODE_BOBBYR_SHIPMENTS:         ExitBobbyRShipments();   break;

		case LAPTOP_MODE_CHAR_PROFILE:             ExitCharProfile();       break;

		case LAPTOP_MODE_FLORIST:                  ExitFlorist();           break;
		case LAPTOP_MODE_FLORIST_FLOWER_GALLERY:   ExitFloristGallery();    break;
		case LAPTOP_MODE_FLORIST_ORDERFORM:        ExitFloristOrderForm();  break;
		case LAPTOP_MODE_FLORIST_CARD_GALLERY:     ExitFloristCards();      break;

		case LAPTOP_MODE_INSURANCE:                ExitInsurance();         break;
		case LAPTOP_MODE_INSURANCE_INFO:           ExitInsuranceInfo();     break;
		case LAPTOP_MODE_INSURANCE_CONTRACT:       ExitInsuranceContract(); break;
		case LAPTOP_MODE_INSURANCE_COMMENTS:       ExitInsuranceComments(); break;

		case LAPTOP_MODE_FUNERAL:                  ExitFuneral();           break;

		case LAPTOP_MODE_FINANCES:                 ExitFinances();          break;
		case LAPTOP_MODE_PERSONNEL:                ExitPersonnel();         break;
		case LAPTOP_MODE_HISTORY:                  ExitHistory();           break;
		case LAPTOP_MODE_FILES:                    ExitFiles();             break;
		case LAPTOP_MODE_EMAIL:                    ExitEmail();             break;
		case LAPTOP_MODE_BROKEN_LINK:              ExitBrokenLink();        break;
	}

	if (uiMode != LAPTOP_MODE_NONE && uiMode < LAPTOP_MODE_WWW)
	{
		CreateDestroyMinimizeButtonForCurrentMode();
	}
}


static void MakeButton(UINT idx, INT16 y, GUI_CALLBACK click, INT8 off_x, const wchar_t* text, const wchar_t* help_text)
{
	INT32 btn = QuickCreateButtonImg("LAPTOP/buttonsforlaptop.sti", -1, idx, -1, idx + 8, -1, 29, y, MSYS_PRIORITY_HIGH, click);
	gLaptopButton[idx] = btn;
	SetButtonFastHelpText(btn, help_text);
	SpecifyButtonTextOffsets(btn, off_x, 11, TRUE);
	SpecifyGeneralButtonTextAttributes(btn, text, FONT10ARIAL, 2, 0);
	SetButtonCursor(btn, CURSOR_LAPTOP_SCREEN);
}


static void BtnOnCallback(                GUI_BUTTON* btn, INT32 reason);
static void EmailRegionButtonCallback(    GUI_BUTTON* btn, INT32 reason);
static void FilesRegionButtonCallback(    GUI_BUTTON* btn, INT32 reason);
static void FinancialRegionButtonCallback(GUI_BUTTON* btn, INT32 reason);
static void HistoryRegionButtonCallback(  GUI_BUTTON* btn, INT32 reason);
static void PersonnelRegionButtonCallback(GUI_BUTTON* btn, INT32 reason);
static void WWWRegionButtonCallback(      GUI_BUTTON* btn, INT32 reason);


static void CreateLaptopButtons(void)
{
	MakeButton(0,  66, EmailRegionButtonCallback,     30, pLaptopIcons[0], gzLaptopHelpText[LAPTOP_BN_HLP_TXT_VIEW_EMAIL]);
	MakeButton(1,  98, WWWRegionButtonCallback,       30, pLaptopIcons[1], gzLaptopHelpText[LAPTOP_BN_HLP_TXT_BROWSE_VARIOUS_WEB_SITES]);
	MakeButton(2, 130, FilesRegionButtonCallback,     30, pLaptopIcons[5], gzLaptopHelpText[LAPTOP_BN_HLP_TXT_VIEW_FILES_AND_EMAIL_ATTACHMENTS]);
	MakeButton(3, 194, PersonnelRegionButtonCallback, 30, pLaptopIcons[3], gzLaptopHelpText[LAPTOP_BN_HLP_TXT_VIEW_TEAM_INFO]);
	MakeButton(4, 162, HistoryRegionButtonCallback,   30, pLaptopIcons[4], gzLaptopHelpText[LAPTOP_BN_HLP_TXT_READ_LOG_OF_EVENTS]);
	MakeButton(5, 241, FinancialRegionButtonCallback, 30, pLaptopIcons[2], gzLaptopHelpText[LAPTOP_BN_HLP_TXT_VIEW_FINANCIAL_SUMMARY_AND_HISTORY]);
	MakeButton(6, 378, BtnOnCallback,                 25, pLaptopIcons[6], gzLaptopHelpText[LAPTOP_BN_HLP_TXT_CLOSE_LAPTOP]);
}


static void DeleteLapTopButtons(void)
{
	for (UINT32 i = 0; i < 7; ++i) RemoveButton(gLaptopButton[i]);
}


static BOOLEAN HandleExit(void);


static void BtnOnCallback(GUI_BUTTON *btn, INT32 reason)
{
	if (reason & MSYS_CALLBACK_REASON_LBUTTON_UP)
	{
		if (HandleExit()) fExitingLaptopFlag = TRUE;
	}
}


static BOOLEAN ExitLaptopDone(void);


BOOLEAN LeaveLapTopScreen(void)
{
	if (ExitLaptopDone())
	{
		SetLaptopExitScreen(MAP_SCREEN);

		if (gfAtLeastOneMercWasHired)
		{
			if (LaptopSaveInfo.gfNewGameLaptop)
			{
				LaptopSaveInfo.gfNewGameLaptop = FALSE;
				fExitingLaptopFlag = TRUE;
				InitNewGame(FALSE);
				gfDontStartTransitionFromLaptop = TRUE;
				return TRUE;
			}
		}
		else
		{
			gfDontStartTransitionFromLaptop = TRUE;
		}

		SetPendingNewScreen(guiExitScreen);

		if (!gfDontStartTransitionFromLaptop)
		{
			gfDontStartTransitionFromLaptop = TRUE;
			SetCurrentCursorFromDatabase(VIDEO_NO_CURSOR);
			//Step 1:  Build the laptop image into the save buffer.
			RestoreBackgroundRects();
			RenderLapTopImage();
			RenderLaptop();
			RenderButtons();
			PrintDate();
			PrintBalance();
			PrintNumberOnTeam();
			ShowLights();

			//Step 2:  The mapscreen image is in the EXTRABUFFER, and laptop is in the SAVEBUFFER
			//         Start transitioning the screen.
			SGPRect DstRect;
			DstRect.iLeft   = 0;
			DstRect.iTop    = 0;
			DstRect.iRight  = SCREEN_WIDTH;
			DstRect.iBottom = SCREEN_HEIGHT;
			const UINT32 uiTimeRange = 1000;
			INT32 iPercentage     = 100;
			INT32 iRealPercentage = 100;
			const UINT32 uiStartTime = GetClock();
			BltVideoSurface(guiSAVEBUFFER, FRAME_BUFFER, 0, 0, NULL);
			PlayJA2SampleFromFile("SOUNDS/Laptop power down (8-11).wav", HIGHVOLUME, 1, MIDDLEPAN);
			while (iRealPercentage > 0)
			{
				BltVideoSurface(FRAME_BUFFER, guiEXTRABUFFER, 0, 0, NULL);

				const UINT32 uiCurrTime = GetClock();
				iPercentage = (uiCurrTime-uiStartTime) * 100 / uiTimeRange;
				iPercentage = min(iPercentage, 100);
				iPercentage = 100 - iPercentage;

				iRealPercentage = iPercentage;

				//Factor the percentage so that it is modified by a gravity falling acceleration effect.
				const INT32 iFactor = (iPercentage - 50) * 2;
				if (iPercentage < 50)
				{
					iPercentage += iPercentage       * iFactor * 0.01 + 0.5;
				}
				else
				{
					iPercentage += (100-iPercentage) * iFactor * 0.01 + 0.5;
				}

				//Mapscreen source rect
				SGPRect SrcRect1;
				SrcRect1.iLeft   =                 464 * iPercentage / 100;
				SrcRect1.iRight  = SCREEN_WIDTH  - 163 * iPercentage / 100;
				SrcRect1.iTop    =                 417 * iPercentage / 100;
				SrcRect1.iBottom = SCREEN_HEIGHT -  55 * iPercentage / 100;
				//Laptop source rect
				INT32 iScalePercentage;
				if (iPercentage < 99)
				{
					iScalePercentage = 10000 / (100-iPercentage);
				}
				else
				{
					iScalePercentage = 5333;
				}
				const INT32 iWidth  = 12 * iScalePercentage / 100;
				const INT32 iHeight =  9 * iScalePercentage / 100;
				const INT32 iX = 472 - (472 - 320) * iScalePercentage / 5333;
				const INT32 iY = 424 - (424 - 240) * iScalePercentage / 5333;

				SGPRect SrcRect2;
				SrcRect2.iLeft   = iX             - iWidth / 2;
				SrcRect2.iRight  = SrcRect2.iLeft + iWidth;
				SrcRect2.iTop    = iY             - iHeight / 2;
				SrcRect2.iBottom = SrcRect2.iTop  + iHeight;
				//SrcRect2.iLeft = 464 - 464 * iScalePercentage / 100;
				//SrcRect2.iRight = 477 + 163 * iScalePercentage / 100;
				//SrcRect2.iTop = 417 - 417 * iScalePercentage / 100;
				//SrcRect2.iBottom = 425 + 55 * iScalePercentage / 100;

				//BltStretchVideoSurface(FRAME_BUFFER, guiEXTRABUFFER, &SrcRect1, &DstRect);

				//SetFont( FONT10ARIAL );
				//SetFontForeground( FONT_YELLOW );
				//SetFontShadow( FONT_NEARBLACK );
				//mprintf( 10, 10, L"%d -> %d", iRealPercentage, iPercentage );
				//{ SGPVSurface::Lock(FRAME_BUFFER);
				//	SetClippingRegionAndImageWidth(l.Pitch(), 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
				//	UINT8* const pDestBuf = l.Buffer<UINT8>();
				//	RectangleDraw(TRUE, SrcRect1.iLeft, SrcRect1.iTop, SrcRect1.iRight, SrcRect1.iBottom, Get16BPPColor(FROMRGB(255, 100, 0)), pDestBuf);
				//	RectangleDraw(TRUE, SrcRect2.iLeft, SrcRect2.iTop, SrcRect2.iRight, SrcRect2.iBottom, Get16BPPColor(FROMRGB(100, 255, 0)), pDestBuf);
				//}

				BltStretchVideoSurface(FRAME_BUFFER, guiSAVEBUFFER, &DstRect, &SrcRect2);
				InvalidateScreen();
				RefreshScreen();
			}
		}
	}
	return TRUE;
}


static BOOLEAN HandleExit(void)
{
	// new game, send email
	if (LaptopSaveInfo.gfNewGameLaptop)
	{
		// Set an event to send this email (day 2 8:00-12:00)
		if (!LaptopSaveInfo.fIMPCompletedFlag && !LaptopSaveInfo.fSentImpWarningAlready)
		{
			AddFutureDayStrategicEvent(EVENT_HAVENT_MADE_IMP_CHARACTER_EMAIL, (8 + Random(4)) * 60, 0, 1);
			fExitingLaptopFlag = TRUE;
			return FALSE;
		}
	}

	return TRUE;
}

void HaventMadeImpMercEmailCallBack()
{
	//if the player STILL hasnt made an imp merc yet
	if (!LaptopSaveInfo.fIMPCompletedFlag && !LaptopSaveInfo.fSentImpWarningAlready)
	{
		LaptopSaveInfo.fSentImpWarningAlready = TRUE;
		AddEmail(IMP_EMAIL_AGAIN,IMP_EMAIL_AGAIN_LENGTH, 1, GetWorldTotalMin());
	}
}


static void CreateLapTopMouseRegions(void)
{
	// the entire laptop display region
	MSYS_DefineRegion(&gLapTopScreenRegion, LaptopScreenRect.iLeft, LaptopScreenRect.iTop, LaptopScreenRect.iRight, LaptopScreenRect.iBottom, MSYS_PRIORITY_NORMAL + 1, CURSOR_LAPTOP_SCREEN, MSYS_NO_CALLBACK, LapTopScreenCallBack);
}


static void DeleteLapTopMouseRegions(void)
{
	MSYS_RemoveRegion(&gLapTopScreenRegion);
}


static void UpdateListToReflectNewProgramOpened(INT32 iOpenedProgram);


static void FinancialRegionButtonCallback(GUI_BUTTON* btn, INT32 reason)
{
	if (reason & MSYS_CALLBACK_REASON_LBUTTON_UP)
	{
		if (gfShowBookmarks)
		{
			gfShowBookmarks = FALSE;
			fReDrawScreenFlag = TRUE;
		}
		guiCurrentLaptopMode = LAPTOP_MODE_FINANCES;

		UpdateListToReflectNewProgramOpened(LAPTOP_PROGRAM_FINANCES);
	}
}


static void PersonnelRegionButtonCallback(GUI_BUTTON *btn, INT32 reason)
{
	if (reason & MSYS_CALLBACK_REASON_LBUTTON_UP)
	{
		guiCurrentLaptopMode = LAPTOP_MODE_PERSONNEL;
		if (gfShowBookmarks)
		{
			gfShowBookmarks = FALSE;
			fReDrawScreenFlag = TRUE;
		}
		gfShowBookmarks = FALSE;

		UpdateListToReflectNewProgramOpened(LAPTOP_PROGRAM_PERSONNEL);
	}
}


static void EmailRegionButtonCallback(GUI_BUTTON *btn, INT32 reason)
{
	if (reason & MSYS_CALLBACK_REASON_LBUTTON_UP)
	{
		guiCurrentLaptopMode = LAPTOP_MODE_EMAIL;
		gfShowBookmarks = FALSE;
		UpdateListToReflectNewProgramOpened(LAPTOP_PROGRAM_MAILER);
		fReDrawScreenFlag = TRUE;
	}
}


static void WWWRegionButtonCallback(GUI_BUTTON *btn, INT32 reason)
{
	if (reason & MSYS_CALLBACK_REASON_LBUTTON_UP)
	{
		// reset show bookmarks
		if (guiCurrentLaptopMode < LAPTOP_MODE_WWW)
		{
			gfShowBookmarks = FALSE;
			fShowBookmarkInfo = TRUE;
		}
		else
		{
			gfShowBookmarks = !gfShowBookmarks;
		}

		if (fNewWWW)
		{
			// no longer a new WWW mode
			fNewWWW = FALSE;

			// make sure program is maximized
			if (gLaptopProgramStates[LAPTOP_PROGRAM_WEB_BROWSER] == LAPTOP_PROGRAM_OPEN)
			{
				RenderLapTopImage();
				DrawDeskTopBackground();
			}
		}

		guiCurrentLaptopMode = (guiCurrentWWWMode == LAPTOP_MODE_NONE ? LAPTOP_MODE_WWW : guiCurrentWWWMode);

		UpdateListToReflectNewProgramOpened(LAPTOP_PROGRAM_WEB_BROWSER);
		fReDrawScreenFlag = TRUE;
	}
	else if (reason & MSYS_CALLBACK_REASON_RBUTTON_UP)
	{
		guiCurrentLaptopMode = (guiCurrentWWWMode == LAPTOP_MODE_NONE ? LAPTOP_MODE_WWW : guiCurrentWWWMode);
		fReDrawScreenFlag = TRUE;
	}
}


static void HistoryRegionButtonCallback(GUI_BUTTON *btn, INT32 reason)
{
	if (reason & MSYS_CALLBACK_REASON_LBUTTON_UP)
	{
		if (gfShowBookmarks)
		{
			gfShowBookmarks = FALSE;
		}
		guiCurrentLaptopMode = LAPTOP_MODE_HISTORY;
		UpdateListToReflectNewProgramOpened(LAPTOP_PROGRAM_HISTORY);
		gfShowBookmarks = FALSE;
		fReDrawScreenFlag = TRUE;
	}
}


static void FilesRegionButtonCallback(GUI_BUTTON *btn, INT32 reason)
{
	if (reason & MSYS_CALLBACK_REASON_LBUTTON_UP)
	{
		if (gfShowBookmarks)
		{
			gfShowBookmarks = FALSE;
		}
		guiCurrentLaptopMode = LAPTOP_MODE_FILES;
		UpdateListToReflectNewProgramOpened(LAPTOP_PROGRAM_FILES);
		fReDrawScreenFlag = TRUE;
	}
}


static void InitBookMarkList(void)
{
	// sets bookmark list to -1
	memset(LaptopSaveInfo.iBookMarkList, -1, sizeof(LaptopSaveInfo.iBookMarkList));
}


void SetBookMark(INT32 iBookId)
{
	// find first empty spot, set to iBookId
	INT32 i;
	for (i = 0; LaptopSaveInfo.iBookMarkList[i] != -1; ++i)
	{
		// move through list until empty
		if (LaptopSaveInfo.iBookMarkList[i] == iBookId)
		{
			// found it, return
			return;
		}
	}
	LaptopSaveInfo.iBookMarkList[i] = iBookId;
}


static void LoadBookmark(void)
try
{
	// grab download bars too
	guiDOWNLOADTOP = AddVideoObjectFromFile("LAPTOP/downloadtop.sti");
	guiDOWNLOADMID = AddVideoObjectFromFile("LAPTOP/downloadmid.sti");
	guiDOWNLOADBOT = AddVideoObjectFromFile("LAPTOP/downloadbot.sti");
	guiBOOKMARK    = AddVideoObjectFromFile("LAPTOP/webpages.sti");
	guiBOOKHIGH    = AddVideoObjectFromFile("LAPTOP/hilite.sti");
}
catch (...) { /* XXX ignore */ }


static void DisplayBookMarks(void)
{
	// will look at bookmarklist and set accordingly

	// check if we are maximizing or minimizing.. if so, do not display
	if (fMaximizingProgram || fMinizingProgram) return;

	SetFont(BOOK_FONT);
	SetFontBackground(FONT_BLACK);
	SetFontShadow(NO_SHADOW);

	SetFontDestBuffer(FRAME_BUFFER, BOOK_X, BOOK_TOP_Y, BOOK_X + BOOK_WIDTH - 10, SCREEN_HEIGHT);

	// blt in book mark background
	INT32 i;
	for (i = 1; LaptopSaveInfo.iBookMarkList[i - 1] != -1; ++i)
	{
		const INT32 y = BOOK_TOP_Y + i * (BOOK_HEIGHT + 6) + 6;

		const SGPVObject* const vo = (iHighLightBookLine == i - 1 ? guiBOOKHIGH : guiBOOKMARK);
		BltVideoObject(FRAME_BUFFER, vo, 0, BOOK_X, y);

		SetFontForeground(iHighLightBookLine == i - 1 ? FONT_WHITE : FONT_BLACK);
		INT16 sX;
		INT16 sY;
		FindFontCenterCoordinates(BOOK_X + 3, y + 2, BOOK_WIDTH - 3, BOOK_HEIGHT + 6, pBookMarkStrings[LaptopSaveInfo.iBookMarkList[i - 1]], BOOK_FONT, &sX, &sY);
		MPrint(sX, sY, pBookMarkStrings[LaptopSaveInfo.iBookMarkList[i - 1]]);
	}

	// blit one more
	const INT32 y = BOOK_TOP_Y + i * (BOOK_HEIGHT + 6) + 6;

	const SGPVObject* const vo = (iHighLightBookLine == i - 1 ? guiBOOKHIGH : guiBOOKMARK);
	BltVideoObject(FRAME_BUFFER, vo, 0, BOOK_X, y);

	SetFontForeground(iHighLightBookLine == i - 1 ? FONT_WHITE : FONT_BLACK);
	INT16 sX;
	INT16 sY;
	FindFontCenterCoordinates(BOOK_X + 3, y + 2, BOOK_WIDTH - 3, BOOK_HEIGHT + 6, pBookMarkStrings[CANCEL_STRING], BOOK_FONT, &sX, &sY);
	MPrint(sX, sY, pBookMarkStrings[CANCEL_STRING]);
	i++;

	SetFontDestBuffer(FRAME_BUFFER, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	SetFontShadow(DEFAULT_SHADOW);

	InvalidateRegion(BOOK_X, BOOK_TOP_Y + i * BOOK_HEIGHT + 12, BOOK_X + BOOK_WIDTH, BOOK_TOP_Y + (i + 1) * BOOK_HEIGHT + 16);
	InvalidateRegion(BOOK_X, BOOK_TOP_Y,                        BOOK_X + BOOK_WIDTH, BOOK_TOP_Y + (i + 6) * BOOK_HEIGHT + 16);
}


static void DeleteBookmark(void)
{
	DeleteVideoObject(guiBOOKHIGH);
	DeleteVideoObject(guiBOOKMARK);
	DeleteVideoObject(guiDOWNLOADTOP);
	DeleteVideoObject(guiDOWNLOADMID);
	DeleteVideoObject(guiDOWNLOADBOT);
}


static void BookmarkCallBack(MOUSE_REGION* pRegion, INT32 iReason);
static void BookmarkMvtCallBack(MOUSE_REGION* pRegion, INT32 iReason);


static void CreateBookMarkMouseRegions(void)
{
	INT32 i;
	// creates regions based on number of entries
	for (i = 0; LaptopSaveInfo.iBookMarkList[i] != -1; ++i)
	{
		const UINT16 y = BOOK_TOP_Y + (i + 1) * (BOOK_HEIGHT + 6) + 6;
		MSYS_DefineRegion(&gBookmarkMouseRegions[i], BOOK_X, y, BOOK_X + BOOK_WIDTH, y + BOOK_HEIGHT + 6, MSYS_PRIORITY_HIGHEST - 2, CURSOR_LAPTOP_SCREEN, BookmarkMvtCallBack, BookmarkCallBack);
		MSYS_SetRegionUserData(&gBookmarkMouseRegions[i], 0, i);
		MSYS_SetRegionUserData(&gBookmarkMouseRegions[i], 1, 0);

		SetRegionFastHelpText(&gBookmarkMouseRegions[i], gzLaptopHelpText[BOOKMARK_TEXT_ASSOCIATION_OF_INTERNATION_MERCENARIES + LaptopSaveInfo.iBookMarkList[i]]);
	}
	// now add one more for the cancel button
	const UINT16 y = BOOK_TOP_Y + (i + 1) * (BOOK_HEIGHT + 6) + 6;
	MSYS_DefineRegion(&gBookmarkMouseRegions[i], BOOK_X, y, BOOK_X + BOOK_WIDTH, y + BOOK_HEIGHT + 6, MSYS_PRIORITY_HIGHEST - 2, CURSOR_LAPTOP_SCREEN, BookmarkMvtCallBack, BookmarkCallBack);
	MSYS_SetRegionUserData(&gBookmarkMouseRegions[i], 0, i);
	MSYS_SetRegionUserData(&gBookmarkMouseRegions[i], 1, CANCEL_STRING);
}


static void DeleteBookmarkRegions(void)
{
	INT32 i;
	// deletes bookmark regions
	for (i = 0; LaptopSaveInfo.iBookMarkList[i] != -1; ++i)
	{
		MSYS_RemoveRegion(&gBookmarkMouseRegions[i]);
	}

	// now one for the cancel
	MSYS_RemoveRegion(&gBookmarkMouseRegions[i]);
}


static void CreateDestoryBookMarkRegions(void)
{
	// checks to see if a bookmark needs to be created or destroyed
	static BOOLEAN fOldShowBookmarks = FALSE;

	if (gfShowBookmarks && !fOldShowBookmarks)
	{
		CreateBookMarkMouseRegions();
		fOldShowBookmarks = TRUE;
	}
	else if (!gfShowBookmarks && fOldShowBookmarks)
	{
		DeleteBookmarkRegions();
		fOldShowBookmarks = FALSE;
	}
}


static void BookmarkCallBack(MOUSE_REGION* pRegion, INT32 iReason)
{
	// we are in process of loading
	if (fLoadPendingFlag) return;

	if (iReason & MSYS_CALLBACK_REASON_LBUTTON_UP)
	{
		const INT32 i = MSYS_GetRegionUserData(pRegion, 0);
		if (MSYS_GetRegionUserData(pRegion, 1) == CANCEL_STRING)
		{
			gfShowBookmarks = FALSE;
			fReDrawScreenFlag = TRUE;
		}
		if (LaptopSaveInfo.iBookMarkList[i] != -1)
		{
			GoToWebPage(LaptopSaveInfo.iBookMarkList[i]);
		}
	}
}


static void InternetRainDelayMessageBoxCallBack(UINT8 bExitValue);


void GoToWebPage(INT32 iPageId)
{
	//if it is raining, popup a warning first saying connection time may be slow
	if (IsItRaining())
	{
		if (giRainDelayInternetSite == -1)
		{
			DoLapTopMessageBox(MSG_BOX_LAPTOP_DEFAULT, pErrorStrings, LAPTOP_SCREEN, MSG_BOX_FLAG_OK, InternetRainDelayMessageBoxCallBack);
			giRainDelayInternetSite = iPageId;
			return;
		}
	}
	else
	{
		giRainDelayInternetSite = -1;
	}

	gfShowBookmarks   = FALSE;
	fReDrawScreenFlag = TRUE;

	switch (iPageId)
	{
		case AIM_BOOKMARK:
			guiCurrentWWWMode    = LAPTOP_MODE_AIM;
			guiCurrentLaptopMode = LAPTOP_MODE_AIM;
			break;

		case BOBBYR_BOOKMARK:
			guiCurrentWWWMode    = LAPTOP_MODE_BOBBY_R;
			guiCurrentLaptopMode = LAPTOP_MODE_BOBBY_R;
			break;

		case IMP_BOOKMARK:
			guiCurrentWWWMode    = LAPTOP_MODE_CHAR_PROFILE;
			guiCurrentLaptopMode = LAPTOP_MODE_CHAR_PROFILE;
			iCurrentImpPage = IMP_HOME_PAGE;
			break;

		case MERC_BOOKMARK:
			//if the mercs server has gone down, but hasnt come up yet
			if (LaptopSaveInfo.fMercSiteHasGoneDownYet &&
					LaptopSaveInfo.fFirstVisitSinceServerWentDown == FALSE)
			{
				guiCurrentWWWMode    = LAPTOP_MODE_BROKEN_LINK;
				guiCurrentLaptopMode = LAPTOP_MODE_BROKEN_LINK;
			}
			else
			{
				guiCurrentWWWMode    = LAPTOP_MODE_MERC;
				guiCurrentLaptopMode = LAPTOP_MODE_MERC;
			}
			break;

		case FUNERAL_BOOKMARK:
			guiCurrentWWWMode    = LAPTOP_MODE_FUNERAL;
			guiCurrentLaptopMode = LAPTOP_MODE_FUNERAL;
			break;

		case FLORIST_BOOKMARK:
			guiCurrentWWWMode    = LAPTOP_MODE_FLORIST;
			guiCurrentLaptopMode = LAPTOP_MODE_FLORIST;
			break;

		case INSURANCE_BOOKMARK:
			guiCurrentWWWMode    = LAPTOP_MODE_INSURANCE;
			guiCurrentLaptopMode = LAPTOP_MODE_INSURANCE;
			break;

		default: return;
	}

	// do we have to have a World Wide Wait
	fFastLoadFlag = LaptopSaveInfo.fVisitedBookmarkAlready[iPageId];
	LaptopSaveInfo.fVisitedBookmarkAlready[iPageId] = TRUE;
	fLoadPendingFlag = TRUE;
}


static void BookmarkMvtCallBack(MOUSE_REGION* pRegion, INT32 iReason)
{
	if (iReason == MSYS_CALLBACK_REASON_MOVE)
	{
		iHighLightBookLine=MSYS_GetRegionUserData(pRegion, 0);
	}
	if (iReason == MSYS_CALLBACK_REASON_LOST_MOUSE)
	{
		iHighLightBookLine = -1;
	}
}


static void LoadLoadPending(void)
try
{
	// function will load the load pending graphics
	// reuse bookmark
	// load graph window and bar
	guiGRAPHWINDOW = AddVideoObjectFromFile("LAPTOP/graphwindow.sti");
	guiGRAPHBAR    = AddVideoObjectFromFile("LAPTOP/graphsegment.sti");
}
catch (...) { /* XXX ignore */ }


static INT32 WWaitDelayIncreasedIfRaining(INT32 iUnitTime);


static void DisplayLoadPending(void)
{
	// this function will display the load pending and return if the load is done
	static INT32 iBaseTime  = 0;
	static INT32 iTotalTime = 0;

	// if merc webpage, make it longer
	// TEMP disables the loadpending
	INT32 iLoadTime;
	INT32 iUnitTime;
	if (gfTemporaryDisablingOfLoadPendingFlag)
	{
		iLoadTime = 1;
		iUnitTime = 1;
	}
	else
	{
		if (fFastLoadFlag)
		{
			iUnitTime = (fConnectingToSubPage ? FASTEST_UNIT_TIME : FAST_UNIT_TIME);
		}
		else if (fConnectingToSubPage)
		{
			iUnitTime = ALMOST_FAST_UNIT_TIME;
		}
		else if (guiCurrentLaptopMode == LAPTOP_MODE_MERC && !LaptopSaveInfo.fMercSiteHasGoneDownYet)
		{
			/* if we are connecting the MERC site, and the MERC site hasnt yet moved
			 * to their new site, have the sloooww wait */
			iUnitTime = LONG_UNIT_TIME;
		}
		else
		{
			iUnitTime = UNIT_TIME;
		}

		iUnitTime += WWaitDelayIncreasedIfRaining(iUnitTime);
		iLoadTime  = iUnitTime * 30;
	}

	// we are now waiting on a web page to download, reset counter
	if (!fLoadPendingFlag)
	{
		fDoneLoadPending     = FALSE;
		fFastLoadFlag        = FALSE;
		fConnectingToSubPage = FALSE;
		iBaseTime            = 0;
		iTotalTime           = 0;
		return;
	}
	if (iBaseTime == 0) iBaseTime = GetJA2Clock();

	// if total time is exceeded, return
	if (iTotalTime >= iLoadTime)
	{
		// done loading, redraw screen
		fLoadPendingFlag        = FALSE;
		fFastLoadFlag           = FALSE;
		iTotalTime              = 0;
		iBaseTime               = 0;
		fDoneLoadPending        = TRUE;
		fConnectingToSubPage    = FALSE;
		fPausedReDrawScreenFlag = TRUE;
		return;
	}

	const INT32 iDifference = GetJA2Clock() - iBaseTime;

	// difference has been long enough or we are redrawing the screen
	if (iDifference > iUnitTime)
	{
		// LONG ENOUGH TIME PASSED
		iBaseTime   = GetJA2Clock();
		iTotalTime += iDifference;
	}

	// new mail, don't redraw
	if (fNewMailFlag) return;

	RenderButtons();

	// display top middle and bottom of box
	BltVideoObject(FRAME_BUFFER, guiDOWNLOADTOP,   0, DOWNLOAD_X,     DOWNLOAD_Y);
	BltVideoObject(FRAME_BUFFER, guiDOWNLOADMID,   0, DOWNLOAD_X,     DOWNLOAD_Y + DOWN_HEIGHT);
	BltVideoObject(FRAME_BUFFER, guiDOWNLOADBOT,   0, DOWNLOAD_X,     DOWNLOAD_Y + DOWN_HEIGHT * 2);
	BltVideoObject(FRAME_BUFFER, guiTITLEBARICONS, 1, DOWNLOAD_X + 4, DOWNLOAD_Y + 1);

	SetFont(DOWNLOAD_FONT);
	SetFontForeground(FONT_WHITE);
	SetFontBackground(FONT_BLACK);
	SetFontShadow(NO_SHADOW);

	// reload or download?
	const wchar_t* const str = (fFastLoadFlag ? pDownloadString[1] : pDownloadString[0]);
	INT16 sXPosition = 0;
	INT16 sYPosition = 0;
	FindFontCenterCoordinates(328, 0, 446 - 328, 0, str, DOWNLOAD_FONT, &sXPosition, &sYPosition);
	MPrint(sXPosition, DOWN_STRING_Y, str);

	BltVideoObject(FRAME_BUFFER, guiGRAPHWINDOW, 0, LAPTOP_WINDOW_X, LAPTOP_WINDOW_Y);

	// check to see if we are only updating screen, but not passed a new element in the load pending display

	// decide how many time units are to be displayed, based on amount of time passed
	for (INT32 i = 0, iTempTime = iTotalTime; i <= 30 && iTempTime > 0; ++i, iTempTime -= iUnitTime)
	{
		BltVideoObject(FRAME_BUFFER, guiGRAPHBAR, 0, LAPTOP_BAR_X + UNIT_WIDTH * i, LAPTOP_BAR_Y);
	}

	InvalidateRegion(DOWNLOAD_X, DOWNLOAD_Y, DOWNLOAD_X + 150, DOWNLOAD_Y + 100);

	// re draw screen and new mail warning box
	SetFontShadow(DEFAULT_SHADOW);

	MarkButtonsDirty();

	DisableMercSiteButton();
}


static void DeleteLoadPending(void)
{
	// this funtion will delete the load pending graphics
	// reuse bookmark
	DeleteVideoObject(guiGRAPHBAR);
	DeleteVideoObject(guiGRAPHWINDOW);
}


// This function is called every time the laptop is FIRST initialized, ie whenever the laptop is loaded.  It calls
// various init function in the laptop pages that need to be inited when the laptop is just loaded.
static void EnterLaptopInitLaptopPages(void)
{
	EnterInitAimMembers();
	EnterInitAimArchives();
	EnterInitAimPolicies();
	EnterInitAimHistory();
	EnterInitFloristGallery();
	EnterInitInsuranceInfo();
	EnterInitBobbyRayOrder();
	EnterInitMercSite();

	// init sub pages for WW Wait
	InitIMPSubPageList();
}


static void CheckMarkButtonsDirtyFlag(void)
{
	// this function checks the fMarkButtonsDirtyFlag, if true, mark buttons dirty
	if (fMarkButtonsDirtyFlag)
	{
		// flag set, mark buttons and reset
		MarkButtonsDirty();
		fMarkButtonsDirtyFlag = FALSE;
	}
}


static void PostButtonRendering(void)
{
	// this function is in place to allow for post button rendering
	switch (guiCurrentLaptopMode)
	{
		case LAPTOP_MODE_AIM_MEMBERS: RenderAIMMembersTopLevel(); break;
	}
}


static void ShouldNewMailBeDisplayed(void)
{
	switch (guiCurrentLaptopMode)
	{
		case LAPTOP_MODE_AIM_MEMBERS: DisableNewMailMessage(); break;
	}
}


static void CheckIfNewWWWW(void)
{
	// if no www mode, set new www flag..until new www mode that is not 0
	fNewWWW = (guiCurrentWWWMode == LAPTOP_MODE_NONE);
}


static void HandleLapTopESCKey(void)
{
	// will handle esc key events, since handling depends on state of laptop
	if (fNewMailFlag)
	{
		// get rid of new mail warning box
		fNewMailFlag = FALSE;
		CreateDestroyNewMailButton();
	}
	else if (MailToDelete != NULL)
	{
		// get rid of delete mail box
		MailToDelete = NULL;
		CreateDestroyDeleteNoticeMailButton();
	}
	else if (gfShowBookmarks)
	{
		// get rid of bookmarks
		gfShowBookmarks = FALSE;
		RenderLapTopImage();
	}
	else
	{
		// leave
		fExitingLaptopFlag = TRUE;
		HandleExit();
		return;
	}

	// force redraw
	fReDrawScreenFlag = TRUE;
	RenderLaptop();
}


void HandleRightButtonUpEvent(void)
{
	// will handle the right button up event
	if (fNewMailFlag)
	{
		// get rid of new mail warning box
		fNewMailFlag = FALSE;
		CreateDestroyNewMailButton();
	}
	else if (MailToDelete != NULL)
	{
		// get rid of delete mail box
		MailToDelete = NULL;
		CreateDestroyDeleteNoticeMailButton();
	}
	else if (gfShowBookmarks)
	{
		// get rid of bookmarks
		gfShowBookmarks = FALSE;
		RenderLapTopImage();
	}
	else if (fDisplayMessageFlag)
	{
		fDisplayMessageFlag = FALSE;
		RenderLapTopImage();
	}
	else
	{
		fShowBookmarkInfo = FALSE;
		return;
	}

	// force redraw
	fReDrawScreenFlag = TRUE;
	RenderLaptop();
}


static void HandleLeftButtonUpEvent(void)
{
	// will handle the left button up event
	if (gfShowBookmarks)
	{
		// get rid of bookmarks
		gfShowBookmarks = FALSE;

		// force redraw
		fReDrawScreenFlag = TRUE;
		RenderLapTopImage();
		RenderLaptop();
	}
	else if (fShowBookmarkInfo)
	{
		fShowBookmarkInfo = FALSE;
	}
}

void LapTopScreenCallBack(MOUSE_REGION* pRegion, INT32 iReason)
{
	if (iReason & MSYS_CALLBACK_REASON_LBUTTON_UP)
	{
		HandleLeftButtonUpEvent();
	}
	if (iReason & MSYS_CALLBACK_REASON_RBUTTON_UP)
	{
		HandleRightButtonUpEvent();
	}
}


void DoLapTopMessageBox(const UINT8 ubStyle, const wchar_t* const zString, const UINT32 uiExitScreen, const UINT8 ubFlags, const MSGBOX_CALLBACK ReturnCallback)
{
	const SGPRect CenteringRect = { LAPTOP_SCREEN_UL_X, LAPTOP_SCREEN_UL_Y, LAPTOP_SCREEN_LR_X, LAPTOP_SCREEN_LR_Y };
	DoLapTopSystemMessageBoxWithRect(ubStyle, zString, uiExitScreen, ubFlags, ReturnCallback, &CenteringRect);
}


void DoLapTopSystemMessageBoxWithRect(UINT8 ubStyle, const wchar_t* zString, UINT32 uiExitScreen, UINT16 usFlags, MSGBOX_CALLBACK ReturnCallback, const SGPRect* pCenteringRect)
{
	// reset exit mode
	fExitDueToMessageBox = TRUE;

	// do message box and return
	DoMessageBox(ubStyle, zString, uiExitScreen, usFlags, ReturnCallback, pCenteringRect);
}


void DoLapTopSystemMessageBox(const UINT8 ubStyle, const wchar_t* const zString, const UINT32 uiExitScreen, const UINT16 usFlags, const MSGBOX_CALLBACK ReturnCallback)
{
	const SGPRect CenteringRect = { 0, 0, SCREEN_WIDTH, INV_INTERFACE_START_Y };
	DoLapTopSystemMessageBoxWithRect(ubStyle, zString, uiExitScreen, usFlags, ReturnCallback, &CenteringRect);
}


//places a tileable pattern down
BOOLEAN WebPageTileBackground(const UINT8 ubNumX, const UINT8 ubNumY, const UINT16 usWidth, const UINT16 usHeight, const SGPVObject* const background)
{
	UINT16 uiPosY = LAPTOP_SCREEN_WEB_UL_Y;
	for (UINT16 y = 0; y < ubNumY; ++y)
	{
		UINT16 uiPosX = LAPTOP_SCREEN_UL_X;
		for (UINT16 x = 0; x < ubNumX; ++x)
		{
			BltVideoObject(FRAME_BUFFER, background, 0, uiPosX, uiPosY);
			uiPosX += usWidth;
		}
		uiPosY += usHeight;
	}
	return TRUE;
}


static void InitTitleBarMaximizeGraphics(const SGPVObject* const uiBackgroundGraphic, const wchar_t* const pTitle, const SGPVObject* const uiIconGraphic, const UINT16 usIconGraphicIndex)
{
	Assert(uiBackgroundGraphic);

	// Create a background video surface to blt the title bar onto
	guiTitleBarSurface = AddVideoSurface(LAPTOP_TITLE_BAR_WIDTH, LAPTOP_TITLE_BAR_HEIGHT, PIXEL_DEPTH);

	BltVideoObject(guiTitleBarSurface, uiBackgroundGraphic, 0, 0, 0);
	BltVideoObject(guiTitleBarSurface, uiIconGraphic, usIconGraphicIndex, LAPTOP_TITLE_BAR_ICON_OFFSET_X, LAPTOP_TITLE_BAR_ICON_OFFSET_Y);

	SetFontDestBuffer(guiTitleBarSurface, 0, 0, LAPTOP_TITLE_BAR_WIDTH, LAPTOP_TITLE_BAR_HEIGHT);
	DrawTextToScreen(pTitle, LAPTOP_TITLE_BAR_TEXT_OFFSET_X, LAPTOP_TITLE_BAR_TEXT_OFFSET_Y, 0, FONT14ARIAL, FONT_MCOLOR_WHITE, FONT_MCOLOR_BLACK, LEFT_JUSTIFIED);
	SetFontDestBuffer(FRAME_BUFFER, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}


static BOOLEAN DisplayTitleBarMaximizeGraphic(BOOLEAN fForward, BOOLEAN fInit, UINT16 usTopLeftX, UINT16 usTopLeftY, UINT16 usTopRightX)
{
	static INT8    ubCount;
	static SGPRect LastRect;

	if (fInit)
	{
		if (gfTitleBarSurfaceAlreadyActive) return FALSE;
		gfTitleBarSurfaceAlreadyActive = TRUE;
		if (fForward)
		{
			ubCount = 1;
		}
		else
		{
			ubCount = NUMBER_OF_LAPTOP_TITLEBAR_ITERATIONS - 1;
		}
	}

	FLOAT dTemp;
	dTemp = (LAPTOP_TITLE_BAR_TOP_LEFT_X  - usTopLeftX)  / (FLOAT)NUMBER_OF_LAPTOP_TITLEBAR_ITERATIONS;
	const INT16 sPosX       = usTopLeftX  + dTemp * ubCount;

	dTemp = (LAPTOP_TITLE_BAR_TOP_RIGHT_X - usTopRightX) / (FLOAT)NUMBER_OF_LAPTOP_TITLEBAR_ITERATIONS;
	const INT16 sPosRightX  = usTopRightX + dTemp * ubCount;

	dTemp = (LAPTOP_TITLE_BAR_TOP_LEFT_Y  - usTopLeftY)  / (FLOAT)NUMBER_OF_LAPTOP_TITLEBAR_ITERATIONS;
	const INT16 sPosY       = usTopLeftY  + dTemp * ubCount;

	const INT16 sPosBottomY = LAPTOP_TITLE_BAR_HEIGHT;

	SGPRect SrcRect;
	SrcRect.iLeft   = 0;
	SrcRect.iTop    = 0;
	SrcRect.iRight  = LAPTOP_TITLE_BAR_WIDTH;
	SrcRect.iBottom = LAPTOP_TITLE_BAR_HEIGHT;

	//if its the last fram, bit the tittle bar to the final position
	SGPRect DestRect;
	if (ubCount == NUMBER_OF_LAPTOP_TITLEBAR_ITERATIONS)
	{
		DestRect.iLeft   = LAPTOP_TITLE_BAR_TOP_LEFT_X;
		DestRect.iTop    = LAPTOP_TITLE_BAR_TOP_LEFT_Y;
		DestRect.iRight  = LAPTOP_TITLE_BAR_TOP_RIGHT_X;
		DestRect.iBottom = DestRect.iTop + sPosBottomY;
	}
	else
	{
		DestRect.iLeft   = sPosX;
		DestRect.iTop    = sPosY;
		DestRect.iRight  = sPosRightX;
		DestRect.iBottom = DestRect.iTop + sPosBottomY;
	}

	if (fForward)
	{
		//Restore the old rect
		if (ubCount > 1)
		{
			const INT16 sWidth  = LastRect.iRight  - LastRect.iLeft;
			const INT16 sHeight = LastRect.iBottom - LastRect.iTop;
			BlitBufferToBuffer(guiSAVEBUFFER, FRAME_BUFFER, LastRect.iLeft, LastRect.iTop, sWidth, sHeight);
		}

		//Save rectangle
		if (ubCount > 0)
		{
			const INT16 sWidth  = DestRect.iRight  - DestRect.iLeft;
			const INT16 sHeight = DestRect.iBottom - DestRect.iTop;
			BlitBufferToBuffer(FRAME_BUFFER, guiSAVEBUFFER, DestRect.iLeft, DestRect.iTop, sWidth, sHeight);
		}
	}
	else
	{
		//Restore the old rect
		if (ubCount < NUMBER_OF_LAPTOP_TITLEBAR_ITERATIONS - 1)
		{
			const INT16 sWidth  = LastRect.iRight  - LastRect.iLeft;
			const INT16 sHeight = LastRect.iBottom - LastRect.iTop;
			BlitBufferToBuffer(guiSAVEBUFFER, FRAME_BUFFER, LastRect.iLeft, LastRect.iTop, sWidth, sHeight);
		}

		//Save rectangle
		if (ubCount < NUMBER_OF_LAPTOP_TITLEBAR_ITERATIONS)
		{
			const INT16 sWidth  = DestRect.iRight  - DestRect.iLeft;
			const INT16 sHeight = DestRect.iBottom - DestRect.iTop;
			BlitBufferToBuffer(FRAME_BUFFER, guiSAVEBUFFER, DestRect.iLeft, DestRect.iTop, sWidth, sHeight);
		}
	}

	BltStretchVideoSurface(FRAME_BUFFER, guiTitleBarSurface, &SrcRect, &DestRect);

	InvalidateRegion(DestRect.iLeft, DestRect.iTop, DestRect.iRight, DestRect.iBottom);
	InvalidateRegion(LastRect.iLeft, LastRect.iTop, LastRect.iRight, LastRect.iBottom);

	LastRect = DestRect;

	if (fForward)
	{
		if (ubCount == NUMBER_OF_LAPTOP_TITLEBAR_ITERATIONS)
		{
			gfTitleBarSurfaceAlreadyActive = FALSE;
			return TRUE;
		}
		else
		{
			ubCount++;
			return FALSE;
		}
	}
	else
	{
		if (ubCount == 0)
		{
			gfTitleBarSurfaceAlreadyActive = FALSE;
			return TRUE;
		}
		else
		{
			ubCount--;
			return FALSE;
		}
	}
}


static void RemoveTitleBarMaximizeGraphics(void)
{
	DeleteVideoSurface(guiTitleBarSurface);
}


static void HandleSlidingTitleBar(void)
{
	if (fExitingLaptopFlag) return;

	if (fMaximizingProgram)
	{
		UINT16 y;
		switch (bProgramBeingMaximized)
		{
			case LAPTOP_PROGRAM_MAILER:      y =  66; break;
			case LAPTOP_PROGRAM_FILES:       y = 120; break;
			case LAPTOP_PROGRAM_FINANCES:    y = 226; break;
			case LAPTOP_PROGRAM_PERSONNEL:   y = 194; break;
			case LAPTOP_PROGRAM_HISTORY:     y = 162; break;
			case LAPTOP_PROGRAM_WEB_BROWSER: y =  99; break;
			default: goto no_display_max;
		}

		fMaximizingProgram = !DisplayTitleBarMaximizeGraphic(TRUE, fInitTitle, 29, y, 29 + 20);
		if (!fMaximizingProgram)
		{
			RemoveTitleBarMaximizeGraphics();
			fEnteredNewLapTopDueToHandleSlidingBars = TRUE;
			EnterNewLaptopMode();
			fEnteredNewLapTopDueToHandleSlidingBars = FALSE;
			fPausedReDrawScreenFlag = TRUE;
		}

no_display_max:
		MarkButtonsDirty();
	}
	else if (fMinizingProgram)
	{
		// minimizing
		UINT16 y;
		switch (bProgramBeingMaximized)
		{
			case LAPTOP_PROGRAM_MAILER:      y =  66; break;
			case LAPTOP_PROGRAM_FILES:       y = 130; break; // XXX only difference to max case
			case LAPTOP_PROGRAM_FINANCES:    y = 226; break;
			case LAPTOP_PROGRAM_PERSONNEL:   y = 194; break;
			case LAPTOP_PROGRAM_HISTORY:     y = 162; break;
			case LAPTOP_PROGRAM_WEB_BROWSER: y =  99; break;
			default: goto no_display_min;
		}

		fMinizingProgram = !DisplayTitleBarMaximizeGraphic(FALSE, fInitTitle, 29, y, 29 + 20);
		if (!fMinizingProgram)
		{
			RemoveTitleBarMaximizeGraphics();
			EnterNewLaptopMode();
			fEnteredNewLapTopDueToHandleSlidingBars = FALSE;
			fPausedReDrawScreenFlag = TRUE;
		}

no_display_min:;
	}
	else
	{
		return;
	}

	// reset init
	fInitTitle = FALSE;
}


static void ShowLights(void)
{
	// will show lights depending on state
	BltVideoObject(FRAME_BUFFER, guiLIGHTS, fPowerLightOn     ? 0 : 1, 44, 466);
	BltVideoObject(FRAME_BUFFER, guiLIGHTS, fHardDriveLightOn ? 0 : 1, 88, 466);
}


static void FlickerHDLight(void)
{
	static INT32 iBaseTime        = 0;
	static INT32 iTotalDifference = 0;

	if (fLoadPendingFlag) fFlickerHD = TRUE;
	if (!fFlickerHD) return;

	if (iBaseTime == 0) iBaseTime = GetJA2Clock();

	const INT32 iDifference = GetJA2Clock() - iBaseTime;

	if (iTotalDifference > HD_FLICKER_TIME && !fLoadPendingFlag)
	{
		iBaseTime         = GetJA2Clock();
		fHardDriveLightOn = FALSE;
		iBaseTime         = 0;
		iTotalDifference  = 0;
		fFlickerHD        = FALSE;
		InvalidateRegion(88, 466, 102, 477);
		return;
	}

	if (iDifference > FLICKER_TIME)
	{
		iTotalDifference += iDifference;

		if (fLoadPendingFlag) iTotalDifference = 0;

		fHardDriveLightOn = (Random(2) == 0);
		InvalidateRegion(88, 466, 102, 477);
	}
}


static BOOLEAN ExitLaptopDone(void)
{
	// check if this is the first time, to reset counter
	static BOOLEAN fOldLeaveLaptopState = FALSE;
	static INT32 iBaseTime              = 0;

	if (!fOldLeaveLaptopState)
	{
		fOldLeaveLaptopState = TRUE;
		iBaseTime = GetJA2Clock();
	}

	fPowerLightOn = FALSE;

	InvalidateRegion(44, 466, 58, 477);
	// get the current difference
	const INT32 iDifference = GetJA2Clock() - iBaseTime;

	// did we wait long enough?
	if (iDifference > EXIT_LAPTOP_DELAY_TIME)
	{
		iBaseTime = 0;
		fOldLeaveLaptopState = FALSE;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


static void CreateMinimizeButtonForCurrentMode(void);
static void CreateMinimizeRegionsForLaptopProgramIcons(void);
static void DestroyMinimizeButtonForCurrentMode(void);
static void DestroyMinimizeRegionsForLaptopProgramIcons(void);


static void CreateDestroyMinimizeButtonForCurrentMode(void)
{
	// will create the minimize button
	static BOOLEAN fAlreadyCreated = FALSE;
	// check to see if created, if so, do nothing

	// check current mode
	if (guiCurrentLaptopMode == LAPTOP_MODE_NONE && guiPreviousLaptopMode != LAPTOP_MODE_NONE)
	{
		fCreateMinimizeButton = FALSE;
	}
	else if (guiCurrentLaptopMode != LAPTOP_MODE_NONE)
	{
		fCreateMinimizeButton = TRUE;
	}
	else if (guiPreviousLaptopMode != LAPTOP_MODE_NONE)
	{
		fCreateMinimizeButton = FALSE;
	}

	// leaving laptop, get rid of the button
	if (fExitingLaptopFlag)
	{
		fCreateMinimizeButton = FALSE;
	}

	if (!fAlreadyCreated && fCreateMinimizeButton)
	{
		// not created, create
		fAlreadyCreated = TRUE;
		CreateMinimizeButtonForCurrentMode();
		CreateMinimizeRegionsForLaptopProgramIcons();
	}
	else if (fAlreadyCreated && !fCreateMinimizeButton)
	{
		// created and must be destroyed
		fAlreadyCreated = FALSE;
		DestroyMinimizeButtonForCurrentMode();
		DestroyMinimizeRegionsForLaptopProgramIcons();
	}
}


static void LaptopMinimizeProgramButtonCallback(GUI_BUTTON* btn, INT32 reason);


static void CreateMinimizeButtonForCurrentMode(void)
{
	// create minimize button
	gLaptopMinButton = QuickCreateButtonImg("LAPTOP/x.sti", -1, 0, -1, 1, -1, 590, 30, MSYS_PRIORITY_HIGH, LaptopMinimizeProgramButtonCallback);
	SetButtonCursor(gLaptopMinButton, CURSOR_LAPTOP_SCREEN);
}


static void DestroyMinimizeButtonForCurrentMode(void)
{
	// destroy minimize button
	RemoveButton(gLaptopMinButton);
}


static void SetCurrentToLastProgramOpened(void);


static void LaptopMinimizeProgramButtonCallback(GUI_BUTTON* btn, INT32 reason)
{
	if (reason & MSYS_CALLBACK_REASON_LBUTTON_UP)
	{
		UINT           prog;
		const wchar_t* title;
		UINT16         gfx_idx;
		switch (guiCurrentLaptopMode)
		{
			case LAPTOP_MODE_EMAIL:
				prog    = LAPTOP_PROGRAM_MAILER;
				title   = pLaptopIcons[0];
				gfx_idx = 0;
				break;

			case LAPTOP_MODE_FILES:
				prog    = LAPTOP_PROGRAM_FILES;
				title   = pLaptopIcons[5];
				gfx_idx = 2;
				break;

			case LAPTOP_MODE_FINANCES:
				prog    = LAPTOP_PROGRAM_FINANCES;
				title   = pLaptopIcons[2];
				gfx_idx = 5;
				break;

			case LAPTOP_MODE_HISTORY:
				prog    = LAPTOP_PROGRAM_HISTORY;
				title   = pLaptopIcons[4];
				gfx_idx = 4;
				break;

			case LAPTOP_MODE_PERSONNEL:
				prog    = LAPTOP_PROGRAM_PERSONNEL;
				title   = pLaptopIcons[3];
				gfx_idx = 3;
				break;

			case LAPTOP_MODE_NONE: return; // nothing

			default:
				gfShowBookmarks = FALSE;
				prog    = LAPTOP_PROGRAM_WEB_BROWSER;
				title   = pLaptopIcons[7];
				gfx_idx = 1;
				break;
		}
		gLaptopProgramStates[prog] = LAPTOP_PROGRAM_MINIMIZED;
		InitTitleBarMaximizeGraphics(guiTITLEBARLAPTOP, title, guiTITLEBARICONS, gfx_idx);
		SetCurrentToLastProgramOpened();
		fMinizingProgram = TRUE;
		fInitTitle = TRUE;
	}
}


static INT32 FindLastProgramStillOpen(void)
{
	INT32 iLowestValue = 6;
	INT32 iLowestValueProgram = 6;

	// returns ID of last program open and not minimized
	for (INT32 i = 0; i < 6; ++i)
	{
		if (gLaptopProgramStates[i] != LAPTOP_PROGRAM_MINIMIZED)
		{
			if (gLaptopProgramQueueList[i] < iLowestValue)
			{
				iLowestValue = gLaptopProgramQueueList[i];
				iLowestValueProgram = i;
			}
		}
	}

	return iLowestValueProgram;
}


static void UpdateListToReflectNewProgramOpened(INT32 iOpenedProgram)
{
	// will update queue of opened programs to show thier states
	// set iOpenedProgram to 1, and update others

	// increment everyone
	for (INT32 i = 0; i < 6; ++i)
	{
		gLaptopProgramQueueList[i]++;
	}

	gLaptopProgramQueueList[iOpenedProgram] = 1;
}


static void InitLaptopOpenQueue(void)
{
	// set evereyone to 1
	for (INT32 i = 0; i < 6; ++i)
	{
		gLaptopProgramQueueList[i] = 1;
	}
}


static void SetCurrentToLastProgramOpened(void)
{
	guiCurrentLaptopMode = LAPTOP_MODE_NONE;

	switch (FindLastProgramStillOpen())
	{
		case LAPTOP_PROGRAM_HISTORY:   guiCurrentLaptopMode = LAPTOP_MODE_HISTORY;   break;
		case LAPTOP_PROGRAM_MAILER:    guiCurrentLaptopMode = LAPTOP_MODE_EMAIL;     break;
		case LAPTOP_PROGRAM_PERSONNEL: guiCurrentLaptopMode = LAPTOP_MODE_PERSONNEL; break;
		case LAPTOP_PROGRAM_FINANCES:  guiCurrentLaptopMode = LAPTOP_MODE_FINANCES;  break;
		case LAPTOP_PROGRAM_FILES:     guiCurrentLaptopMode = LAPTOP_MODE_FILES;     break;

		case LAPTOP_PROGRAM_WEB_BROWSER:
			// last www mode
			guiCurrentLaptopMode = (guiCurrentWWWMode == 0 ? LAPTOP_MODE_WWW : guiCurrentWWWMode);
			fShowBookmarkInfo = TRUE;
			break;
	}
}


void BlitTitleBarIcons(void)
{
	// will blit the icons for the title bar of the program we are in
	UINT32 Index;
	switch (guiCurrentLaptopMode)
	{
		case LAPTOP_MODE_HISTORY:   Index = 4; break;
		case LAPTOP_MODE_EMAIL:     Index = 0; break;
		case LAPTOP_MODE_PERSONNEL: Index = 3; break;
		case LAPTOP_MODE_FINANCES:  Index = 5; break;
		case LAPTOP_MODE_FILES:     Index = 2; break;
		case LAPTOP_MODE_NONE:      return;           // do nothing
		default:                    Index = 1; break; // www pages
	}
	BltVideoObject(FRAME_BUFFER, guiTITLEBARICONS, Index, LAPTOP_TITLE_ICONS_X, LAPTOP_TITLE_ICONS_Y);
}


static void DrawDeskTopBackground(void)
{
	BltVideoSurface(FRAME_BUFFER, guiDESKTOP, LAPTOP_SCREEN_UL_X - 2, LAPTOP_SCREEN_UL_Y - 4, NULL);
}


static void LoadDesktopBackground(void)
{
	const char* const ImageFile = GetMLGFilename(MLG_DESKTOP);
	guiDESKTOP = AddVideoSurfaceFromFile(ImageFile);
}


static void DeleteDesktopBackground(void)
{
	DeleteVideoSurface(guiDESKTOP);
}


void PrintBalance(void)
{
	SetFont(FONT10ARIAL);
	SetFontForeground(FONT_BLACK);
	SetFontBackground(FONT_BLACK);
	SetFontShadow(NO_SHADOW);

	wchar_t pString[32];
	SPrintMoney(pString, LaptopSaveInfo.iCurrentBalance);

	INT32 x = 47;
	INT32 y = 257 + 15;
	if (ButtonList[gLaptopButton[5]]->uiFlags & BUTTON_CLICKED_ON)
	{
		++x;
		++y;
	}
	MPrint(x, y, pString);

	SetFontShadow(DEFAULT_SHADOW);
}


void PrintNumberOnTeam(void)
{
	SetFont(FONT10ARIAL);
	SetFontForeground(FONT_BLACK);
	SetFontBackground(FONT_BLACK);
	SetFontShadow(NO_SHADOW);

	const INT32 iCounter = NumberOfMercsOnPlayerTeam();

	UINT16 usPosX = 47;
	UINT16 usPosY = 194 + 30;
	if (ButtonList[gLaptopButton[3]]->uiFlags & BUTTON_CLICKED_ON)
	{
		++usPosX;
		++usPosY;
	}
	mprintf(usPosX, usPosY, L"%ls %d", pPersonnelString, iCounter);

	SetFontShadow(DEFAULT_SHADOW);
}


void PrintDate(void)
{
	SetFont(FONT10ARIAL);
	SetFontForeground(FONT_BLACK);
	SetFontBackground(FONT_BLACK);
	SetFontShadow(NO_SHADOW);
	MPrint(30 + (70 - StringPixLength(WORLDTIMESTR, FONT10ARIAL)) / 2, 433, WORLDTIMESTR);
	SetFontShadow(DEFAULT_SHADOW);
}


static void DisplayTaskBarIcons(void)
{
	// display the files icon, if there is any
	if (fNewFilesInFileViewer)
	{
		BltVideoObject(FRAME_BUFFER, guiTITLEBARICONS, 7, LAPTOP__NEW_FILE_ICON_X, LAPTOP__NEW_FILE_ICON_Y);
	}

	// display the email icon, if there is email
	if (fUnReadMailFlag)
	{
		BltVideoObject(FRAME_BUFFER, guiTITLEBARICONS, 6, LAPTOP__NEW_EMAIL_ICON_X, LAPTOP__NEW_EMAIL_ICON_Y);
	}
}


static void HandleAltTabKeyInLaptop(void);
static void HandleShiftAltTabKeyInLaptop(void);


void HandleKeyBoardShortCutsForLapTop(UINT16 usEvent, UINT32 usParam, UINT16 usKeyState)
{
	// will handle keyboard shortcuts for the laptop ... to be added to later
	if (fExitingLaptopFlag || fTabHandled) return;

	if (usEvent != KEY_DOWN) return;

	switch (usParam)
	{
		case SDLK_ESCAPE:
			// esc hit, check to see if boomark list is shown, if so, get rid of it, otherwise, leave
			HandleLapTopESCKey();
			break;

		case SDLK_TAB:
			if (usKeyState & CTRL_DOWN)
			{
				HandleShiftAltTabKeyInLaptop();
			}
			else
			{
				HandleAltTabKeyInLaptop();
			}
			fTabHandled = TRUE;
			break;

#if defined JA2TESTVERSION
		case 'm':
			if (usKeyState & ALT_DOWN) CheatToGetAll5Merc();
			break;
#endif

		case 'b':
			if (CHEATER_CHEAT_LEVEL())
			{
				if (usKeyState & ALT_DOWN)
				{
					LaptopSaveInfo.fBobbyRSiteCanBeAccessed = TRUE;
				}
				else if (usKeyState & CTRL_DOWN)
				{
					guiCurrentLaptopMode = LAPTOP_MODE_BROKEN_LINK;
				}
			}
			break;

		case 'x':
			if (usKeyState & ALT_DOWN) HandleShortCutExitState();
			break;

#if defined JA2TESTVERSION
		case 'q':
			//if we dont currently have mercs on the team, hire some
			if (NumberOfMercsOnPlayerTeam() == 0)
			{
				UINT8 ubRand = (UINT8)Random(2) + 2;
				TempHiringOfMercs(ubRand, FALSE);
			}
			MarkButtonsDirty();
			fExitingLaptopFlag = TRUE;
			break;

		case 's':
			if (usKeyState & ALT_DOWN)
			{
				SetBookMark(AIM_BOOKMARK);
				SetBookMark(BOBBYR_BOOKMARK);
				SetBookMark(IMP_BOOKMARK);
				SetBookMark(MERC_BOOKMARK);
				SetBookMark(FUNERAL_BOOKMARK);
				SetBookMark(FLORIST_BOOKMARK);
				SetBookMark(INSURANCE_BOOKMARK);
			}
			break;
#endif

		case SDLK_h:
			ShouldTheHelpScreenComeUp(HELP_SCREEN_LAPTOP, TRUE);
			break;

#if defined JA2TESTVERSION || defined JA2DEMO
		case 'w':
			DemoHiringOfMercs();
			break;
#endif

#if defined JA2BETAVERSION
		case 'e':
			if (usKeyState & ALT_DOWN && CHEATER_CHEAT_LEVEL()) AddAllEmails();
			break;
#endif

		case '=':
			if (CHEATER_CHEAT_LEVEL())
			{
				// adding money
				AddTransactionToPlayersBook(ANONYMOUS_DEPOSIT, 0, GetWorldTotalMin(), 100000);
				MarkButtonsDirty();
			}
			break;

#if defined JA2TESTVERSION
		case 'd':
			gfTemporaryDisablingOfLoadPendingFlag = !gfTemporaryDisablingOfLoadPendingFlag;
			break;

		case '+':
			if (usKeyState & ALT_DOWN)
			{
				gStrategicStatus.ubHighestProgress += 10;
				if (gStrategicStatus.ubHighestProgress > 100)
				{
					gStrategicStatus.ubHighestProgress = 100;
				}
				InitAllArmsDealers();
				InitBobbyRayInventory();
			}
			break;
#endif

		case '-':
#if defined JA2TESTVERSION
			if (usKeyState & ALT_DOWN)
			{
				if (gStrategicStatus.ubHighestProgress >= 10)
				{
					gStrategicStatus.ubHighestProgress -= 10;
				}
				else
				{
					gStrategicStatus.ubHighestProgress = 0;
				}
				InitAllArmsDealers();
				InitBobbyRayInventory();
			}
			else
#endif
			if (CHEATER_CHEAT_LEVEL())
			{
				// subtracting money
				AddTransactionToPlayersBook(ANONYMOUS_DEPOSIT, 0, GetWorldTotalMin(), -10000);
				MarkButtonsDirty();
			}
			break;

#if defined JA2TESTVERSION
		case '*':
			if (usKeyState & ALT_DOWN)
			{
				DeleteAllStrategicEventsOfType(EVENT_EVALUATE_QUEEN_SITUATION);
				AdvanceToNextDay();
			}
			break;

#	if defined SGP_VIDEO_DEBUGGING
		case 'v':
			if (usKeyState & CTRL_DOWN)
			{
				ScreenMsg(FONT_MCOLOR_LTYELLOW, MSG_INTERFACE, L"VObjects:  %d", guiVObjectSize);
				ScreenMsg(FONT_MCOLOR_LTYELLOW, MSG_INTERFACE, L"VSurfaces:  %d", guiVSurfaceSize);
				ScreenMsg(FONT_MCOLOR_LTYELLOW, MSG_INTERFACE, L"SGPVideoDump.txt updated...");
				PerformVideoInfoDumpIntoFile("SGPVideoDump.txt", TRUE);
			}
			break;
#	endif

		case '1': TempHiringOfMercs( 1, FALSE); break;
		case '2': TempHiringOfMercs( 2, FALSE); break;
		case '3': TempHiringOfMercs( 3, FALSE); break;
		case '4': TempHiringOfMercs( 4, FALSE); break;
		case '5': TempHiringOfMercs( 5, FALSE); break;
		case '6': TempHiringOfMercs( 6, FALSE); break;
		case '7': TempHiringOfMercs( 7, FALSE); break;
		case '8': TempHiringOfMercs( 8, FALSE); break;
		case '9': TempHiringOfMercs( 9, FALSE); break;
		case '0': TempHiringOfMercs(10, FALSE); break;
#endif
	}
}


BOOLEAN RenderWWWProgramTitleBar(void)
{
	// will render the title bar for the www program
	CHECKF(BltVideoObjectOnce(FRAME_BUFFER, "LAPTOP/programtitlebar.sti", 0, LAPTOP_SCREEN_UL_X, LAPTOP_SCREEN_UL_Y - 2));

	// now slapdown text
	SetFont(FONT14ARIAL);
	SetFontForeground(FONT_WHITE);
	SetFontBackground(FONT_BLACK);

	// display title

	// no page loaded yet, do not handle yet

	if (guiCurrentLaptopMode == LAPTOP_MODE_WWW)
	{
		MPrint(140, 33, pWebTitle);
	}
	else
	{
		const INT32 iIndex = guiCurrentLaptopMode - LAPTOP_MODE_WWW-1;
		mprintf(140, 33, L"%ls  -  %ls", pWebTitle, pWebPagesTitles[iIndex]);
	}

	BlitTitleBarIcons();
	DisplayProgramBoundingBox(FALSE);
	return TRUE;
}


static void LaptopProgramIconMinimizeCallback(MOUSE_REGION* pRegion, INT32 iReason);


static void CreateMinimizeRegionsForLaptopProgramIcons(void)
{
	// will create the minizing region to lie over the icon for this particular laptop program
	const UINT16 x = LAPTOP_PROGRAM_ICON_X;
	const UINT16 y = LAPTOP_PROGRAM_ICON_Y;
	const UINT16 w = LAPTOP_PROGRAM_ICON_WIDTH;
	const UINT16 h = LAPTOP_PROGRAM_ICON_HEIGHT;
	MSYS_DefineRegion(&gLapTopProgramMinIcon, x, y, x + w, y + h, MSYS_PRIORITY_NORMAL + 1, CURSOR_LAPTOP_SCREEN, MSYS_NO_CALLBACK, LaptopProgramIconMinimizeCallback);
}


static void DestroyMinimizeRegionsForLaptopProgramIcons(void)
{
	// will destroy the minizmize regions to be placed over the laptop icons that will be
	// displayed on the top of the laptop program bar
	MSYS_RemoveRegion(&gLapTopProgramMinIcon);
}


static void LaptopProgramIconMinimizeCallback(MOUSE_REGION* pRegion, INT32 iReason)
{
	// callback handler for the minize region that is attatched to the laptop program icon
	if (iReason & MSYS_CALLBACK_REASON_LBUTTON_UP)
	{
		switch (guiCurrentLaptopMode)
		{
			case LAPTOP_MODE_EMAIL:
				gLaptopProgramStates[LAPTOP_PROGRAM_MAILER] = LAPTOP_PROGRAM_MINIMIZED;
				InitTitleBarMaximizeGraphics(guiTITLEBARLAPTOP, pLaptopIcons[0], guiTITLEBARICONS, 0);
				SetCurrentToLastProgramOpened();
				break;

			case LAPTOP_MODE_FILES:
				gLaptopProgramStates[LAPTOP_PROGRAM_FILES] = LAPTOP_PROGRAM_MINIMIZED;
				InitTitleBarMaximizeGraphics(guiTITLEBARLAPTOP, pLaptopIcons[5], guiTITLEBARICONS, 2);
				SetCurrentToLastProgramOpened();
				break;

			case LAPTOP_MODE_FINANCES:
				gLaptopProgramStates[LAPTOP_PROGRAM_FINANCES] = LAPTOP_PROGRAM_MINIMIZED;
				InitTitleBarMaximizeGraphics(guiTITLEBARLAPTOP, pLaptopIcons[2], guiTITLEBARICONS, 5);
				SetCurrentToLastProgramOpened();
				break;

			case LAPTOP_MODE_HISTORY:
				gLaptopProgramStates[LAPTOP_PROGRAM_HISTORY] = LAPTOP_PROGRAM_MINIMIZED;
				InitTitleBarMaximizeGraphics(guiTITLEBARLAPTOP, pLaptopIcons[4], guiTITLEBARICONS, 4);
				SetCurrentToLastProgramOpened();
				break;

			case LAPTOP_MODE_PERSONNEL:
				gLaptopProgramStates[LAPTOP_PROGRAM_PERSONNEL] = LAPTOP_PROGRAM_MINIMIZED;
				InitTitleBarMaximizeGraphics(guiTITLEBARLAPTOP, pLaptopIcons[3], guiTITLEBARICONS, 3);
				SetCurrentToLastProgramOpened();
				break;

			case LAPTOP_MODE_NONE: return; // nothing

			default:
				gLaptopProgramStates[LAPTOP_PROGRAM_WEB_BROWSER] = LAPTOP_PROGRAM_MINIMIZED;
				InitTitleBarMaximizeGraphics(guiTITLEBARLAPTOP, pWebTitle, guiTITLEBARICONS, 1);
				SetCurrentToLastProgramOpened();
				gfShowBookmarks = FALSE;
				fShowBookmarkInfo = FALSE;
				break;
		}
		fMinizingProgram = TRUE;
		fInitTitle = TRUE;
	}
}


void DisplayProgramBoundingBox(BOOLEAN fMarkButtons)
{
	// the border for the program
	BltVideoObject(FRAME_BUFFER, guiLaptopBACKGROUND, 1, 25, 23);

	// no laptop mode, no border around the program
	if (guiCurrentLaptopMode != LAPTOP_MODE_NONE)
	{
		BltVideoObject(FRAME_BUFFER, guiLaptopBACKGROUND, 0, 108, 23);
	}

	if (fMarkButtons || fLoadPendingFlag)
	{
		MarkButtonsDirty();
		RenderButtons();
	}

	PrintDate();
	PrintBalance();
	PrintNumberOnTeam();
	DisplayTaskBarIcons();
}


static void NewEmailIconCallback(MOUSE_REGION* pRegion, INT32 iReason);
static void NewFileIconCallback(MOUSE_REGION* pRegion, INT32 iReason);


static void CreateDestroyMouseRegionForNewMailIcon(void)
{
	static BOOLEAN fCreated = FALSE;

	// will toggle creation/destruction of the mouse regions used by the new mail icon
	if (!fCreated)
	{
		fCreated = TRUE;
		{
			const UINT16 x = LAPTOP__NEW_EMAIL_ICON_X;
			const UINT16 y = LAPTOP__NEW_EMAIL_ICON_Y;
			MSYS_DefineRegion(&gNewMailIconRegion, x, y + 5,  x + 16,  y + 16, MSYS_PRIORITY_HIGHEST - 3, CURSOR_LAPTOP_SCREEN, MSYS_NO_CALLBACK, NewEmailIconCallback);
			CreateFileAndNewEmailIconFastHelpText(LAPTOP_BN_HLP_TXT_YOU_HAVE_NEW_MAIL, fUnReadMailFlag == 0);
		}

		{
			const UINT16 x = LAPTOP__NEW_FILE_ICON_X;
			const UINT16 y = LAPTOP__NEW_FILE_ICON_Y;
			MSYS_DefineRegion(&gNewFileIconRegion, x, y + 5,  x + 16,  y + 16, MSYS_PRIORITY_HIGHEST - 3, CURSOR_LAPTOP_SCREEN, MSYS_NO_CALLBACK, NewFileIconCallback);
			CreateFileAndNewEmailIconFastHelpText(LAPTOP_BN_HLP_TXT_YOU_HAVE_NEW_FILE, fNewFilesInFileViewer == 0);
		}
	}
	else
	{
		fCreated = FALSE;
		MSYS_RemoveRegion(&gNewMailIconRegion);
		MSYS_RemoveRegion(&gNewFileIconRegion);
	}
}


static void NewEmailIconCallback(MOUSE_REGION* pRegion, INT32 iReason)
{
	if (iReason & MSYS_CALLBACK_REASON_LBUTTON_UP)
	{
		if (fUnReadMailFlag)
		{
			fOpenMostRecentUnReadFlag = TRUE;
			guiCurrentLaptopMode = LAPTOP_MODE_EMAIL;
		}
	}
}


static void NewFileIconCallback(MOUSE_REGION* pRegion, INT32 iReason)
{
	if (iReason & MSYS_CALLBACK_REASON_LBUTTON_UP)
	{
		if (fNewFilesInFileViewer)
		{
			fEnteredFileViewerFromNewFileIcon = TRUE;
			guiCurrentLaptopMode = LAPTOP_MODE_FILES;
		}
	}
}


static void HandleWWWSubSites(void)
{
	// check to see if WW Wait is needed for a sub site within the Web Browser
	if (guiCurrentLaptopMode == guiPreviousLaptopMode ||
			guiCurrentLaptopMode < LAPTOP_MODE_WWW ||
			fLoadPendingFlag ||
			fDoneLoadPending ||
			guiPreviousLaptopMode < LAPTOP_MODE_WWW)
	{
		// no go, leave
		return;
	}

	fLoadPendingFlag = TRUE;
	fConnectingToSubPage = TRUE;

	// fast or slow load?
	if (gfWWWaitSubSitesVisitedFlags[guiCurrentLaptopMode - (LAPTOP_MODE_WWW + 1)])
	{
		fFastLoadFlag = TRUE;
	}

	// set fact we were here
	gfWWWaitSubSitesVisitedFlags[guiCurrentLaptopMode - (LAPTOP_MODE_WWW + 1)] = TRUE;

	//Dont show the dlownload screen when switching between these pages
	if (guiCurrentLaptopMode == LAPTOP_MODE_AIM_MEMBERS              && guiPreviousLaptopMode == LAPTOP_MODE_AIM_MEMBERS_FACIAL_INDEX ||
			guiCurrentLaptopMode == LAPTOP_MODE_AIM_MEMBERS_FACIAL_INDEX && guiPreviousLaptopMode == LAPTOP_MODE_AIM_MEMBERS)
	{
		fFastLoadFlag = FALSE;
		fLoadPendingFlag = FALSE;

		// set fact we were here
		gfWWWaitSubSitesVisitedFlags[LAPTOP_MODE_AIM_MEMBERS_FACIAL_INDEX - (LAPTOP_MODE_WWW + 1)] = TRUE;
		gfWWWaitSubSitesVisitedFlags[LAPTOP_MODE_AIM_MEMBERS              - (LAPTOP_MODE_WWW + 1)] = TRUE;
	}
}


static void UpdateStatusOfDisplayingBookMarks(void)
{
	// this function will disable showing of bookmarks if in process of download or if we miniming web browser
	if (fLoadPendingFlag || guiCurrentLaptopMode < LAPTOP_MODE_WWW)
	{
		gfShowBookmarks = FALSE;
	}
}


static void InitalizeSubSitesList(void)
{
	// init all subsites list to not visited
	for (INT32 i = LAPTOP_MODE_WWW + 1; i <= LAPTOP_MODE_FUNERAL; ++i)
	{
		gfWWWaitSubSitesVisitedFlags[i - (LAPTOP_MODE_WWW + 1)] = FALSE;
	}
}


static void SetSubSiteAsVisted(void)
{
	// sets a www sub site as visited
	// not at a web page yet?
	if (guiCurrentLaptopMode <= LAPTOP_MODE_WWW) return;

	gfWWWaitSubSitesVisitedFlags[guiCurrentLaptopMode - (LAPTOP_MODE_WWW + 1)] = TRUE;
}


static void HandleShiftAltTabKeyInLaptop(void)
{
	// will handle the alt tab keying in laptop

	// move to next program
	if (fMaximizingProgram) return;

	switch (guiCurrentLaptopMode)
	{
		case LAPTOP_MODE_FINANCES:  guiCurrentLaptopMode = LAPTOP_MODE_PERSONNEL; break;
		case LAPTOP_MODE_PERSONNEL: guiCurrentLaptopMode = LAPTOP_MODE_HISTORY;   break;
		case LAPTOP_MODE_HISTORY:   guiCurrentLaptopMode = LAPTOP_MODE_FILES;     break;
		case LAPTOP_MODE_EMAIL:     guiCurrentLaptopMode = LAPTOP_MODE_FINANCES;  break;
		case LAPTOP_MODE_FILES:     guiCurrentLaptopMode = LAPTOP_MODE_WWW;       break;
		case LAPTOP_MODE_NONE:      guiCurrentLaptopMode = LAPTOP_MODE_FINANCES;  break;
		case LAPTOP_MODE_WWW:       guiCurrentLaptopMode = LAPTOP_MODE_EMAIL;     break;
		default:                    guiCurrentLaptopMode = LAPTOP_MODE_EMAIL;     break;
	}

	fPausedReDrawScreenFlag = TRUE;
}


static void HandleAltTabKeyInLaptop(void)
{
	// will handle the alt tab keying in laptop

	// move to next program
	if (fMaximizingProgram) return;

	switch (guiCurrentLaptopMode)
	{
		case LAPTOP_MODE_FINANCES:  guiCurrentLaptopMode = LAPTOP_MODE_EMAIL;     break;
		case LAPTOP_MODE_PERSONNEL: guiCurrentLaptopMode = LAPTOP_MODE_FINANCES;  break;
		case LAPTOP_MODE_HISTORY:   guiCurrentLaptopMode = LAPTOP_MODE_PERSONNEL; break;
		case LAPTOP_MODE_EMAIL:     guiCurrentLaptopMode = LAPTOP_MODE_WWW;       break;
		case LAPTOP_MODE_FILES:     guiCurrentLaptopMode = LAPTOP_MODE_HISTORY;   break;
		case LAPTOP_MODE_NONE:      guiCurrentLaptopMode = LAPTOP_MODE_EMAIL;     break;
		default:                    guiCurrentLaptopMode = LAPTOP_MODE_FILES;     break;
	}

	fPausedReDrawScreenFlag = TRUE;
}


static void HandleWebBookMarkNotifyTimer(void);


// display the 2 second book mark instruction
static void DisplayWebBookMarkNotify(void)
{
	static BOOLEAN fOldShow = FALSE;

	// handle the timer for this thing
	HandleWebBookMarkNotifyTimer();

	// are we about to start showing box? or did we just stop?
	if ((!fOldShow || fReDrawBookMarkInfo) && fShowBookmarkInfo)
	{
		fOldShow = TRUE;
		fReDrawBookMarkInfo = FALSE;

		// show background objects
		BltVideoObject(FRAME_BUFFER, guiDOWNLOADTOP,   0,DOWNLOAD_X,     DOWNLOAD_Y);
		BltVideoObject(FRAME_BUFFER, guiDOWNLOADMID,   0,DOWNLOAD_X,     DOWNLOAD_Y +     DOWN_HEIGHT);
		BltVideoObject(FRAME_BUFFER, guiDOWNLOADBOT,   0,DOWNLOAD_X,     DOWNLOAD_Y + 2 * DOWN_HEIGHT);
		BltVideoObject(FRAME_BUFFER, guiTITLEBARICONS, 1,DOWNLOAD_X + 4, DOWNLOAD_Y + 1);

		// font stuff
		SetFont(DOWNLOAD_FONT);
		SetFontForeground(FONT_WHITE);
		SetFontBackground(FONT_BLACK);
		SetFontShadow(NO_SHADOW);

		// display download string
		MPrint(DOWN_STRING_X, DOWN_STRING_Y, pShowBookmarkString[0]);

		SetFont(BOOK_FONT);
		SetFontForeground(FONT_BLACK);

		// now draw the message
		DisplayWrappedString(DOWN_STRING_X - 42, DOWN_STRING_Y + 20, BOOK_WIDTH + 45, 2, BOOK_FONT,FONT_BLACK, pShowBookmarkString[1], FONT_BLACK, CENTER_JUSTIFIED);

		// invalidate region
		InvalidateRegion(DOWNLOAD_X, DOWNLOAD_Y, DOWNLOAD_X + 150, DOWNLOAD_Y + 100);

		SetFontShadow(DEFAULT_SHADOW);
	}
	else if (fOldShow && !fShowBookmarkInfo)
	{
		fOldShow = FALSE;
		fPausedReDrawScreenFlag = TRUE;
	}
}


// handle timer for bookmark notify
static void HandleWebBookMarkNotifyTimer(void)
{
	static INT32 iBaseTime              = 0;
	static BOOLEAN fOldShowBookMarkInfo = FALSE;

	// check if maxing or mining?
	if (fMaximizingProgram || fMinizingProgram)
	{
		fOldShowBookMarkInfo |= fShowBookmarkInfo;
		fShowBookmarkInfo     = FALSE;
		return;
	}

	// if we were going to show this pop up, but were delayed, then do so now
	fShowBookmarkInfo |= fOldShowBookMarkInfo;

	// reset old flag
	fOldShowBookMarkInfo = FALSE;

	// if current mode is too low, then reset
	if (guiCurrentLaptopMode < LAPTOP_MODE_WWW)
	{
		fShowBookmarkInfo = FALSE;
	}

	// if showing bookmarks, don't show help
	if (gfShowBookmarks) fShowBookmarkInfo = FALSE;

	// check if flag false, is so, leave
	if (!fShowBookmarkInfo)
	{
		iBaseTime = 0;
		return;
	}

	// check if this is the first time in here
	if (iBaseTime == 0)
	{
		iBaseTime = GetJA2Clock();
		return;
	}

	const INT32 iDifference = GetJA2Clock() - iBaseTime;

	fReDrawBookMarkInfo = TRUE;

	if (iDifference > DISPLAY_TIME_FOR_WEB_BOOKMARK_NOTIFY)
	{
		// waited long enough, stop showing
		iBaseTime = 0;
		fShowBookmarkInfo = FALSE;
	}
}


void ClearOutTempLaptopFiles(void)
{
	// clear out all temp files from laptop
	FileDelete("files.dat");
	FileDelete("finances.dat");
	FileDelete("email.dat");
	FileDelete("history.dat");
}


BOOLEAN SaveLaptopInfoToSavedGame(HWFILE hFile)
{
	// Save The laptop information
	if (!FileWrite(hFile, &LaptopSaveInfo, sizeof(LaptopSaveInfoStruct))) return FALSE;

	//If there is anything in the Bobby Ray Orders on Delivery
	if (LaptopSaveInfo.usNumberOfBobbyRayOrderUsed)
	{
		//Allocate memory for the information
		const UINT32 uiSize = sizeof(BobbyRayOrderStruct) * LaptopSaveInfo.usNumberOfBobbyRayOrderItems;

		// Load The laptop information
		if (!FileWrite(hFile, LaptopSaveInfo.BobbyRayOrdersOnDeliveryArray, uiSize)) return FALSE;
	}


	//If there is any Insurance Payouts in progress
	if (LaptopSaveInfo.ubNumberLifeInsurancePayoutUsed)
	{
		//Allocate memory for the information
		const UINT32 uiSize = sizeof(LIFE_INSURANCE_PAYOUT) * LaptopSaveInfo.ubNumberLifeInsurancePayouts;

		// Load The laptop information
		if (!FileWrite(hFile, LaptopSaveInfo.pLifeInsurancePayouts, uiSize)) return FALSE;
	}

	return TRUE;
}


BOOLEAN LoadLaptopInfoFromSavedGame(HWFILE hFile)
{
	//if there is memory allocated for the BobbyR orders
	if (LaptopSaveInfo.usNumberOfBobbyRayOrderItems)
	{
		//Free the memory
		if (LaptopSaveInfo.BobbyRayOrdersOnDeliveryArray)
		{
			MemFree(LaptopSaveInfo.BobbyRayOrdersOnDeliveryArray);
		}
		LaptopSaveInfo.BobbyRayOrdersOnDeliveryArray = NULL;
	}

	//if there is memory allocated for life insurance payouts
	if (LaptopSaveInfo.ubNumberLifeInsurancePayouts)
	{
		Assert(LaptopSaveInfo.pLifeInsurancePayouts != NULL); //Should never happen

		//Free the memory
		MemFree(LaptopSaveInfo.pLifeInsurancePayouts);
		LaptopSaveInfo.pLifeInsurancePayouts = NULL;
	}


	// Load The laptop information
	if (!FileRead(hFile, &LaptopSaveInfo, sizeof(LaptopSaveInfoStruct))) return FALSE;


	//If there is anything in the Bobby Ray Orders on Delivery
	if (LaptopSaveInfo.usNumberOfBobbyRayOrderUsed)
	{
		//Allocate memory for the information
		const UINT32 uiSize = sizeof(BobbyRayOrderStruct) * LaptopSaveInfo.usNumberOfBobbyRayOrderItems;
		LaptopSaveInfo.BobbyRayOrdersOnDeliveryArray = MALLOCN(BobbyRayOrderStruct, LaptopSaveInfo.usNumberOfBobbyRayOrderItems);
		Assert(LaptopSaveInfo.BobbyRayOrdersOnDeliveryArray);

		// Load The laptop information
		if (!FileRead(hFile, LaptopSaveInfo.BobbyRayOrdersOnDeliveryArray, uiSize)) return FALSE;
	}
	else
	{
		LaptopSaveInfo.usNumberOfBobbyRayOrderItems  = 0;
		LaptopSaveInfo.BobbyRayOrdersOnDeliveryArray = NULL;
	}


	//If there is any Insurance Payouts in progress
	if (LaptopSaveInfo.ubNumberLifeInsurancePayoutUsed)
	{
		//Allocate memory for the information
		const UINT32 uiSize = sizeof(LIFE_INSURANCE_PAYOUT) * LaptopSaveInfo.ubNumberLifeInsurancePayouts;
		LaptopSaveInfo.pLifeInsurancePayouts = MALLOCN(LIFE_INSURANCE_PAYOUT, LaptopSaveInfo.ubNumberLifeInsurancePayouts);
		Assert(LaptopSaveInfo.pLifeInsurancePayouts);

		// Load The laptop information
		if (!FileRead(hFile, LaptopSaveInfo.pLifeInsurancePayouts, uiSize)) return FALSE;
	}
	else
	{
		LaptopSaveInfo.ubNumberLifeInsurancePayouts = 0;
		LaptopSaveInfo.pLifeInsurancePayouts        = NULL;
	}

	return TRUE;
}


static INT32 WWaitDelayIncreasedIfRaining(INT32 iUnitTime)
{
	INT32 iRetVal = 0;
	if (guiEnvWeather & WEATHER_FORECAST_THUNDERSHOWERS)
	{
		iRetVal = iUnitTime * 0.8f;
	}
	else if (guiEnvWeather & WEATHER_FORECAST_SHOWERS)
	{
		iRetVal = iUnitTime * 0.6f;
	}
	return iRetVal;
}


// Used to determine delay if its raining
static BOOLEAN IsItRaining(void)
{
	return
		guiEnvWeather & WEATHER_FORECAST_SHOWERS ||
		guiEnvWeather & WEATHER_FORECAST_THUNDERSHOWERS;
}


static void InternetRainDelayMessageBoxCallBack(UINT8 bExitValue)
{
	GoToWebPage(giRainDelayInternetSite);

	//Set to -2 so we dont due the message for this occurence of laptop
	giRainDelayInternetSite = -2;
}


void CreateFileAndNewEmailIconFastHelpText(UINT32 uiHelpTextID, BOOLEAN fClearHelpText)
{
	MOUSE_REGION* pRegion;
	switch (uiHelpTextID)
	{
		case LAPTOP_BN_HLP_TXT_YOU_HAVE_NEW_MAIL: pRegion = &gNewMailIconRegion; break;
		case LAPTOP_BN_HLP_TXT_YOU_HAVE_NEW_FILE: pRegion = &gNewFileIconRegion; break;

		default:
			Assert(0);
			return;
	}

	const wchar_t* help = (fClearHelpText ? L"" : gzLaptopHelpText[uiHelpTextID]);
	SetRegionFastHelpText(pRegion, help);
}
