#include "BuildDefines.h"

#ifdef JA2EDITOR

#include "HImage.h"
#include "LoadSaveSoldierCreate.h"
#include "Local.h"
#include "Font.h"
#include "Overhead_Map.h"
#include "Types.h"
#include "Sector_Summary.h"
#include "Timer_Control.h"
#include "VObject.h"
#include "VSurface.h"
#include "Button_System.h"
#include "Font_Control.h"
#include "Simple_Render_Utils.h"
#include "Editor_Taskbar_Utils.h"
#include "Line.h"
#include "Input.h"
#include "LoadScreen.h"
#include "Text_Input.h"
#include "MouseSystem.h"
#include "StrategicMap.h"
#include "FileMan.h"
#include "Exit_Grids.h"
#include "Map_Information.h"
#include "Summary_Info.h"
#include "Animated_ProgressBar.h"
#include "WorldDef.h"
#include "WorldDat.h"
#include "EditorDefines.h"
#include "EditScreen.h"
#include "English.h"
#include "World_Items.h"
#include "Text.h"
#include "Debug.h"
#include "MemMan.h"
#include "Soldier_Create.h"
#include "Video.h"
#include "Stubs.h" // XXX


#define MAP_SIZE			208
#define MAP_LEFT			417
#define MAP_TOP				15
#define MAP_RIGHT			(MAP_LEFT+MAP_SIZE)
#define MAP_BOTTOM		(MAP_TOP+MAP_SIZE)

enum{
	PRE_ALPHA,
	ALPHA,
	DEMO,
	BETA,
	RELEASE
};
static const wchar_t* const gszVersionType[5] = { L"Pre-Alpha", L"Alpha", L"Demo", L"Beta", L"Release" };
#define GLOBAL_SUMMARY_STATE			RELEASE

//Regular masks
#define GROUND_LEVEL_MASK			0x01
#define BASEMENT1_LEVEL_MASK	0x02
#define BASEMENT2_LEVEL_MASK	0x04
#define BASEMENT3_LEVEL_MASK	0x08
#define ALL_LEVELS_MASK				0x0f
//Alternate masks
#define ALTERNATE_GROUND_MASK 0x10
#define ALTERNATE_B1_MASK			0x20
#define ALTERNATE_B2_MASK			0x40
#define ALTERNATE_B3_MASK			0x80
#define ALTERNATE_LEVELS_MASK	0xf0

INT32 giCurrLevel;

BOOLEAN gfOutdatedDenied;
UINT16 gusNumEntriesWithOutdatedOrNoSummaryInfo;

BOOLEAN gfUpdatingNow;
UINT16 gusTotal, gusCurrent;

BOOLEAN gfMustForceUpdateAllMaps = FALSE;
UINT16 gusNumberOfMapsToBeForceUpdated = 0;
BOOLEAN gfMajorUpdate = FALSE;

INT32 giCurrentViewLevel = ALL_LEVELS_MASK;

BOOLEAN gbSectorLevels[16][16];
BOOLEAN gfGlobalSummaryLoaded = FALSE;

SUMMARYFILE *gpSectorSummary[16][16][8];
SUMMARYFILE *gpCurrentSectorSummary;

MOUSE_REGION MapRegion;

extern INT8 gbMercSlotTypes[9];

extern void UpdateSummaryInfo( SUMMARYFILE *pSummary );

//Set if there is an existing global summary.  The first time this is run on your computer, it
//will not exist, and will have to be generated before this will be set.
BOOLEAN gfGlobalSummaryExists;
//If you don't wish to create a global summary, you can deny it.  This safely locks the system
//from generating one.
BOOLEAN gfDeniedSummaryCreation;
//Set whenever the entire display is to be marked dirty.
BOOLEAN gfRenderSummary;
//Used externally to determine if the summary window is up or not.
BOOLEAN gfSummaryWindowActive;
//When set, the summary window stays up until told otherwise.  When clear, the summary will disappear
//when the assigned key (F5) is released.  The latter mode is initiated when F5 is held down for longer
//than .4 seconds, and is useful for quickly looking at the information in the current map being edited.
BOOLEAN gfPersistantSummary;
//When set, a grid is overlayed on top of the sector.  This grid defines each of the 256 sectors.  It is
//on by default.
BOOLEAN gfRenderGrid;
//When set, parts of the map are darkened, showing that those sectors don't exist in the currently selected
//layer.  When clear, the entire map is shown in full color.
BOOLEAN gfRenderProgress;
//When set, only the map section is rerendered.
BOOLEAN gfRenderMap;
//When set, then we are overriding the ability to use normal methods for selecting sectors for saving and
//loading.  Immediately upon entering the text input mode; for the temp file; then we are assuming that
//the user will type in a name that doesn't follow standard naming conventions for the purposes of the
//campaign editor.  This is useful for naming a temp file for saving or loading.
BOOLEAN gfTempFile;
//When set, only the alternate version of the maps will be displayed.  This is used for alternate maps in
//particular sectors, such as the SkyRider quest which could be located at one of four maps randomly.  If
//that particular map is chosen, then the alternate map will be used.
BOOLEAN gfAlternateMaps = FALSE;


enum
{
	ITEMMODE_SCIFI,
	ITEMMODE_REAL,
	ITEMMODE_ENEMY,
};
UINT8 gubSummaryItemMode = ITEMMODE_SCIFI;

BOOLEAN gfItemDetailsMode = FALSE;

WORLDITEM *gpWorldItemsSummaryArray = NULL;
UINT16 gusWorldItemsSummaryArraySize = 0;
OBJECTTYPE *gpPEnemyItemsSummaryArray = NULL;
UINT16 gusPEnemyItemsSummaryArraySize = 0;
OBJECTTYPE *gpNEnemyItemsSummaryArray = NULL;
UINT16 gusNEnemyItemsSummaryArraySize = 0;

BOOLEAN gfSetupItemDetailsMode = TRUE;

BOOLEAN gfUpdateSummaryInfo;

UINT16 usNumSummaryFilesOutOfDate;

BOOLEAN gfMapFileDirty;

//Override status.  Hide is when there is nothing to override, readonly, when checked is to override a
//readonly status file, so that you can write to it, and overwrite, when checked, allows you to save,
//replacing the existing file.  These states are not persistant, which forces the user to check the
//box before saving.
enum{ INACTIVE, READONLY, OVERWRITE };
UINT8 gubOverrideStatus;
//Set when the a new sector/level is selected, forcing the user to reselect the override status.
BOOLEAN gfOverrideDirty;
//The state of the override button, true if overriden intended.
BOOLEAN gfOverride;

//The sector coordinates of the map currently loaded in memory (blue)
INT16 gsSectorX, gsSectorY;
//The layer of the sector that is currently loaded in memory.
INT32 gsSectorLayer;
//The sector coordinates of the mouse position (yellow)
INT16 gsHiSectorX, gsHiSectorY;
//The sector coordinates of the selected sector (red)
INT16 gsSelSectorX, gsSelSectorY;

//Used to determine how long the F5 key has been held down for to determine whether or not the
//summary is going to be persistant or not.
UINT32 giInitTimer;

static wchar_t gszFilename[40];
static wchar_t gszTempFilename[21];
static wchar_t gszDisplayName[21];

enum{
	SUMMARY_BACKGROUND,
	SUMMARY_OKAY,
	SUMMARY_GRIDCHECKBOX,
	SUMMARY_PROGRESSCHECKBOX,
	SUMMARY_ALL,
	SUMMARY_G,
	SUMMARY_B1,
	SUMMARY_B2,
	SUMMARY_B3,
	SUMMARY_ALTERNATE,
	SUMMARY_LOAD,
	SUMMARY_SAVE,
	SUMMARY_OVERRIDE,
	SUMMARY_UPDATE,
	SUMMARY_SCIFI,
	SUMMARY_REAL,
	SUMMARY_ENEMY,
	NUM_SUMMARY_BUTTONS
};
INT32 iSummaryButton[ NUM_SUMMARY_BUTTONS ];


static void LoadGlobalSummary(void);
static void MapClickCallback(MOUSE_REGION* reg, INT32 reason);
static void MapMoveCallback(MOUSE_REGION* reg, INT32 reason);
static void ReleaseSummaryWindow(void);
static void SummaryEnemyCallback(GUI_BUTTON* btn, INT32 reason);
static void SummaryLoadMapCallback(GUI_BUTTON* btn, INT32 reason);
static void SummaryOkayCallback(GUI_BUTTON* btn, INT32 reason);
static void SummaryOverrideCallback(GUI_BUTTON* btn, INT32 reason);
static void SummaryRealCallback(GUI_BUTTON* btn, INT32 reason);
static void SummarySaveMapCallback(GUI_BUTTON* btn, INT32 reason);
static void SummarySciFiCallback(GUI_BUTTON* btn, INT32 reason);
static void SummaryToggleAlternateCallback(GUI_BUTTON* btn, INT32 reason);
static void SummaryToggleGridCallback(GUI_BUTTON* btn, INT32 reason);
static void SummaryToggleLevelCallback(GUI_BUTTON* btn, INT32 reason);
static void SummaryToggleProgressCallback(GUI_BUTTON* btn, INT32 reason);
static void SummaryUpdateCallback(GUI_BUTTON* btn, INT32 reason);


void CreateSummaryWindow()
{
	INT32 i;

	if( !gfGlobalSummaryLoaded )
	{
		LoadGlobalSummary();
		gfGlobalSummaryLoaded = TRUE;
	}

	if( gfSummaryWindowActive )
		return;

	DisableEditorTaskbar();
	DisableAllTextFields();

	GetCurrentWorldSector( &gsSectorX, &gsSectorY );
	gsSelSectorX = gsSectorX;
	gsSelSectorY = gsSectorY;
	gfSummaryWindowActive = TRUE;
	gfPersistantSummary = FALSE;
	giInitTimer = GetJA2Clock();
	gfDeniedSummaryCreation = FALSE;
	gfRenderSummary = TRUE;
	if( gfWorldLoaded )
		gfMapFileDirty = TRUE;
	//Create all of the buttons here
	iSummaryButton[SUMMARY_BACKGROUND] = CreateLabel(NULL, 0, 0, 0, 0, 0, SCREEN_WIDTH, 360, MSYS_PRIORITY_HIGH - 1);

	iSummaryButton[SUMMARY_OKAY] = CreateTextButton(L"Okay", FONT12POINT1, FONT_BLACK, FONT_BLACK, 585, 325, 50, 30, MSYS_PRIORITY_HIGH, SummaryOkayCallback);

	iSummaryButton[ SUMMARY_GRIDCHECKBOX ] =
		CreateCheckBoxButton(	MAP_LEFT, ( INT16 ) ( MAP_BOTTOM + 5 ), "EDITOR/smcheckbox.sti", MSYS_PRIORITY_HIGH, SummaryToggleGridCallback );
	ButtonList[ iSummaryButton[ SUMMARY_GRIDCHECKBOX ] ]->uiFlags |= BUTTON_CLICKED_ON;
	gfRenderGrid = TRUE;

	iSummaryButton[ SUMMARY_PROGRESSCHECKBOX ] =
		CreateCheckBoxButton(	( INT16 ) ( MAP_LEFT + 50 ), ( INT16 ) ( MAP_BOTTOM + 5 ), "EDITOR/smcheckbox.sti", MSYS_PRIORITY_HIGH, SummaryToggleProgressCallback );
	ButtonList[ iSummaryButton[ SUMMARY_PROGRESSCHECKBOX ] ]->uiFlags |= BUTTON_CLICKED_ON;
	gfRenderProgress = TRUE;

	iSummaryButton[SUMMARY_ALL] = CreateTextButton(L"A",  SMALLCOMPFONT, FONT_BLACK, FONT_BLACK, MAP_LEFT + 110, MAP_BOTTOM + 5, 16, 16, MSYS_PRIORITY_HIGH, SummaryToggleLevelCallback);
	if( giCurrentViewLevel == ALL_LEVELS_MASK || giCurrentViewLevel == ALTERNATE_LEVELS_MASK )
		ButtonList[ iSummaryButton[ SUMMARY_ALL ] ]->uiFlags |= BUTTON_CLICKED_ON;
	iSummaryButton[SUMMARY_G]   = CreateTextButton(L"G",  SMALLCOMPFONT, FONT_BLACK, FONT_BLACK, MAP_LEFT + 128, MAP_BOTTOM + 5, 16, 16, MSYS_PRIORITY_HIGH, SummaryToggleLevelCallback);
	if( giCurrentViewLevel == GROUND_LEVEL_MASK || giCurrentViewLevel == ALTERNATE_GROUND_MASK )
		ButtonList[ iSummaryButton[ SUMMARY_G ] ]->uiFlags |= BUTTON_CLICKED_ON;
	iSummaryButton[SUMMARY_B1]  = CreateTextButton(L"B1", SMALLCOMPFONT, FONT_BLACK, FONT_BLACK, MAP_LEFT + 146, MAP_BOTTOM + 5, 16, 16, MSYS_PRIORITY_HIGH, SummaryToggleLevelCallback);
	if( giCurrentViewLevel == BASEMENT1_LEVEL_MASK || giCurrentViewLevel == ALTERNATE_B1_MASK )
		ButtonList[ iSummaryButton[ SUMMARY_B1 ] ]->uiFlags |= BUTTON_CLICKED_ON;
	iSummaryButton[SUMMARY_B2]  = CreateTextButton(L"B2", SMALLCOMPFONT, FONT_BLACK, FONT_BLACK, MAP_LEFT + 164, MAP_BOTTOM + 5, 16, 16, MSYS_PRIORITY_HIGH, SummaryToggleLevelCallback);
	if( giCurrentViewLevel == BASEMENT2_LEVEL_MASK || giCurrentViewLevel == ALTERNATE_B2_MASK )
		ButtonList[ iSummaryButton[ SUMMARY_B2 ] ]->uiFlags |= BUTTON_CLICKED_ON;
	iSummaryButton[SUMMARY_B3]  = CreateTextButton(L"B3", SMALLCOMPFONT, FONT_BLACK, FONT_BLACK, MAP_LEFT + 182, MAP_BOTTOM + 5, 16, 16, MSYS_PRIORITY_HIGH, SummaryToggleLevelCallback);
	if( giCurrentViewLevel == BASEMENT3_LEVEL_MASK || giCurrentViewLevel == ALTERNATE_B3_MASK )
		ButtonList[ iSummaryButton[ SUMMARY_B3 ] ]->uiFlags |= BUTTON_CLICKED_ON;

	iSummaryButton[ SUMMARY_ALTERNATE ] =
		CreateCheckBoxButton(	MAP_LEFT, ( INT16 ) ( MAP_BOTTOM + 25 ), "EDITOR/smcheckbox.sti", MSYS_PRIORITY_HIGH, SummaryToggleAlternateCallback );
	if( gfAlternateMaps )
		ButtonList[ iSummaryButton[ SUMMARY_ALTERNATE ] ]->uiFlags |= BUTTON_CLICKED_ON;

	iSummaryButton[SUMMARY_LOAD] = CreateTextButton(L"LOAD", FONT12POINT1, FONT_BLACK, FONT_BLACK, MAP_LEFT,      MAP_BOTTOM + 45, 50, 26, MSYS_PRIORITY_HIGH, SummaryLoadMapCallback);
	iSummaryButton[SUMMARY_SAVE] = CreateTextButton(L"SAVE", FONT12POINT1, FONT_BLACK, FONT_BLACK, MAP_LEFT + 55, MAP_BOTTOM + 45, 50, 26, MSYS_PRIORITY_HIGH, SummarySaveMapCallback);
	iSummaryButton[ SUMMARY_OVERRIDE ] =
		CreateCheckBoxButton( ( INT16 ) ( MAP_LEFT + 110 ), ( INT16 ) ( MAP_BOTTOM + 59 ), "EDITOR/smcheckbox.sti", MSYS_PRIORITY_HIGH, SummaryOverrideCallback );


	iSummaryButton[SUMMARY_UPDATE] = CreateTextButton(L"Update", FONT12POINT1, FONT_BLACK, FONT_BLACK, 255, 15, 40, 16, MSYS_PRIORITY_HIGH, SummaryUpdateCallback);

	iSummaryButton[ SUMMARY_REAL ] =
		CreateCheckBoxButton( 350, 47, "EDITOR/radiobutton.sti", MSYS_PRIORITY_HIGH, SummaryRealCallback );
	iSummaryButton[ SUMMARY_SCIFI ] =
		CreateCheckBoxButton( 376, 47, "EDITOR/radiobutton.sti", MSYS_PRIORITY_HIGH, SummarySciFiCallback );
	iSummaryButton[ SUMMARY_ENEMY ] =
		CreateCheckBoxButton( 350, 60, "EDITOR/radiobutton.sti", MSYS_PRIORITY_HIGH, SummaryEnemyCallback );

	//SetButtonFastHelpText( iSummaryButton[ SUMMARY_SCIFI ], L"Display items that appear in SciFi mode." );
	//SetButtonFastHelpText( iSummaryButton[ SUMMARY_REAL ], L"Display items that appear in Realistic mode." );
	switch( gubSummaryItemMode )
	{
		case ITEMMODE_SCIFI:
			ButtonList[ iSummaryButton[ SUMMARY_SCIFI ] ]->uiFlags |= BUTTON_CLICKED_ON;
			break;
		case ITEMMODE_REAL:
			ButtonList[ iSummaryButton[ SUMMARY_REAL ] ]->uiFlags |= BUTTON_CLICKED_ON;
			break;
		case ITEMMODE_ENEMY:
			ButtonList[ iSummaryButton[ SUMMARY_ENEMY ] ]->uiFlags |= BUTTON_CLICKED_ON;
			break;
	}

	//Init the textinput field.
	InitTextInputModeWithScheme( DEFAULT_SCHEME );
	AddUserInputField( NULL );  //just so we can use short cut keys while not typing.
	AddTextInputField( MAP_LEFT+112, MAP_BOTTOM+75, 100, 18, MSYS_PRIORITY_HIGH, L"", 20, INPUTTYPE_EXCLUSIVE_DOSFILENAME );

	for( i = 1; i < NUM_SUMMARY_BUTTONS; i++ )
		HideButton( iSummaryButton[ i ] );

	MSYS_DefineRegion( &MapRegion, MAP_LEFT, MAP_TOP, MAP_RIGHT, MAP_BOTTOM, MSYS_PRIORITY_HIGH, 0,
		MapMoveCallback, MapClickCallback );
	MSYS_DisableRegion( &MapRegion );

	//if( gfItemDetailsMode )
//	{
		gfItemDetailsMode = FALSE;
	//	gfSetupItemDetailsMode = TRUE;
	//}
	if( !gfWorldLoaded )
	{
		gfConfirmExitFirst = FALSE;
		ReleaseSummaryWindow();
		DisableButton( iSummaryButton[ SUMMARY_OKAY ] );
		DisableButton( iSummaryButton[ SUMMARY_SAVE ] );
	}
	if( gfAutoLoadA9 )
	{
		gfAutoLoadA9++;
		gsSelSectorX = 9;
		gsSelSectorY = 1;
		gpCurrentSectorSummary = gpSectorSummary[ 8 ][ 0 ][ 0 ];
		ButtonList[ iSummaryButton[ SUMMARY_LOAD ] ]->uiFlags |= BUTTON_CLICKED_ON;
	}
}

void AutoLoadMap()
{
	SummaryLoadMapCallback( ButtonList[ iSummaryButton[ SUMMARY_LOAD ] ], MSYS_CALLBACK_REASON_LBUTTON_UP );
	if( gfWorldLoaded )
		DestroySummaryWindow();
	gfAutoLoadA9 = FALSE;
	gfConfirmExitFirst = TRUE;
}


static void ReleaseSummaryWindow(void)
{
	INT32 i;
	UINT32 uiCurrTimer;
	if( !gfSummaryWindowActive || gfPersistantSummary )
		return;
	uiCurrTimer = GetJA2Clock();
	if( !gfWorldLoaded || uiCurrTimer - giInitTimer < 400 )
	{ //make window persistant
		for( i = 1; i < NUM_SUMMARY_BUTTONS; i++ )
			ShowButton( iSummaryButton[ i ] );
		HideButton( iSummaryButton[ SUMMARY_UPDATE ] );
		HideButton( iSummaryButton[ SUMMARY_OVERRIDE ] );
		HideButton( iSummaryButton[ SUMMARY_REAL ] );
		HideButton( iSummaryButton[ SUMMARY_SCIFI ] );
		HideButton( iSummaryButton[ SUMMARY_ENEMY ] );
		MSYS_EnableRegion( &MapRegion );
		gfPersistantSummary = TRUE;
		gfOverrideDirty = TRUE;
		gfRenderSummary = TRUE;
	}
	else
	{
		DestroySummaryWindow();
	}
}

void DestroySummaryWindow()
{
	INT32 i;
	if( !gfSummaryWindowActive )
		return;
	for( i = 0; i < NUM_SUMMARY_BUTTONS; i++ )
	{
		RemoveButton( iSummaryButton[ i ] );
	}

	MSYS_RemoveRegion( &MapRegion );

	gfSummaryWindowActive = FALSE;
	gfPersistantSummary = FALSE;
	MarkWorldDirty();
	KillTextInputMode();
	EnableEditorTaskbar();
	EnableAllTextFields();

	if( gpWorldItemsSummaryArray )
	{
		MemFree( gpWorldItemsSummaryArray );
		gpWorldItemsSummaryArray = NULL;
		gusWorldItemsSummaryArraySize = 0;
	}
	if( gpPEnemyItemsSummaryArray )
	{
		MemFree( gpPEnemyItemsSummaryArray );
		gpPEnemyItemsSummaryArray = NULL;
		gusPEnemyItemsSummaryArraySize = 0;
	}
	if( gpNEnemyItemsSummaryArray )
	{
		MemFree( gpNEnemyItemsSummaryArray );
		gpNEnemyItemsSummaryArray = NULL;
		gusNEnemyItemsSummaryArraySize = 0;
	}
	if( gfWorldLoaded )
	{
		gfConfirmExitFirst = TRUE;
	}
}


static void RenderSectorInformation(void)
{
	//UINT16 str[ 100 ];
	MAPCREATE_STRUCT *m;
	SUMMARYFILE *s;
	UINT8 ePoints = 0;
	UINT16 usLine = 35;
	INT32 iOverall;

	SetFont( FONT10ARIAL );
	SetFontShadow( FONT_NEARBLACK );

	s = gpCurrentSectorSummary;
	m = &gpCurrentSectorSummary->MapInfo;

	if( m->sNorthGridNo != -1 )
		ePoints++;
	if( m->sEastGridNo != -1 )
		ePoints++;
	if( m->sSouthGridNo != -1 )
		ePoints++;
	if( m->sWestGridNo != -1 )
		ePoints++;
	if( m->sCenterGridNo != -1 )
		ePoints++;
	if( m->sIsolatedGridNo != -1 )
		ePoints++;
	//start at 10,35
	SetFontForeground( FONT_ORANGE );
	mprintf(10, 32, L"Tileset:  %ls", gTilesets[s->ubTilesetID].zName);
	if( m->ubMapVersion < 10 )
		SetFontForeground( FONT_RED );
	mprintf( 10, 42,    L"Version Info:  Summary:  1.%02d,  Map:  %d.%02d", s->ubSummaryVersion, (INT32)s->dMajorMapVersion, m->ubMapVersion );
	SetFontForeground( FONT_GRAY2 );
	mprintf( 10, 55,		L"Number of items:  %d", s->usNumItems );
	mprintf( 10, 65,		L"Number of lights:  %d", s->usNumLights );
	mprintf( 10, 75,		L"Number of entry points:  %d", ePoints );
	if( ePoints )
	{
		INT32 x;
		x = 140;
		mprintf( x, 75, L"(" );
		x += StringPixLength( L"(", FONT10ARIAL ) + 2;
		if( m->sNorthGridNo			!= -1	)	{	mprintf( x, 75, L"N" );	x += StringPixLength( L"N", FONT10ARIAL ) + 2; }
		if( m->sEastGridNo			!= -1	)	{	mprintf( x, 75, L"E" );	x += StringPixLength( L"E", FONT10ARIAL ) + 2; }
		if( m->sSouthGridNo			!= -1	)	{	mprintf( x, 75, L"S" );	x += StringPixLength( L"S", FONT10ARIAL ) + 2; }
		if( m->sWestGridNo			!= -1	)	{	mprintf( x, 75, L"W" );	x += StringPixLength( L"W", FONT10ARIAL ) + 2; }
		if( m->sCenterGridNo		!= -1	)	{	mprintf( x, 75, L"C" );	x += StringPixLength( L"C", FONT10ARIAL ) + 2; }
		if( m->sIsolatedGridNo	!= -1	)	{	mprintf( x, 75, L"I" );	x += StringPixLength( L"I", FONT10ARIAL ) + 2; }
		mprintf( x, 75, L")" );
	}
	mprintf( 10, 85,		L"Number of rooms:  %d", s->ubNumRooms );
	mprintf( 10, 95,		L"Total map population:  %d", m->ubNumIndividuals );
	mprintf( 20, 105,			L"Enemies:  %d", s->EnemyTeam.ubTotal );
	mprintf( 30, 115,			L"Admins:  %d", s->ubNumAdmins );
	if( s->ubNumAdmins )
		mprintf( 100, 115,			L"(%d detailed, %d profile -- %d have priority existance)", s->ubAdminDetailed, s->ubAdminProfile, s->ubAdminExistance );
	mprintf( 30, 125,			L"Troops:  %d", s->ubNumTroops );
	if( s->ubNumTroops )
		mprintf( 100, 125,			L"(%d detailed, %d profile -- %d have priority existance)", s->ubTroopDetailed, s->ubTroopProfile, s->ubTroopExistance );
	mprintf( 30, 135,			L"Elites:  %d", s->ubNumElites );
	if( s->ubNumElites )
		mprintf( 100, 135,			L"(%d detailed, %d profile -- %d have priority existance)", s->ubEliteDetailed, s->ubEliteProfile, s->ubEliteExistance );
	mprintf( 20, 145,			L"Civilians:  %d", s->CivTeam.ubTotal );
	if( s->CivTeam.ubTotal )
		mprintf( 100, 145,			L"(%d detailed, %d profile -- %d have priority existance)", s->CivTeam.ubDetailed, s->CivTeam.ubProfile, s->CivTeam.ubExistance );
	if( s->ubSummaryVersion >= 9 )
	{
		mprintf( 30, 155,		  L"Humans:  %d", s->CivTeam.ubTotal - s->ubCivCows - s->ubCivBloodcats );
		mprintf( 30, 165,			L"Cows:  %d", s->ubCivCows );
		mprintf( 30, 175,			L"Bloodcats:  %d", s->ubCivBloodcats );
	}
	mprintf( 20, 185,			L"Creatures:  %d", s->CreatureTeam.ubTotal );
	if( s->ubSummaryVersion >= 9 )
	{
		mprintf( 30, 195,     L"Monsters:  %d", s->CreatureTeam.ubTotal - s->CreatureTeam.ubNumAnimals );
		mprintf( 30, 205,     L"Bloodcats:  %d", s->CreatureTeam.ubNumAnimals );
	}
	mprintf( 10, 215,		L"Number of locked and/or trapped doors:  %d", s->ubNumDoors );
	mprintf( 20, 225,			L"Locked:  %d", s->ubNumDoorsLocked );
	mprintf( 20, 235,			L"Trapped:  %d", s->ubNumDoorsTrapped );
	mprintf( 20, 245,			L"Locked & Trapped:  %d", s->ubNumDoorsLockedAndTrapped );
	if( s->ubSummaryVersion >= 8 )
		mprintf( 10, 255,			L"Civilians with schedules:  %d", s->ubCivSchedules );
	if( s->ubSummaryVersion >= 10 )
	{
		if( s->fTooManyExitGridDests )
		{
			SetFontForeground( FONT_RED );
			mprintf( 10, 265, L"Too many exit grid destinations (more than 4)...");
		}
		else
		{
			UINT8 i;
			UINT8 ubNumInvalid = 0;
			for( i = 0; i < 4; i++ )
			{
				if( s->fInvalidDest[i] )
					ubNumInvalid++;
			}
			if( ubNumInvalid )
			{
				SetFontForeground( FONT_RED );
				mprintf( 10, 265, L"ExitGrids:  %d (%d with a long distance destination)", s->ubNumExitGridDests, ubNumInvalid );
			}
			else switch( s->ubNumExitGridDests )
			{
				case 0:
					mprintf( 10, 265, L"ExitGrids:  none" );
					break;
				case 1:
					mprintf( 10, 265, L"ExitGrids:  1 destination using %d exitgrids", s->usExitGridSize[0] );
					break;
				case 2:
					mprintf( 10, 265, L"ExitGrids:  2 -- 1) Qty: %d, 2) Qty: %d", s->usExitGridSize[0], s->usExitGridSize[1] );
					break;
				case 3:
					mprintf( 10, 265, L"ExitGrids:  3 -- 1) Qty: %d, 2) Qty: %d, 3) Qty: %d",
						s->usExitGridSize[0], s->usExitGridSize[1], s->usExitGridSize[2] );
					break;
				case 4:
					mprintf( 10, 265, L"ExitGrids:  3 -- 1) Qty: %d, 2) Qty: %d, 3) Qty: %d, 4) Qty: %d",
						s->usExitGridSize[0], s->usExitGridSize[1], s->usExitGridSize[2], s->usExitGridSize[3] );
					break;
			}
		}
	}
	iOverall = - ( 2 * s->EnemyTeam.ubBadA ) - s->EnemyTeam.ubPoorA + s->EnemyTeam.ubGoodA + ( 2 * s->EnemyTeam.ubGreatA );
	usLine = 275;
	mprintf( 10, usLine, L"Enemy Relative Attributes:  %d bad, %d poor, %d norm, %d good, %d great (%+d Overall)",
		s->EnemyTeam.ubBadA,
		s->EnemyTeam.ubPoorA,
		s->EnemyTeam.ubAvgA,
		s->EnemyTeam.ubGoodA,
		s->EnemyTeam.ubGreatA,
		iOverall );
	iOverall = - ( 2 * s->EnemyTeam.ubBadE ) - s->EnemyTeam.ubPoorE + s->EnemyTeam.ubGoodE + ( 2 * s->EnemyTeam.ubGreatE );
	usLine += 10;
	mprintf( 10, usLine, L"Enemy Relative Equipment:  %d bad, %d poor, %d norm, %d good, %d great (%+d Overall)",
		s->EnemyTeam.ubBadE,
		s->EnemyTeam.ubPoorE,
		s->EnemyTeam.ubAvgE,
		s->EnemyTeam.ubGoodE,
		s->EnemyTeam.ubGreatE,
		iOverall );
	usLine += 10;
	if( s->ubSummaryVersion >= 11 )
	{
		if( s->ubEnemiesReqWaypoints )
		{
			SetFontForeground( FONT_RED );
			mprintf( 10, usLine, L"%d placements have patrol orders without any waypoints defined.", s->ubEnemiesReqWaypoints );
			usLine += 10;
		}
	}
	if( s->ubSummaryVersion >= 13 )
	{
		if( s->ubEnemiesHaveWaypoints )
		{
			SetFontForeground( FONT_RED );
			mprintf( 10, usLine, L"%d placements have waypoints, but without any patrol orders.", s->ubEnemiesHaveWaypoints );
			usLine += 10;
		}
	}
	if( s->ubSummaryVersion >= 12 )
	{
		if( s->usWarningRoomNums )
		{
			SetFontForeground( FONT_RED );
			mprintf( 10, usLine, L"%d gridnos have questionable room numbers.  Please validate.", s->usWarningRoomNums );
		}
	}
}


//2)  CODE TRIGGER/ACTION NAMES
static void RenderItemDetails(void)
{
	FLOAT dAvgExistChance, dAvgStatus;
	OBJECTTYPE *pItem;
	INT32 index, i;
	UINT32 uiQuantity, uiExistChance, uiStatus;
	UINT32 uiTriggerQuantity[8], uiActionQuantity[8], uiTriggerExistChance[8], uiActionExistChance[8];
	UINT32 xp, yp;
	INT8 bFreqIndex;
	SetFont( FONT10ARIAL );
	SetFontForeground( FONT_GRAY2 );
	SetFontShadow( FONT_NEARBLACK );
	mprintf( 364, 49, L"R" );
	mprintf( 390, 49, L"S" );
	mprintf( 364, 62, L"Enemy" );
	yp = 20;
	xp = 5;
	if( gubSummaryItemMode != ITEMMODE_ENEMY && gpWorldItemsSummaryArray )
	{
		memset( uiTriggerQuantity, 0, 32 );
		memset( uiActionQuantity, 0, 32 );
		memset( uiTriggerExistChance, 0, 32 );
		memset( uiActionExistChance, 0, 32 );
		for( index = 1; index < MAXITEMS; index++ )
		{
			uiQuantity = 0;
			uiExistChance = 0;
			uiStatus = 0;
			for( i = 0; i < gusWorldItemsSummaryArraySize; i++ )
			{
				if( index == SWITCH || index == ACTION_ITEM )
				{
					if( gpWorldItemsSummaryArray[ i ].o.usItem == index )
					{
						if( gubSummaryItemMode == ITEMMODE_SCIFI && !(gpWorldItemsSummaryArray[ i ].usFlags & WORLD_ITEM_REALISTIC_ONLY) ||
								gubSummaryItemMode == ITEMMODE_REAL && !(gpWorldItemsSummaryArray[ i ].usFlags & WORLD_ITEM_SCIFI_ONLY) )
						{
							pItem = &gpWorldItemsSummaryArray[ i ].o;
							if( !pItem->bFrequency )
								bFreqIndex = 7;
							else if( pItem->bFrequency == PANIC_FREQUENCY )
								bFreqIndex = 0;
							else if( pItem->bFrequency == PANIC_FREQUENCY_2 )
								bFreqIndex = 1;
							else if( pItem->bFrequency == PANIC_FREQUENCY_3 )
								bFreqIndex = 2;
							else if( pItem->bFrequency == FIRST_MAP_PLACED_FREQUENCY + 1 )
								bFreqIndex = 3;
							else if( pItem->bFrequency == FIRST_MAP_PLACED_FREQUENCY + 2 )
								bFreqIndex = 4;
							else if( pItem->bFrequency == FIRST_MAP_PLACED_FREQUENCY + 3 )
								bFreqIndex = 5;
							else if( pItem->bFrequency == FIRST_MAP_PLACED_FREQUENCY + 4 )
								bFreqIndex = 6;
							else
								continue;
							if( index == SWITCH )
							{
								uiTriggerQuantity[ bFreqIndex ]++;
								uiTriggerExistChance[ bFreqIndex ] += 100 - gpWorldItemsSummaryArray[ i ].ubNonExistChance;
							}
							else
							{
								uiActionQuantity[ bFreqIndex ]++;
								uiActionExistChance[ bFreqIndex ] += 100 - gpWorldItemsSummaryArray[ i ].ubNonExistChance;
							}
						}
					}
					continue;
				}
				if( gpWorldItemsSummaryArray[ i ].o.usItem == index )
				{
					if( gubSummaryItemMode == ITEMMODE_SCIFI && !(gpWorldItemsSummaryArray[ i ].usFlags & WORLD_ITEM_REALISTIC_ONLY) ||
						  gubSummaryItemMode == ITEMMODE_REAL  && !(gpWorldItemsSummaryArray[ i ].usFlags & WORLD_ITEM_SCIFI_ONLY) )
					{
						pItem = &gpWorldItemsSummaryArray[ i ].o;
						uiExistChance += (100 - gpWorldItemsSummaryArray[ i ].ubNonExistChance) * pItem->ubNumberOfObjects;
						uiStatus += pItem->bStatus[0];
						uiQuantity += pItem->ubNumberOfObjects;
					}
				}
			}
			if( uiQuantity )
			{
				if( !(yp % 20) )
					SetFontForeground( FONT_LTKHAKI );
				else
					SetFontForeground( FONT_GRAY2 );
				//calc averages
				dAvgExistChance = (FLOAT)(uiExistChance / 100.0);
				dAvgStatus = uiStatus / (FLOAT)uiQuantity;
				//Display stats.
				mprintf(xp, yp, L"%ls", ShortItemNames[index]);
				mprintf( xp + 85, yp, L"%3.02f", dAvgExistChance );
				mprintf( xp + 110, yp, L"@ %3.02f%%", dAvgStatus );
				yp += 10;
				if( yp >= 355 )
				{
					xp += 170;
					yp = 20;
					if( xp >= 300 )
					{
						SetFontForeground( FONT_RED );
						mprintf( 350, 350, L"TOO MANY ITEMS TO DISPLAY!");
						return;
					}
				}
			}
		}
		//Now list the number of actions/triggers of each type
		for( i = 0; i < 8; i++ )
		{
			if( uiTriggerQuantity[i] || uiActionQuantity[i] )
			{
				if( i == 7 )
					SetFontForeground( FONT_DKYELLOW );
				else if( !uiTriggerQuantity[i] || !uiActionQuantity[i] )
					SetFontForeground( FONT_RED );
				else
					SetFontForeground( 77 );
				const wchar_t* Type;
				switch( i )
				{
					case 0: Type = L"Panic1";           break;
					case 1:	Type = L"Panic2";           break;
					case 2:	Type = L"Panic3";           break;
					case 3:	Type = L"Norm1";            break;
					case 4:	Type = L"Norm2";            break;
					case 5:	Type = L"Norm3";            break;
					case 6:	Type = L"Norm4";            break;
					case 7:	Type = L"Pressure Actions"; break;
				}
				if( i < 7 )
				{
					dAvgExistChance = (FLOAT)(uiTriggerExistChance[i] / 100.0);
					dAvgStatus = (FLOAT)(uiActionExistChance[i] / 100.0);
					mprintf(xp, yp, L"%ls:  %3.02f trigger(s), %3.02f action(s)", Type, dAvgExistChance, dAvgStatus);
				}
				else
				{
					dAvgExistChance = (FLOAT)(uiActionExistChance[i] / 100.0);
					mprintf(xp, yp, L"%ls:  %3.02f", Type, dAvgExistChance);
				}
				yp += 10;
				if( yp >= 355 )
				{
					xp += 170;
					yp = 20;
					if( xp >= 300 )
					{
						SetFontForeground( FONT_RED );
						mprintf( 350, 350, L"TOO MANY ITEMS TO DISPLAY!");
						return;
					}
				}
			}
		}
	}
	else if( gubSummaryItemMode == ITEMMODE_ENEMY )
	{

		SetFontForeground( FONT_YELLOW );
		mprintf( xp, yp, L"PRIORITY ENEMY DROPPED ITEMS" );
		yp += 10;

		//Do the priority existance guys first
		if( !gpPEnemyItemsSummaryArray )
		{
			SetFontForeground( FONT_DKYELLOW );
			mprintf( xp, yp, L"None" );
			yp += 10;
		}
		else for( index = 1; index < MAXITEMS; index++ )
		{
			uiQuantity = 0;
			uiExistChance = 0;
			uiStatus = 0;
			for( i = 0; i < gusPEnemyItemsSummaryArraySize; i++ )
			{
				if( gpPEnemyItemsSummaryArray[ i ].usItem == index )
				{
					pItem = &gpPEnemyItemsSummaryArray[ i ];
					uiExistChance += 100 * pItem->ubNumberOfObjects;
					uiStatus += pItem->bStatus[0];
					uiQuantity += pItem->ubNumberOfObjects;
				}
			}
			if( uiQuantity )
			{
				if( !(yp % 20) )
					SetFontForeground( FONT_LTKHAKI );
				else
					SetFontForeground( FONT_GRAY2 );
				//calc averages
				dAvgExistChance = (FLOAT)(uiExistChance / 100.0);
				dAvgStatus = uiStatus / (FLOAT)uiQuantity;
				//Display stats.
				mprintf(xp, yp, L"%ls", ShortItemNames[index]);
				mprintf( xp + 85, yp, L"%3.02f", dAvgExistChance );
				mprintf( xp + 110, yp, L"@ %3.02f%%", dAvgStatus );
				yp += 10;
				if( yp >= 355 )
				{
					xp += 170;
					yp = 20;
					if( xp >= 300 )
					{
						SetFontForeground( FONT_RED );
						mprintf( 350, 350, L"TOO MANY ITEMS TO DISPLAY!");
						return;
					}
				}
			}
		}

		yp += 5;

		SetFontForeground( FONT_YELLOW );
		mprintf( xp, yp, L"NORMAL ENEMY DROPPED ITEMS" );
		yp += 10;
		if( yp >= 355 )
		{
			xp += 170;
			yp = 20;
			if( xp >= 300 )
			{
				SetFontForeground( FONT_RED );
				mprintf( 350, 350, L"TOO MANY ITEMS TO DISPLAY!");
				return;
			}
		}

		//Do the priority existance guys first
		if( !gpNEnemyItemsSummaryArray )
		{
			SetFontForeground( FONT_DKYELLOW );
			mprintf( xp, yp, L"None" );
			yp += 10;
		}
		for( index = 1; index < MAXITEMS; index++ )
		{
			uiQuantity = 0;
			uiExistChance = 0;
			uiStatus = 0;
			for( i = 0; i < gusNEnemyItemsSummaryArraySize; i++ )
			{
				if( gpNEnemyItemsSummaryArray[ i ].usItem == index )
				{
					pItem = &gpNEnemyItemsSummaryArray[ i ];
					uiExistChance += 100 * pItem->ubNumberOfObjects;
					uiStatus += pItem->bStatus[0];
					uiQuantity += pItem->ubNumberOfObjects;
				}
			}
			if( uiQuantity )
			{
				if( !(yp % 20) )
					SetFontForeground( FONT_LTKHAKI );
				else
					SetFontForeground( FONT_GRAY2 );
				//calc averages
				dAvgExistChance = (FLOAT)(uiExistChance / 100.0);
				dAvgStatus = uiStatus / (FLOAT)uiQuantity;
				//Display stats.
				mprintf(xp, yp, L"%ls", ShortItemNames[index]);
				mprintf( xp + 85, yp, L"%3.02f", dAvgExistChance );
				mprintf( xp + 110, yp, L"@ %3.02f%%", dAvgStatus );
				yp += 10;
				if( yp >= 355 )
				{
					xp += 170;
					yp = 20;
					if( xp >= 300 )
					{
						SetFontForeground( FONT_RED );
						mprintf( 350, 350, L"TOO MANY ITEMS TO DISPLAY!");
						return;
					}
				}
			}
		}



	}
	else
	{
		SetFontForeground( FONT_RED );
		mprintf( 5, 50, L"ERROR:  Can't load the items for this map.  Reason unknown." );
	}
}


static void CalculateOverrideStatus(void);
static void ExtractTempFilename(void);
static void SetupItemDetailsMode(BOOLEAN fAllowRecursion);


void RenderSummaryWindow()
{
	UINT8 *pDestBuf;
	UINT32 uiDestPitchBYTES;
	SGPRect ClipRect;
	INT32 i, x, y;
	if( (GetActiveFieldID() == 1 ) != gfTempFile )
	{
		gfTempFile ^= 1;
		SetInputFieldStringWith16BitString( 1, L"" );
		gfRenderSummary = TRUE;
	}
	if( gfTempFile ) //constantly extract the temp filename for updating purposes.
		ExtractTempFilename();
	if( gfRenderSummary )
	{
		gfRenderSummary = FALSE;
		gfRenderMap = TRUE;
		for( i = 1; i < NUM_SUMMARY_BUTTONS; i++ )
		{
			MarkAButtonDirty( iSummaryButton[ i ] );
		}

		DrawButton( iSummaryButton[ SUMMARY_BACKGROUND ] );
		InvalidateRegion(0, 0, SCREEN_WIDTH, 360);

		SetFont( BLOCKFONT2 );
		SetFontForeground( FONT_LTKHAKI );
		SetFontShadow( FONT_DKKHAKI );
		SetFontBackground( 0 );
		if( !gfItemDetailsMode )
		{
			mprintf(10, 5, L"CAMPAIGN EDITOR -- %ls Version 1.%02d",
				gszVersionType[ GLOBAL_SUMMARY_STATE ], GLOBAL_SUMMARY_VERSION );
		}

		//This section builds the proper header to be displayed for an existing global summary.
		if( !gfWorldLoaded )
		{
			SetFontForeground( FONT_RED );
			SetFontShadow( FONT_NEARBLACK );
			mprintf( 270, 5, L"(NO MAP LOADED).");
		}
		SetFont( FONT10ARIAL );
		SetFontShadow( FONT_NEARBLACK );
		if( gfGlobalSummaryExists )
		{
			wchar_t str[100];
			BOOLEAN fSectorSummaryExists = FALSE;
			if( gusNumEntriesWithOutdatedOrNoSummaryInfo && !gfOutdatedDenied )
			{
				DisableButton( iSummaryButton[ SUMMARY_LOAD ] );
				SetFontForeground( FONT_YELLOW );
				mprintf( 10, 20, L"You currently have %d outdated maps.", gusNumEntriesWithOutdatedOrNoSummaryInfo);
				mprintf( 10, 30, L"The more maps that need to be updated, the longer it takes.  It'll take ");
				mprintf( 10, 40, L"approximately 4 minutes on a P200MMX to analyse 100 maps, so");
				mprintf( 10, 50, L"depending on your computer, it may vary.");
				SetFontForeground( FONT_LTRED );
				mprintf( 10, 65, L"Do you wish to regenerate info for ALL these maps at this time (y/n)?" );
			}
			else if( !gsSelSectorX && !gsSectorX || gfTempFile )
			{
				DisableButton( iSummaryButton[ SUMMARY_LOAD ] );
				SetFontForeground( FONT_LTRED );
				mprintf( 10, 20, L"There is no sector currently selected." );
				if( gfTempFile )
				{
					SetFontForeground( FONT_YELLOW );
					mprintf( 10, 30, L"Entering a temp file name that doesn't follow campaign editor conventions..." );
					goto SPECIALCASE_LABEL;  //OUCH!!!
				}
				else if( !gfWorldLoaded )
				{
					SetFontForeground( FONT_YELLOW );
					mprintf( 10, 30, L"You need to either load an existing map or create a new map before being" );
					mprintf( 10, 40, L"able to enter the editor, or you can quit (ESC or Alt+x).");
				}
			}
			else
			{
				//Build sector string
				if( gsSelSectorX )
					x = gsSelSectorX - 1, y = gsSelSectorY - 1;
				else
					x = gsSectorX - 1, y = gsSectorY - 1;
				swprintf(str, lengthof(str), L"%c%d", y + 'A', x + 1);
				swprintf(gszFilename, lengthof(gszFilename), str);
				giCurrLevel = giCurrentViewLevel;
				switch( giCurrentViewLevel )
				{
					case ALL_LEVELS_MASK:
						//search for the highest level
						fSectorSummaryExists = TRUE;
						if( gbSectorLevels[x][y] & GROUND_LEVEL_MASK )
							giCurrLevel = GROUND_LEVEL_MASK;
						else if( gbSectorLevels[x][y] & BASEMENT1_LEVEL_MASK )
							giCurrLevel = BASEMENT1_LEVEL_MASK;
						else if( gbSectorLevels[x][y] & BASEMENT2_LEVEL_MASK )
							giCurrLevel = BASEMENT2_LEVEL_MASK;
						else if( gbSectorLevels[x][y] & BASEMENT3_LEVEL_MASK )
							giCurrLevel = BASEMENT3_LEVEL_MASK;
						else
							fSectorSummaryExists = FALSE;
						break;
					case ALTERNATE_LEVELS_MASK:
						//search for the highest alternate level
						fSectorSummaryExists = TRUE;
						if( gbSectorLevels[x][y] & ALTERNATE_GROUND_MASK )
							giCurrLevel = ALTERNATE_GROUND_MASK;
						else if( gbSectorLevels[x][y] & ALTERNATE_B1_MASK )
							giCurrLevel = ALTERNATE_B1_MASK;
						else if( gbSectorLevels[x][y] & ALTERNATE_B2_MASK )
							giCurrLevel = ALTERNATE_B2_MASK;
						else if( gbSectorLevels[x][y] & ALTERNATE_B3_MASK )
							giCurrLevel = ALTERNATE_B3_MASK;
						else
							fSectorSummaryExists = FALSE;
						break;
					case GROUND_LEVEL_MASK:
						if( gbSectorLevels[x][y] & GROUND_LEVEL_MASK )
							fSectorSummaryExists = TRUE;
						break;
					case BASEMENT1_LEVEL_MASK:
						if( gbSectorLevels[x][y] & BASEMENT1_LEVEL_MASK )
							fSectorSummaryExists = TRUE;
						break;
					case BASEMENT2_LEVEL_MASK:
						if( gbSectorLevels[x][y] & BASEMENT2_LEVEL_MASK )
							fSectorSummaryExists = TRUE;
						break;
					case BASEMENT3_LEVEL_MASK:
						if( gbSectorLevels[x][y] & BASEMENT3_LEVEL_MASK )
							fSectorSummaryExists = TRUE;
						break;
					case ALTERNATE_GROUND_MASK:
						if( gbSectorLevels[x][y] & ALTERNATE_GROUND_MASK )
							fSectorSummaryExists = TRUE;
						break;
					case ALTERNATE_B1_MASK:
						if( gbSectorLevels[x][y] & ALTERNATE_B1_MASK )
							fSectorSummaryExists = TRUE;
						break;
					case ALTERNATE_B2_MASK:
						if( gbSectorLevels[x][y] & ALTERNATE_B2_MASK )
							fSectorSummaryExists = TRUE;
						break;
					case ALTERNATE_B3_MASK:
						if( gbSectorLevels[x][y] & ALTERNATE_B3_MASK )
							fSectorSummaryExists = TRUE;
						break;
				}
				if( gbSectorLevels[x][y] )
				{
					switch( giCurrLevel )
					{
						case GROUND_LEVEL_MASK:
							giCurrLevel = 0;
							wcscat( str, L", ground level" );
							gpCurrentSectorSummary = gpSectorSummary[x][y][0];
							break;
						case BASEMENT1_LEVEL_MASK:
							giCurrLevel = 1;
							wcscat( str, L", underground level 1" );
							gpCurrentSectorSummary = gpSectorSummary[x][y][1];
							break;
						case BASEMENT2_LEVEL_MASK:
							giCurrLevel = 2;
							wcscat( str, L", underground level 2" );
							gpCurrentSectorSummary = gpSectorSummary[x][y][2];
							break;
						case BASEMENT3_LEVEL_MASK:
							giCurrLevel = 3;
							wcscat( str, L", underground level 3" );
							gpCurrentSectorSummary = gpSectorSummary[x][y][3];
							break;
						case ALTERNATE_GROUND_MASK:
							giCurrLevel = 4;
							wcscat( str, L", alternate G level" );
							gpCurrentSectorSummary = gpSectorSummary[x][y][4];
							break;
						case ALTERNATE_B1_MASK:
							giCurrLevel = 5;
							wcscat( str, L", alternate B1 level" );
							gpCurrentSectorSummary = gpSectorSummary[x][y][5];
							break;
						case ALTERNATE_B2_MASK:
							giCurrLevel = 6;
							wcscat( str, L", alternate B2 level" );
							gpCurrentSectorSummary = gpSectorSummary[x][y][6];
							break;
						case ALTERNATE_B3_MASK:
							giCurrLevel = 7;
							wcscat( str, L", alternate B3 level" );
							gpCurrentSectorSummary = gpSectorSummary[x][y][7];
							break;
					}
				}
				else
				{
					DisableButton( iSummaryButton[ SUMMARY_LOAD ] );
				}
				if( fSectorSummaryExists )
				{
					switch( giCurrLevel )
					{
						case 0:	wcscat( gszFilename, L".dat" );				break;
						case 1:	wcscat( gszFilename, L"_b1.dat" );		break;
						case 2:	wcscat( gszFilename, L"_b2.dat" );		break;
						case 3:	wcscat( gszFilename, L"_b3.dat" );		break;
						case 4:	wcscat( gszFilename, L"_a.dat" );			break;
						case 5:	wcscat( gszFilename, L"_b1_a.dat" );	break;
						case 6:	wcscat( gszFilename, L"_b2_a.dat" );	break;
						case 7:	wcscat( gszFilename, L"_b3_a.dat" );	break;
					}
					swprintf(gszDisplayName, lengthof(gszDisplayName), gszFilename);
					EnableButton( iSummaryButton[ SUMMARY_LOAD ] );
					if( gpCurrentSectorSummary )
					{
						if( gpCurrentSectorSummary->ubSummaryVersion < GLOBAL_SUMMARY_VERSION )
							ShowButton( iSummaryButton[ SUMMARY_UPDATE ] );
						else
							HideButton( iSummaryButton[ SUMMARY_UPDATE ] );
						if( !gfAlternateMaps )
							SetFontForeground( FONT_YELLOW );
						else
							SetFontForeground( FONT_LTBLUE );
						if( gfItemDetailsMode )
						{
							if( gfSetupItemDetailsMode )
							{
								SetupItemDetailsMode( TRUE );
								gfSetupItemDetailsMode = FALSE;
							}
							mprintf(10, 5, L"ITEM DETAILS -- sector %ls", str);
							RenderItemDetails();
						}
						else
						{
							mprintf(10, 20, L"Summary Information for sector %ls:", str);
							HideButton( iSummaryButton[ SUMMARY_REAL ] );
							HideButton( iSummaryButton[ SUMMARY_SCIFI ] );
							HideButton( iSummaryButton[ SUMMARY_ENEMY ] );
							RenderSectorInformation();
						}
					}
					else
					{
						SetFontForeground( FONT_RED );
						if( gfItemDetailsMode )
						{
							mprintf(10, 5, L"Summary Information for sector %ls" , str);
							mprintf( 10, 15, L"does not exist." );
						}
						else
						{
							mprintf(10, 20, L"Summary Information for sector %ls" , str);
							mprintf( 10, 30, L"does not exist." );
						}
						ShowButton( iSummaryButton[ SUMMARY_UPDATE ] );
					}
				}
				else
				{
					if( !gfAlternateMaps )
						SetFontForeground( FONT_ORANGE );
					else
					{
						SetFontForeground( FONT_DKBLUE );
						SetFontShadow( 0 );
					}
					if( gfItemDetailsMode )
						mprintf(10, 5, L"No information exists for sector %ls.", str);
					else
						mprintf(10, 20, L"No information exists for sector %ls.", str);
					SetFontShadow( FONT_NEARBLACK );

					switch( giCurrentViewLevel )
					{
						case ALL_LEVELS_MASK:
						case GROUND_LEVEL_MASK:			wcscat( gszFilename, L".dat" );				break;
						case BASEMENT1_LEVEL_MASK:	wcscat( gszFilename, L"_b1.dat" );		break;
						case BASEMENT2_LEVEL_MASK:	wcscat( gszFilename, L"_b2.dat" );		break;
						case BASEMENT3_LEVEL_MASK:	wcscat( gszFilename, L"_b3.dat" );		break;
						case ALTERNATE_LEVELS_MASK:
						case ALTERNATE_GROUND_MASK:	wcscat( gszFilename, L"_a.dat" );			break;
						case ALTERNATE_B1_MASK:			wcscat( gszFilename, L"_b1_a.dat" );	break;
						case ALTERNATE_B2_MASK:			wcscat( gszFilename, L"_b2_a.dat" );	break;
						case ALTERNATE_B3_MASK:			wcscat( gszFilename, L"_b3_a.dat" );	break;
					}
					swprintf(gszDisplayName, lengthof(gszDisplayName), gszFilename);
					DisableButton( iSummaryButton[ SUMMARY_LOAD ] );
				}
				SPECIALCASE_LABEL:
				if( gfOverrideDirty && gfPersistantSummary )
					CalculateOverrideStatus();
				if( gubOverrideStatus == INACTIVE )
				{
					if( !gfAlternateMaps )
						SetFontForeground( FONT_LTKHAKI );
					else
						SetFontForeground( FONT_LTBLUE );
					mprintf(MAP_LEFT + 110, MAP_BOTTOM + 55, L"FILE:  %ls", gszDisplayName);
				}
				else //little higher to make room for the override checkbox and text.
				{
					if( !gfAlternateMaps )
						SetFontForeground( FONT_LTKHAKI );
					else
						SetFontForeground( FONT_LTBLUE );
					mprintf(MAP_LEFT + 110, MAP_BOTTOM + 46, L"FILE:  %ls", gszDisplayName);
					if( gubOverrideStatus == READONLY )
					{
						SetFontForeground(gfOverride ? FONT_YELLOW : FONT_LTRED);
						mprintf( MAP_LEFT+124, MAP_BOTTOM+61, L"Override READONLY" );
					}
					else
					{
						SetFontForeground(gfOverride ? FONT_YELLOW: FONT_ORANGE);
						mprintf( MAP_LEFT+124, MAP_BOTTOM+61, L"Overwrite File");
					}
				}
			}
		}
		else if( !gfDeniedSummaryCreation )
		{
			SetFontForeground( FONT_GRAY1 );
			mprintf( 10, 20, L"You currently have no summary data.  By creating one, you will be able to keep track" );
			mprintf( 10, 30, L"of information pertaining to all of the sectors you edit and save.  The creation process" );
			mprintf( 10, 40, L"will analyse all maps in your /MAPS directory, and generate a new one.  This could" );
			mprintf( 10, 50, L"take a few minutes depending on how many valid maps you have.  Valid maps are" );
			mprintf( 10, 60, L"maps following the proper naming convention from a1.dat - p16.dat.  Underground maps" );
			mprintf( 10, 70, L"are signified by appending _b1 to _b3 before the .dat (ex:  a9_b1.dat). ");
			SetFontForeground( FONT_LTRED );
			mprintf( 10, 85, L"Do you wish to do this now (y/n)?" );
		}
		else
		{
			SetFontForeground( FONT_LTRED );
			mprintf( 10, 20, L"No summary info.  Creation denied." );
		}

		SetFont( FONT10ARIAL );
		SetFontForeground( FONT_GRAY3 );
		mprintf( MAP_LEFT + 15, MAP_BOTTOM + 7, L"Grid" );
		mprintf( MAP_LEFT + 65, MAP_BOTTOM + 7, L"Progress" );
		mprintf( MAP_LEFT + 15, MAP_BOTTOM + 27, L"Use Alternate Maps" );
		//Draw the mode tabs
		SetFontForeground( FONT_YELLOW );
		mprintf( 354, 18, L"Summary" );
		pDestBuf = LockVideoSurface( FRAME_BUFFER, &uiDestPitchBYTES );
		SetClippingRegionAndImageWidth(uiDestPitchBYTES, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		RectangleDraw( TRUE, 350, 15, 405, 28, 0, pDestBuf );
		UnLockVideoSurface( FRAME_BUFFER );
		ShadowVideoSurfaceRectUsingLowPercentTable( FRAME_BUFFER, 351, 16, 404, 27 );
		if( gpCurrentSectorSummary )
			/*&& gpCurrentSectorSummary->usNumItems ||
				gpPEnemyItemsSummaryArray && gusPEnemyItemsSummaryArraySize ||
				gpNEnemyItemsSummaryArray && gusNEnemyItemsSummaryArraySize )*/
		{
			SetFontForeground( FONT_YELLOW );
		}
		else
		{
			SetFontForeground( FONT_RED );
		}
		mprintf( 354, 33, L"Items" );
		pDestBuf = LockVideoSurface( FRAME_BUFFER, &uiDestPitchBYTES );
		SetClippingRegionAndImageWidth(uiDestPitchBYTES, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		RectangleDraw( TRUE, 350, 30, 405, 43, 0, pDestBuf );
		UnLockVideoSurface( FRAME_BUFFER );
		if( gpCurrentSectorSummary )
			/*&& gpCurrentSectorSummary->usNumItems ||
				gpPEnemyItemsSummaryArray && gusPEnemyItemsSummaryArraySize ||
				gpNEnemyItemsSummaryArray && gusNEnemyItemsSummaryArraySize )
				*/
		{
			ShadowVideoSurfaceRectUsingLowPercentTable( FRAME_BUFFER, 351, 31, 404, 42 );
		}
		else
		{
			ShadowVideoSurfaceRect( FRAME_BUFFER, 351, 31, 404, 42 );
		}
		SetFontForeground( FONT_GRAY2 );
	}

	RenderButtons();

	if( gfRenderMap )
	{
		gfRenderMap = FALSE;
		BltVideoObject(FRAME_BUFFER, guiOmertaMap, 0, MAP_LEFT - 2, MAP_TOP - 2);
		InvalidateRegion( MAP_LEFT-1, MAP_TOP-1, MAP_RIGHT+1, MAP_BOTTOM+1 );
		//Draw the coordinates
		SetFont( SMALLCOMPFONT );
		SetFontForeground( FONT_BLACK );
		for( y = 0; y < 16; y++ )
		{
			mprintf( MAP_LEFT-8, MAP_TOP+4+y*13, L"%c", 65 + y );
		}
		for( x = 1; x <= 16; x++ )
		{
			wchar_t str[3];
			swprintf(str, lengthof(str), L"%d", x);
			mprintf( MAP_LEFT+x*13-(13+StringPixLength( str, SMALLCOMPFONT ))/2, MAP_TOP-8, str );
		}
		if( gfRenderGrid )
		{
			UINT16 pos;
			pDestBuf = LockVideoSurface( FRAME_BUFFER, &uiDestPitchBYTES );
			SetClippingRegionAndImageWidth(uiDestPitchBYTES, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
			for( i = 1; i <= 15; i++ )
			{
				//draw vertical lines
				pos = (UINT16)(i * 13 + MAP_LEFT);
				LineDraw( TRUE, pos, MAP_TOP, pos, MAP_BOTTOM-1, 0, pDestBuf );
				//draw horizontal lines
				pos = (UINT16)(i * 13 + MAP_TOP);
				LineDraw( TRUE, MAP_LEFT, pos, MAP_RIGHT-1, pos, 0, pDestBuf );
			}
			UnLockVideoSurface( FRAME_BUFFER );
		}
		if( gfRenderProgress )
		{
			UINT8 ubNumUndergroundLevels;
			for( y = 0; y < 16; y++ )
			{
				ClipRect.iTop = MAP_TOP + y*13;
				ClipRect.iBottom = ClipRect.iTop + 12;
				for( x = 0; x < 16; x++ )
				{
					if( giCurrentViewLevel == ALL_LEVELS_MASK )
					{
						ubNumUndergroundLevels = 0;
						if( gbSectorLevels[x][y] & BASEMENT1_LEVEL_MASK )		ubNumUndergroundLevels++;
						if( gbSectorLevels[x][y] & BASEMENT2_LEVEL_MASK )		ubNumUndergroundLevels++;
						if( gbSectorLevels[x][y] & BASEMENT3_LEVEL_MASK )		ubNumUndergroundLevels++;
						if( ubNumUndergroundLevels )
						{ //display the number of underground levels.  If there
							//is no ground level, then it'll be shadowed.
							SetFont( SMALLCOMPFONT );
							SetFontForeground( FONT_YELLOW );
							mprintf(MAP_LEFT + x * 13 + 4, ClipRect.iTop + 4, L"%d", ubNumUndergroundLevels);
						}
						if( gbSectorLevels[x][y] & GROUND_LEVEL_MASK )
						{
							if( !gfItemDetailsMode || !gpSectorSummary[x][y][0] || gpSectorSummary[x][y][0]->usNumItems )
								continue;
						}
					}
					else if( giCurrentViewLevel == ALTERNATE_LEVELS_MASK )
					{
						ubNumUndergroundLevels = 0;
						if( gbSectorLevels[x][y] & ALTERNATE_B1_MASK )		ubNumUndergroundLevels++;
						if( gbSectorLevels[x][y] & ALTERNATE_B2_MASK )		ubNumUndergroundLevels++;
						if( gbSectorLevels[x][y] & ALTERNATE_B3_MASK )		ubNumUndergroundLevels++;
						if( ubNumUndergroundLevels )
						{ //display the number of underground levels.  If there
							//is no ground level, then it'll be shadowed.
							SetFont( SMALLCOMPFONT );
							SetFontForeground( FONT_YELLOW );
							mprintf(MAP_LEFT + x * 13 + 4, ClipRect.iTop + 4, L"%d", ubNumUndergroundLevels);
						}
						if( gbSectorLevels[x][y] & ALTERNATE_GROUND_MASK )
						{
							if( !gfItemDetailsMode || !gpSectorSummary[x][y][4] || gpSectorSummary[x][y][4]->usNumItems )
								continue;
						}
					}
					else if( gbSectorLevels[x][y] & giCurrentViewLevel )
					{
						if( !gfItemDetailsMode || !gpSectorSummary[x][y][giCurrLevel] || gpSectorSummary[x][y][giCurrLevel]->usNumItems )
							continue;
					}
					ClipRect.iLeft = MAP_LEFT + x*13;
					ClipRect.iRight = ClipRect.iLeft + 12;
					ShadowVideoSurfaceRect(FRAME_BUFFER, ClipRect.iLeft, ClipRect.iTop, ClipRect.iRight, ClipRect.iBottom);
					if( giCurrentViewLevel == BASEMENT1_LEVEL_MASK ||
						  giCurrentViewLevel == BASEMENT2_LEVEL_MASK ||
							giCurrentViewLevel == BASEMENT3_LEVEL_MASK ||
							giCurrentViewLevel == ALTERNATE_B1_MASK		 ||
							giCurrentViewLevel == ALTERNATE_B2_MASK		 ||
							giCurrentViewLevel == ALTERNATE_B3_MASK		 )
					{
						ShadowVideoSurfaceRect(FRAME_BUFFER, ClipRect.iLeft, ClipRect.iTop, ClipRect.iRight, ClipRect.iBottom);
					}
				}
			}
		}
	}

	if( gfGlobalSummaryExists )
	{
		pDestBuf = LockVideoSurface( FRAME_BUFFER, &uiDestPitchBYTES );
		SetClippingRegionAndImageWidth(uiDestPitchBYTES, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		//Render the grid for the map currently residing in memory (blue).
		if( gfWorldLoaded && !gfTempFile && gsSectorX )
		{
			x = MAP_LEFT + (gsSectorX-1) * 13 + 1;
			y = MAP_TOP + (gsSectorY-1) * 13 + 1;
			RectangleDraw( TRUE, x, y, x+11, y+11, Get16BPPColor( FROMRGB( 50, 50, 200 ) ), pDestBuf );
		}

		//Render the grid for the sector currently in focus (red).
		if( gsSelSectorX > 0 && !gfTempFile )
		{
			x = MAP_LEFT + (gsSelSectorX-1) * 13 ;
			y = MAP_TOP + (gsSelSectorY-1) * 13 ;
			RectangleDraw( TRUE, x, y, x+13, y+13, Get16BPPColor( FROMRGB( 200, 50, 50 ) ), pDestBuf );
		}

		//Render the grid for the sector if the mouse is over it (yellow).
		if( gsHiSectorX > 0 )
		{
			x = MAP_LEFT + (gsHiSectorX-1) * 13 - 1;
			y = MAP_TOP + (gsHiSectorY-1) * 13 - 1;
			RectangleDraw( TRUE, x, y, x+15, y+15, Get16BPPColor( FROMRGB( 200, 200, 50 ) ), pDestBuf );
		}
		UnLockVideoSurface( FRAME_BUFFER );
	}
	//Check to see if the user clicked on one of the hot spot mode change areas.
	if( gfLeftButtonState )
	{
		if( !gfItemDetailsMode )
		{
			if( gpCurrentSectorSummary )
				/*&& gpCurrentSectorSummary->usNumItems ||
					gpPEnemyItemsSummaryArray && gusPEnemyItemsSummaryArraySize ||
					gpNEnemyItemsSummaryArray && gusNEnemyItemsSummaryArraySize )*/
			{
				if( gusMouseXPos >= 350 && gusMouseYPos >= 30 && gusMouseXPos <= 404 && gusMouseYPos <= 42 )
				{
					gfItemDetailsMode = TRUE;
					gfSetupItemDetailsMode = TRUE;
					gfRenderSummary = TRUE;
				}
			}
		}
		else if( gfItemDetailsMode && gusMouseXPos >= 350 && gusMouseYPos >= 15 && gusMouseXPos <= 404 && gusMouseYPos <= 27 )
		{
			gfItemDetailsMode = FALSE;
			gfRenderSummary = TRUE;
		}
	}
}

void UpdateSectorSummary(const wchar_t* gszFilename, BOOLEAN fUpdate)
{
	wchar_t str[50];
	const wchar_t* ptr;
	INT16 x, y;

	gfRenderSummary = TRUE;
	//Extract the sector
	if( gszFilename[2] < L'0' || gszFilename[2] > L'9' )
		x = gszFilename[1] - L'0';
	else
		x = (gszFilename[1] - L'0') * 10 + gszFilename[2] - L'0';
	if( gszFilename[0] >= 'a' )
		y = gszFilename[0] - L'a' + 1;
	else
		y = gszFilename[0] - L'A' + 1;
	gfMapFileDirty = FALSE;

	//Validate that the values extracted are in fact a sector
	if( x < 1 || x > 16 || y < 1 || y > 16 )
		return;
	gsSectorX = gsSelSectorX = x;
	gsSectorY = gsSelSectorY = y;

	//The idea here is to get a pointer to the filename's period, then
	//focus on the character previous to it.  If it is a 1, 2, or 3, then
	//the filename was in a basement level.  Otherwise, it is a ground level.
	ptr = wcsstr( gszFilename, L"_a.dat" );
	if( ptr )
	{
		ptr = wcsstr( gszFilename, L"_b" );
		if( ptr && ptr[2] >= '1' && ptr[2] <= '3' && ptr[5] == '.' )
		{ //it's a alternate basement map
			switch( ptr[2] )
			{
				case '1':
					gsSectorLayer = ALTERNATE_B1_MASK;
					giCurrLevel = 5;
					break;
				case '2':
					gsSectorLayer = ALTERNATE_B2_MASK;
					giCurrLevel = 6;
					break;
				case '3':
					gsSectorLayer = ALTERNATE_B3_MASK;
					giCurrLevel = 7;
					break;
			}
		}
		else
		{
			gsSectorLayer = ALTERNATE_GROUND_MASK;
			giCurrLevel = 4;
		}
	}
	else
	{
		ptr = wcsstr( gszFilename, L"_b" );
		if( ptr && ptr[2] >= '1' && ptr[2] <= '3' && ptr[3] == '.' )
		{ //it's a alternate basement map
			switch( ptr[2] )
			{
				case '1':
					gsSectorLayer = BASEMENT1_LEVEL_MASK;
					giCurrLevel = 1;
					break;
				case '2':
					gsSectorLayer = BASEMENT2_LEVEL_MASK;
					giCurrLevel = 2;
					break;
				case '3':
					gsSectorLayer = BASEMENT3_LEVEL_MASK;
					giCurrLevel = 3;
					break;
			}
		}
		else
		{
			gsSectorLayer = GROUND_LEVEL_MASK;
			giCurrLevel = 0;
		}
	}

	giCurrentViewLevel = gsSectorLayer;
	if( !(gbSectorLevels[gsSectorX-1][gsSectorY-1] & gsSectorLayer) )
	{
		//new sector map saved, so update the global file.
		gbSectorLevels[gsSectorX-1][gsSectorY-1] |= gsSectorLayer;
	}

	if( fUpdate )
	{
		SetFont( FONT10ARIAL );
		SetFontForeground( FONT_LTKHAKI );
		SetFontShadow( FONT_NEARBLACK );
		swprintf(str, lengthof(str), L"Analyzing map:  %ls...", gszFilename);

		if( gfSummaryWindowActive )
		{
			mprintf( MAP_LEFT, MAP_BOTTOM+100, str );
			InvalidateRegion( MAP_LEFT, MAP_BOTTOM+100, MAP_LEFT+150,	MAP_BOTTOM+110 );
			CreateProgressBar( 0, MAP_LEFT, MAP_BOTTOM+110, MAP_LEFT+140, MAP_BOTTOM+120 );
		}
		else
		{
			mprintf((SCREEN_WIDTH - StringPixLength(str, FONT10ARIAL)) / 2, 190, str);
			InvalidateRegion( 200, 190, 400, 200 );
			CreateProgressBar( 0, 250, 200, 390, 210 );
		}

		char szCoord[40];
		sprintf( szCoord, "%ls", gszFilename );
		if( gsSectorX > 9 )
			szCoord[3] = '\0';
		else
			szCoord[2] = '\0';
		gusNumEntriesWithOutdatedOrNoSummaryInfo++;
		EvaluateWorld( szCoord, (UINT8)giCurrLevel );

		RemoveProgressBar( 0 );
	}
	else
		gusNumEntriesWithOutdatedOrNoSummaryInfo++;
}


static void SummaryOkayCallback(GUI_BUTTON* btn, INT32 reason)
{
	if( reason & MSYS_CALLBACK_REASON_LBUTTON_UP )
	{
		DestroySummaryWindow();
	}
}


static void SummaryToggleGridCallback(GUI_BUTTON* btn, INT32 reason)
{
	if( reason & MSYS_CALLBACK_REASON_LBUTTON_UP )
	{
		gfRenderGrid = (BOOLEAN)(btn->uiFlags & BUTTON_CLICKED_ON);
		gfRenderMap = TRUE;
	}
}


static void SummaryToggleAlternateCallback(GUI_BUTTON* btn, INT32 reason)
{
	if( reason & MSYS_CALLBACK_REASON_LBUTTON_UP )
	{
		if( btn->uiFlags & BUTTON_CLICKED_ON )
		{
			giCurrentViewLevel <<= 4;
			gfAlternateMaps = TRUE;
		}
		else
		{
			giCurrentViewLevel >>= 4;
			gfAlternateMaps = FALSE;
		}
		gfRenderSummary = TRUE;
	}
}


static void SummarySciFiCallback(GUI_BUTTON* btn, INT32 reason)
{
	if( reason & MSYS_CALLBACK_REASON_LBUTTON_UP )
	{
		ButtonList[ iSummaryButton[ SUMMARY_SCIFI ] ]->uiFlags |= (BUTTON_CLICKED_ON | BUTTON_DIRTY);
		ButtonList[ iSummaryButton[ SUMMARY_REAL ] ]->uiFlags &= ~BUTTON_CLICKED_ON;
		ButtonList[ iSummaryButton[ SUMMARY_REAL ] ]->uiFlags |= BUTTON_DIRTY;
		ButtonList[ iSummaryButton[ SUMMARY_ENEMY ] ]->uiFlags &= ~BUTTON_CLICKED_ON;
		ButtonList[ iSummaryButton[ SUMMARY_ENEMY ] ]->uiFlags |= BUTTON_DIRTY;
		gubSummaryItemMode = ITEMMODE_SCIFI;
		gfRenderSummary = TRUE;
	}
}


static void SummaryRealCallback(GUI_BUTTON* btn, INT32 reason)
{
	if( reason & MSYS_CALLBACK_REASON_LBUTTON_UP )
	{
		ButtonList[ iSummaryButton[ SUMMARY_SCIFI ] ]->uiFlags &= ~BUTTON_CLICKED_ON;
		ButtonList[ iSummaryButton[ SUMMARY_SCIFI ] ]->uiFlags |= BUTTON_DIRTY;
		ButtonList[ iSummaryButton[ SUMMARY_REAL ] ]->uiFlags |= (BUTTON_CLICKED_ON | BUTTON_DIRTY);
		ButtonList[ iSummaryButton[ SUMMARY_ENEMY ] ]->uiFlags &= ~BUTTON_CLICKED_ON;
		ButtonList[ iSummaryButton[ SUMMARY_ENEMY ] ]->uiFlags |= BUTTON_DIRTY;
		gubSummaryItemMode = ITEMMODE_REAL;
		gfRenderSummary = TRUE;
	}
}


static void SummaryEnemyCallback(GUI_BUTTON* btn, INT32 reason)
{
	if( reason & MSYS_CALLBACK_REASON_LBUTTON_UP )
	{
		ButtonList[ iSummaryButton[ SUMMARY_SCIFI ] ]->uiFlags &= ~BUTTON_CLICKED_ON;
		ButtonList[ iSummaryButton[ SUMMARY_SCIFI ] ]->uiFlags |= BUTTON_DIRTY;
		ButtonList[ iSummaryButton[ SUMMARY_REAL ] ]->uiFlags &= ~BUTTON_CLICKED_ON;
		ButtonList[ iSummaryButton[ SUMMARY_REAL ] ]->uiFlags |= BUTTON_DIRTY;
		ButtonList[ iSummaryButton[ SUMMARY_ENEMY ] ]->uiFlags |= (BUTTON_CLICKED_ON | BUTTON_DIRTY);
		gubSummaryItemMode = ITEMMODE_ENEMY;
		gfRenderSummary = TRUE;
	}
}


static void SummaryToggleProgressCallback(GUI_BUTTON* btn, INT32 reason)
{
	if( reason & MSYS_CALLBACK_REASON_LBUTTON_UP )
	{
		gfRenderProgress = (BOOLEAN)(btn->uiFlags & BUTTON_CLICKED_ON);
		gfRenderMap = TRUE;
	}
}


static void CreateGlobalSummary(void);
static void RegenerateSummaryInfoForAllOutdatedMaps(void);


BOOLEAN HandleSummaryInput( InputAtom *pEvent )
{
	if( !gfSummaryWindowActive )
		return FALSE;
	if( !HandleTextInput( pEvent ) && pEvent->usEvent == KEY_DOWN || pEvent->usEvent == KEY_REPEAT )
	{
		switch( pEvent->usParam )
		{
			case SDLK_ESCAPE:
				if( !gfWorldLoaded )
				{
					DestroySummaryWindow();
					pEvent->usParam = 'x';
					pEvent->usKeyState |= ALT_DOWN;
					gfOverheadMapDirty = TRUE;
					return FALSE;
				}
			case SDLK_RETURN:
				if( GetActiveFieldID() == 1 )
					SelectNextField();
				else if( gfWorldLoaded )
					DestroySummaryWindow();
				break;

			case SDLK_y:
				if( gusNumEntriesWithOutdatedOrNoSummaryInfo && !gfOutdatedDenied )
				{
					gfRenderSummary = TRUE;
					RegenerateSummaryInfoForAllOutdatedMaps();
				}
				if( !gfGlobalSummaryExists && !gfDeniedSummaryCreation )
				{
					gfGlobalSummaryExists = TRUE;
					CreateGlobalSummary();
					gfRenderSummary = TRUE;
				}
				break;

			case SDLK_n:
				if( gusNumEntriesWithOutdatedOrNoSummaryInfo && !gfOutdatedDenied )
				{
					gfOutdatedDenied = TRUE;
					gfRenderSummary = TRUE;
				}
				if( !gfGlobalSummaryExists && !gfDeniedSummaryCreation )
				{
					gfDeniedSummaryCreation = TRUE;
					gfRenderSummary = TRUE;
				}
				break;
			case 'x':
				if (pEvent->usKeyState & ALT_DOWN )
				{
					DestroySummaryWindow();
					return FALSE;
				}
				break;

			case SDLK_RIGHT:
				gfRenderSummary = TRUE;
				if( !gsSelSectorY )
					gsSelSectorY = 1;
				gsSelSectorX++;
				if( gsSelSectorX > 16 )
					gsSelSectorX = 1;
				break;

			case SDLK_LEFT:
				gfRenderSummary = TRUE;
				if( !gsSelSectorY )
					gsSelSectorY = 1;
				gsSelSectorX--;
				if( gsSelSectorX < 1 )
					gsSelSectorX = 16;
				break;

			case SDLK_UP:
				gfRenderSummary = TRUE;
				if( !gsSelSectorX )
					gsSelSectorX = 1;
				gsSelSectorY--;
				if( gsSelSectorY < 1 )
					gsSelSectorY = 16;
				break;

			case SDLK_DOWN:
				gfRenderSummary = TRUE;
				if( !gsSelSectorX )
					gsSelSectorX = 1;
				gsSelSectorY++;
				if( gsSelSectorY > 16 )
					gsSelSectorY = 1;
				break;
		}
	}
	else if( pEvent->usEvent == KEY_UP )
	{ //for releasing modes requiring persistant state keys
		switch( pEvent->usParam )
		{
			case SDLK_F5: ReleaseSummaryWindow(); break;
		}
	}
	return TRUE;
}


//This function can be very time consuming as it loads every map file with a valid
//coordinate name, analyses it, and builds a new global summary file.
static void CreateGlobalSummary(void)
{
	FILE *fp;
	STRING512			Dir;

	OutputDebugString( "Generating GlobalSummary Information...\n" );

	gfGlobalSummaryExists = FALSE;
	//Set current directory to JA2/DevInfo which contains all of the summary data
	const char* ExecDir = GetExecutableDirectory();
	sprintf( Dir, "%s/DevInfo", ExecDir );

	//Directory doesn't exist, so create it, and continue.
	if( !MakeFileManDirectory( Dir ) )
		AssertMsg( 0, "Can't create new directory, JA2/DevInfo for summary information." );
	if( !SetFileManCurrentDirectory( Dir ) )
		AssertMsg( 0, "Can't set to new directory, JA2/DevInfo for summary information." );
	//Generate a simple readme file.
	fp = fopen( "readme.txt", "w" );
	Assert( fp );
	fprintf( fp, "%s\n%s\n", "This information is used in conjunction with the editor.",
		"This directory or it's contents shouldn't be included with final release." );
	fclose( fp );

	sprintf( Dir, "%s/Data", ExecDir );
	SetFileManCurrentDirectory( Dir );

	LoadGlobalSummary();
	RegenerateSummaryInfoForAllOutdatedMaps();
	gfRenderSummary = TRUE;

	OutputDebugString( "GlobalSummary Information generated successfully.\n" );
}


static void MapMoveCallback(MOUSE_REGION* reg, INT32 reason)
{
	static INT16 gsPrevX = 0, gsPrevY = 0;
	//calc current sector highlighted.
	if( reason & MSYS_CALLBACK_REASON_LOST_MOUSE )
	{
		gsPrevX = gsHiSectorX = 0;
		gsPrevY = gsHiSectorY = 0;
		gfRenderMap = TRUE;
		return;
	}
	gsHiSectorX = min( (reg->RelativeXPos / 13) + 1, 16 );
	gsHiSectorY = min( (reg->RelativeYPos / 13) + 1, 16 );
	if( gsPrevX != gsHiSectorX || gsPrevY != gsHiSectorY )
	{
		gsPrevX = gsHiSectorX;
		gsPrevY = gsHiSectorY;
		gfRenderMap = TRUE;
	}
}


static void MapClickCallback(MOUSE_REGION* reg, INT32 reason)
{
	static INT16 sLastX = -1, sLastY = -1;
	//calc current sector selected.
	if( reason & MSYS_CALLBACK_REASON_LBUTTON_UP )
	{
		if( GetActiveFieldID() == 1 )
		{
			gsSelSectorX = 0;
			SelectNextField();
		}
		gsSelSectorX = min( (reg->RelativeXPos / 13) + 1, 16 );
		gsSelSectorY = min( (reg->RelativeYPos / 13) + 1, 16 );
		if( gsSelSectorX != sLastX || gsSelSectorY != sLastY )
		{ //clicked in a new sector
			gfOverrideDirty = TRUE;
			sLastX = gsSelSectorX;
			sLastY = gsSelSectorY;
			switch( giCurrentViewLevel )
			{
				case ALL_LEVELS_MASK:
					if( gpSectorSummary[ gsSelSectorX - 1 ][ gsSelSectorY - 1 ][ 0 ] )
						gpCurrentSectorSummary = gpSectorSummary[ gsSelSectorX - 1 ][ gsSelSectorY - 1 ][ 0 ];
					else if( gpSectorSummary[ gsSelSectorX - 1 ][ gsSelSectorY - 1 ][ 1 ] )
						gpCurrentSectorSummary = gpSectorSummary[ gsSelSectorX - 1 ][ gsSelSectorY - 1 ][ 1 ];
					else if( gpSectorSummary[ gsSelSectorX - 1 ][ gsSelSectorY - 1 ][ 2 ] )
						gpCurrentSectorSummary = gpSectorSummary[ gsSelSectorX - 1 ][ gsSelSectorY - 1 ][ 2 ];
					else if( gpSectorSummary[ gsSelSectorX - 1 ][ gsSelSectorY - 1 ][ 3 ] )
						gpCurrentSectorSummary = gpSectorSummary[ gsSelSectorX - 1 ][ gsSelSectorY - 1 ][ 3 ];
					else
						gpCurrentSectorSummary = NULL;
					break;
				case GROUND_LEVEL_MASK: //already pointing to the correct level
					gpCurrentSectorSummary = gpSectorSummary[ gsSelSectorX - 1 ][ gsSelSectorY - 1 ][ 0 ];
					break;
				case BASEMENT1_LEVEL_MASK:
					gpCurrentSectorSummary = gpSectorSummary[ gsSelSectorX - 1 ][ gsSelSectorY - 1 ][ 1 ];
					break;
				case BASEMENT2_LEVEL_MASK:
					gpCurrentSectorSummary = gpSectorSummary[ gsSelSectorX - 1 ][ gsSelSectorY - 1 ][ 2 ];
					break;
				case BASEMENT3_LEVEL_MASK:
					gpCurrentSectorSummary = gpSectorSummary[ gsSelSectorX - 1 ][ gsSelSectorY - 1 ][ 3 ];
					break;
				case ALTERNATE_LEVELS_MASK:
					if( gpSectorSummary[ gsSelSectorX - 1 ][ gsSelSectorY - 1 ][ 4 ] )
						gpCurrentSectorSummary = gpSectorSummary[ gsSelSectorX - 1 ][ gsSelSectorY - 1 ][ 4 ];
					else if( gpSectorSummary[ gsSelSectorX - 1 ][ gsSelSectorY - 1 ][ 5 ] )
						gpCurrentSectorSummary = gpSectorSummary[ gsSelSectorX - 1 ][ gsSelSectorY - 1 ][ 5 ];
					else if( gpSectorSummary[ gsSelSectorX - 1 ][ gsSelSectorY - 1 ][ 6 ] )
						gpCurrentSectorSummary = gpSectorSummary[ gsSelSectorX - 1 ][ gsSelSectorY - 1 ][ 6 ];
					else if( gpSectorSummary[ gsSelSectorX - 1 ][ gsSelSectorY - 1 ][ 7 ] )
						gpCurrentSectorSummary = gpSectorSummary[ gsSelSectorX - 1 ][ gsSelSectorY - 1 ][ 7 ];
					else
						gpCurrentSectorSummary = NULL;
					break;
				case ALTERNATE_GROUND_MASK: //already pointing to the correct level
					gpCurrentSectorSummary = gpSectorSummary[ gsSelSectorX - 1 ][ gsSelSectorY - 1 ][ 4 ];
					break;
				case ALTERNATE_B1_MASK:
					gpCurrentSectorSummary = gpSectorSummary[ gsSelSectorX - 1 ][ gsSelSectorY - 1 ][ 5 ];
					break;
				case ALTERNATE_B2_MASK:
					gpCurrentSectorSummary = gpSectorSummary[ gsSelSectorX - 1 ][ gsSelSectorY - 1 ][ 6 ];
					break;
				case ALTERNATE_B3_MASK:
					gpCurrentSectorSummary = gpSectorSummary[ gsSelSectorX - 1 ][ gsSelSectorY - 1 ][ 7 ];
					break;
			}
			if( gpWorldItemsSummaryArray )
			{
				MemFree( gpWorldItemsSummaryArray );
				gpWorldItemsSummaryArray = NULL;
				gusWorldItemsSummaryArraySize = 0;
			}
			if( gfItemDetailsMode )
			{
				if( gpCurrentSectorSummary )
					/*&& gpCurrentSectorSummary->usNumItems ||
						gpPEnemyItemsSummaryArray && gusPEnemyItemsSummaryArraySize ||
						gpNEnemyItemsSummaryArray && gusNEnemyItemsSummaryArraySize )*/
				{
					gfSetupItemDetailsMode = TRUE;
				}
			}
		}
		gfRenderSummary = TRUE;
	}
}


static void SummaryToggleLevelCallback(GUI_BUTTON* btn, INT32 reason)
{
	INT8 i;
	if( reason & MSYS_CALLBACK_REASON_LBUTTON_UP )
	{
		if( GetActiveFieldID() == 1 )
			SelectNextField();
		gfRenderSummary = TRUE;
		for( i = SUMMARY_ALL; i <= SUMMARY_B3; i++ )
		{
			if( btn->IDNum == iSummaryButton[ i ] )
			{
				switch( i )
				{
					case SUMMARY_ALL:
						giCurrentViewLevel = ALL_LEVELS_MASK;
						break;
					case SUMMARY_G:
						giCurrentViewLevel = GROUND_LEVEL_MASK;
						break;
					case SUMMARY_B1:
						giCurrentViewLevel = BASEMENT1_LEVEL_MASK;
						break;
					case SUMMARY_B2:
						giCurrentViewLevel = BASEMENT2_LEVEL_MASK;
						break;
					case SUMMARY_B3:
						giCurrentViewLevel = BASEMENT3_LEVEL_MASK;
						break;
				}
				if( gfAlternateMaps )
					giCurrentViewLevel <<= 4;
			}
			else
			{
				ButtonList[ iSummaryButton[ i ] ]->uiFlags &= (~BUTTON_CLICKED_ON);
			}
		}
	}
}


static void SummaryLoadMapCallback(GUI_BUTTON* btn, INT32 reason)
{
	if( reason & MSYS_CALLBACK_REASON_LBUTTON_UP )
	{
		const wchar_t* ptr;
		wchar_t str[50];
		gfRenderSummary = TRUE;

		SetFont( FONT10ARIAL );
		SetFontForeground( FONT_LTKHAKI );
		SetFontShadow( FONT_NEARBLACK );

		CreateProgressBar( 0, MAP_LEFT+5, MAP_BOTTOM+110, 573, MAP_BOTTOM+120 );

		DefineProgressBarPanel( 0, 65, 79, 94, MAP_LEFT, 318, 578, 356 );
		swprintf(str, lengthof(str), L"Loading map:  %ls", gszDisplayName);
		SetProgressBarTitle( 0, str, BLOCKFONT2, FONT_RED, FONT_NEARBLACK );
		SetProgressBarMsgAttributes( 0, SMALLCOMPFONT, FONT_BLACK, FONT_BLACK );

		if(	ExternalLoadMap( gszDisplayName ) )
		{
			EnableButton( iSummaryButton[ SUMMARY_OKAY ] );
			gsSectorX = gsSelSectorX;
			gsSectorY = gsSelSectorY;
			gfOverrideDirty = TRUE;
			gfMapFileDirty = FALSE;
		}
		RemoveProgressBar( 0 );
		ptr = wcsstr( gszDisplayName, L"_b" );
		if( !ptr || ptr[3] != L'.' )
		{
			gsSectorLayer = GROUND_LEVEL_MASK;
			giCurrLevel = 0;
		}
		else
		{
			switch( ptr[2] - L'0' )
			{
				case 1:		gsSectorLayer = BASEMENT1_LEVEL_MASK;	break;
				case 2:		gsSectorLayer = BASEMENT2_LEVEL_MASK;	break;
				case 3:		gsSectorLayer = BASEMENT3_LEVEL_MASK;	break;
				default:	gsSectorLayer = GROUND_LEVEL_MASK;		break;
			}
		}
	}
}


static void SummarySaveMapCallback(GUI_BUTTON* btn, INT32 reason)
{
	if( reason & MSYS_CALLBACK_REASON_LBUTTON_UP )
	{
		gfRenderSummary = TRUE;
		if( gubOverrideStatus == INACTIVE || gfOverride == TRUE )
		{
			if( gubOverrideStatus == READONLY )
			{
				char filename[40];
				sprintf( filename, "MAPS/%ls", gszDisplayName );
				FileClearAttributes( filename );
			}
			if(	ExternalSaveMap( gszDisplayName ) )
			{
				if( gsSelSectorX && gsSelSectorY )
				{
					gsSectorX = gsSelSectorX;
					gsSectorY = gsSelSectorY;
					gfMapFileDirty = FALSE;
					gfOverrideDirty = TRUE;
				}
			}
		}
	}
}


static void SummaryOverrideCallback(GUI_BUTTON* btn, INT32 reason)
{
	if( reason & MSYS_CALLBACK_REASON_LBUTTON_UP )
	{
		gfOverride ^= TRUE;
		gfRenderSummary = TRUE;
		if( gfOverride )
			EnableButton( iSummaryButton[ SUMMARY_SAVE ] );
		else
			DisableButton( iSummaryButton[ SUMMARY_SAVE ] );
	}
}


static void CalculateOverrideStatus(void)
{
	GETFILESTRUCT FileInfo;
	char szFilename[40];
	gfOverrideDirty = FALSE;
	gfOverride = FALSE;
	if( gfTempFile )
	{
		sprintf( szFilename, "MAPS/%ls", gszTempFilename );
		if( strlen( szFilename ) == 5 )
			strcat( szFilename, "test.dat" );
		char* ptr = strstr( szFilename, "." );
		if( !ptr )
			strcat( szFilename, ".dat" );
		else
			strcpy(ptr, ".dat");
	}
	else
		sprintf( szFilename, "MAPS/%ls", gszFilename );
	swprintf(gszDisplayName, lengthof(gszDisplayName), L"%hs", szFilename + 5);
	if( GetFileFirst( szFilename, &FileInfo) )
	{
		if( gfWorldLoaded )
		{
			if( FileInfo.uiFileAttribs & ( FILE_IS_READONLY | FILE_IS_SYSTEM ) )
				gubOverrideStatus = READONLY;
			else
				gubOverrideStatus = OVERWRITE;
			ShowButton( iSummaryButton[ SUMMARY_OVERRIDE ] );
			ButtonList[ iSummaryButton[ SUMMARY_OVERRIDE ] ]->uiFlags &= (~BUTTON_CLICKED_ON);
			GetFileClose(&FileInfo);
			DisableButton( iSummaryButton[ SUMMARY_SAVE ] );
		}
		if( gfTempFile )
			EnableButton( iSummaryButton[ SUMMARY_LOAD ] );
	}
	else
	{
		gubOverrideStatus = INACTIVE;
		HideButton( iSummaryButton[ SUMMARY_OVERRIDE ] );
		if( gfWorldLoaded )
			EnableButton( iSummaryButton[ SUMMARY_SAVE ] );
	}
}


static void LoadSummary(const char* pSector, UINT8 ubLevel, FLOAT dMajorMapVersion);


static void LoadGlobalSummary(void)
{
	HWFILE	hfile;
	STRING512			DevInfoDir;
	STRING512			MapsDir;
	FLOAT	dMajorVersion;
  INT32 x,y;

	OutputDebugString( "Executing LoadGlobalSummary()...\n" );

	gfMustForceUpdateAllMaps = FALSE;
	gusNumberOfMapsToBeForceUpdated = 0;
	gfGlobalSummaryExists = FALSE;
	//Set current directory to JA2/DevInfo which contains all of the summary data
	const char* ExecDir = GetExecutableDirectory();
	sprintf( DevInfoDir, "%s/DevInfo", ExecDir );
	sprintf( MapsDir, "%s/Data/Maps", ExecDir );

	//Check to make sure we have a DevInfo directory.  If we don't create one!
	if( !SetFileManCurrentDirectory( DevInfoDir ) )
	{
		OutputDebugString( "LoadGlobalSummary() aborted -- doesn't exist on this local computer.\n");
		return;
	}

	//TEMP
	FileDelete( "_global.sum" );

	gfGlobalSummaryExists = TRUE;

	//Analyse all sectors to see if matching maps exist.  For any maps found, the information
	//will be stored in the gbSectorLevels array.  Also, it attempts to load summaries for those
	//maps.  If the summary information isn't found, then the occurrences are recorded and reported
	//to the user when finished to give the option to generate them.
	for( y = 0; y < 16; y++ )
	{
		for( x = 0; x < 16; x++ )
		{
			char szFilename[40];
			char szSector[6];

			gbSectorLevels[x][y] = 0;
			sprintf( szSector, "%c%d", 'A' + y, x + 1 );

			//main ground level
			sprintf( szFilename, "%c%d.dat", 'A' + y, x + 1 );
			SetFileManCurrentDirectory( MapsDir );
			hfile = FileOpen(szFilename, FILE_ACCESS_READ);
			SetFileManCurrentDirectory( DevInfoDir );
			if( hfile )
			{
				gbSectorLevels[x][y] |= GROUND_LEVEL_MASK;
				FileRead(hfile, &dMajorVersion, sizeof(FLOAT));
				FileClose( hfile );
				LoadSummary( szSector, 0, dMajorVersion );
			}
			else
			{
				sprintf( szFilename, "%s.sum", szSector );
				FileDelete( szFilename );
			}
			//main B1 level
			sprintf( szFilename, "%c%d_b1.dat", 'A' + y, x + 1 );
			SetFileManCurrentDirectory( MapsDir );
			hfile = FileOpen(szFilename, FILE_ACCESS_READ);
			SetFileManCurrentDirectory( DevInfoDir );
			if( hfile )
			{
				gbSectorLevels[x][y] |= BASEMENT1_LEVEL_MASK;
				FileRead(hfile, &dMajorVersion, sizeof(FLOAT));
				FileClose( hfile );
				LoadSummary( szSector, 1, dMajorVersion );
			}
			else
			{
				sprintf( szFilename, "%s_b1.sum", szSector );
				FileDelete( szFilename );
			}
			//main B2 level
			sprintf( szFilename, "%c%d_b2.dat", 'A' + y, x + 1 );
			SetFileManCurrentDirectory( MapsDir );
			hfile = FileOpen(szFilename, FILE_ACCESS_READ);
			SetFileManCurrentDirectory( DevInfoDir );
			if( hfile )
			{
				gbSectorLevels[x][y] |= BASEMENT2_LEVEL_MASK;
				FileRead(hfile, &dMajorVersion, sizeof(FLOAT));
				FileClose( hfile );
				LoadSummary( szSector, 2, dMajorVersion );
			}
			else
			{
				sprintf( szFilename, "%s_b2.sum", szSector );
				FileDelete( szFilename );
			}
			//main B3 level
			sprintf( szFilename, "%c%d_b3.dat", 'A' + y, x + 1 );
			SetFileManCurrentDirectory( MapsDir );
			hfile = FileOpen(szFilename, FILE_ACCESS_READ);
			SetFileManCurrentDirectory( DevInfoDir );
			if( hfile )
			{
				gbSectorLevels[x][y] |= BASEMENT3_LEVEL_MASK;
				FileRead(hfile, &dMajorVersion, sizeof(FLOAT));
				FileClose( hfile );
				LoadSummary( szSector, 3, dMajorVersion );
			}
			else
			{
				sprintf( szFilename, "%s_b3.sum", szSector );
				FileDelete( szFilename );
			}
			//alternate ground level
			sprintf( szFilename, "%c%d_a.dat", 'A' + y, x + 1 );
			SetFileManCurrentDirectory( MapsDir );
			hfile = FileOpen(szFilename, FILE_ACCESS_READ);
			SetFileManCurrentDirectory( DevInfoDir );
			if( hfile )
			{
				gbSectorLevels[x][y] |= ALTERNATE_GROUND_MASK;
				FileRead(hfile, &dMajorVersion, sizeof(FLOAT));
				FileClose( hfile );
				LoadSummary( szSector, 4, dMajorVersion );
			}
			else
			{
				sprintf( szFilename, "%s_a.sum", szSector );
				FileDelete( szFilename );
			}
			//alternate B1 level
			sprintf( szFilename, "%c%d_b1_a.dat", 'A' + y, x + 1 );
			SetFileManCurrentDirectory( MapsDir );
			hfile = FileOpen(szFilename, FILE_ACCESS_READ);
			SetFileManCurrentDirectory( DevInfoDir );
			if( hfile )
			{
				gbSectorLevels[x][y] |= ALTERNATE_B1_MASK;
				FileRead(hfile, &dMajorVersion, sizeof(FLOAT));
				FileClose( hfile );
				LoadSummary( szSector, 5, dMajorVersion );
			}
			else
			{
				sprintf( szFilename, "%s_b1_a.sum", szSector );
				FileDelete( szFilename );
			}
			//alternate B2 level
			sprintf( szFilename, "%c%d_b2_a.dat", 'A' + y, x + 1 );
			SetFileManCurrentDirectory( MapsDir );
			hfile = FileOpen(szFilename, FILE_ACCESS_READ);
			SetFileManCurrentDirectory( DevInfoDir );
			if( hfile )
			{
				gbSectorLevels[x][y] |= ALTERNATE_B2_MASK;
				FileRead(hfile, &dMajorVersion, sizeof(FLOAT));
				FileClose( hfile );
				LoadSummary( szSector, 6, dMajorVersion );
			}
			else
			{
				sprintf( szFilename, "%s_b2_a.sum", szSector );
				FileDelete( szFilename );
			}
			//alternate B3 level
			sprintf( szFilename, "%c%d_b3_a.dat", 'A' + y, x + 1 );
			SetFileManCurrentDirectory( MapsDir );
			hfile = FileOpen(szFilename, FILE_ACCESS_READ);
			SetFileManCurrentDirectory( DevInfoDir );
			if( hfile )
			{
				gbSectorLevels[x][y] |= ALTERNATE_B1_MASK;
				FileRead(hfile, &dMajorVersion, sizeof(FLOAT));
				FileClose( hfile );
				LoadSummary( szSector, 7, dMajorVersion );
			}
			else
			{
				sprintf( szFilename, "%s_b3_a.sum", szSector );
				FileDelete( szFilename );
			}
		}
		OutputDebugString( String("Sector Row %c complete... \n", y + 'A') );
	}

	sprintf( MapsDir, "%s/Data", ExecDir );
	SetFileManCurrentDirectory( MapsDir );

	if( gfMustForceUpdateAllMaps )
	{
		OutputDebugString( String( "A MAJOR MAP UPDATE EVENT HAS BEEN DETECTED FOR %d MAPS!!!!.\n", gusNumberOfMapsToBeForceUpdated ) );
	}


	OutputDebugString( "LoadGlobalSummary() finished...\n" );
}


static void UpdateMasterProgress(void);


void WriteSectorSummaryUpdate(char* puiFilename, UINT8 ubLevel, SUMMARYFILE* pSummaryFileInfo)
{
	FILE *fp;
	STRING512			Dir;
	INT8 x, y;

	//Set current directory to JA2/DevInfo which contains all of the summary data
	const char* ExecDir = GetExecutableDirectory();
	sprintf( Dir, "%s/DevInfo", ExecDir );
	if( !SetFileManCurrentDirectory( Dir ) )
		AssertMsg( 0, "JA2/DevInfo folder not found and should exist!");

	char* ptr = strstr(puiFilename, ".dat");
	if( !ptr )
		AssertMsg( 0, "Illegal sector summary filename.");
	strcpy(ptr, ".sum");

	//write the summary information
	fp = fopen( puiFilename, "wb" );
	Assert( fp );
	fwrite( pSummaryFileInfo, 1, sizeof( SUMMARYFILE ), fp );
	fclose( fp );

	gusNumEntriesWithOutdatedOrNoSummaryInfo--;
	UpdateMasterProgress();

	//extract the sector information out of the filename.
	if( puiFilename[0] >= 'a' )
		y = puiFilename[0] - 'a';
	else
		y = puiFilename[0] - 'A';
	if( puiFilename[2] < '0' || puiFilename[2] > '9' )
		x = puiFilename[ 1 ] - '0' - 1;
	else
		x = (puiFilename[ 1 ] - '0') * 10 + puiFilename[ 2 ] - '0' - 1;

	gpSectorSummary[x][y][ubLevel] = pSummaryFileInfo;

	//Set current directory back to data directory!
	sprintf( Dir, "%s/Data", ExecDir );
	SetFileManCurrentDirectory( Dir );
}


static void LoadSummary(const char* pSector, UINT8 ubLevel, FLOAT dMajorMapVersion)
{
	char filename[40];
	SUMMARYFILE temp;
	INT32 x, y;
	FILE *fp;
	strcpy(filename, pSector);
	if( ubLevel % 4 )
	{
		char str[4];
		sprintf( str, "_b%d", ubLevel % 4 );
		strcat( filename, str );
	}
	if( ubLevel >= 4 )
	{
		strcat( filename, "_a" );
	}
	strcat( filename, ".sum" );

	fp = fopen( filename, "rb" );
	if( !fp )
	{
		gusNumEntriesWithOutdatedOrNoSummaryInfo++;
		return;
	}
	fread( &temp, 1, sizeof( SUMMARYFILE ), fp );
	if( temp.ubSummaryVersion < MINIMUMVERSION || dMajorMapVersion < gdMajorMapVersion )
	{
		gusNumberOfMapsToBeForceUpdated++;
		gfMustForceUpdateAllMaps = TRUE;
	}
	temp.dMajorMapVersion = dMajorMapVersion;
	UpdateSummaryInfo( &temp );
	//even if the info is outdated (but existing), allocate the structure, but indicate that the info
	//is bad.
	y = pSector[0] - 'A';
	if( pSector[2] >= '0' && pSector[2] <= '9' )
		x = (pSector[1] - '0') * 10 + pSector[2] - '0' - 1;
	else
		x = pSector[1] - '0' - 1;
	if( gpSectorSummary[x][y][ubLevel] )
	{
		MemFree( gpSectorSummary[x][y][ubLevel] );
		gpSectorSummary[x][y][ubLevel] = NULL;
	}
	gpSectorSummary[x][y][ubLevel] = MALLOC(SUMMARYFILE);
	if( gpSectorSummary[x][y][ubLevel] )
		*gpSectorSummary[x][y][ubLevel] = temp;
	if( gpSectorSummary[x][y][ubLevel]->ubSummaryVersion < GLOBAL_SUMMARY_VERSION )
		gusNumEntriesWithOutdatedOrNoSummaryInfo++;

	fclose( fp );
}


double MasterStart, MasterEnd;


static void UpdateMasterProgress(void)
{
	if( gfUpdatingNow && gusTotal )
	{
		MasterStart = (gusCurrent / (double)gusTotal) * 100.0;
		gusCurrent++;
		MasterEnd = (gusCurrent / (double)gusTotal) * 100.0;
		if( gfMajorUpdate )
		{
			SetRelativeStartAndEndPercentage( 2, (UINT16)MasterStart, (UINT16)MasterEnd, NULL );
			RenderProgressBar( 2, 0 );
		}
		else
			SetRelativeStartAndEndPercentage( 1, (UINT16)MasterStart, (UINT16)MasterEnd, NULL );
	}
}


static void ReportError(const char* pSector, UINT8 ubLevel)
{
	static INT32 yp = 180;
	wchar_t str[40];
	wchar_t temp[10];

	//Make sure the file exists... if not, then return false
	swprintf(str, lengthof(str), L"%hs", pSector);
	if( ubLevel % 4  )
	{
		swprintf(temp, lengthof(temp), L"_b%d.dat", ubLevel % 4);
		wcscat( str, temp );
	}
	mprintf(10, yp, L"Skipping update for %ls.  Probably due to tileset conflicts...", str);
	InvalidateScreen();
	yp++;
}


static void RegenerateSummaryInfoForAllOutdatedMaps(void)
{
	INT32 x, y;
	SUMMARYFILE *pSF;
	//CreateProgressBar( 0, 20, 120, 300, 132 ); //slave (individual)
	//CreateProgressBar( 1, 20, 100, 300, 112 ); //master (total)
	//DefineProgressBarPanel( 0, 65, 79, 94, 10, 80, 310, 152 );
	CreateProgressBar( 0, 20, 100, 300, 112 ); //master (total)
	DefineProgressBarPanel( 0, 65, 79, 94, 10, 80, 310, 132 );
	SetProgressBarTitle( 0, L"Generating map information", BLOCKFONT2, FONT_RED, FONT_NEARBLACK );
	SetProgressBarMsgAttributes( 0, SMALLCOMPFONT, FONT_BLACK, FONT_BLACK );
	gfUpdatingNow = TRUE;

	gusCurrent = 0;
	gusTotal = gusNumEntriesWithOutdatedOrNoSummaryInfo;
	UpdateMasterProgress();

	for( y = 0; y < 16; y++ ) for( x = 0; x < 16; x++ )
	{
		char str[40];
		sprintf( str, "%c%d", y + 'A', x + 1 );
		if( gbSectorLevels[x][y] & GROUND_LEVEL_MASK )
		{
			pSF = gpSectorSummary[x][y][0];
			if( !pSF || pSF->ubSummaryVersion != GLOBAL_SUMMARY_VERSION )
				if( !EvaluateWorld( str, 0 ) )
					ReportError( str, 0 );
		}
		if( gbSectorLevels[x][y] & BASEMENT1_LEVEL_MASK )
		{
			pSF = gpSectorSummary[x][y][1];
			if( !pSF || pSF->ubSummaryVersion != GLOBAL_SUMMARY_VERSION )
				if( !EvaluateWorld( str, 1 ) )
					ReportError( str, 1 );
		}
		if( gbSectorLevels[x][y] & BASEMENT2_LEVEL_MASK )
		{
			pSF = gpSectorSummary[x][y][2];
			if( !pSF || pSF->ubSummaryVersion != GLOBAL_SUMMARY_VERSION )
				if( !EvaluateWorld( str, 2 ) )
					ReportError( str, 2 );
		}
		if( gbSectorLevels[x][y] & BASEMENT3_LEVEL_MASK )
		{
			pSF = gpSectorSummary[x][y][3];
			if( !pSF || pSF->ubSummaryVersion != GLOBAL_SUMMARY_VERSION )
				if( !EvaluateWorld( str, 3 ) )
					ReportError( str, 3 );
		}
		if( gbSectorLevels[x][y] & ALTERNATE_GROUND_MASK )
		{
			pSF = gpSectorSummary[x][y][4];
			if( !pSF || pSF->ubSummaryVersion != GLOBAL_SUMMARY_VERSION )
				if( !EvaluateWorld( str, 4 ) )
					ReportError( str, 4 );
		}
		if( gbSectorLevels[x][y] & ALTERNATE_B1_MASK )
		{
			pSF = gpSectorSummary[x][y][5];
			if( !pSF || pSF->ubSummaryVersion != GLOBAL_SUMMARY_VERSION )
				if( !EvaluateWorld( str, 5 ) )
					ReportError( str, 5 );
		}
		if( gbSectorLevels[x][y] & ALTERNATE_B2_MASK )
		{
			pSF = gpSectorSummary[x][y][6];
			if( !pSF || pSF->ubSummaryVersion != GLOBAL_SUMMARY_VERSION )
				if( !EvaluateWorld( str, 6 ) )
					ReportError( str, 6 );
		}
		if( gbSectorLevels[x][y] & ALTERNATE_B3_MASK )
		{
			pSF = gpSectorSummary[x][y][7];
			if( !pSF || pSF->ubSummaryVersion != GLOBAL_SUMMARY_VERSION )
				if( !EvaluateWorld( str, 7 ) )
					ReportError( str, 7 );
		}
	}
	RemoveProgressBar( 0 );
	RemoveProgressBar( 1 );
	gfUpdatingNow = FALSE;
}


static void SummaryUpdateCallback(GUI_BUTTON* btn, INT32 reason)
{
	if( reason & MSYS_CALLBACK_REASON_LBUTTON_UP )
	{
		CreateProgressBar( 0, 20, 100, 300, 112 ); //slave (individual)
		DefineProgressBarPanel( 0, 65, 79, 94, 10, 80, 310, 132 );
		SetProgressBarTitle( 0, L"Generating map summary", BLOCKFONT2, FONT_RED, FONT_NEARBLACK );
		SetProgressBarMsgAttributes( 0, SMALLCOMPFONT, FONT_BLACK, FONT_BLACK );

		if( gpCurrentSectorSummary )
		{
			MemFree( gpCurrentSectorSummary );
			gpCurrentSectorSummary = NULL;
		}

		char str[40];
		sprintf( str, "%c%d", gsSelSectorY + 'A' - 1, gsSelSectorX );
		EvaluateWorld( str, (UINT8)giCurrLevel );

		gpSectorSummary[ gsSelSectorX ][ gsSelSectorY ][ giCurrLevel ] = gpCurrentSectorSummary;

		gfRenderSummary = TRUE;

		RemoveProgressBar( 0 );
	}
}


static void ExtractTempFilename(void)
{
	wchar_t str[40];
	Get16BitStringFromField(1, str, lengthof(str));
	if( wcscmp( gszTempFilename, str ) )
	{
		wcscpy( gszTempFilename, str );
		gfRenderSummary = TRUE;
		gfOverrideDirty = TRUE;
	}
	if( !wcslen( str ) )
		wcslcpy(gszDisplayName, L"test.dat", lengthof(gszDisplayName));
}

void ApologizeOverrideAndForceUpdateEverything()
{
	INT32 x, y;
	wchar_t str[50];
	SUMMARYFILE *pSF;
	//Create one huge assed button
	gfMajorUpdate = TRUE;
	iSummaryButton[SUMMARY_BACKGROUND] = CreateLabel(NULL, 0, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, MSYS_PRIORITY_HIGH - 1);
	//Draw it
	DrawButton( iSummaryButton[ SUMMARY_BACKGROUND ] );
	InvalidateScreen();
	SetFont( HUGEFONT );
	SetFontForeground( FONT_RED );
	SetFontShadow( FONT_NEARBLACK );
	const wchar_t* UpdateInfo = L"MAJOR VERSION UPDATE";
	mprintf((SCREEN_WIDTH - StringPixLength(UpdateInfo, HUGEFONT)) / 2, 105, UpdateInfo);
	SetFont( FONT10ARIAL );
	SetFontForeground( FONT_YELLOW );
	swprintf(str, lengthof(str), L"There are %d maps requiring a major version update.", gusNumberOfMapsToBeForceUpdated);
	mprintf((SCREEN_WIDTH - StringPixLength(str, FONT10ARIAL)) / 2, 130, str);

	CreateProgressBar( 2, 120, 170, 520, 202 );
	DefineProgressBarPanel( 2, 65, 79, 94, 100, 150, 540, 222 );
	SetProgressBarTitle( 2, L"Updating all outdated maps", BLOCKFONT2, FONT_RED, 0 );
	SetProgressBarMsgAttributes( 2, SMALLCOMPFONT, FONT_BLACK, FONT_BLACK );

	gusCurrent = 0;
	gusTotal = gusNumberOfMapsToBeForceUpdated;
	gfUpdatingNow = TRUE;
	UpdateMasterProgress();

	for( y = 0; y < 16; y++ ) for( x = 0; x < 16; x++ )
	{
		char name[50];
		sprintf( name, "%c%d", y + 'A', x + 1 );
		if( gbSectorLevels[x][y] & GROUND_LEVEL_MASK )
		{
			pSF = gpSectorSummary[x][y][0];
			if( !pSF || pSF->ubSummaryVersion < MINIMUMVERSION || pSF->dMajorMapVersion < gdMajorMapVersion )
			{
				gpCurrentSectorSummary = pSF;
				if( !EvaluateWorld( name, 0 ) )
					return;
			}
		}
		if( gbSectorLevels[x][y] & BASEMENT1_LEVEL_MASK )
		{
			pSF = gpSectorSummary[x][y][1];
			if( !pSF || pSF->ubSummaryVersion < MINIMUMVERSION || pSF->dMajorMapVersion < gdMajorMapVersion )
			{
				gpCurrentSectorSummary = pSF;
				if( !EvaluateWorld( name, 1 ) )
					return;
			}
		}
		if( gbSectorLevels[x][y] & BASEMENT2_LEVEL_MASK )
		{
			pSF = gpSectorSummary[x][y][2];
			if( !pSF || pSF->ubSummaryVersion < MINIMUMVERSION || pSF->dMajorMapVersion < gdMajorMapVersion )
			{
				gpCurrentSectorSummary = pSF;
				if( !EvaluateWorld( name, 2 ) )
					return;
			}
		}
		if( gbSectorLevels[x][y] & BASEMENT3_LEVEL_MASK )
		{
			pSF = gpSectorSummary[x][y][3];
			if( !pSF || pSF->ubSummaryVersion < MINIMUMVERSION || pSF->dMajorMapVersion < gdMajorMapVersion )
			{
				gpCurrentSectorSummary = pSF;
				if( !EvaluateWorld( name, 3 ) )
					return;
			}
		}
		if( gbSectorLevels[x][y] & ALTERNATE_GROUND_MASK )
		{
			pSF = gpSectorSummary[x][y][4];
			if( !pSF || pSF->ubSummaryVersion < MINIMUMVERSION || pSF->dMajorMapVersion < gdMajorMapVersion )
			{
				gpCurrentSectorSummary = pSF;
				if( !EvaluateWorld( name, 4 ) )
					return;
			}
		}
		if( gbSectorLevels[x][y] & ALTERNATE_B1_MASK )
		{
			pSF = gpSectorSummary[x][y][5];
			if( !pSF || pSF->ubSummaryVersion < MINIMUMVERSION || pSF->dMajorMapVersion < gdMajorMapVersion )
			{
				gpCurrentSectorSummary = pSF;
				if( !EvaluateWorld( name, 5 ) )
					return;
			}
		}
		if( gbSectorLevels[x][y] & ALTERNATE_B2_MASK )
		{
			pSF = gpSectorSummary[x][y][6];
			if( !pSF || pSF->ubSummaryVersion < MINIMUMVERSION || pSF->dMajorMapVersion < gdMajorMapVersion )
			{
				gpCurrentSectorSummary = pSF;
				if( !EvaluateWorld( name, 6 ) )
					return;
			}
		}
		if( gbSectorLevels[x][y] & ALTERNATE_B3_MASK )
		{
			pSF = gpSectorSummary[x][y][7];
			if( !pSF || pSF->ubSummaryVersion < MINIMUMVERSION || pSF->dMajorMapVersion < gdMajorMapVersion )
			{
				gpCurrentSectorSummary = pSF;
				if( !EvaluateWorld( name, 7 ) )
					return;
			}
		}
	}

	EvaluateWorld( "p3_m.dat", 0 );

	RemoveProgressBar( 2 );
	gfUpdatingNow = FALSE;
	InvalidateScreen();

	RemoveButton( iSummaryButton[ SUMMARY_BACKGROUND ] );
	gfMajorUpdate = FALSE;
	gfMustForceUpdateAllMaps = FALSE;
	gusNumberOfMapsToBeForceUpdated = 0;
}


static void SetupItemDetailsMode(BOOLEAN fAllowRecursion)
{
	HWFILE hfile;
	UINT32 uiNumItems;
	BASIC_SOLDIERCREATE_STRUCT basic;
	SOLDIERCREATE_STRUCT priority;
	INT32 i, j;
	UINT16 usNumItems;
	OBJECTTYPE *pItem;
	UINT16 usPEnemyIndex, usNEnemyIndex;

	//Clear memory for all the item summaries loaded
	if( gpWorldItemsSummaryArray )
	{
		MemFree( gpWorldItemsSummaryArray );
		gpWorldItemsSummaryArray = NULL;
		gusWorldItemsSummaryArraySize = 0;
	}
	if( gpPEnemyItemsSummaryArray )
	{
		MemFree( gpPEnemyItemsSummaryArray );
		gpPEnemyItemsSummaryArray = NULL;
		gusPEnemyItemsSummaryArraySize = 0;
	}
	if( gpNEnemyItemsSummaryArray )
	{
		MemFree( gpNEnemyItemsSummaryArray );
		gpNEnemyItemsSummaryArray = NULL;
		gusNEnemyItemsSummaryArraySize = 0;
	}

	if( !gpCurrentSectorSummary->uiNumItemsPosition )
	{	//Don't have one, so generate them
		if( gpCurrentSectorSummary->ubSummaryVersion == GLOBAL_SUMMARY_VERSION )
			gusNumEntriesWithOutdatedOrNoSummaryInfo++;
		SummaryUpdateCallback( ButtonList[ iSummaryButton[ SUMMARY_UPDATE ] ], MSYS_CALLBACK_REASON_LBUTTON_UP );
		gpCurrentSectorSummary = gpSectorSummary[ gsSelSectorX - 1 ][ gsSelSectorY - 1 ][ giCurrLevel ];
	}
	//Open the original map for the sector
	char szFilename[40];
	sprintf( szFilename, "MAPS/%ls", gszFilename );
	hfile = FileOpen(szFilename, FILE_ACCESS_READ);
	if( !hfile )
	{ //The file couldn't be found!
		return;
	}
	//Now fileseek directly to the file position where the number of world items are stored
	if( !FileSeek( hfile, gpCurrentSectorSummary->uiNumItemsPosition, FILE_SEEK_FROM_START ) )
	{ //Position couldn't be found!
		FileClose( hfile );
		return;
	}
	//Now load the number of world items from the map.
	if (!FileRead(hfile, &uiNumItems, 4))
	{ //Invalid situation.
		FileClose( hfile );
		return;
	}
	//Now compare this number with the number the summary thinks we should have.  If they are different,
	//the the summary doesn't match the map.  What we will do is force regenerate the map so that they do
	//match
	if( uiNumItems != gpCurrentSectorSummary->usNumItems && fAllowRecursion )
	{
		FileClose( hfile );
		gpCurrentSectorSummary->uiNumItemsPosition = 0;
		SetupItemDetailsMode( FALSE );
		return;
	}
	//Passed the gauntlet, so now allocate memory for it, and load all the world items into this array.
	ShowButton( iSummaryButton[ SUMMARY_SCIFI ] );
	ShowButton( iSummaryButton[ SUMMARY_REAL ] );
	ShowButton( iSummaryButton[ SUMMARY_ENEMY ] );
	gpWorldItemsSummaryArray = MALLOCN(WORLDITEM, uiNumItems);
	gusWorldItemsSummaryArraySize = gpCurrentSectorSummary->usNumItems;
	FileRead(hfile, gpWorldItemsSummaryArray, sizeof(WORLDITEM) * uiNumItems);

	//NOW, do the enemy's items!
	//We need to do two passes.  The first pass simply processes all the enemies and counts all the droppable items
	//keeping track of two different values.  The first value is the number of droppable items that come off of
	//enemy detailed placements, the other counter keeps track of the number of droppable items that come off of
	//normal enemy placements.  After doing this, the memory is allocated for the tables that will store all the item
	//summary information, then the second pass will repeat the process, except it will record the actual items.

	//PASS #1
	if( !FileSeek( hfile, gpCurrentSectorSummary->uiEnemyPlacementPosition, FILE_SEEK_FROM_START ) )
	{ //Position couldn't be found!
		FileClose( hfile );
		return;
	}
	for( i = 0; i < gpCurrentSectorSummary->MapInfo.ubNumIndividuals ; i++ )
	{
		if (!FileRead(hfile, &basic, sizeof(BASIC_SOLDIERCREATE_STRUCT)))
		{ //Invalid situation.
			FileClose( hfile );
			return;
		}
		if( basic.fDetailedPlacement )
		{ //skip static priority placement
			if (!ExtractSoldierCreateFromFileUTF16(hfile, &priority))
			{ //Invalid situation.
				FileClose( hfile );
				return;
			}
		}
		else
		{ //non detailed placements don't have items, so skip
			continue;
		}
		if( basic.bTeam == ENEMY_TEAM )
		{
			//Count the items that this enemy placement drops
			usNumItems = 0;
			for( j = 0; j < 9; j++ )
			{
				pItem = &priority.Inv[ gbMercSlotTypes[ j ] ];
				if( pItem->usItem != NOTHING && !( pItem->fFlags & OBJECT_UNDROPPABLE ) )
				{
					usNumItems++;
				}
			}
			if( basic.fPriorityExistance )
			{
				gusPEnemyItemsSummaryArraySize += usNumItems;
			}
			else
			{
				gusNEnemyItemsSummaryArraySize += usNumItems;
			}
		}
	}

	//Pass 1 completed, so now allocate enough space to hold all the items
	if( gusPEnemyItemsSummaryArraySize )
	{
		gpPEnemyItemsSummaryArray = MALLOCNZ(OBJECTTYPE, gusPEnemyItemsSummaryArraySize);
	}
	if( gusNEnemyItemsSummaryArraySize )
	{
		gpNEnemyItemsSummaryArray = MALLOCNZ(OBJECTTYPE, gusNEnemyItemsSummaryArraySize);
	}

	//PASS #2
	//During this pass, simply copy all the data instead of counting it, now that we have already done so.
	usPEnemyIndex = usNEnemyIndex = 0;
	if( !FileSeek( hfile, gpCurrentSectorSummary->uiEnemyPlacementPosition, FILE_SEEK_FROM_START ) )
	{ //Position couldn't be found!
		FileClose( hfile );
		return;
	}
	for( i = 0; i < gpCurrentSectorSummary->MapInfo.ubNumIndividuals ; i++ )
	{
		if (!FileRead(hfile, &basic, sizeof(BASIC_SOLDIERCREATE_STRUCT)))
		{ //Invalid situation.
			FileClose( hfile );
			return;
		}
		if( basic.fDetailedPlacement )
		{ //skip static priority placement
			if (!ExtractSoldierCreateFromFileUTF16(hfile, &priority))
			{ //Invalid situation.
				FileClose( hfile );
				return;
			}
		}
		else
		{ //non detailed placements don't have items, so skip
			continue;
		}
		if( basic.bTeam == ENEMY_TEAM )
		{
			//Copy the items that this enemy placement drops
			usNumItems = 0;
			for( j = 0; j < 9; j++ )
			{
				pItem = &priority.Inv[ gbMercSlotTypes[ j ] ];
				if( pItem->usItem != NOTHING && !( pItem->fFlags & OBJECT_UNDROPPABLE ) )
				{
					if( basic.fPriorityExistance )
					{
						gpPEnemyItemsSummaryArray[usPEnemyIndex] = *pItem;
						usPEnemyIndex++;
					}
					else
					{
						gpNEnemyItemsSummaryArray[usNEnemyIndex] = *pItem;
						usNEnemyIndex++;
					}
				}
			}
		}
	}
	FileClose( hfile );
}

#endif
