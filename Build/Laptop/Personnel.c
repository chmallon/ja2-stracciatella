#include "Font.h"
#include "Laptop.h"
#include "Local.h"
#include "Personnel.h"
#include "VObject.h"
#include "WCheck.h"
#include "Debug.h"
#include "WordWrap.h"
#include "Render_Dirty.h"
#include "Cursors.h"
#include "Overhead.h"
#include "Soldier_Profile.h"
#include "Text.h"
#include "MapScreen.h"
#include "Game_Clock.h"
#include "Finances.h"
#include "LaptopSave.h"
#include "Input.h"
#include "Random.h"
#include "Line.h"
#include "Assignments.h"
#include "Interface_Items.h"
#include "Weapons.h"
#include "StrategicMap.h"
#include "Animation_Data.h"
#include "Merc_Contract.h"
#include "Button_System.h"
#include "VSurface.h"
#include "Font_Control.h"
#include "EMail.h"
#include "Soldier_Macros.h"


#define INVENTORY_BOX_X 399
#define INVENTORY_BOX_Y 205
#define INVENTORY_BOX_W 171
#define INVENTORY_BOX_H 232


#define IMAGE_BOX_X              395
#define IMAGE_BOX_Y              LAPTOP_SCREEN_UL_Y + 24
#define IMAGE_NAME_WIDTH         106
#define IMAGE_FULL_NAME_OFFSET_Y 111
#define TEXT_BOX_WIDTH           160
#define TEXT_DELTA_OFFSET          9
#define PERS_CURR_TEAM_X         LAPTOP_SCREEN_UL_X +  39 - 15
#define PERS_CURR_TEAM_Y         LAPTOP_SCREEN_UL_Y + 218
#define PERS_DEPART_TEAM_Y       LAPTOP_SCREEN_UL_Y + 247

#define MAX_STATS                       20
#define PERS_FONT                      FONT10ARIAL
#define CHAR_NAME_FONT                 FONT12ARIAL
#define CHAR_NAME_LOC_X                432
#define CHAR_NAME_LOC_WIDTH            124
#define CHAR_NAME_Y                    177
#define CHAR_LOC_Y                     189
#define PERS_TEXT_FONT_COLOR           FONT_WHITE
#define PERS_TEXT_FONT_ALTERNATE_COLOR FONT_YELLOW
#define PERS_FONT_COLOR                FONT_WHITE


#define FACES_DIR       "FACES/BIGFACES/"
#define SMALL_FACES_DIR "FACES/"

#define NEXT_MERC_FACE_X   LAPTOP_SCREEN_UL_X + 448
#define MERC_FACE_SCROLL_Y LAPTOP_SCREEN_UL_Y + 150
#define PREV_MERC_FACE_X   LAPTOP_SCREEN_UL_X + 285


#define DEPARTED_X LAPTOP_SCREEN_UL_X +  29 - 10
#define DEPARTED_Y LAPTOP_SCREEN_UL_Y + 207


#define PERSONNEL_PORTRAIT_NUMBER       20
#define PERSONNEL_PORTRAIT_NUMBER_WIDTH  5

#define SMALL_PORTRAIT_WIDTH  46
#define SMALL_PORTRAIT_HEIGHT 42

#define SMALL_PORT_WIDTH  52
#define SMALL_PORT_HEIGHT 45

#define SMALL_PORTRAIT_WIDTH_NO_BORDERS 48

#define SMALL_PORTRAIT_START_X 141 - 10
#define SMALL_PORTRAIT_START_Y  53

#define PERS_CURR_TEAM_COST_X LAPTOP_SCREEN_UL_X + 150 - 10
#define PERS_CURR_TEAM_COST_Y LAPTOP_SCREEN_UL_Y + 218

#define PERS_CURR_TEAM_HIGHEST_Y PERS_CURR_TEAM_COST_Y    + 15
#define PERS_CURR_TEAM_LOWEST_Y  PERS_CURR_TEAM_HIGHEST_Y + 15

#define PERS_CURR_TEAM_WIDTH 286 - 160

#define PERS_DEPART_TEAM_WIDTH PERS_CURR_TEAM_WIDTH - 20

#define PERS_STAT_AVG_X        LAPTOP_SCREEN_UL_X + 157 - 10
#define PERS_STAT_AVG_Y        LAPTOP_SCREEN_UL_Y + 274
#define PERS_STAT_AVG_WIDTH    202 - 159
#define PERS_STAT_LOWEST_X     LAPTOP_SCREEN_UL_X +  72 - 10
#define PERS_STAT_LOWEST_WIDTH 155 - 75
#define PERS_STAT_HIGHEST_X    LAPTOP_SCREEN_UL_X + 205 - 10
#define PERS_STAT_LIST_X       LAPTOP_SCREEN_UL_X +  33 - 10


#define PERS_TOGGLE_CUR_DEPART_WIDTH  106 -  35
#define PERS_TOGGLE_CUR_DEPART_HEIGHT 236 - 212

#define PERS_TOGGLE_CUR_DEPART_X LAPTOP_SCREEN_UL_X +  35 - 10
#define PERS_TOGGLE_CUR_Y        LAPTOP_SCREEN_UL_Y + 208
#define PERS_TOGGLE_DEPART_Y     LAPTOP_SCREEN_UL_Y + 238

#define PERS_DEPARTED_UP_X   LAPTOP_SCREEN_UL_X + 265 - 10
#define PERS_DEPARTED_UP_Y   LAPTOP_SCREEN_UL_Y + 210
#define PERS_DEPARTED_DOWN_Y LAPTOP_SCREEN_UL_Y + 237


#define PERS_TITLE_X 140
#define PERS_TITLE_Y  33

#define ATM_UL_X LAPTOP_SCREEN_UL_X + 397
#define ATM_UL_Y LAPTOP_SCREEN_UL_Y +  27

#define ATM_FONT PERS_FONT

// departed states
enum
{
	DEPARTED_DEAD = 0,
	DEPARTED_FIRED,
	DEPARTED_OTHER,
	DEPARTED_MARRIED,
	DEPARTED_CONTRACT_EXPIRED,
	DEPARTED_QUIT,
};

// atm button positions
#define ATM_DISPLAY_X      509
#define ATM_DISPLAY_Y       58
#define ATM_DISPLAY_HEIGHT  10
#define ATM_DISPLAY_WIDTH   81


// the number of inventory items per personnel page
#define NUMBER_OF_INVENTORY_PERSONNEL 8
#define Y_SIZE_OF_PERSONNEL_SCROLL_REGION (422 - 219)
#define X_SIZE_OF_PERSONNEL_SCROLL_REGION (589 - 573)
#define Y_OF_PERSONNEL_SCROLL_REGION 219
#define X_OF_PERSONNEL_SCROLL_REGION 573
#define SIZE_OF_PERSONNEL_CURSOR 19


// enums for the buttons in the information side bar (used with giPersonnelATMStartButton[])
enum
{
	PERSONNEL_STAT_BTN,
	PERSONNEL_EMPLOYMENT_BTN,
	PERSONNEL_INV_BTN,

	PERSONNEL_NUM_BTN,
};


//enums for the current state of the information side bar (stat panel)
enum
{
	PRSNL_STATS,
	PRSNL_EMPLOYMENT,
	PRSNL_INV,
};
static UINT8 gubPersonnelInfoState = PRSNL_STATS;


//enums for the pPersonnelScreenStrings[]
enum
{
	PRSNL_TXT_HEALTH,           //    HEALTH OF MERC
	PRSNL_TXT_AGILITY,
	PRSNL_TXT_DEXTERITY,
	PRSNL_TXT_STRENGTH,
	PRSNL_TXT_LEADERSHIP,
	PRSNL_TXT_WISDOM,           //  5
	PRSNL_TXT_EXP_LVL,          //    EXPERIENCE LEVEL
	PRSNL_TXT_MARKSMANSHIP,
	PRSNL_TXT_MECHANICAL,
	PRSNL_TXT_EXPLOSIVES,
	PRSNL_TXT_MEDICAL,          // 10
	PRSNL_TXT_MED_DEPOSIT,      //    AMOUNT OF MEDICAL DEPOSIT PUT DOWN ON THE MERC
	PRSNL_TXT_CURRENT_CONTRACT, //    COST OF CURRENT CONTRACT
	PRSNL_TXT_KILLS,            //    NUMBER OF KILLS BY MERC
	PRSNL_TXT_ASSISTS,          //    NUMBER OF ASSISTS ON KILLS BY MERC
	PRSNL_TXT_DAILY_COST,       // 15 DAILY COST OF MERC
	PRSNL_TXT_TOTAL_COST,       //    TOTAL COST OF MERC
	PRSNL_TXT_CONTRACT,         //    COST OF CURRENT CONTRACT
	PRSNL_TXT_TOTAL_SERVICE,    //    TOTAL SERVICE RENDERED BY MERC
	PRSNL_TXT_UNPAID_AMOUNT,    //    AMOUNT LEFT ON MERC MERC TO BE PAID
	PRSNL_TXT_HIT_PERCENTAGE,   // 20 PERCENTAGE OF SHOTS THAT HIT TARGET
	PRSNL_TXT_BATTLES,          //    NUMBER OF BATTLES FOUGHT
	PRSNL_TXT_TIMES_WOUNDED,    //    NUMBER OF TIMES MERC HAS BEEN WOUNDED
	PRSNL_TXT_SKILLS,
	PRSNL_TXT_NOSKILLS,
};

static UINT8 uiCurrentInventoryIndex = 0;

static UINT32 guiSliderPosition;


static const INT16 pers_stat_x       = 407;
static const INT16 pers_stat_delta_x = 407 + TEXT_BOX_WIDTH - 20 + TEXT_DELTA_OFFSET;
static const INT16 pers_stat_data_x  = 407 + 36;
static const INT16 pers_stat_y[] =
{
	215,
	225,
	235,
	245,
	255,
	265,
	325,
	280,
	290,
	300,
	310, // 10
	405,
	395,
	425,
	435,
	455,
	390, // for contract price
	445,
	 33, // Personnel Header // XXX unused
	340,
	350, // 20
	365,
	375,
	385,
	395,
	405
};


static UINT32 guiSCREEN;
static UINT32 guiTITLE;
static UINT32 guiDEPARTEDTEAM;
static UINT32 guiCURRENTTEAM;
static UINT32 guiPersonnelInventory;

static INT32 giPersonnelButton[6];
static INT32 giPersonnelInventoryButtons[2];

// buttons for ATM
static INT32 giPersonnelATMStartButton[3];
static INT32 giPersonnelATMStartButtonImage[3];

// the id of currently displayed merc in right half of screen
static INT32 iCurrentPersonSelectedId = -1;

static INT32 giCurrentUpperLeftPortraitNumber = 0;

// which mode are we showing?..current team?...or deadly departed?
static BOOLEAN fCurrentTeamMode = TRUE;

BOOLEAN fShowAtmPanelStartButton = TRUE;

// mouse regions
static MOUSE_REGION gPortraitMouseRegions[PERSONNEL_PORTRAIT_NUMBER];

static MOUSE_REGION gTogglePastCurrentTeam[2];

static MOUSE_REGION gMouseScrollPersonnelINV;

static INT32 iCurPortraitId = 0;


static void InitPastCharactersList(void);


void GameInitPersonnel(void)
{
	// init past characters lists
	InitPastCharactersList();
}


static void CreateDestroyCurrentDepartedMouseRegions(BOOLEAN create);
static void CreateDestroyMouseRegionsForPersonnelPortraits(BOOLEAN create);
static void CreatePersonnelButtons(void);
static void SelectFirstDisplayedMerc(void);
static BOOLEAN LoadPersonnelGraphics(void);
static BOOLEAN LoadPersonnelScreenBackgroundGraphics(void);
static void SetPersonnelButtonStates(void);


void EnterPersonnel(void)
{
	fReDrawScreenFlag=TRUE;

	uiCurrentInventoryIndex = 0;
	guiSliderPosition = 0;

	iCurPortraitId = 0;

	// load graphics for screen
	LoadPersonnelGraphics();

	// show atm panel
	fShowAtmPanelStartButton = TRUE;

	// load personnel
	LoadPersonnelScreenBackgroundGraphics();

	SelectFirstDisplayedMerc();

	// render screen
	RenderPersonnel();

	CreateDestroyMouseRegionsForPersonnelPortraits(TRUE);
	CreateDestroyCurrentDepartedMouseRegions(TRUE);

	// create buttons for screen
	CreatePersonnelButtons();

	// set states of en- dis able buttons
	SetPersonnelButtonStates();
}


static void CreateDestroyButtonsForDepartedTeamList(void);
static void CreateDestroyPersonnelInventoryScrollButtons(void);
static void CreateDestroyStartATMButton(void);
static void DeletePersonnelButtons(void);
static void DeletePersonnelScreenBackgroundGraphics(void);
static void RemovePersonnelGraphics(void);


void ExitPersonnel(void)
{
	if (!fCurrentTeamMode)
	{
		fCurrentTeamMode = TRUE;
		CreateDestroyButtonsForDepartedTeamList();
		fCurrentTeamMode = FALSE;
	}

	// get rid of atm panel buttons
	fShowAtmPanelStartButton = FALSE;
	CreateDestroyStartATMButton();

	gubPersonnelInfoState = PRSNL_STATS;

	CreateDestroyPersonnelInventoryScrollButtons();

	// get rid of graphics
	RemovePersonnelGraphics();

	DeletePersonnelScreenBackgroundGraphics();

	// delete buttons
	DeletePersonnelButtons();

	CreateDestroyMouseRegionsForPersonnelPortraits(FALSE);
	CreateDestroyCurrentDepartedMouseRegions(FALSE);
}


static void EnableDisableDeparturesButtons(void);
static void EnableDisableInventoryScrollButtons(void);
static void HandlePersonnelKeyboard(void);


void HandlePersonnel(void)
{
	// create / destroy buttons for scrolling departed list
	CreateDestroyButtonsForDepartedTeamList();

	// enable / disable departures buttons
	EnableDisableDeparturesButtons();

	// create destroy inv buttons as needed
	CreateDestroyPersonnelInventoryScrollButtons();

	// enable disable buttons as needed
	EnableDisableInventoryScrollButtons();

	HandlePersonnelKeyboard();
}


static BOOLEAN LoadPersonnelGraphics(void)
{
	// load graphics needed for personnel screen

	// title bar
	guiTITLE = AddVideoObjectFromFile("LAPTOP/programtitlebar.sti");
	CHECKF(guiTITLE != NO_VOBJECT);

	// the background grpahics
	guiSCREEN = AddVideoObjectFromFile("LAPTOP/personnelwindow.sti");
	CHECKF(guiSCREEN != NO_VOBJECT);

	guiPersonnelInventory = AddVideoObjectFromFile("LAPTOP/personnel_inventory.sti");
	CHECKF(guiPersonnelInventory != NO_VOBJECT);

	return TRUE;
}


static void RemovePersonnelGraphics(void)
{
	// delete graphics needed for personnel screen
	DeleteVideoObjectFromIndex(guiSCREEN);
	DeleteVideoObjectFromIndex(guiTITLE);
	DeleteVideoObjectFromIndex(guiPersonnelInventory);
}


static void DisplayAmountOnCurrentMerc(void);
static void DisplayFaceOfDisplayedMerc(void);
static void DisplayInventoryForSelectedChar(void);
static void DisplayNumberDeparted(void);
static void DisplayNumberOnCurrentTeam(void);
static void DisplayPastMercsPortraits(void);
static void DisplayPersonnelTextOnTitleBar(void);
static BOOLEAN DisplayPicturesOfCurrentTeam(void);
static void DisplayStateOfPastTeamMembers(void);
static void DisplayTeamStats(void);
static BOOLEAN RenderAtmPanel(void);
static void RenderPersonnelScreenBackground(void);
static void UpDateStateOfStartButton(void);


void RenderPersonnel(void)
{
	// re-renders personnel screen
	// render main background

	BltVideoObjectFromIndex(FRAME_BUFFER, guiTITLE,  0, LAPTOP_SCREEN_UL_X, LAPTOP_SCREEN_UL_Y -  2);
	BltVideoObjectFromIndex(FRAME_BUFFER, guiSCREEN, 0, LAPTOP_SCREEN_UL_X, LAPTOP_SCREEN_UL_Y + 22);

	// render personnel screen background
	RenderPersonnelScreenBackground();

	// show team
	DisplayPicturesOfCurrentTeam();

	DisplayPastMercsPortraits();

	// show selected merc
	DisplayFaceOfDisplayedMerc();

	// show current team size
	DisplayNumberOnCurrentTeam();

	// show departed team size
	DisplayNumberDeparted();

	// showinventory of selected guy if applicable
	DisplayInventoryForSelectedChar();

	DisplayTeamStats();

	// past team
	DisplayStateOfPastTeamMembers();

	// title bar
	BlitTitleBarIcons();

	// show text on titlebar
	DisplayPersonnelTextOnTitleBar();

	// render the atm panel
	RenderAtmPanel();

	DisplayAmountOnCurrentMerc();

	// en-dis-able start button
	UpDateStateOfStartButton();
}


static void DisplayCharStats(const SOLDIERTYPE* s);
static void DisplayEmploymentinformation(const SOLDIERTYPE* s);


static void RenderPersonnelStats(const SOLDIERTYPE* const s)
{
	// will render the stats of person iId
	SetFont(PERS_FONT);
	SetFontForeground(PERS_TEXT_FONT_COLOR);
	SetFontBackground(FONT_BLACK);

	if (gubPersonnelInfoState == PERSONNEL_STAT_BTN)
	{
		DisplayCharStats(s);
	}
	else if (gubPersonnelInfoState == PERSONNEL_EMPLOYMENT_BTN)
	{
		DisplayEmploymentinformation(s);
	}
}


static void RenderPersonnelFace(const INT32 profile, const BOOLEAN alive)
{
	// draw face to profile iId
	const MERCPROFILESTRUCT* const p = &gMercProfiles[profile];

	char sTemp[100];
	sprintf(sTemp, FACES_DIR "%02d.sti", p->ubFaceIndex);

	UINT32 guiFACE = AddVideoObjectFromFile(sTemp);
	CHECKV(guiFACE != NO_VOBJECT);

	HVOBJECT hFaceHandle = GetVideoObject(guiFACE);

	if (!alive)
	{
		hFaceHandle->pShades[0] = Create16BPPPaletteShaded(hFaceHandle->pPaletteEntry, DEAD_MERC_COLOR_RED, DEAD_MERC_COLOR_GREEN, DEAD_MERC_COLOR_BLUE, TRUE);
		//set the red pallete to the face
		SetObjectHandleShade(guiFACE, 0);
	}

	BltVideoObject(FRAME_BUFFER, hFaceHandle, 0, IMAGE_BOX_X, IMAGE_BOX_Y);
	DeleteVideoObjectFromIndex(guiFACE);

	// Display the merc's name on the portrait
	const wchar_t* name = p->zName;

	const UINT16 x = IMAGE_BOX_X;
	const UINT16 y = IMAGE_BOX_Y + IMAGE_FULL_NAME_OFFSET_Y;
	const UINT16 w = IMAGE_NAME_WIDTH;
	const INT32 iHeightOfText = DisplayWrappedString(x, y, w, 1, PERS_FONT, PERS_FONT_COLOR, name, 0, CENTER_JUSTIFIED | DONT_DISPLAY_TEXT);

	const UINT16 line_height = GetFontHeight(PERS_FONT);
	if (iHeightOfText - 2 > line_height) // If the string will wrap
	{
		// Raise where we display it, and wrap it
		DisplayWrappedString(x, y - line_height, w, 1, PERS_FONT, PERS_FONT_COLOR, name, 0, CENTER_JUSTIFIED);
	}
	else
	{
		DrawTextToScreen(name, x, y, w, PERS_FONT, PERS_FONT_COLOR, 0, CENTER_JUSTIFIED);
	}
}


static void RenderPersonnelFaceCurrent(const SOLDIERTYPE* const s)
{
	if (s->uiStatusFlags & SOLDIER_VEHICLE) return;
	RenderPersonnelFace(s->ubProfile, s->bLife > 0);
}


static void RenderPersonnelFacePast(const UINT8 profile, const INT32 state)
{
	//if this is not a valid merc
	if (state != DEPARTED_DEAD &&
			state != DEPARTED_FIRED &&
			state != DEPARTED_OTHER)
	{
		return;
	}
	RenderPersonnelFace(profile, state != DEPARTED_DEAD);
}


static INT32 GetNumberOfMercsDeadOrAliveOnPlayersTeam(void);
static INT32 GetNumberOfPastMercsOnPlayersTeam(void);


static BOOLEAN NextPersonnelFace(void)
{
	if (iCurrentPersonSelectedId == -1)
	{
		return TRUE;
	}

	if (fCurrentTeamMode)
	{
		// wrap around?
		if (iCurrentPersonSelectedId == GetNumberOfMercsDeadOrAliveOnPlayersTeam() - 1)
		{
			iCurrentPersonSelectedId = 0;
			return FALSE; //def added 3/14/99 to enable disable buttons properly
		}
		else
		{
			iCurrentPersonSelectedId++;
		}
	}
	else
	{
		if (iCurPortraitId + 1 == GetNumberOfPastMercsOnPlayersTeam() - giCurrentUpperLeftPortraitNumber)
		{
			// about to go off the end
			giCurrentUpperLeftPortraitNumber = 0;
			iCurPortraitId = 0;
		}
		else if (iCurPortraitId == 19)
		{
			giCurrentUpperLeftPortraitNumber += PERSONNEL_PORTRAIT_NUMBER;
			iCurPortraitId = 0;
		}
		else
		{
			iCurPortraitId++;
		}
		// get of this merc in this slot
		iCurrentPersonSelectedId =  iCurPortraitId;
		fReDrawScreenFlag = TRUE;
	}

	return TRUE;
}


static BOOLEAN PrevPersonnelFace(void)
{
	if (iCurrentPersonSelectedId == -1)
	{
		return TRUE;
	}

	if (fCurrentTeamMode)
	{
		// wrap around?
		if (iCurrentPersonSelectedId == 0)
		{
			iCurrentPersonSelectedId = GetNumberOfMercsDeadOrAliveOnPlayersTeam() - 1;

			if (iCurrentPersonSelectedId == 0)
			{
				return FALSE; //def added 3/14/99 to enable disable buttons properly
			}
		}
		else
		{
			iCurrentPersonSelectedId--;
		}
	}
	else
	{
		if (iCurPortraitId == 0 && giCurrentUpperLeftPortraitNumber == 0)
		{
			// about to go off the end
			INT32 count_past = GetNumberOfPastMercsOnPlayersTeam();
			giCurrentUpperLeftPortraitNumber = count_past - count_past % PERSONNEL_PORTRAIT_NUMBER;
			iCurPortraitId = count_past % PERSONNEL_PORTRAIT_NUMBER;
			iCurPortraitId--;
		}
		else if (iCurPortraitId == 0)
		{
			giCurrentUpperLeftPortraitNumber -= PERSONNEL_PORTRAIT_NUMBER;
			iCurPortraitId = 19;
		}
		else
		{
			iCurPortraitId--;
		}
		// get of this merc in this slot

		iCurrentPersonSelectedId = iCurPortraitId;
		fReDrawScreenFlag = TRUE;
	}

	return TRUE;
}


static void LeftButtonCallBack(GUI_BUTTON* btn, INT32 reason);
static void RightButtonCallBack(GUI_BUTTON* btn, INT32 reason);


static void CreatePersonnelButtons(void)
{
	// left/right buttons
	giPersonnelButton[0] = QuickCreateButtonImg("LAPTOP/personnelbuttons.sti", -1, 0, -1, 1, -1, PREV_MERC_FACE_X, MERC_FACE_SCROLL_Y, MSYS_PRIORITY_HIGHEST - 1, LeftButtonCallBack);
	giPersonnelButton[1] = QuickCreateButtonImg("LAPTOP/personnelbuttons.sti", -1, 2, -1, 3, -1, NEXT_MERC_FACE_X, MERC_FACE_SCROLL_Y, MSYS_PRIORITY_HIGHEST - 1, RightButtonCallBack);

	SetButtonCursor(giPersonnelButton[0], CURSOR_LAPTOP_SCREEN);
	SetButtonCursor(giPersonnelButton[1], CURSOR_LAPTOP_SCREEN);
}


static void DeletePersonnelButtons(void)
{
	RemoveButton(giPersonnelButton[0]);
	RemoveButton(giPersonnelButton[1]);
}


static void LeftButtonCallBack(GUI_BUTTON* btn, INT32 reason)
{
	if (reason & MSYS_CALLBACK_REASON_LBUTTON_UP)
	{
		fReDrawScreenFlag = TRUE;
		PrevPersonnelFace();
		uiCurrentInventoryIndex = 0;
		guiSliderPosition = 0;
	}
}


static void RightButtonCallBack(GUI_BUTTON* btn, INT32 reason)
{
	if (reason & MSYS_CALLBACK_REASON_LBUTTON_UP)
	{
		fReDrawScreenFlag = TRUE;
		NextPersonnelFace();
		uiCurrentInventoryIndex = 0;
		guiSliderPosition = 0;
	}
}


static void DisplayCharName(const SOLDIERTYPE* const s)
{
	// get merc's nickName, assignment, and sector location info
	INT16 sX, sY;

	SetFont(CHAR_NAME_FONT);
	SetFontForeground(PERS_TEXT_FONT_COLOR);
	SetFontBackground(FONT_BLACK);

	if (s->uiStatusFlags & SOLDIER_VEHICLE) return;

	const wchar_t* sTownName = NULL;
	if (s->bAssignment != ASSIGNMENT_POW &&
			s->bAssignment != IN_TRANSIT)
	{
		// name of town, if any
		INT8 bTownId = GetTownIdForSector(s->sSectorX, s->sSectorY);
		if (bTownId != BLANK_SECTOR) sTownName = pTownNames[bTownId];
	}

	wchar_t sString[64];
	if (sTownName != NULL)
	{
		//nick name - town name
		swprintf(sString, lengthof(sString), L"%ls - %ls", s->name, sTownName);
	}
	else
	{
		//nick name
		wcslcpy(sString, s->name, lengthof(sString));
	}
	FindFontCenterCoordinates(CHAR_NAME_LOC_X, 0, CHAR_NAME_LOC_WIDTH, 0, sString, CHAR_NAME_FONT, &sX, &sY);
	mprintf(sX, CHAR_NAME_Y, sString);

	const wchar_t* Assignment = pPersonnelAssignmentStrings[s->bAssignment];
	FindFontCenterCoordinates(CHAR_NAME_LOC_X, 0, CHAR_NAME_LOC_WIDTH, 0, Assignment, CHAR_NAME_FONT, &sX, &sY);
	mprintf(sX, CHAR_LOC_Y, Assignment);
}


static void PrintStatWithDelta(UINT idx, INT8 stat, INT8 delta)
{
	wchar_t sString[50];
	INT16 sX;
	INT16 sY;

	const INT32 y = pers_stat_y[idx];
	if (delta > 0)
	{
		swprintf(sString, lengthof(sString), L"( %+d )", delta);
		FindFontRightCoordinates(pers_stat_delta_x, 0, 30, 0, sString, PERS_FONT, &sX, &sY);
		mprintf(sX, y, sString);
	}
	swprintf(sString, lengthof(sString), L"%d", stat);
	mprintf(pers_stat_x, y, pPersonnelScreenStrings[idx]);
	FindFontRightCoordinates(pers_stat_x, 0, TEXT_BOX_WIDTH - 20, 0, sString, PERS_FONT, &sX, &sY);
	mprintf(sX, y, sString);
}


static void PrintStat(UINT16 stat, INT32 y, const wchar_t* text)
{
	wchar_t sString[50];
	INT16 sX;
	INT16 sY;

	mprintf(pers_stat_x, y, text);
	swprintf(sString, lengthof(sString), L"%d", stat);
	FindFontRightCoordinates(pers_stat_x, 0, TEXT_BOX_WIDTH - 20, 0, sString, PERS_FONT, &sX, &sY);
	mprintf(sX, y, sString);
}


static void DisplayCharStats(const SOLDIERTYPE* const s)
{
	wchar_t sString[50];
	INT16 sX;
	INT16 sY;

	if (s->uiStatusFlags & SOLDIER_VEHICLE) return;

	const MERCPROFILESTRUCT* const p = &gMercProfiles[s->ubProfile];
	const BOOLEAN fAmIaRobot = AM_A_ROBOT(s);

	// Health
	if (s->bAssignment != ASSIGNMENT_POW)
	{
		if (p->bLifeDelta > 0)
		{
			swprintf(sString, lengthof(sString), L"( %+d )", p->bLifeDelta);
			FindFontRightCoordinates(pers_stat_delta_x, 0, 30, 0, sString, PERS_FONT, &sX, &sY);
			mprintf(sX, pers_stat_y[0], sString);
		}
		swprintf(sString, lengthof(sString), L"%d/%d", s->bLife, s->bLifeMax);
	}
	else
	{
		wcslcpy(sString, pPOWStrings[1], lengthof(sString));
	}
	mprintf(pers_stat_x, pers_stat_y[0], pPersonnelScreenStrings[PRSNL_TXT_HEALTH]);
	FindFontRightCoordinates(pers_stat_x, 0, TEXT_BOX_WIDTH - 20, 0, sString, PERS_FONT, &sX, &sY);
	mprintf(sX, pers_stat_y[0], sString);

	if  (fAmIaRobot)
	{
		for (INT32 i = 1; i < 11; ++i)
		{
			const INT32 y = pers_stat_y[i];
			mprintf(pers_stat_x, y, pPersonnelScreenStrings[i]);
			const wchar_t* const na = gpStrategicString[STR_PB_NOTAPPLICABLE_ABBREVIATION];
			FindFontRightCoordinates(pers_stat_x, 0, TEXT_BOX_WIDTH - 20, 0, na, PERS_FONT, &sX, &sY);
			mprintf(sX, y, na);
		}
	}
	else
	{
		PrintStatWithDelta( 1, p->bAgility,      p->bAgilityDelta);
		PrintStatWithDelta( 2, p->bDexterity,    p->bDexterityDelta);
		PrintStatWithDelta( 3, p->bStrength,     p->bStrengthDelta);
		PrintStatWithDelta( 4, p->bLeadership,   p->bLeadershipDelta);
		PrintStatWithDelta( 5, p->bWisdom,       p->bWisdomDelta);
		PrintStatWithDelta( 6, p->bExpLevel,     p->bExpLevelDelta);
		PrintStatWithDelta( 7, p->bMarksmanship, p->bMarksmanshipDelta);
		PrintStatWithDelta( 8, p->bMechanical,   p->bMechanicDelta);
		PrintStatWithDelta( 9, p->bExplosive,    p->bExplosivesDelta);
		PrintStatWithDelta(10, p->bMedical,      p->bMedicalDelta);
	}

	PrintStat(p->usKills,   pers_stat_y[21], pPersonnelScreenStrings[PRSNL_TXT_KILLS]);
	PrintStat(p->usAssists, pers_stat_y[22], pPersonnelScreenStrings[PRSNL_TXT_ASSISTS]);

	// Shots/hits
	mprintf(pers_stat_x, pers_stat_y[23], pPersonnelScreenStrings[PRSNL_TXT_HIT_PERCENTAGE]);
	// check we have shot at least once
	const UINT32 fired = p->usShotsFired;
	const UINT32 hits  = (fired > 0 ? 100 * p->usShotsHit / fired : 0);
	swprintf(sString, lengthof(sString), L"%d %%", hits);
	FindFontRightCoordinates(pers_stat_x, 0, TEXT_BOX_WIDTH - 20, 0, sString, PERS_FONT, &sX, &sY);
	mprintf(sX, pers_stat_y[23], L"%ls", sString);

	PrintStat(p->usBattlesFought, pers_stat_y[24], pPersonnelScreenStrings[PRSNL_TXT_BATTLES]);
	PrintStat(p->usTimesWounded,  pers_stat_y[25], pPersonnelScreenStrings[PRSNL_TXT_TIMES_WOUNDED]);

	//Display the 'Skills' text
	mprintf(pers_stat_x, pers_stat_y[19], pPersonnelScreenStrings[PRSNL_TXT_SKILLS]);

	/* KM: April 16, 1999
	 * Added support for the German version, which has potential string
	 * overrun problems.  For example, the text "Skills:" can overlap
	 * "NightOps (Expert)" because the German strings are much longer.  In
	 * these cases, I ensure that the right justification of the traits
	 * do not overlap.  If it would, I move it over to the right. */
	const INT32 iWidth = StringPixLength(pPersonnelScreenStrings[PRSNL_TXT_SKILLS], PERS_FONT);
	const INT32 iMinimumX = iWidth + pers_stat_x + 2;

	if (!fAmIaRobot)
	{
		INT8 bSkill1 = p->bSkillTrait;
		INT8 bSkill2 = p->bSkillTrait2;

		if (bSkill1 == NO_SKILLTRAIT)
		{
			bSkill1 = bSkill2;
			bSkill2 = NO_SKILLTRAIT;
		}

		if (bSkill1 != NO_SKILLTRAIT)
		{
			if (bSkill1 == bSkill2)
			{
				// The 2 skills are the same, add the '(expert)' at the end
				swprintf(sString, lengthof(sString), L"%ls %ls", gzMercSkillText[bSkill1], gzMercSkillText[NUM_SKILLTRAITS]);
				FindFontRightCoordinates(pers_stat_x, 0, TEXT_BOX_WIDTH - 20, 0, sString, PERS_FONT, &sX, &sY);

				//KM: April 16, 1999
				//Perform the potential overrun check
				if (sX <= iMinimumX)
				{
					FindFontRightCoordinates(pers_stat_x + TEXT_BOX_WIDTH - 20 + TEXT_DELTA_OFFSET, 0, 30, 0, sString, PERS_FONT, &sX, &sY);
					sX = max(sX, iMinimumX);
				}

				mprintf(sX, pers_stat_y[19], sString);
			}
			else
			{
				const wchar_t* Skill = gzMercSkillText[bSkill1];

				FindFontRightCoordinates(pers_stat_x, 0, TEXT_BOX_WIDTH - 20, 0, Skill, PERS_FONT, &sX, &sY);

				//KM: April 16, 1999
				//Perform the potential overrun check
				sX = max(sX, iMinimumX);

				mprintf(sX, pers_stat_y[19], Skill);

				if (bSkill2 != NO_SKILLTRAIT)
				{
					const wchar_t* Skill = gzMercSkillText[bSkill2];

					FindFontRightCoordinates(pers_stat_x, 0, TEXT_BOX_WIDTH - 20, 0, Skill, PERS_FONT, &sX, &sY);

					//KM: April 16, 1999
					//Perform the potential overrun check
					sX = max(sX, iMinimumX);

					mprintf(sX, pers_stat_y[20], Skill);
				}
			}
		}
		else
		{
			const wchar_t* NoSkill = pPersonnelScreenStrings[PRSNL_TXT_NOSKILLS];
			FindFontRightCoordinates(pers_stat_x, 0, TEXT_BOX_WIDTH - 20, 0, NoSkill, PERS_FONT, &sX, &sY);
			mprintf(sX, pers_stat_y[19], NoSkill);
		}
	}
}


static void SetPersonnelButtonStates(void)
{
	// this function will look at what page we are viewing, enable and disable buttons as needed
	if (!PrevPersonnelFace())
	{
		// first page, disable left buttons
		DisableButton(giPersonnelButton[0]);
	}
	else
	{
		// enable buttons
		NextPersonnelFace();
		EnableButton(giPersonnelButton[0]);
	}

	if (!NextPersonnelFace())
	{
		DisableButton(giPersonnelButton[1]);
	}
	else
	{
		// decrement page
		PrevPersonnelFace();
		EnableButton(giPersonnelButton[1]);
	}
}


static void RenderPersonnelScreenBackground(void)
{
	// this fucntion will render the background for the personnel screen
	BltVideoObjectFromIndex(FRAME_BUFFER, fCurrentTeamMode ? guiCURRENTTEAM : guiDEPARTEDTEAM, 0, DEPARTED_X, DEPARTED_Y);
}


static BOOLEAN LoadPersonnelScreenBackgroundGraphics(void)
{
	// will load the graphics for the personeel screen background

	// departed bar
	guiDEPARTEDTEAM = AddVideoObjectFromFile("LAPTOP/departed.sti");
	CHECKF(guiDEPARTEDTEAM != NO_VOBJECT);

	// current bar
	guiCURRENTTEAM = AddVideoObjectFromFile("LAPTOP/CurrentTeam.sti");
	CHECKF(guiCURRENTTEAM != NO_VOBJECT);

	return TRUE;
}


static void DeletePersonnelScreenBackgroundGraphics(void)
{
	// delete background V/O's
	DeleteVideoObjectFromIndex(guiCURRENTTEAM);
	DeleteVideoObjectFromIndex(guiDEPARTEDTEAM);
}


static INT32 GetNumberOfMercsDeadOrAliveOnPlayersTeam(void)
{
	INT32 iCounter = 0;

	// grab number on team
	CFOR_ALL_IN_TEAM(i, OUR_TEAM)
	{
		if (!(i->uiStatusFlags & SOLDIER_VEHICLE)) iCounter++;
	}

	return iCounter;
}


static void PersonnelPortraitCallback(MOUSE_REGION* pRegion, INT32 iReason);


static void CreateDestroyMouseRegionsForPersonnelPortraits(BOOLEAN create)
{
	// creates/destroys mouse regions for portraits
	static BOOLEAN fCreated = FALSE;

	if (!fCreated && create)
	{
		// create regions
		for (INT16 i = 0; i < PERSONNEL_PORTRAIT_NUMBER; i++)
		{
			const UINT16 tlx = SMALL_PORTRAIT_START_X + i % PERSONNEL_PORTRAIT_NUMBER_WIDTH * SMALL_PORT_WIDTH;
			const UINT16 tly = SMALL_PORTRAIT_START_Y + i / PERSONNEL_PORTRAIT_NUMBER_WIDTH * SMALL_PORT_HEIGHT;
			const UINT16 brx = tlx + SMALL_PORTRAIT_WIDTH;
			const UINT16 bry = tly + SMALL_PORTRAIT_HEIGHT;
			MSYS_DefineRegion(&gPortraitMouseRegions[i], tlx, tly, brx, bry, MSYS_PRIORITY_HIGHEST, CURSOR_LAPTOP_SCREEN, MSYS_NO_CALLBACK, PersonnelPortraitCallback);
			MSYS_SetRegionUserData(&gPortraitMouseRegions[i], 0, i);
		}

		fCreated = TRUE;
	}
	else if (fCreated && !create)
	{
		// destroy regions
		for (INT16 i = 0; i < PERSONNEL_PORTRAIT_NUMBER; i++)
		{
			MSYS_RemoveRegion(&gPortraitMouseRegions[i]);
		}

		fCreated = FALSE;
	}
}


static BOOLEAN DisplayPicturesOfCurrentTeam(void)
{
	// will display the small portraits of the current team
	if (!fCurrentTeamMode) return TRUE;

	INT32 i = 0;
	CFOR_ALL_IN_TEAM(s, OUR_TEAM)
	{
		if (s->uiStatusFlags & SOLDIER_VEHICLE) continue;

		// found the next actual guy
		char sTemp[100];
		sprintf(sTemp, SMALL_FACES_DIR "%02d.sti", gMercProfiles[s->ubProfile].ubFaceIndex);

		UINT32 guiFACE = AddVideoObjectFromFile(sTemp);
		CHECKF(guiFACE != NO_VOBJECT);

		HVOBJECT hFaceHandle = GetVideoObject(guiFACE);

		if (s->bLife <= 0)
		{
			hFaceHandle->pShades[0] = Create16BPPPaletteShaded(hFaceHandle->pPaletteEntry, DEAD_MERC_COLOR_RED, DEAD_MERC_COLOR_GREEN, DEAD_MERC_COLOR_BLUE, TRUE);
			//set the red pallete to the face
			SetObjectHandleShade(guiFACE, 0);
		}

		const INT32 x = SMALL_PORTRAIT_START_X + i % PERSONNEL_PORTRAIT_NUMBER_WIDTH * SMALL_PORT_WIDTH;
		const INT32 y = SMALL_PORTRAIT_START_Y + i / PERSONNEL_PORTRAIT_NUMBER_WIDTH * SMALL_PORT_HEIGHT;
		BltVideoObject(FRAME_BUFFER, hFaceHandle, 0, x, y);

		if (s->bLife <= 0)
		{
			//if the merc is dead, display it
			DrawTextToScreen(AimPopUpText[AIM_MEMBER_DEAD], x, y + SMALL_PORT_HEIGHT / 2, SMALL_PORTRAIT_WIDTH_NO_BORDERS, FONT10ARIAL, 145, FONT_MCOLOR_BLACK, CENTER_JUSTIFIED);
		}

		DeleteVideoObjectFromIndex(guiFACE);
		i++;
	}

	return TRUE;
}


static const SOLDIERTYPE* GetSoldierOfCurrentSlot(void);


static void PersonnelPortraitCallback(MOUSE_REGION* pRegion, INT32 iReason)
{
	INT32 iPortraitId = 0;
	INT32 iOldPortraitId;

	iPortraitId = MSYS_GetRegionUserData(pRegion, 0);
	iOldPortraitId = iCurrentPersonSelectedId;

	// callback handler for the minize region that is attatched to the laptop program icon
	if (iReason & MSYS_CALLBACK_REASON_LBUTTON_UP)
	{
		// get id of portrait
		if (fCurrentTeamMode)
		{
			// valid portrait, set up id
			if (iPortraitId >= GetNumberOfMercsDeadOrAliveOnPlayersTeam())
			{
				// not a valid id, leave
				return;
			}

			iCurrentPersonSelectedId = iPortraitId;
			fReDrawScreenFlag = TRUE;

			if (iCurrentPersonSelectedId != -1 &&
					GetSoldierOfCurrentSlot()->bAssignment == ASSIGNMENT_POW &&
					gubPersonnelInfoState == PERSONNEL_INV_BTN)
			{
				gubPersonnelInfoState = PERSONNEL_STAT_BTN;
			}
		}
		else
		{
			if (iPortraitId >= GetNumberOfPastMercsOnPlayersTeam())
			{
				return;
			}
			iCurrentPersonSelectedId = iPortraitId;
			fReDrawScreenFlag = TRUE;
			iCurPortraitId = iPortraitId;
		}

		if (iOldPortraitId != iPortraitId)
		{
			uiCurrentInventoryIndex = 0;
			guiSliderPosition = 0;
		}
	}

	if (iReason & MSYS_CALLBACK_REASON_RBUTTON_UP)
	{
		if (fCurrentTeamMode)
		{
			// valid portrait, set up id
			if (iPortraitId >= GetNumberOfMercsDeadOrAliveOnPlayersTeam())
			{
				// not a valid id, leave
				return;
			}

			//if the user is rigt clicking on the same face
			if (iCurrentPersonSelectedId == iPortraitId)
			{
				//increment the info page when the user right clicks
				if (gubPersonnelInfoState < PERSONNEL_NUM_BTN - 1)
					gubPersonnelInfoState++;
				else
					gubPersonnelInfoState = PERSONNEL_STAT_BTN;
			}

			iCurrentPersonSelectedId = iPortraitId;
			fReDrawScreenFlag = TRUE;

			uiCurrentInventoryIndex = 0;
			guiSliderPosition = 0;

			//if the selected merc is valid, and they are a POW, change to the inventory display
			if (iCurrentPersonSelectedId != -1 &&
					GetSoldierOfCurrentSlot()->bAssignment == ASSIGNMENT_POW &&
					gubPersonnelInfoState == PERSONNEL_INV_BTN)
			{
				gubPersonnelInfoState = PERSONNEL_STAT_BTN;
			}
		}
	}
}


typedef struct PastMercInfo
{
	ProfileID id;
	INT8      state;
} PastMercInfo;


static void DisplayDepartedCharName(INT32 iId, INT32 iState);
static void DisplayDepartedCharStats(INT32 iId, INT32 iState);
static BOOLEAN DisplayHighLightBox(void);
static PastMercInfo GetSelectedPastMercInfo(void);


static void DisplayFaceOfDisplayedMerc(void)
{
	// valid person?, display
	if (iCurrentPersonSelectedId != -1)
	{
		// highlight it
		DisplayHighLightBox();

		// if showing inventory, leave
		if (fCurrentTeamMode)
		{
			const SOLDIERTYPE* const s = GetSoldierOfCurrentSlot();
			RenderPersonnelFaceCurrent(s);
			DisplayCharName(s);

			if (gubPersonnelInfoState == PRSNL_INV) return;

			RenderPersonnelStats(s);
		}
		else
		{
			const PastMercInfo info = GetSelectedPastMercInfo();
			RenderPersonnelFacePast(info.id, info.state);
			DisplayDepartedCharName(info.id, info.state);
			DisplayDepartedCharStats(info.id, info.state);
		}
	}
}


static void RenderSliderBarForPersonnelInventory(void);


static void DisplayInventoryForSelectedChar(void)
{
	// display the inventory for this merc
	if (gubPersonnelInfoState != PRSNL_INV)
	{
		return;
	}

	CreateDestroyPersonnelInventoryScrollButtons();

	UINT8 ubItemCount = 0;
	UINT8 ubUpToCount = 0;
	INT16 sX, sY;
	CHAR16 sString[128];

	BltVideoObjectFromIndex(FRAME_BUFFER, guiPersonnelInventory, 0, 397, 200);

	if (!fCurrentTeamMode) return;

	// render the bar for the character
	RenderSliderBarForPersonnelInventory();

	const SOLDIERTYPE* const pSoldier = GetSoldierOfCurrentSlot();

	//if this is a robot, dont display any inventory
	if (AM_A_ROBOT(pSoldier)) return;

	for (UINT ubCounter = 0; ubCounter < NUM_INV_SLOTS; ubCounter++)
	{
		INT16 PosX = 397 + 3;
		INT16 PosY = 200 + 8 + ubItemCount * 29;

		//if the character is a robot, only display the inv for the hand pos
		if (pSoldier->ubProfile == ROBOT && ubCounter != HANDPOS) // XXX can this ever be true? before is if (AM_A_ROBOT()) return;
		{
			continue;
		}

		if (pSoldier->inv[ubCounter].ubNumberOfObjects > 0)
		{
			if (uiCurrentInventoryIndex > ubUpToCount)
			{
				ubUpToCount++;
			}
			else
			{
				INT16 sIndex = pSoldier->inv[ubCounter].usItem;
				const INVTYPE* pItem = &Item[sIndex];
				UINT32 ItemVOIdx = GetInterfaceGraphicForItem(pItem);

				const ETRLEObject* pTrav = GetVideoObjectETRLESubregionProperties(ItemVOIdx, pItem->ubGraphicNum);
				INT16 sCenX = PosX + abs(57 - pTrav->usWidth)  / 2 - pTrav->sOffsetX;
				INT16 sCenY = PosY + abs(22 - pTrav->usHeight) / 2 - pTrav->sOffsetY;

				BltVideoObjectOutlineFromIndex(FRAME_BUFFER, ItemVOIdx, pItem->ubGraphicNum, sCenX, sCenY, 0, FALSE);

				SetFont(FONT10ARIAL);
				SetFontForeground(FONT_WHITE);
				SetFontBackground(FONT_BLACK);
				SetFontDestBuffer(FRAME_BUFFER, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

				wcslcpy(sString, ItemNames[sIndex], lengthof(sString));
				ReduceStringLength(sString, lengthof(sString), 171 - 75, FONT10ARIAL);
				mprintf(PosX + 65, PosY + 3, sString);

				// condition
				if (Item[pSoldier->inv[ubCounter].usItem].usItemClass & IC_AMMO)
				{
					INT32 iTotalAmmo = 0;
					for (INT32 cnt = 0; cnt < pSoldier->inv[ubCounter].ubNumberOfObjects; cnt++)
					{
						iTotalAmmo += pSoldier->inv[ubCounter].ubShotsLeft[cnt];
					}

					swprintf(sString, lengthof(sString), L"%d/%d", iTotalAmmo, pSoldier->inv[ubCounter].ubNumberOfObjects * Magazine[Item[pSoldier->inv[ubCounter].usItem].ubClassIndex].ubMagSize);
				}
				else
				{
					swprintf(sString, lengthof(sString), L"%2d%%", pSoldier->inv[ubCounter].bStatus[0]);
				}

				FindFontRightCoordinates(PosX + 65, PosY + 15, 171 - 75, GetFontHeight(FONT10ARIAL), sString, FONT10ARIAL, &sX, &sY);
				mprintf(sX, sY, L"%ls", sString);

				if (Item[pSoldier->inv[ubCounter].usItem].usItemClass & IC_GUN)
				{
					wcslcpy(sString, AmmoCaliber[Weapon[Item[pSoldier->inv[ubCounter].usItem].ubClassIndex].ubCalibre], lengthof(sString));
					ReduceStringLength(sString, lengthof(sString), 171 - 75, FONT10ARIAL);
					mprintf(PosX + 65, PosY + 15, sString);
				}

				// if more than 1?
				if (pSoldier->inv[ubCounter].ubNumberOfObjects > 1)
				{
					swprintf(sString, lengthof(sString), L"x%d",  pSoldier->inv[ubCounter].ubNumberOfObjects);
					FindFontRightCoordinates(PosX, PosY + 15, 58, GetFontHeight(FONT10ARIAL), sString, FONT10ARIAL, &sX, &sY);
					mprintf(sX, sY, sString);
				}

				ubItemCount++;
			}
		}

		if (ubItemCount == NUMBER_OF_INVENTORY_PERSONNEL) break;
	}
}


static void FindPositionOfPersInvSlider(void);


static void InventoryUp(void)
{
	if (uiCurrentInventoryIndex == 0) return;
	uiCurrentInventoryIndex--;
	fReDrawScreenFlag = TRUE;
	FindPositionOfPersInvSlider();
}


static INT32 GetNumberOfInventoryItemsOnCurrentMerc(void);


static void InventoryDown(void)
{
	if ((INT32)uiCurrentInventoryIndex >= (INT32)(GetNumberOfInventoryItemsOnCurrentMerc() - NUMBER_OF_INVENTORY_PERSONNEL))
	{
		return;
	}
	uiCurrentInventoryIndex++;
	fReDrawScreenFlag = TRUE;
	FindPositionOfPersInvSlider();
}


static void InventoryUpButtonCallback(GUI_BUTTON *btn, INT32 reason)
{
	if (reason & MSYS_CALLBACK_REASON_LBUTTON_REPEAT ||
			reason & MSYS_CALLBACK_REASON_LBUTTON_UP)
	{
		InventoryUp();
	}
}


static void InventoryDownButtonCallback(GUI_BUTTON *btn, INT32 reason)
{
	if (reason & MSYS_CALLBACK_REASON_LBUTTON_REPEAT ||
			reason & MSYS_CALLBACK_REASON_LBUTTON_UP)
	{
		InventoryDown();
	}
}


// decide which buttons can and can't be accessed based on what the current item is
static void EnableDisableInventoryScrollButtons(void)
{
	if (gubPersonnelInfoState != PRSNL_INV)
	{
		return;
	}

	if (uiCurrentInventoryIndex == 0)
	{
		ButtonList[giPersonnelInventoryButtons[0]]->uiFlags &= ~(BUTTON_CLICKED_ON);
		DisableButton(giPersonnelInventoryButtons[0]);
	}
	else
	{
		EnableButton(giPersonnelInventoryButtons[0]);
	}


	if ((INT32)uiCurrentInventoryIndex >= (INT32)(GetNumberOfInventoryItemsOnCurrentMerc() - NUMBER_OF_INVENTORY_PERSONNEL))
	{
		ButtonList[giPersonnelInventoryButtons[1]]->uiFlags &= ~(BUTTON_CLICKED_ON);
		DisableButton(giPersonnelInventoryButtons[1]);
	}
	else
	{
		EnableButton(giPersonnelInventoryButtons[1]);
	}
}


static INT32 GetNumberOfInventoryItemsOnCurrentMerc(void)
{
	// in current team mode?..nope...move on
	if (!fCurrentTeamMode) return 0;

	const OBJECTTYPE* const Inv = GetSoldierOfCurrentSlot()->inv;

	UINT32 ubCount = 0;
	for (UINT32 ubCounter = 0; ubCounter < NUM_INV_SLOTS; ubCounter++)
	{
		const OBJECTTYPE* o = &Inv[ubCounter];
		if (o->ubNumberOfObjects != 0 && o->usItem != NOTHING) ubCount++;
	}

	return ubCount;
}


static void HandleInventoryCallBack(MOUSE_REGION* pRegion, INT32 iReason)
{
	if (iReason & MSYS_CALLBACK_REASON_WHEEL_UP)
	{
		InventoryUp();
	}
	else if (iReason & MSYS_CALLBACK_REASON_WHEEL_DOWN)
	{
		InventoryDown();
	}
}


static MOUSE_REGION InventoryRegion;


static void HandleSliderBarClickCallback(MOUSE_REGION* pRegion, INT32 iReason);


static void CreateDestroyPersonnelInventoryScrollButtons(void)
{
	static BOOLEAN fCreated = FALSE;

	if (gubPersonnelInfoState == PRSNL_INV && !fCreated)
	{
		// create buttons
		giPersonnelInventoryButtons[0] = QuickCreateButtonImg("LAPTOP/personnel_inventory.sti", -1, 1, -1, 2, -1, 176 + 397, 2 +   200, MSYS_PRIORITY_HIGHEST - 1, InventoryUpButtonCallback);
		giPersonnelInventoryButtons[1] = QuickCreateButtonImg("LAPTOP/personnel_inventory.sti", -1, 3, -1, 4, -1, 176 + 397, 200 + 223, MSYS_PRIORITY_HIGHEST - 1, InventoryDownButtonCallback);

		// set up cursors for these buttons
		SetButtonCursor(giPersonnelInventoryButtons[0], CURSOR_LAPTOP_SCREEN);
		SetButtonCursor(giPersonnelInventoryButtons[1], CURSOR_LAPTOP_SCREEN);

		MSYS_DefineRegion(&gMouseScrollPersonnelINV, X_OF_PERSONNEL_SCROLL_REGION, Y_OF_PERSONNEL_SCROLL_REGION, X_OF_PERSONNEL_SCROLL_REGION + X_SIZE_OF_PERSONNEL_SCROLL_REGION, Y_OF_PERSONNEL_SCROLL_REGION + Y_SIZE_OF_PERSONNEL_SCROLL_REGION, MSYS_PRIORITY_HIGHEST - 3, CURSOR_LAPTOP_SCREEN, MSYS_NO_CALLBACK, HandleSliderBarClickCallback);

		const UINT16 x = INVENTORY_BOX_X;
		const UINT16 y = INVENTORY_BOX_Y;
		const UINT16 w = INVENTORY_BOX_W;
		const UINT16 h = INVENTORY_BOX_H;
		MSYS_DefineRegion(&InventoryRegion, x, y, x + w, y + h, MSYS_PRIORITY_HIGHEST - 3, CURSOR_LAPTOP_SCREEN, MSYS_NO_CALLBACK, HandleInventoryCallBack);

		fCreated = TRUE;
	}
	else if (fCreated && gubPersonnelInfoState != PERSONNEL_INV_BTN)
	{
		// destroy buttons
		RemoveButton(giPersonnelInventoryButtons[0]);
		RemoveButton(giPersonnelInventoryButtons[1]);

		MSYS_RemoveRegion(&gMouseScrollPersonnelINV);
		MSYS_RemoveRegion(&InventoryRegion);

		fCreated = FALSE;
	}
}


static void DisplayCostOfCurrentTeam(void);


static void DisplayNumberOnCurrentTeam(void)
{
	// display number on team
	SetFont(FONT10ARIAL);
	SetFontBackground(FONT_BLACK);
	SetFontForeground(PERS_TEXT_FONT_COLOR);

	if (fCurrentTeamMode)
	{
		mprintf(PERS_CURR_TEAM_X, PERS_CURR_TEAM_Y, L"%ls ( %d )", pPersonelTeamStrings[0], GetNumberOfMercsDeadOrAliveOnPlayersTeam());
	}
	else
	{
		const wchar_t* s = pPersonelTeamStrings[0];
		INT16 sX = 0;
		INT16 sY = 0;
		FindFontCenterCoordinates(PERS_CURR_TEAM_X, 0, 65, 0, s, FONT10ARIAL, &sX, &sY);
		mprintf(sX, PERS_CURR_TEAM_Y, s);
	}

	// now the cost of the current team, if applicable
	DisplayCostOfCurrentTeam();
}


static void DisplayNumberDeparted(void)
{
	// display number departed from team
	SetFont(FONT10ARIAL);
	SetFontBackground(FONT_BLACK);
	SetFontForeground(PERS_TEXT_FONT_COLOR);

	if (!fCurrentTeamMode)
	{
		mprintf(PERS_CURR_TEAM_X, PERS_DEPART_TEAM_Y, L"%ls ( %d )", pPersonelTeamStrings[1], GetNumberOfPastMercsOnPlayersTeam());
	}
	else
	{
		const wchar_t* s = pPersonelTeamStrings[1];
		INT16 sX = 0;
		INT16 sY = 0;
		FindFontCenterCoordinates(PERS_CURR_TEAM_X, 0, 65, 0, s, FONT10ARIAL, &sX, &sY);
		mprintf(sX, PERS_DEPART_TEAM_Y, s);
	}
}


static void DisplayCostOfCurrentTeam(void)
{
	if (!fCurrentTeamMode) return;

	SetFont(FONT10ARIAL);
	SetFontBackground(FONT_BLACK);
	SetFontForeground(PERS_TEXT_FONT_COLOR);

	INT32 min_cost = 999999;
	INT32 max_cost = 0;
	INT32 sum_cost = 0;

	CFOR_ALL_IN_TEAM(s, OUR_TEAM)
	{
		if (s->bLife <= 0) continue;

		// valid soldier, get cost
		INT32 cost;
		const MERCPROFILESTRUCT* const p = &gMercProfiles[s->ubProfile];
		switch (s->ubWhatKindOfMercAmI)
		{
			case MERC_TYPE__AIM_MERC:
				switch (s->bTypeOfLastContract)
				{
					case CONTRACT_EXTEND_2_WEEK: cost = p->uiBiWeeklySalary / 14; break;
					case CONTRACT_EXTEND_1_WEEK: cost = p->uiWeeklySalary   /  7; break;
					default:                     cost = p->sSalary;               break;
				}
				break;

			default: cost = p->sSalary; break;
		}

		if (cost > max_cost) max_cost = cost;
		if (cost < min_cost) min_cost = cost;
		sum_cost += cost;
	}

	if (min_cost == 999999) min_cost = 0;

	wchar_t sString[32];
	INT16 sX;
	INT16 sY;

	// daily cost
	mprintf(PERS_CURR_TEAM_COST_X, PERS_CURR_TEAM_COST_Y, pPersonelTeamStrings[2]);
	SPrintMoney(sString, sum_cost);
	FindFontRightCoordinates(PERS_CURR_TEAM_COST_X, 0, PERS_CURR_TEAM_WIDTH, 0, sString, PERS_FONT, &sX, &sY);
	mprintf(sX, PERS_CURR_TEAM_COST_Y, sString);

	// highest cost
	mprintf(PERS_CURR_TEAM_COST_X, PERS_CURR_TEAM_HIGHEST_Y, pPersonelTeamStrings[3]);
	SPrintMoney(sString, max_cost);
	FindFontRightCoordinates(PERS_CURR_TEAM_COST_X, 0, PERS_CURR_TEAM_WIDTH, 0, sString, PERS_FONT, &sX, &sY);
	mprintf(sX, PERS_CURR_TEAM_HIGHEST_Y, sString);

	// the lowest cost
	mprintf(PERS_CURR_TEAM_COST_X, PERS_CURR_TEAM_LOWEST_Y, pPersonelTeamStrings[4]);
	SPrintMoney(sString, min_cost);
	FindFontRightCoordinates(PERS_CURR_TEAM_COST_X, 0, PERS_CURR_TEAM_WIDTH, 0, sString, PERS_FONT, &sX, &sY);
	mprintf(sX, PERS_CURR_TEAM_LOWEST_Y, sString);
}


static void DisplayTeamStats(void)
{
	INT16 sX;
	INT16 sY;

	SetFont(FONT10ARIAL);
	SetFontBackground(FONT_BLACK);
	SetFontForeground(PERS_TEXT_FONT_COLOR);

	// display headers
	// lowest
	FindFontCenterCoordinates(PERS_STAT_LOWEST_X,  0, PERS_STAT_LOWEST_WIDTH, 0, pPersonnelCurrentTeamStatsStrings[0], FONT10ARIAL, &sX, &sY);
	mprintf(sX, PERS_STAT_AVG_Y, pPersonnelCurrentTeamStatsStrings[0]);
	// average
	FindFontCenterCoordinates(PERS_STAT_AVG_X,     0, PERS_STAT_AVG_WIDTH,    0, pPersonnelCurrentTeamStatsStrings[1], FONT10ARIAL, &sX, &sY);
	mprintf(sX, PERS_STAT_AVG_Y, pPersonnelCurrentTeamStatsStrings[1]);
	// highest
	FindFontCenterCoordinates(PERS_STAT_HIGHEST_X, 0, PERS_STAT_LOWEST_WIDTH, 0, pPersonnelCurrentTeamStatsStrings[2], FONT10ARIAL, &sX, &sY);
	mprintf(sX, PERS_STAT_AVG_Y, pPersonnelCurrentTeamStatsStrings[2]);

	for (INT32 stat = 0; stat < 11; stat++)
	{
		// even or odd?..color black or yellow?
		SetFontForeground(stat % 2 == 0 ? PERS_TEXT_FONT_ALTERNATE_COLOR : PERS_TEXT_FONT_COLOR);

		const INT32 y = PERS_STAT_AVG_Y + (stat + 1) * (GetFontHeight(FONT10ARIAL) + 3);

		// row header
		mprintf(PERS_STAT_LIST_X, y, pPersonnelTeamStatsStrings[stat]);

		const wchar_t* min_name = NULL;
		const wchar_t* max_name = NULL;
		INT32 min_val           = 100 + 1;
		INT32 max_val           = 0;
		INT32 sum_val           = 0;
		INT32 count             = 0;
		if (fCurrentTeamMode)
		{
			CFOR_ALL_IN_TEAM(s, OUR_TEAM)
			{
				if (s->uiStatusFlags & SOLDIER_VEHICLE || s->bLife <= 0 || AM_A_ROBOT(s)) continue;

				INT32 val;
				switch (stat)
				{
					case  0: val = s->bLifeMax;      break;
					case  1: val = s->bAgility;      break;
					case  2: val = s->bDexterity;    break;
					case  3: val = s->bStrength;     break;
					case  4: val = s->bLeadership;   break;
					case  5: val = s->bWisdom;       break;
					case  6: val = s->bExpLevel;     break;
					case  7: val = s->bMarksmanship; break;
					case  8: val = s->bMechanical;   break;
					case  9: val = s->bExplosive;    break;
					case 10: val = s->bMedical;      break;
				}
				if (min_val > val)
				{
					min_name = s->name;
					min_val  = val;
				}
				if (max_val < val)
				{
					max_name = s->name;
					max_val = val;
				}
				sum_val += val;
				++count;
			}
		}
		else
		{
			for (UINT CurrentList = 0; CurrentList < 3; ++CurrentList)
			{
				const INT16* CurrentListValue;
				switch (CurrentList)
				{
					case 0: CurrentListValue = LaptopSaveInfo.ubDeadCharactersList;  break;
					case 1: CurrentListValue = LaptopSaveInfo.ubLeftCharactersList;  break;
					case 2: CurrentListValue = LaptopSaveInfo.ubOtherCharactersList; break;
				}

				for (UINT32 i = 0; i < 256; i++)
				{
					const INT32 id = CurrentListValue[i];
					if (id == -1) continue;

					INT32 val;
					const MERCPROFILESTRUCT* const p = &gMercProfiles[id];
					switch (stat)
					{
						case  0: val = p->bLifeMax;      break;
						case  1: val = p->bAgility;      break;
						case  2: val = p->bDexterity;    break;
						case  3: val = p->bStrength;     break;
						case  4: val = p->bLeadership;   break;
						case  5: val = p->bWisdom;       break;
						case  6: val = p->bExpLevel;     break;
						case  7: val = p->bMarksmanship; break;
						case  8: val = p->bMechanical;   break;
						case  9: val = p->bExplosive;    break;
						case 10: val = p->bMedical;      break;
					}
					if (min_val > val)
					{
						min_name = p->zNickname;
						min_val  = val;
					}
					if (max_val < val)
					{
						max_name = p->zNickname;
						max_val = val;
					}
					sum_val += val;
					++count;
				}
			}
		}

		if (count == 0) continue;

		mprintf(PERS_STAT_LOWEST_X,  y, L"%ls", min_name);
		mprintf(PERS_STAT_HIGHEST_X, y, L"%ls", max_name);

		wchar_t val_str[32];

		swprintf(val_str, lengthof(val_str), L"%d", min_val);
		FindFontRightCoordinates(PERS_STAT_LOWEST_X, 0, PERS_STAT_LOWEST_WIDTH, 0, val_str, FONT10ARIAL, &sX, &sY);
		mprintf(sX, y, val_str);

		swprintf(val_str, lengthof(val_str), L"%d", sum_val / count);
		FindFontCenterCoordinates(PERS_STAT_AVG_X, 0, PERS_STAT_AVG_WIDTH, 0, val_str, FONT10ARIAL, &sX, &sY);
		mprintf(sX, y, val_str);

		swprintf(val_str, lengthof(val_str), L"%d", max_val);
		FindFontRightCoordinates(PERS_STAT_HIGHEST_X, 0, PERS_STAT_LOWEST_WIDTH, 0, val_str, FONT10ARIAL, &sX, &sY);
		mprintf(sX, y, val_str);
	}
}


static INT32 GetNumberOfDeadOnPastTeam(void);
static INT32 GetNumberOfLeftOnPastTeam(void);
static INT32 GetNumberOfOtherOnPastTeam(void);


static INT32 GetNumberOfPastMercsOnPlayersTeam(void)
{
	INT32 iPastNumberOfMercs = 0;
	// will run through the list of past mercs on the players team and return their number

	iPastNumberOfMercs += GetNumberOfDeadOnPastTeam();
	iPastNumberOfMercs += GetNumberOfLeftOnPastTeam();
	iPastNumberOfMercs += GetNumberOfOtherOnPastTeam();

	return iPastNumberOfMercs;
}


static void InitPastCharactersList(void)
{
	// inits the past characters list
	memset(&LaptopSaveInfo.ubDeadCharactersList,  -1, sizeof(LaptopSaveInfo.ubDeadCharactersList));
	memset(&LaptopSaveInfo.ubLeftCharactersList,  -1, sizeof(LaptopSaveInfo.ubLeftCharactersList));
	memset(&LaptopSaveInfo.ubOtherCharactersList, -1, sizeof(LaptopSaveInfo.ubOtherCharactersList));
}


static INT32 CountList(const INT16* const list)
{
	INT32 count = 0;
	for (const INT16* i = list, * const end = list + 256; i != end; ++i)
	{
		if (*i != -1) ++count;
	}
	return count;
}


static INT32 GetNumberOfDeadOnPastTeam(void)
{
	return CountList(LaptopSaveInfo.ubDeadCharactersList);
}


static INT32 GetNumberOfLeftOnPastTeam(void)
{
	return CountList(LaptopSaveInfo.ubLeftCharactersList);
}


static INT32 GetNumberOfOtherOnPastTeam(void)
{
	return CountList(LaptopSaveInfo.ubOtherCharactersList);
}


static void DisplayStateOfPastTeamMembers(void)
{
	INT16 sX, sY;
	CHAR16 sString[32];

	SetFont(FONT10ARIAL);
	SetFontBackground(FONT_BLACK);
	SetFontForeground(PERS_TEXT_FONT_COLOR);

	// diplsya numbers fired, dead and othered
	if (!fCurrentTeamMode)
	{
		// dead
		mprintf(PERS_CURR_TEAM_COST_X, PERS_CURR_TEAM_COST_Y, pPersonelTeamStrings[5]);
		swprintf(sString, lengthof(sString), L"%d", GetNumberOfDeadOnPastTeam());
		FindFontRightCoordinates(PERS_CURR_TEAM_COST_X, 0, PERS_DEPART_TEAM_WIDTH, 0, sString, PERS_FONT, &sX, &sY);
		mprintf(sX, PERS_CURR_TEAM_COST_Y, sString);

		// fired
		mprintf(PERS_CURR_TEAM_COST_X, PERS_CURR_TEAM_HIGHEST_Y, pPersonelTeamStrings[6]);
		swprintf(sString, lengthof(sString), L"%d", GetNumberOfLeftOnPastTeam());
		FindFontRightCoordinates(PERS_CURR_TEAM_COST_X, 0, PERS_DEPART_TEAM_WIDTH, 0, sString, PERS_FONT, &sX, &sY);
		mprintf(sX, PERS_CURR_TEAM_HIGHEST_Y, sString);

		// other
		mprintf(PERS_CURR_TEAM_COST_X, PERS_CURR_TEAM_LOWEST_Y, pPersonelTeamStrings[7]);
		swprintf(sString, lengthof(sString), L"%d", GetNumberOfOtherOnPastTeam());
		FindFontRightCoordinates(PERS_CURR_TEAM_COST_X, 0, PERS_DEPART_TEAM_WIDTH, 0, sString, PERS_FONT, &sX, &sY);
		mprintf(sX, PERS_CURR_TEAM_LOWEST_Y, sString);
	}
}


static void PersonnelCurrentTeamCallback(MOUSE_REGION* pRegion, INT32 iReason);
static void PersonnelDepartedTeamCallback(MOUSE_REGION* pRegion, INT32 iReason);


static void CreateDestroyCurrentDepartedMouseRegions(BOOLEAN create)
{
	static BOOLEAN fCreated = FALSE;

	// will arbitrate the creation/deletion of mouse regions for current/past team toggles

	if (create && !fCreated)
	{
		// not created, create
		UINT16 tlx = PERS_TOGGLE_CUR_DEPART_X;
		UINT16 tly = PERS_TOGGLE_CUR_Y;
		UINT16 brx = tlx + PERS_TOGGLE_CUR_DEPART_WIDTH;
		UINT16 bry = tly + PERS_TOGGLE_CUR_DEPART_HEIGHT;
		MSYS_DefineRegion(&gTogglePastCurrentTeam[0], tlx, tly, brx, bry, MSYS_PRIORITY_HIGHEST - 3, CURSOR_LAPTOP_SCREEN, MSYS_NO_CALLBACK, PersonnelCurrentTeamCallback);

		tly = PERS_TOGGLE_DEPART_Y;
		bry = tly + PERS_TOGGLE_CUR_DEPART_HEIGHT;
		MSYS_DefineRegion(&gTogglePastCurrentTeam[1], tlx, tly, brx, bry, MSYS_PRIORITY_HIGHEST - 3, CURSOR_LAPTOP_SCREEN, MSYS_NO_CALLBACK, PersonnelDepartedTeamCallback);

		fCreated = TRUE;
	}
	else if (!create && fCreated)
	{
		// created, get rid of

		MSYS_RemoveRegion(&gTogglePastCurrentTeam[0]);
		MSYS_RemoveRegion(&gTogglePastCurrentTeam[1]);
		fCreated = FALSE;
	}
}


static void PersonnelCurrentTeamCallback(MOUSE_REGION* pRegion, INT32 iReason)
{
	if (iReason & MSYS_CALLBACK_REASON_LBUTTON_UP)
	{
		if (fCurrentTeamMode) return;
		fCurrentTeamMode = TRUE;

		SelectFirstDisplayedMerc();
		fReDrawScreenFlag = TRUE;
	}
}


static void PersonnelDepartedTeamCallback(MOUSE_REGION* pRegion, INT32 iReason)
{
	if (iReason & MSYS_CALLBACK_REASON_LBUTTON_UP)
	{
		if (!fCurrentTeamMode) return;
		fCurrentTeamMode = FALSE;

		SelectFirstDisplayedMerc();

		//Switch the panel on the right to be the stat panel
		gubPersonnelInfoState = PERSONNEL_STAT_BTN;

		fReDrawScreenFlag = TRUE;
	}
}


static void DepartedDownCallBack(GUI_BUTTON* btn, INT32 reason);
static void DepartedUpCallBack(GUI_BUTTON* btn, INT32 reason);


static void CreateDestroyButtonsForDepartedTeamList(void)
{
	// creates/ destroys the buttons for cdeparted team list
	static BOOLEAN fCreated = FALSE;

	if (!fCurrentTeamMode && !fCreated)
	{
		// not created. create
		giPersonnelButton[4] = QuickCreateButtonImg("LAPTOP/departuresbuttons.sti", -1, 0, -1, 2, -1, PERS_DEPARTED_UP_X, PERS_DEPARTED_UP_Y,   MSYS_PRIORITY_HIGHEST - 1, DepartedUpCallBack);
		giPersonnelButton[5] = QuickCreateButtonImg("LAPTOP/departuresbuttons.sti", -1, 1, -1, 3, -1, PERS_DEPARTED_UP_X, PERS_DEPARTED_DOWN_Y, MSYS_PRIORITY_HIGHEST - 1, DepartedDownCallBack);

		// set up cursors for these buttons
		SetButtonCursor(giPersonnelButton[4], CURSOR_LAPTOP_SCREEN);
		SetButtonCursor(giPersonnelButton[5], CURSOR_LAPTOP_SCREEN);

		fCreated = TRUE;
	}
	else if (fCurrentTeamMode && fCreated)
	{
		// created. destroy
		RemoveButton(giPersonnelButton[4]);
		RemoveButton(giPersonnelButton[5]);
		fCreated = FALSE;
		fReDrawScreenFlag = TRUE;
	}
}


static void DepartedUpCallBack(GUI_BUTTON *btn, INT32 reason)
{
	if (reason & MSYS_CALLBACK_REASON_LBUTTON_UP)
	{
		if (giCurrentUpperLeftPortraitNumber - PERSONNEL_PORTRAIT_NUMBER >= 0)
		{
			giCurrentUpperLeftPortraitNumber -= PERSONNEL_PORTRAIT_NUMBER;
			fReDrawScreenFlag = TRUE;
		}
	}
}


static void DepartedDownCallBack(GUI_BUTTON *btn, INT32 reason)
{
	if (reason & MSYS_CALLBACK_REASON_LBUTTON_UP)
	{
		if (giCurrentUpperLeftPortraitNumber + PERSONNEL_PORTRAIT_NUMBER < GetNumberOfPastMercsOnPlayersTeam())
		{
			giCurrentUpperLeftPortraitNumber += PERSONNEL_PORTRAIT_NUMBER;
			fReDrawScreenFlag = TRUE;
		}
	}
}


static BOOLEAN DisplayPortraitOfPastMerc(INT32 iId, INT32 iCounter, BOOLEAN fDead, BOOLEAN fFired, BOOLEAN fOther);


static void DisplayPastMercsPortraits(void)
{
	/* display past mercs portraits, starting at giCurrentUpperLeftPortraitNumber
	 * and going up PERSONNEL_PORTRAIT_NUMBER mercs start at dead mercs, then
	 * fired, then other */
	INT32 iCounter = 0;
	INT32 iCounterA;
	INT32 iStartArray; // 0 = dead list, 1 = fired list, 2 = other list

	// not time to display
	if (fCurrentTeamMode) return;

	// go through dead list
	for (iCounterA = 0; iCounter < giCurrentUpperLeftPortraitNumber; iCounterA++)
	{
		if (LaptopSaveInfo.ubDeadCharactersList[iCounterA] != -1)
			iCounter++;
	}

	if (iCounter < giCurrentUpperLeftPortraitNumber)
	{
		// now the fired list
		for (iCounterA = 0; iCounter < giCurrentUpperLeftPortraitNumber; iCounterA++)
		{
			if (LaptopSaveInfo.ubLeftCharactersList[iCounterA] != -1)
			{
				iCounter++;
			}
		}

		iStartArray = (iCounter < PERSONNEL_PORTRAIT_NUMBER ? 0 : 1);
	}
	else
	{
		iStartArray = 0;
	}

	if (iCounter < giCurrentUpperLeftPortraitNumber && iStartArray != 0)
	{
		// now the fired list
		for (iCounterA = 0; iCounter < giCurrentUpperLeftPortraitNumber; iCounterA++)
		{
			if (LaptopSaveInfo.ubOtherCharactersList[iCounterA] != -1)
				iCounter++;
		}

		iStartArray = (iCounter < PERSONNEL_PORTRAIT_NUMBER ? 1 : 2);
	}
	else if (iStartArray != 0)
	{
		iStartArray = 1;
	}

	// We now have the array to start in, the position
	iCounter = 0;

	if (iStartArray == 0)
	{
		// run through list and display
		for (iCounterA; iCounter < PERSONNEL_PORTRAIT_NUMBER && iCounterA < 256; iCounterA++)
		{
			// show dead pictures
			if (LaptopSaveInfo.ubDeadCharactersList[iCounterA] != -1)
			{
				DisplayPortraitOfPastMerc(LaptopSaveInfo.ubDeadCharactersList[iCounterA], iCounter, TRUE, FALSE, FALSE);
				iCounter++;
			}
		}
		// reset counter A for the next array, if applicable
		iCounterA = 0;
	}

	if (iStartArray <= 1)
	{
		for (iCounterA; iCounter < PERSONNEL_PORTRAIT_NUMBER  && iCounterA < 256; iCounterA++)
		{
			// show fired pics
			if (LaptopSaveInfo.ubLeftCharactersList[iCounterA] != -1)
			{
				DisplayPortraitOfPastMerc(LaptopSaveInfo.ubLeftCharactersList[iCounterA], iCounter, FALSE, TRUE, FALSE);
				iCounter++;
			}
		}
		// reset counter A for the next array, if applicable
		iCounterA = 0;
	}

	if (iStartArray <= 2)
	{
		for (iCounterA; iCounter < PERSONNEL_PORTRAIT_NUMBER  && iCounterA < 256; iCounterA++)
		{
			// show other pics
			if (LaptopSaveInfo.ubOtherCharactersList[iCounterA] != -1)
			{
				DisplayPortraitOfPastMerc(LaptopSaveInfo.ubOtherCharactersList[iCounterA], iCounter, FALSE, FALSE, TRUE);
				iCounter++;
			}
		}
	}
}


// returns ID of Merc in this slot
static PastMercInfo GetSelectedPastMercInfo(void)
{
	INT32 iSlot = giCurrentUpperLeftPortraitNumber + iCurrentPersonSelectedId;

	Assert(!fCurrentTeamMode);
	Assert(iSlot < GetNumberOfPastMercsOnPlayersTeam());

	const LaptopSaveInfoStruct* const l = &LaptopSaveInfo;
	for (const INT16* i = l->ubDeadCharactersList; i != endof(l->ubDeadCharactersList); ++i)
	{
		if (*i != -1 && iSlot-- == 0) return (PastMercInfo){ *i, DEPARTED_DEAD };
	}
	for (const INT16* i = l->ubLeftCharactersList; i != endof(l->ubLeftCharactersList); ++i)
	{
		if (*i != -1 && iSlot-- == 0) return (PastMercInfo){ *i, DEPARTED_FIRED };
	}
	for (const INT16* i = l->ubOtherCharactersList; i != endof(l->ubOtherCharactersList); ++i)
	{
		if (*i != -1 && iSlot-- == 0) return (PastMercInfo){ *i, DEPARTED_OTHER };
	}
	return (PastMercInfo){ -1, -1 };
}


static BOOLEAN DisplayPortraitOfPastMerc(INT32 iId, INT32 iCounter, BOOLEAN fDead, BOOLEAN fFired, BOOLEAN fOther)
{
	char sTemp[100];
	sprintf(sTemp, SMALL_FACES_DIR "%02d.sti", gMercProfiles[iId].ubFaceIndex);

	UINT32 guiFACE = AddVideoObjectFromFile(sTemp);
	CHECKF(guiFACE != NO_VOBJECT);

	HVOBJECT hFaceHandle = GetVideoObject(guiFACE);

	if (fDead)
	{
		hFaceHandle->pShades[0] = Create16BPPPaletteShaded(hFaceHandle->pPaletteEntry, DEAD_MERC_COLOR_RED, DEAD_MERC_COLOR_GREEN, DEAD_MERC_COLOR_BLUE, TRUE);
		//set the red pallete to the face
		SetObjectHandleShade(guiFACE, 0);
	}

	const INT32 x = SMALL_PORTRAIT_START_X + iCounter % PERSONNEL_PORTRAIT_NUMBER_WIDTH * SMALL_PORT_WIDTH;
	const INT32 y = SMALL_PORTRAIT_START_Y + iCounter / PERSONNEL_PORTRAIT_NUMBER_WIDTH * SMALL_PORT_HEIGHT;
	BltVideoObject(FRAME_BUFFER, hFaceHandle, 0, x, y);

	DeleteVideoObjectFromIndex(guiFACE);

	return TRUE;
}


static void DisplayDepartedCharStats(INT32 iId, INT32 iState)
{
	wchar_t sString[50];
	INT16 sX;
	INT16 sY;

	SetFont(FONT10ARIAL);
	SetFontBackground(FONT_BLACK);
	SetFontForeground(PERS_TEXT_FONT_COLOR);

	const MERCPROFILESTRUCT* const p = &gMercProfiles[iId];

	const INT8 life = p->bLife;
	const INT8 cur  = (iState == DEPARTED_DEAD ? 0 : life);
	swprintf(sString, lengthof(sString), L"%d/%d", cur, life);
	mprintf(pers_stat_x, pers_stat_y[0], pPersonnelScreenStrings[0]);
	FindFontRightCoordinates(pers_stat_x, 0, TEXT_BOX_WIDTH - 20, 0, sString, PERS_FONT, &sX, &sY);
	mprintf(sX, pers_stat_y[0], sString);

	PrintStat(p->bAgility,     pers_stat_y[ 1], pPersonnelScreenStrings[1]);
	PrintStat(p->bDexterity,   pers_stat_y[ 2], pPersonnelScreenStrings[2]);
	PrintStat(p->bStrength,    pers_stat_y[ 3], pPersonnelScreenStrings[3]);
	PrintStat(p->bLeadership,  pers_stat_y[ 4], pPersonnelScreenStrings[4]);
	PrintStat(p->bWisdom,      pers_stat_y[ 5], pPersonnelScreenStrings[5]);
	PrintStat(p->bExpLevel,    pers_stat_y[ 6], pPersonnelScreenStrings[6]);
	PrintStat(p->bMarksmanship,pers_stat_y[ 7], pPersonnelScreenStrings[7]);
	PrintStat(p->bMechanical,  pers_stat_y[ 8], pPersonnelScreenStrings[8]);
	PrintStat(p->bExplosive,   pers_stat_y[ 9], pPersonnelScreenStrings[9]);
	PrintStat(p->bMedical,     pers_stat_y[10], pPersonnelScreenStrings[10]);
	PrintStat(p->usKills,      pers_stat_y[21], pPersonnelScreenStrings[PRSNL_TXT_KILLS]);
	PrintStat(p->usAssists,    pers_stat_y[22], pPersonnelScreenStrings[PRSNL_TXT_ASSISTS]);

	// Shots/hits
	mprintf(pers_stat_x, pers_stat_y[23], pPersonnelScreenStrings[PRSNL_TXT_HIT_PERCENTAGE]);
	// check we have shot at least once
	const UINT32 fired = p->usShotsFired;
	const UINT32 hits  = (fired > 0 ? 100 * p->usShotsHit / fired : 0);
	swprintf(sString, lengthof(sString), L"%d %%", hits);
	FindFontRightCoordinates(pers_stat_x, 0, TEXT_BOX_WIDTH - 20, 0, sString, PERS_FONT, &sX, &sY);
	mprintf(sX, pers_stat_y[23], L"%ls", sString);

	PrintStat(p->usBattlesFought, pers_stat_y[24], pPersonnelScreenStrings[PRSNL_TXT_BATTLES]);
	PrintStat(p->usTimesWounded,  pers_stat_y[25], pPersonnelScreenStrings[PRSNL_TXT_TIMES_WOUNDED]);
}


static void EnableDisableDeparturesButtons(void)
{
	// will enable or disable departures buttons based on upperleft picutre index value
	if (fCurrentTeamMode || fNewMailFlag)
	{
		return;
	}

	// disable both buttons
	DisableButton(giPersonnelButton[4]);
	DisableButton(giPersonnelButton[5]);


	if (giCurrentUpperLeftPortraitNumber != 0)
	{
		// enable up button
		EnableButton(giPersonnelButton[4]);
	}
	if (GetNumberOfPastMercsOnPlayersTeam() - giCurrentUpperLeftPortraitNumber >= PERSONNEL_PORTRAIT_NUMBER)
	{
		// enable down button
		EnableButton(giPersonnelButton[5]);
	}
}


static void DisplayDepartedCharName(INT32 iId, INT32 iState)
{
	// get merc's nickName, assignment, and sector location info
	INT16 sX, sY;

	SetFont(CHAR_NAME_FONT);
	SetFontForeground(PERS_TEXT_FONT_COLOR);
	SetFontBackground(FONT_BLACK);

	if (iState == -1 || iId == -1)
	{
		return;
	}

	const MERCPROFILESTRUCT* const p = &gMercProfiles[iId];

	const wchar_t* name = p->zNickname;
	FindFontCenterCoordinates(CHAR_NAME_LOC_X, 0, CHAR_NAME_LOC_WIDTH, 0, name, CHAR_NAME_FONT, &sX, &sY);
	mprintf(sX, CHAR_NAME_Y, name);

	const wchar_t* State;
	if (p->ubMiscFlags2 & PROFILE_MISC_FLAG2_MARRIED_TO_HICKS)
	{
		//displaye 'married'
		State = pPersonnelDepartedStateStrings[DEPARTED_MARRIED];
	}
	else if (iState == DEPARTED_DEAD)
	{
		State = pPersonnelDepartedStateStrings[DEPARTED_DEAD];
	}
	else if (iId < BIFF) // if the merc is an AIM merc
	{
		//if dismissed
		if (iState == DEPARTED_FIRED)
			State = pPersonnelDepartedStateStrings[DEPARTED_FIRED];
		else
			State = pPersonnelDepartedStateStrings[DEPARTED_CONTRACT_EXPIRED];
	}

	//else if its a MERC merc
	else if (iId >= BIFF && iId <= BUBBA)
	{
		if (iState == DEPARTED_FIRED)
			State = pPersonnelDepartedStateStrings[DEPARTED_FIRED];
		else
			State = pPersonnelDepartedStateStrings[DEPARTED_QUIT];
	}
	//must be a RPC
	else
	{
		if (iState == DEPARTED_FIRED)
			State = pPersonnelDepartedStateStrings[DEPARTED_FIRED];
		else
			State = pPersonnelDepartedStateStrings[DEPARTED_QUIT];
	}

	FindFontCenterCoordinates(CHAR_NAME_LOC_X, 0, CHAR_NAME_LOC_WIDTH, 0, State, CHAR_NAME_FONT, &sX, &sY);
	mprintf(sX, CHAR_LOC_Y, State);
}


static void DisplayPersonnelTextOnTitleBar(void)
{
	SetFont(FONT14ARIAL);
	SetFontForeground(FONT_WHITE);
	SetFontBackground(FONT_BLACK);
	mprintf(PERS_TITLE_X, PERS_TITLE_Y, pPersTitleText);
}


// display box around currently selected merc
static BOOLEAN DisplayHighLightBox(void)
{
	// will display highlight box around selected merc

	// is the current selected face valid?
	if (iCurrentPersonSelectedId == -1)
	{
		// no, leave
		return FALSE;
	}

	UINT32 uiBox = AddVideoObjectFromFile("LAPTOP/PicBorde.sti");
	CHECKF(uiBox != NO_VOBJECT);
	const INT32 x = SMALL_PORTRAIT_START_X + iCurrentPersonSelectedId % PERSONNEL_PORTRAIT_NUMBER_WIDTH * SMALL_PORT_WIDTH  - 2;
	const INT32 y = SMALL_PORTRAIT_START_Y + iCurrentPersonSelectedId / PERSONNEL_PORTRAIT_NUMBER_WIDTH * SMALL_PORT_HEIGHT - 3;
	BltVideoObjectFromIndex(FRAME_BUFFER, uiBox, 0, x, y);
	DeleteVideoObjectFromIndex(uiBox);

	return TRUE;
}


// add to dead list
void AddCharacterToDeadList(SOLDIERTYPE *pSoldier)
{
	for (INT32 i = 0; i < 256; i++)
	{
		if (LaptopSaveInfo.ubDeadCharactersList[i] == -1)
		{
			// valid slot, merc not found yet, inset here
			LaptopSaveInfo.ubDeadCharactersList[i] = pSoldier->ubProfile;
			return;
		}

		// are they already in the list?
		if (LaptopSaveInfo.ubDeadCharactersList[i] == pSoldier->ubProfile)
		{
			return;
		}
	}
}


void AddCharacterToFiredList(SOLDIERTYPE *pSoldier)
{
	for (INT32 i = 0; i < 256; i++)
	{
		if (LaptopSaveInfo.ubLeftCharactersList[i] == -1)
		{
			// valid slot, merc not found yet, inset here
			LaptopSaveInfo.ubLeftCharactersList[i] = pSoldier->ubProfile;
			return;
		}

		// are they already in the list?
		if (LaptopSaveInfo.ubLeftCharactersList[i] == pSoldier->ubProfile)
		{
			return;
		}
	}
}


void AddCharacterToOtherList(SOLDIERTYPE *pSoldier)
{
	for (INT32 i = 0; i < 256; i++)
	{
		if (LaptopSaveInfo.ubOtherCharactersList[i] == -1)
		{
			// valid slot, merc not found yet, inset here
			LaptopSaveInfo.ubOtherCharactersList[i] = pSoldier->ubProfile;
			return;
		}

		// are they already in the list?
		if (LaptopSaveInfo.ubOtherCharactersList[i] == pSoldier->ubProfile)
		{
			return;
		}
	}
}


// If you have hired a merc before, then the they left for whatever reason, and now you are hiring them again,
// we must get rid of them from the departed section in the personnel screen.  (wouldnt make sense for them
//to be on your team list, and departed list)
BOOLEAN RemoveNewlyHiredMercFromPersonnelDepartedList(UINT8 ubProfile)
{
	for (INT32 i = 0; i < 256; i++)
	{
		// are they already in the Dead list?
		if (LaptopSaveInfo.ubDeadCharactersList[i] == ubProfile)
		{
			//Reset the fact that they were once hired
			LaptopSaveInfo.ubDeadCharactersList[i] = -1;
			return TRUE;
		}

		// are they already in the other list?
		if (LaptopSaveInfo.ubLeftCharactersList[i] == ubProfile)
		{
			//Reset the fact that they were once hired
			LaptopSaveInfo.ubLeftCharactersList[i] = -1;
			return TRUE;
		}

		// are they already in the list?
		if (LaptopSaveInfo.ubOtherCharactersList[i] == ubProfile)
		{
			//Reset the fact that they were once hired
			LaptopSaveInfo.ubOtherCharactersList[i] = -1;
			return TRUE;
		}
	}

	return FALSE;
}


// Select the first displayed merc, if there is any
static void SelectFirstDisplayedMerc(void)
{
	// set current soldier
	if (fCurrentTeamMode)
	{
		CFOR_ALL_IN_TEAM(s, OUR_TEAM)
		{
			if (!(s->uiStatusFlags & SOLDIER_VEHICLE))
			{
				iCurrentPersonSelectedId = 0;
				return;
			}
		}
		iCurrentPersonSelectedId = -1;
	}
	else
	{
		iCurrentPersonSelectedId = GetNumberOfPastMercsOnPlayersTeam() > 0 ? 0 : -1;
	}
}


static const SOLDIERTYPE* GetSoldierOfCurrentSlot(void)
{
	Assert(fCurrentTeamMode);

	INT32 slot = iCurrentPersonSelectedId;
	CFOR_ALL_IN_TEAM(s, OUR_TEAM)
	{
		if (slot-- == 0) return s;
	}

	return NULL;
}


static BOOLEAN RenderAtmPanel(void)
{
	// just show basic panel
	// bounding
	UINT32 uiBox = AddVideoObjectFromFile("LAPTOP/AtmButtons.sti");
	CHECKF(uiBox != NO_VOBJECT);
	BltVideoObjectFromIndex(FRAME_BUFFER, uiBox, 0, ATM_UL_X,     ATM_UL_Y);
	BltVideoObjectFromIndex(FRAME_BUFFER, uiBox, 1, ATM_UL_X + 1, ATM_UL_Y + 18);
	DeleteVideoObjectFromIndex(uiBox);

	// create destroy
	CreateDestroyStartATMButton();
	return TRUE;
}


static void MakeButton(UINT idx, INT16 y, GUI_CALLBACK click, const wchar_t* text)
{
	INT32 img = LoadButtonImage("LAPTOP/AtmButtons.sti", -1, 2, -1, 3, -1);
	giPersonnelATMStartButtonImage[idx] = img;
	INT32 btn = QuickCreateButtonNoMove(img, 519, y, MSYS_PRIORITY_HIGHEST - 1, click);
	giPersonnelATMStartButton[idx] = btn;
	SpecifyGeneralButtonTextAttributes(btn, text, PERS_FONT, FONT_BLACK, FONT_BLACK);
	SetButtonCursor(btn, CURSOR_LAPTOP_SCREEN);
}


static void EmployementInfoButtonCallback(GUI_BUTTON* btn, INT32 reason);
static void PersonnelINVStartButtonCallback(GUI_BUTTON* btn, INT32 reason);
static void PersonnelStatStartButtonCallback(GUI_BUTTON* btn, INT32 reason);


static void CreateDestroyStartATMButton(void)
{
	static BOOLEAN fCreated = FALSE;
	// create/destroy atm start button as needed

	if (!fCreated && fShowAtmPanelStartButton)
	{
		// not created, must create
		MakeButton(PERSONNEL_STAT_BTN,        80, PersonnelStatStartButtonCallback, gsAtmStartButtonText[0]);
		MakeButton(PERSONNEL_EMPLOYMENT_BTN, 110, EmployementInfoButtonCallback,    gsAtmStartButtonText[2]);
		MakeButton(PERSONNEL_INV_BTN,        140, PersonnelINVStartButtonCallback,  gsAtmStartButtonText[1]);

		fCreated = TRUE;
	}
	else if (fCreated && !fShowAtmPanelStartButton)
	{
		// stop showing
		RemoveButton(giPersonnelATMStartButton[PERSONNEL_STAT_BTN]);
		UnloadButtonImage(giPersonnelATMStartButtonImage[PERSONNEL_STAT_BTN]);
		RemoveButton(giPersonnelATMStartButton[PERSONNEL_EMPLOYMENT_BTN]);
		UnloadButtonImage(giPersonnelATMStartButtonImage[PERSONNEL_EMPLOYMENT_BTN]);
		RemoveButton(giPersonnelATMStartButton[PERSONNEL_INV_BTN]);
		UnloadButtonImage(giPersonnelATMStartButtonImage[PERSONNEL_INV_BTN]);

		fCreated = FALSE;
	}
}


static void FindPositionOfPersInvSlider(void)
{
	const INT32 item_count   = GetNumberOfInventoryItemsOnCurrentMerc();
	const INT32 scroll_count = item_count - NUMBER_OF_INVENTORY_PERSONNEL;
	guiSliderPosition = (Y_SIZE_OF_PERSONNEL_SCROLL_REGION - SIZE_OF_PERSONNEL_CURSOR) * uiCurrentInventoryIndex / scroll_count;
}


static void HandleSliderBarClickCallback(MOUSE_REGION* pRegion, INT32 iReason)
{
	if (iReason & MSYS_CALLBACK_REASON_LBUTTON_DWN || iReason & MSYS_CALLBACK_REASON_LBUTTON_REPEAT)
	{
		const INT32 item_count = GetNumberOfInventoryItemsOnCurrentMerc();
		if (item_count <= NUMBER_OF_INVENTORY_PERSONNEL) return;

		const INT32 scroll_count = item_count - NUMBER_OF_INVENTORY_PERSONNEL;

		// get the actual item position
		const INT16 new_item_idx = scroll_count * pRegion->RelativeYPos / (Y_SIZE_OF_PERSONNEL_SCROLL_REGION - SIZE_OF_PERSONNEL_CURSOR);

		if (uiCurrentInventoryIndex != new_item_idx)
		{
			// get slider position
			guiSliderPosition = (Y_SIZE_OF_PERSONNEL_SCROLL_REGION - SIZE_OF_PERSONNEL_CURSOR) * new_item_idx / scroll_count;

			// set current inventory value
			uiCurrentInventoryIndex = new_item_idx;

			// force update
			fReDrawScreenFlag = TRUE;
		}
	}
	else if (iReason & MSYS_CALLBACK_REASON_WHEEL_UP)
	{
		InventoryUp();
	}
	else if (iReason & MSYS_CALLBACK_REASON_WHEEL_DOWN)
	{
		InventoryDown();
	}
}


static void RenderSliderBarForPersonnelInventory(void)
{
	// render slider bar for personnel
	BltVideoObjectFromIndex(FRAME_BUFFER, guiPersonnelInventory, 5, X_OF_PERSONNEL_SCROLL_REGION, guiSliderPosition + Y_OF_PERSONNEL_SCROLL_REGION);
}


static void PersonnelINVStartButtonCallback(GUI_BUTTON *btn, INT32 reason)
{
	if (reason & MSYS_CALLBACK_REASON_LBUTTON_DWN)
	{
		fReDrawScreenFlag = TRUE;
		btn->uiFlags |= BUTTON_CLICKED_ON;
		ButtonList[giPersonnelATMStartButton[PERSONNEL_STAT_BTN]]->uiFlags       &= ~BUTTON_CLICKED_ON;
		ButtonList[giPersonnelATMStartButton[PERSONNEL_EMPLOYMENT_BTN]]->uiFlags &= ~BUTTON_CLICKED_ON;
		gubPersonnelInfoState = PRSNL_INV;
	}
}


static void PersonnelStatStartButtonCallback(GUI_BUTTON *btn, INT32 reason)
{
	if (reason & MSYS_CALLBACK_REASON_LBUTTON_DWN)
	{
		fReDrawScreenFlag = TRUE;
		btn->uiFlags |= BUTTON_CLICKED_ON;
		ButtonList[giPersonnelATMStartButton[PERSONNEL_EMPLOYMENT_BTN]]->uiFlags &= ~BUTTON_CLICKED_ON;
		ButtonList[giPersonnelATMStartButton[PERSONNEL_INV_BTN]]->uiFlags        &= ~BUTTON_CLICKED_ON;
		gubPersonnelInfoState = PRSNL_STATS;
	}
}


static void EmployementInfoButtonCallback(GUI_BUTTON *btn, INT32 reason)
{
	if (reason & MSYS_CALLBACK_REASON_LBUTTON_DWN)
	{
		fReDrawScreenFlag = TRUE;
		btn->uiFlags |= BUTTON_CLICKED_ON;
		ButtonList[giPersonnelATMStartButton[PERSONNEL_INV_BTN]]->uiFlags  &= ~BUTTON_CLICKED_ON;
		ButtonList[giPersonnelATMStartButton[PERSONNEL_STAT_BTN]]->uiFlags &= ~BUTTON_CLICKED_ON;
		gubPersonnelInfoState = PRSNL_EMPLOYMENT;
	}
}


// get the total amt of money on this guy
static INT32 GetFundsOnMerc(const SOLDIERTYPE* pSoldier)
{
	INT32 iCurrentAmount = 0;
	INT32 iCurrentPocket = 0;
	// run through mercs pockets, if any money in them, add to total

	// error check
	if (pSoldier == NULL)
	{
		return 0;
	}

	// run through grunts pockets and count all the spare change
	for (iCurrentPocket = 0; iCurrentPocket < NUM_INV_SLOTS; iCurrentPocket++)
	{
		if (Item[pSoldier->inv[iCurrentPocket].usItem].usItemClass == IC_MONEY)
		{
			iCurrentAmount += pSoldier->inv[iCurrentPocket].uiMoneyAmount;
		}
	}

	return iCurrentAmount;
}


// check if current guy can have atm
static void UpDateStateOfStartButton(void)
{
	// start button being shown?
	if (!fShowAtmPanelStartButton)
	{
		return;
	}

	if (gubPersonnelInfoState == PRSNL_INV)
	{
		ButtonList[giPersonnelATMStartButton[PERSONNEL_INV_BTN]]->uiFlags |= BUTTON_CLICKED_ON;
		ButtonList[giPersonnelATMStartButton[PERSONNEL_STAT_BTN]]->uiFlags &= ~(BUTTON_CLICKED_ON);
		ButtonList[giPersonnelATMStartButton[PERSONNEL_EMPLOYMENT_BTN]]->uiFlags &= ~(BUTTON_CLICKED_ON);
	}
	else if (gubPersonnelInfoState == PRSNL_STATS)
	{
		ButtonList[giPersonnelATMStartButton[PERSONNEL_INV_BTN]]->uiFlags &= ~BUTTON_CLICKED_ON;
		ButtonList[giPersonnelATMStartButton[PERSONNEL_STAT_BTN]]->uiFlags |= BUTTON_CLICKED_ON;
		ButtonList[giPersonnelATMStartButton[PERSONNEL_EMPLOYMENT_BTN]]->uiFlags &= ~(BUTTON_CLICKED_ON);
	}
	else
	{
		ButtonList[giPersonnelATMStartButton[PERSONNEL_STAT_BTN]]->uiFlags &= ~BUTTON_CLICKED_ON;
		ButtonList[giPersonnelATMStartButton[PERSONNEL_INV_BTN]]->uiFlags &= ~(BUTTON_CLICKED_ON);
		ButtonList[giPersonnelATMStartButton[PERSONNEL_EMPLOYMENT_BTN]]->uiFlags |= BUTTON_CLICKED_ON;
	}

	// if in current mercs and the currently selected guy is valid, enable button, else disable it
	if (fCurrentTeamMode)
	{
		// is the current guy valid
		if (GetNumberOfMercsDeadOrAliveOnPlayersTeam() > 0)
		{
			EnableButton(giPersonnelATMStartButton[PERSONNEL_STAT_BTN]);
			EnableButton(giPersonnelATMStartButton[PERSONNEL_INV_BTN]);
			EnableButton(giPersonnelATMStartButton[PERSONNEL_EMPLOYMENT_BTN]);

			const SOLDIERTYPE* const s = GetSoldierOfCurrentSlot();
			if (s != NULL && s->bAssignment == ASSIGNMENT_POW)
			{
				DisableButton(giPersonnelATMStartButton[PERSONNEL_INV_BTN]);

				if (gubPersonnelInfoState == PRSNL_INV)
				{
					gubPersonnelInfoState = PRSNL_STATS;
					fPausedReDrawScreenFlag = TRUE;
				}
			}
		}
		else
		{
			// not valid, disable
			DisableButton(giPersonnelATMStartButton[PERSONNEL_STAT_BTN]);
			DisableButton(giPersonnelATMStartButton[PERSONNEL_INV_BTN]);
			DisableButton(giPersonnelATMStartButton[PERSONNEL_EMPLOYMENT_BTN]);
		}
	}
	else
	{
		// disable button
		EnableButton(giPersonnelATMStartButton[PERSONNEL_STAT_BTN]);
		DisableButton(giPersonnelATMStartButton[PERSONNEL_INV_BTN]);
		DisableButton(giPersonnelATMStartButton[PERSONNEL_EMPLOYMENT_BTN]);
	}
}


static void DisplayAmountOnCurrentMerc(void)
{
	if (!fCurrentTeamMode) return;

	// will display the amount that the merc is carrying on him or herself
	CHAR16 sString[64];
	SPrintMoney(sString, GetFundsOnMerc(GetSoldierOfCurrentSlot()));

	SetFont(ATM_FONT);
	SetFontForeground(FONT_WHITE);
	SetFontBackground(FONT_BLACK);

	INT16 sX;
	INT16 sY;
	FindFontRightCoordinates(ATM_DISPLAY_X, ATM_DISPLAY_Y, ATM_DISPLAY_WIDTH, ATM_DISPLAY_HEIGHT, sString, ATM_FONT, &sX, &sY);
	mprintf(sX, sY, sString);
}


static void HandlePersonnelKeyboard(void)
{
	InputAtom InputEvent;
	while (DequeueEvent(&InputEvent))
	{
		HandleKeyBoardShortCutsForLapTop(InputEvent.usEvent, InputEvent.usParam, InputEvent.usKeyState);
	}
}


static INT32 CalcTimeLeftOnMercContract(const SOLDIERTYPE* pSoldier);


static void DisplayEmploymentinformation(const SOLDIERTYPE* const s)
{
	wchar_t sString[50];
	wchar_t sStringA[50];
	INT16 sX, sY;

	if (s->uiStatusFlags & SOLDIER_VEHICLE) return;

	const MERCPROFILESTRUCT* p = &gMercProfiles[s->ubProfile];

	// display the stats for a char
	for (INT32 i = 0; i < MAX_STATS; i++)
	{
		switch (i)
		{
			case 0: //Remaining Contract:
			{
				if (s->ubWhatKindOfMercAmI == MERC_TYPE__AIM_MERC || s->ubProfile == SLAY)
				{
					const UINT32 uiMinutesInDay = 24 * 60;
					INT32 iTimeLeftOnContract = CalcTimeLeftOnMercContract(s);

					//if the merc is in transit
					if (s->bAssignment == IN_TRANSIT)
					{
						//and if the ttime left on the cotract is greater then the contract time
						if (iTimeLeftOnContract > (INT32)(s->iTotalContractLength * uiMinutesInDay))
						{
							iTimeLeftOnContract = (s->iTotalContractLength * uiMinutesInDay);
						}
					}
					// if there is going to be a both days and hours left on the contract
					const INT days  = iTimeLeftOnContract / uiMinutesInDay;
					const INT hours = iTimeLeftOnContract % uiMinutesInDay / 60;
					if (days > 0)
					{
						swprintf(sString, lengthof(sString), L"%d%ls %d%ls / %d%ls", days, gpStrategicString[STR_PB_DAYS_ABBREVIATION], hours, gpStrategicString[STR_PB_HOURS_ABBREVIATION], s->iTotalContractLength, gpStrategicString[STR_PB_DAYS_ABBREVIATION]);
					}
					else //else there is under a day left
					{
						//DEF: removed 2/7/99
						swprintf(sString, lengthof(sString), L"%d%ls / %d%ls", hours, gpStrategicString[STR_PB_HOURS_ABBREVIATION], s->iTotalContractLength, gpStrategicString[STR_PB_DAYS_ABBREVIATION]);
					}
				}
				else if (s->ubWhatKindOfMercAmI == MERC_TYPE__MERC)
				{
					wcscpy(sString, gpStrategicString[STR_PB_NOTAPPLICABLE_ABBREVIATION]);
				}
				else
				{
					wcscpy(sString, gpStrategicString[STR_PB_NOTAPPLICABLE_ABBREVIATION]);
				}

				mprintf(pers_stat_x, pers_stat_y[i], pPersonnelScreenStrings[PRSNL_TXT_CURRENT_CONTRACT]);
				FindFontRightCoordinates(pers_stat_data_x, 0, TEXT_BOX_WIDTH - 20, 0, sString, PERS_FONT, &sX, &sY);
				mprintf(sX, pers_stat_y[i], sString);
			}
			break;

			case 1: // total contract time served
				mprintf(pers_stat_x, pers_stat_y[i], pPersonnelScreenStrings[PRSNL_TXT_TOTAL_SERVICE]);
				//./DEF 2/4/99: total service days used to be calced as 'days -1'
				swprintf(sString, lengthof(sString), L"%d %ls", p->usTotalDaysServed, gpStrategicString[STR_PB_DAYS_ABBREVIATION]);
				FindFontRightCoordinates(pers_stat_data_x, 0, TEXT_BOX_WIDTH - 20, 0, sString, PERS_FONT, &sX, &sY);
				mprintf(sX, pers_stat_y[i], sString);
				break;

			case 3: // cost (PRSNL_TXT_TOTAL_COST)
			{
				SPrintMoney(sString, p->uiTotalCostToDate);
				FindFontRightCoordinates(pers_stat_data_x, 0, TEXT_BOX_WIDTH - 20, 0, sString, PERS_FONT, &sX, &sY);
				mprintf(pers_stat_x, pers_stat_y[i], pPersonnelScreenStrings[PRSNL_TXT_TOTAL_COST]);

				// print contract cost
				mprintf(sX, pers_stat_y[i], sString);

				INT32 salary;
				switch (s->ubWhatKindOfMercAmI)
				{
					case MERC_TYPE__AIM_MERC:
						switch (s->bTypeOfLastContract)
						{
							case CONTRACT_EXTEND_2_WEEK: salary = p->uiBiWeeklySalary / 14; break;
							case CONTRACT_EXTEND_1_WEEK: salary = p->uiWeeklySalary   /  7; break;
							default:                     salary = p->sSalary;               break;
						}
						break;

					default: salary = p->sSalary; break;
				}

				SPrintMoney(sStringA, salary);
				FindFontRightCoordinates(pers_stat_data_x, 0, TEXT_BOX_WIDTH - 20, 0, sStringA, PERS_FONT,  &sX, &sY);

				i++;

				// now print daily rate
				mprintf(sX, pers_stat_y[i + 1], sStringA);
				mprintf(pers_stat_x, pers_stat_y[i + 1], pPersonnelScreenStrings[PRSNL_TXT_DAILY_COST]);
				break;
			}

			case 5: // medical deposit
				//if its a merc merc, display the salary oweing
				if (s->ubWhatKindOfMercAmI == MERC_TYPE__MERC)
				{
					mprintf(pers_stat_x, pers_stat_y[i - 1], pPersonnelScreenStrings[PRSNL_TXT_UNPAID_AMOUNT]);
					SPrintMoney(sString, p->sSalary * p->iMercMercContractLength);
				}
				else
				{
					mprintf(pers_stat_x, pers_stat_y[i - 1], pPersonnelScreenStrings[PRSNL_TXT_MED_DEPOSIT]);
					SPrintMoney(sString, p->sMedicalDepositAmount);
				}
				FindFontRightCoordinates(pers_stat_data_x, 0, TEXT_BOX_WIDTH - 20, 0, sString, PERS_FONT, &sX, &sY);
				mprintf(sX, pers_stat_y[i - 1], sString);
				break;
		}
	}
}


/* AIM merc:  Returns the amount of time left on mercs contract
 * MERC merc: Returns the amount of time the merc has worked
 * IMP merc:  Returns the amount of time the merc has worked
 * else:      returns -1 */
static INT32 CalcTimeLeftOnMercContract(const SOLDIERTYPE* pSoldier)
{
	INT32 iTimeLeftOnContract = -1;

	if (pSoldier->ubWhatKindOfMercAmI == MERC_TYPE__AIM_MERC)
	{
		iTimeLeftOnContract = pSoldier->iEndofContractTime-GetWorldTotalMin();

		if (iTimeLeftOnContract < 0)
			iTimeLeftOnContract = 0;
	}
	else if (pSoldier->ubWhatKindOfMercAmI == MERC_TYPE__MERC)
	{
		iTimeLeftOnContract = gMercProfiles[pSoldier->ubProfile].iMercMercContractLength;
	}

	else if (pSoldier->ubWhatKindOfMercAmI == MERC_TYPE__PLAYER_CHARACTER)
	{
		iTimeLeftOnContract = pSoldier->iTotalContractLength;
	}

	else
	{
		iTimeLeftOnContract = -1;
	}

	return iTimeLeftOnContract;
}
