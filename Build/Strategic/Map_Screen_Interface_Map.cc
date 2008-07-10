#include "Font.h"
#include "Font_Control.h"
#include "HImage.h"
#include "Interface.h"
#include "Local.h"
#include "MapScreen.h"
#include "Map_Screen_Interface.h"
#include "Map_Screen_Interface_Map.h"
#include "Map_Screen_Interface_Border.h"
#include "Merc_Hiring.h"
#include "Overhead.h"
#include "Render_Dirty.h"
#include "SysUtil.h"
#include "Strategic.h"
#include "StrategicMap.h"
#include "Strategic_Pathing.h"
#include "Text.h"
#include "Timer_Control.h"
#include "VObject.h"
#include "VSurface.h"
#include "Video.h"
#include "VObject_Blitters.h"
#include "Assignments.h"
#include "Squads.h"
#include "Message.h"
#include "Soldier_Profile.h"
#include "Player_Command.h"
#include "Strategic_Movement.h"
#include "Queen_Command.h"
#include "Campaign_Types.h"
#include "Strategic_Town_Loyalty.h"
#include "Strategic_Mines.h"
#include "Vehicles.h"
#include "Map_Screen_Helicopter.h"
#include "Game_Clock.h"
#include "Finances.h"
#include "Line.h"
#include "PreBattle_Interface.h"
#include "Town_Militia.h"
#include "Militia_Control.h"
#include "Tactical_Save.h"
#include "Map_Information.h"
#include "Air_Raid.h"
#include "MemMan.h"
#include "Button_System.h"
#include "Debug.h"


// zoom x and y coords for map scrolling
INT32 iZoomX = 0;
INT32 iZoomY = 0;

// Scroll region width
#define SCROLL_REGION 4

// The Map/Mouse Scroll defines
#define EAST_DIR  0
#define WEST_DIR  1
#define NORTH_DIR 2
#define SOUTH_DIR 3
#define TOP_NORTH  2
#define TOP_SOUTH  13
#define RIGHT_WEST 250
#define RIGHT_EAST 260
#define LEFT_EAST  640
#define LEFT_WEST  630
#define BOTTOM_NORTH 320
#define BOTTOM_SOUTH 330

// Map Scroll Defines
#define SCROLL_EAST  0
#define SCROLL_WEST  1
#define SCROLL_NORTH 2
#define SCROLL_SOUTH 3
#define SCROLL_DELAY 50
#define HORT_SCROLL 14
#define VERT_SCROLL 10

// the pop up for helicopter stuff
#define MAP_HELICOPTER_ETA_POPUP_X 400
#define MAP_HELICOPTER_ETA_POPUP_Y 250
#define MAP_HELICOPTER_UPPER_ETA_POPUP_Y 50
#define MAP_HELICOPTER_ETA_POPUP_WIDTH 120
#define MAP_HELICOPTER_ETA_POPUP_HEIGHT 68

#define MAP_LEVEL_STRING_X 432
#define MAP_LEVEL_STRING_Y 305

// font
#define MAP_FONT BLOCKFONT2

// index color
#define MAP_INDEX_COLOR 32*4-9

// max number of sectors viewable
#define MAX_VIEW_SECTORS      16


//Map Location index regions

// x start of hort index
#define MAP_HORT_INDEX_X 292

// y position of hort index
#define MAP_HORT_INDEX_Y  10

// height of hort index
#define MAP_HORT_HEIGHT  GetFontHeight(MAP_FONT)

// vert index start x
#define MAP_VERT_INDEX_X 273

// vert index start y
#define MAP_VERT_INDEX_Y  31

// vert width
#define MAP_VERT_WIDTH   GetFontHeight(MAP_FONT)

// "Boxes" Icons
#define SMALL_YELLOW_BOX			0
#define BIG_YELLOW_BOX				1
#define SMALL_DULL_YELLOW_BOX	2
#define BIG_DULL_YELLOW_BOX		3
#define SMALL_WHITE_BOX				4
#define BIG_WHITE_BOX					5
#define SMALL_RED_BOX					6
#define BIG_RED_BOX						7
#define SMALL_QUESTION_MARK		8
#define BIG_QUESTION_MARK			9


#define MERC_ICONS_PER_LINE 6
#define ROWS_PER_SECTOR			5

#define MAP_X_ICON_OFFSET   2
#define MAP_Y_ICON_OFFSET   1

// Arrow Offsets
#define UP_X                13
#define UP_Y                 7
#define DOWN_X               0
#define DOWN_Y              -2
#define RIGHT_X             -2
#define RIGHT_Y             11
#define LEFT_X               2
#define LEFT_Y               5


// The Path Lines
#define NORTH_LINE           1
#define SOUTH_LINE           0
#define WEST_LINE            3
#define EAST_LINE            2
#define N_TO_E_LINE          4
#define E_TO_S_LINE          5
#define W_TO_N_LINE          6
#define S_TO_W_LINE          7
#define W_TO_S_LINE          8
#define N_TO_W_LINE          9
#define S_TO_E_LINE         10
#define E_TO_N_LINE         11
#define W_TO_E_LINE         12
#define N_TO_S_LINE         13
#define E_TO_W_LINE         14
#define S_TO_N_LINE         15
#define W_TO_E_PART1_LINE   16
#define W_TO_E_PART2_LINE   17
#define E_TO_W_PART1_LINE   18
#define E_TO_W_PART2_LINE   19
#define N_TO_S_PART1_LINE   20
#define N_TO_S_PART2_LINE   21
#define S_TO_N_PART1_LINE   22
#define S_TO_N_PART2_LINE   23
#define GREEN_X_WEST        36
#define GREEN_X_EAST        37
#define GREEN_X_NORTH       38
#define GREEN_X_SOUTH       39
#define RED_X_WEST          40
#define RED_X_EAST          41
#define RED_X_NORTH         42
#define RED_X_SOUTH         43

// The arrows
#define Y_NORTH_ARROW       24
#define Y_SOUTH_ARROW       25
#define Y_EAST_ARROW        26
#define Y_WEST_ARROW        27
#define W_NORTH_ARROW       28
#define W_SOUTH_ARROW       29
#define W_EAST_ARROW        30
#define W_WEST_ARROW        31
#define NORTH_ARROW         32
#define SOUTH_ARROW         33
#define EAST_ARROW          34
#define WEST_ARROW          35

#define ZOOM_Y_NORTH_ARROW  68
#define ZOOM_Y_SOUTH_ARROW  69
#define ZOOM_Y_EAST_ARROW   70
#define ZOOM_Y_WEST_ARROW   71
#define ZOOM_W_NORTH_ARROW  72
#define ZOOM_W_SOUTH_ARROW  73
#define ZOOM_W_EAST_ARROW   74
#define ZOOM_W_WEST_ARROW   75
#define ZOOM_NORTH_ARROW    76
#define ZOOM_SOUTH_ARROW    77
#define ZOOM_EAST_ARROW     78
#define ZOOM_WEST_ARROW     79
#define ARROW_DELAY         20
#define PAUSE_DELAY       1000

// The zoomed in path lines
#define SOUTH_ZOOM_LINE         44
#define NORTH_ZOOM_LINE         45
#define EAST_ZOOM_LINE          46
#define WEST_ZOOM_LINE          47
#define N_TO_E_ZOOM_LINE        48
#define E_TO_S_ZOOM_LINE        49
#define W_TO_N_ZOOM_LINE        50
#define S_TO_W_ZOOM_LINE        51
#define W_TO_S_ZOOM_LINE        52
#define N_TO_W_ZOOM_LINE        53
#define S_TO_E_ZOOM_LINE        54
#define E_TO_N_ZOOM_LINE        55
#define W_TO_E_ZOOM_LINE        56
#define N_TO_S_ZOOM_LINE        57
#define E_TO_W_ZOOM_LINE        58
#define S_TO_N_ZOOM_LINE        59
#define ZOOM_GREEN_X_WEST       80
#define ZOOM_GREEN_X_EAST       81
#define ZOOM_GREEN_X_NORTH      82
#define ZOOM_GREEN_X_SOUTH      83
#define ZOOM_RED_X_WEST         84
#define ZOOM_RED_X_EAST         85
#define ZOOM_RED_X_NORTH        86
#define ZOOM_RED_X_SOUTH        87

#define CHAR_FONT_COLOR 32*4-9

// Arrow Offsets
#define EAST_OFFSET_X  11
#define EAST_OFFSET_Y  0
#define NORTH_OFFSET_X 9
#define NORTH_OFFSET_Y -9
#define SOUTH_OFFSET_X -9
#define SOUTH_OFFSET_Y 9
#define WEST_OFFSET_X  -11
#define WEST_OFFSET_Y  0
#define WEST_TO_SOUTH_OFFSET_Y  0
#define EAST_TO_NORTH_OFFSET_Y  0
#define RED_WEST_OFF_X  -MAP_GRID_X
#define RED_EAST_OFF_X  MAP_GRID_X
#define RED_NORTH_OFF_Y -21
#define RED_SOUTH_OFF_Y 21

// the font use on the mvt icons for mapscreen
#define MAP_MVT_ICON_FONT SMALLCOMPFONT


// map shading colors

#ifndef JA2DEMO
enum{
	MAP_SHADE_BLACK =0,
	MAP_SHADE_LT_GREEN,
	MAP_SHADE_DK_GREEN,
	MAP_SHADE_LT_RED,
	MAP_SHADE_DK_RED,
};
// the big map .pcx
SGPVSurface* guiBIGMAP;
#endif

// orta .sti icon
SGPVObject* guiORTAICON;
SGPVObject* guiTIXAICON;

// boxes for characters on the map
SGPVObject* guiCHARICONS;

#ifndef JA2DEMO
// the merc arrival sector landing zone icon
SGPVObject* guiBULLSEYE;
#endif


// the max allowable towns militia in a sector
#define MAP_MILITIA_MAP_X 4
#define MAP_MILITIA_MAP_Y 20
#define MAP_MILITIA_LOWER_ROW_Y 142
#define NUMBER_OF_MILITIA_ICONS_PER_LOWER_ROW 25
#define MILITIA_BOX_ROWS 3
#define MILITIA_BOX_BOX_HEIGHT 36
#define MILITIA_BOX_BOX_WIDTH 42
#define MAP_MILITIA_BOX_POS_X 400
#define MAP_MILITIA_BOX_POS_Y 125

#define POPUP_MILITIA_ICONS_PER_ROW 5				// max 6 rows gives the limit of 30 militia
#define MEDIUM_MILITIA_ICON_SPACING 5
#define LARGE_MILITIA_ICON_SPACING  6

#define MILITIA_BTN_OFFSET_X 26
#define MILITIA_BTN_HEIGHT 11
#define MILITIA_BOX_WIDTH 133
#define MILITIA_BOX_TEXT_OFFSET_Y 4
#define MILITIA_BOX_UNASSIGNED_TEXT_OFFSET_Y 132
#define MILITIA_BOX_TEXT_TITLE_HEIGHT 13

#define MAP_MILITIA_BOX_AUTO_BOX_X 4
#define MAP_MILITIA_BOX_AUTO_BOX_Y 167
#define MAP_MILITIA_BOX_DONE_BOX_X 67

#define HELI_ICON					0
#define HELI_SHADOW_ICON	1

#define HELI_ICON_WIDTH		20
#define HELI_ICON_HEIGHT	10
#define HELI_SHADOW_ICON_WIDTH	19
#define HELI_SHADOW_ICON_HEIGHT	11


// the militia box buttons
static GUIButtonRef giMapMilitiaButton[5];


static const INT16 gsMilitiaSectorButtonColors[] =
{
	FONT_LTGREEN,
	FONT_LTBLUE,
	16
};

// track number of townspeople picked up
INT16 sGreensOnCursor = 0;
INT16 sRegularsOnCursor = 0;
INT16 sElitesOnCursor = 0;

// the current militia town id
INT16 sSelectedMilitiaTown = 0;


// sublevel graphics
SGPVObject* guiSubLevel1;
SGPVObject* guiSubLevel2;
SGPVObject* guiSubLevel3;

// the between sector icons
SGPVObject* guiCHARBETWEENSECTORICONS;
SGPVObject* guiCHARBETWEENSECTORICONSCLOSE;

// tixa found
BOOLEAN fFoundTixa = FALSE;

// selected sector
UINT16		sSelMapX = 9;
UINT16		sSelMapY = 1;

// highlighted sector
INT16 gsHighlightSectorX=-1;
INT16 gsHighlightSectorY=-1;

// the current sector Z value of the map being displayed
INT32 iCurrentMapSectorZ = 0;

// the palettes
static UINT16* pMapLTRedPalette;
static UINT16* pMapDKRedPalette;
static UINT16* pMapLTGreenPalette;
static UINT16* pMapDKGreenPalette;


// the map border eta pop up
SGPVObject* guiMapBorderEtaPopUp;

// heli pop up
SGPVObject* guiMapBorderHeliSectors;

// list of map sectors that player isn't allowed to even highlight
BOOLEAN sBadSectorsList[ WORLD_MAP_X ][ WORLD_MAP_X ];


INT16 sBaseSectorList[]={
	// NOTE: These co-ordinates must match the top left corner of the 3x3 town tiles cutouts in Interface/MilitiaMaps.sti!
	SECTOR(  9, 1 ), // Omerta
	SECTOR( 13, 2 ), // Drassen
	SECTOR( 13, 8 ), // Alma
	SECTOR(  1, 7 ), // Grumm
	SECTOR(  8, 9 ), // Tixa
	SECTOR(  8, 6 ), // Cambria
	SECTOR(  4, 2 ), // San Mona
	SECTOR(  5, 8 ), // Estoni
	SECTOR(  3,10 ), // Orta
	SECTOR( 11,11 ), // Balime
	SECTOR(  3,14 ), // Meduna
	SECTOR(  2, 1 ), // Chitzena
};

// position of town names on the map
// these are no longer PIXELS, but 10 * the X,Y position in SECTORS (fractions possible) to the X-CENTER of the town
static const SGPPoint pTownPoints[] =
{
	{ 0 ,  0 },
	{ 90, 10}, // Omerta
	{125, 40}, // Drassen
	{130, 90}, // Alma
	{ 15, 80}, // Grumm
	{ 85,100}, // Tixa
	{ 95, 70}, // Cambria
	{ 45, 40}, // San Mona
	{ 55, 90}, // Estoni
	{ 35,110}, // Orta
	{110,120}, // Balime
	{ 45,150}, // Meduna
	{ 15, 20}, // Chitzena
};


INT16 gpSamSectorX[] = { SAM_1_X, SAM_2_X, SAM_3_X, SAM_4_X };
INT16 gpSamSectorY[] = { SAM_1_Y, SAM_2_Y, SAM_3_Y, SAM_4_Y };


// map region
SGPRect MapScreenRect={	(MAP_VIEW_START_X+MAP_GRID_X - 2),	( MAP_VIEW_START_Y+MAP_GRID_Y - 1), MAP_VIEW_START_X + MAP_VIEW_WIDTH - 1 + MAP_GRID_X , MAP_VIEW_START_Y+MAP_VIEW_HEIGHT-10+MAP_GRID_Y};

static SGPRect gOldClipRect;

// screen region
SGPRect FullScreenRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

// temp helicopter path
PathSt* pTempHelicopterPath = NULL;

// character temp path
PathSt* pTempCharacterPath = NULL;

// draw temp path?
BOOLEAN fDrawTempHeliPath = FALSE;

// the map arrows graphics
SGPVObject* guiMAPCURSORS;

// destination plotting character
INT8 bSelectedDestChar = -1;

// assignment selection character
INT8 bSelectedAssignChar=-1;

// current contract char
INT8 bSelectedContractChar = -1;


// has the temp path for character or helicopter been already drawn?
BOOLEAN   fTempPathAlreadyDrawn = FALSE;

// the regions for the mapscreen militia box
MOUSE_REGION gMapScreenMilitiaBoxRegions[ 9 ];
MOUSE_REGION gMapScreenMilitiaRegion;

// the mine icon
SGPVObject* guiMINEICON;

// militia graphics
static SGPVObject* guiMilitia;
static SGPVObject* guiMilitiaMaps;
static SGPVObject* guiMilitiaSectorHighLight;
static SGPVObject* guiMilitiaSectorOutline;

// the sector that is highlighted on the militia map
INT16 sSectorMilitiaMapSector = -1;
BOOLEAN fMilitiaMapButtonsCreated = FALSE;
INT16 sSectorMilitiaMapSectorOutline = -1;


// have any nodes in the current path list been deleted?
BOOLEAN fDeletedNode = FALSE;

UINT16 gusUndergroundNearBlack;

BOOLEAN gfMilitiaPopupCreated = FALSE;

INT32 giAnimateRouteBaseTime = 0;
INT32 giPotHeliPathBaseTime = 0;


void DrawMapIndexBigMap( BOOLEAN fSelectedCursorIsYellow )
{
	// this procedure will draw the coord indexes on the zoomed out map
	INT16 usX, usY;
	INT32 iCount=0;
	BOOLEAN fDrawCursors;

	SetFontDestBuffer(FRAME_BUFFER, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
  //SetFontColors(FONT_FCOLOR_GREEN)
  SetFont(MAP_FONT);
  SetFontForeground(MAP_INDEX_COLOR);
	// Dk Red is 163
  SetFontBackground(FONT_MCOLOR_BLACK);

	fDrawCursors = CanDrawSectorCursor( );

	for(iCount=1; iCount <= MAX_VIEW_SECTORS; iCount++)
	{
		if (fDrawCursors && iCount == sSelMapX && bSelectedDestChar == -1 && !fPlotForHelicopter)
		SetFontForeground(fSelectedCursorIsYellow ? FONT_YELLOW : FONT_WHITE);
   else if( fDrawCursors && ( iCount == gsHighlightSectorX ) )
    SetFontForeground(FONT_WHITE);
   else
    SetFontForeground(MAP_INDEX_COLOR);

	 FindFontCenterCoordinates(MAP_HORT_INDEX_X + (iCount - 1) * MAP_GRID_X, MAP_HORT_INDEX_Y, MAP_GRID_X, MAP_HORT_HEIGHT, pMapHortIndex[iCount], MAP_FONT, &usX, &usY);
		MPrint(usX, usY, pMapHortIndex[iCount]);

		if (fDrawCursors && iCount == sSelMapY && bSelectedDestChar == -1 && !fPlotForHelicopter)
		SetFontForeground(fSelectedCursorIsYellow ? FONT_YELLOW : FONT_WHITE);
   else if( fDrawCursors && ( iCount == gsHighlightSectorY ) )
    SetFontForeground(FONT_WHITE);
   else
    SetFontForeground(MAP_INDEX_COLOR);

	 FindFontCenterCoordinates(MAP_VERT_INDEX_X, MAP_VERT_INDEX_Y + (iCount - 1) * MAP_GRID_Y, MAP_HORT_HEIGHT, MAP_GRID_Y, pMapVertIndex[iCount], MAP_FONT, &usX, &usY);
		MPrint(usX, usY, pMapVertIndex[iCount]);
	}

  InvalidateRegion(MAP_VERT_INDEX_X, MAP_VERT_INDEX_Y,MAP_VERT_INDEX_X+MAP_HORT_HEIGHT,  MAP_VERT_INDEX_Y+( iCount - 1 ) * MAP_GRID_Y );
  InvalidateRegion(MAP_HORT_INDEX_X, MAP_HORT_INDEX_Y,MAP_HORT_INDEX_X + ( iCount - 1) * MAP_GRID_X,  MAP_HORT_INDEX_Y+ MAP_HORT_HEIGHT);

	SetFontDestBuffer(FRAME_BUFFER, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}


/*
void DrawMapIndexSmallMap( BOOLEAN fSelectedCursorIsYellow )
{
	// this procedure will draw the coord indexes on the zoomed in map
	INT16 usX, usY;
	INT32 iCount=0;
	BOOLEAN fDrawCursors;


	SetFont(MAP_FONT);
	SetFontDestBuffer(FRAME_BUFFER, MAP_HORT_INDEX_X, MAP_HORT_INDEX_Y, MAP_HORT_INDEX_X + MAX_VIEW_SECTORS * MAP_GRID_X, MAP_HORT_INDEX_Y + MAP_GRID_Y);
  //SetFontColors(FONT_FCOLOR_GREEN)
  SetFont(MAP_FONT);
  SetFontForeground(MAP_INDEX_COLOR);
  SetFontBackground(FONT_MCOLOR_BLACK);

	fDrawCursors = CanDrawSectorCursor( );

	for(iCount=1; iCount <= MAX_VIEW_SECTORS; iCount++)
	{
		if (fDrawCursors && iCount == sSelMapX && bSelectedDestChar == -1 && !fPlotForHelicopter)
		SetFontForeground(fSelectedCursorIsYellow ? FONT_YELLOW : FONT_WHITE);
   else if( fDrawCursors && ( iCount == gsHighlightSectorX ) )
    SetFontForeground(FONT_WHITE);
   else
    SetFontForeground(MAP_INDEX_COLOR);

   FindFontCenterCoordinates(MAP_HORT_INDEX_X + iCount * MAP_GRID_X * 2 - iZoomX, MAP_HORT_INDEX_Y, MAP_GRID_X * 2, MAP_HORT_HEIGHT, pMapHortIndex[iCount], MAP_FONT, &usX, &usY);
		MPrint(usX, usY, pMapHortIndex[iCount]);
  }
	SetFontDestBuffer(FRAME_BUFFER, MAP_VERT_INDEX_X, MAP_VERT_INDEX_Y, MAP_VERT_INDEX_X + MAP_GRID_X, MAP_VERT_INDEX_Y + MAX_VIEW_SECTORS * MAP_GRID_Y);

	for(iCount=1; iCount <= MAX_VIEW_SECTORS; iCount++)
	{
		if (fDrawCursors && iCount == sSelMapY && bSelectedDestChar == -1 && !fPlotForHelicopter)
		SetFontForeground(fSelectedCursorIsYellow ? FONT_YELLOW : FONT_WHITE);
   else if( fDrawCursors && ( iCount == gsHighlightSectorY ) )
    SetFontForeground(FONT_WHITE);
   else
    SetFontForeground(MAP_INDEX_COLOR);

	 FindFontCenterCoordinates(MAP_VERT_INDEX_X, MAP_VERT_INDEX_Y + iCount * MAP_GRID_Y * 2 - iZoomY, MAP_HORT_HEIGHT, MAP_GRID_Y * 2, pMapVertIndex[iCount], MAP_FONT, &usX, &usY);
		MPrint(usX, usY, pMapVertIndex[iCount]);
	}

  InvalidateRegion(MAP_VERT_INDEX_X, MAP_VERT_INDEX_Y,MAP_VERT_INDEX_X+MAP_HORT_HEIGHT,  MAP_VERT_INDEX_Y+iCount*MAP_GRID_Y );
  InvalidateRegion(MAP_HORT_INDEX_X, MAP_HORT_INDEX_Y,MAP_HORT_INDEX_X+iCount*MAP_GRID_X,  MAP_HORT_INDEX_Y+ MAP_HORT_HEIGHT);
  SetFontDestBuffer(FRAME_BUFFER, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}
*/


static void HandleShowingOfEnemyForcesInSector(INT16 sSectorX, INT16 sSectorY, INT8 bSectorZ, UINT8 ubIconPosition);


static void HandleShowingOfEnemiesWithMilitiaOn(void)
{
	for (INT16 sX = 1; sX < MAP_WORLD_X - 1; ++sX)
	{
		for (INT16 sY = 1; sY < MAP_WORLD_Y - 1; ++sY)
		{
			HandleShowingOfEnemyForcesInSector(sX, sY, iCurrentMapSectorZ, CountAllMilitiaInSector(sX, sY));
		}
	}
}


static void BlitMineGridMarkers(void);
static void BlitMineIcon(INT16 sMapX, INT16 sMapY);
static void BlitMineText(INT16 sMapX, INT16 sMapY);
static void BlitTownGridMarkers(void);
static void DisplayLevelString(void);
static void DrawBullseye(void);
static void DrawOrta(void);
static void DrawTixa(void);
static void DrawTownMilitiaForcesOnMap(void);
static void HandleLowerLevelMapBlit(void);
static void ShadeMapElem(INT16 sMapX, INT16 sMapY, INT32 iColor);
static void ShowItemsOnMap(void);
static void ShowSAMSitesOnStrategicMap(void);
static void ShowTeamAndVehicles(INT32 fShowFlags);
static void ShowTownText(void);


void DrawMap(void)
{
#ifdef JA2DEMO
	BltVideoObjectOnce(guiSAVEBUFFER, "INTERFACE/map_1.sti", 0, 290, 26);
#else
	if (!iCurrentMapSectorZ)
	{
		if (fZoomFlag)
		{
			if (iZoomX < WEST_ZOOM_BOUND)      iZoomX = WEST_ZOOM_BOUND;
			if (iZoomX > EAST_ZOOM_BOUND)      iZoomX = EAST_ZOOM_BOUND;
			if (iZoomY < NORTH_ZOOM_BOUND + 1) iZoomY = NORTH_ZOOM_BOUND;
			if (iZoomY > SOUTH_ZOOM_BOUND)     iZoomY = SOUTH_ZOOM_BOUND;

			SGPRect clip;
			clip.iLeft   = iZoomX - 2;
			clip.iTop    = iZoomY - 3;
			clip.iRight  = clip.iLeft + MAP_VIEW_WIDTH  + 2;
			clip.iBottom = clip.iTop  + MAP_VIEW_HEIGHT - 1;

			if (clip.iBottom > guiBIGMAP->Height()) clip.iBottom = guiBIGMAP->Height();
			if (clip.iRight  > guiBIGMAP->Width())  clip.iRight  = guiBIGMAP->Width();

			BltVideoSurface(guiSAVEBUFFER, guiBIGMAP, MAP_VIEW_START_X + MAP_GRID_X, MAP_VIEW_START_Y + MAP_GRID_Y - 2, &clip);
		}
		else
		{
			BltVideoSurfaceHalf(guiSAVEBUFFER, guiBIGMAP, MAP_VIEW_START_X + 1, MAP_VIEW_START_Y, NULL);
		}

		// shade map sectors (must be done after Tixa/Orta/Mine icons have been blitted, but before icons!)
		for (INT16 cnt = 1; cnt < MAP_WORLD_X - 1; ++cnt)
		{
			for (INT16 cnt2 = 1; cnt2 < MAP_WORLD_Y - 1; ++cnt2)
			{
				if (!GetSectorFlagStatus(cnt, cnt2, iCurrentMapSectorZ, SF_ALREADY_VISITED))
				{
					INT32 color;
					if (fShowAircraftFlag)
					{
						if (!StrategicMap[cnt + cnt2 * WORLD_MAP_X].fEnemyAirControlled)
						{
							// sector not visited, not air controlled
							color = MAP_SHADE_DK_GREEN;
						}
						else
						{
							// sector not visited, controlled and air not
							color = MAP_SHADE_DK_RED;
						}
					}
					else
					{
						// not visited
						color = MAP_SHADE_BLACK;
					}
					ShadeMapElem(cnt, cnt2, color);
				}
				else
				{
					if (fShowAircraftFlag)
					{
						INT32 color;
						if (!StrategicMap[cnt + cnt2 * WORLD_MAP_X].fEnemyAirControlled)
						{
							// sector visited and air controlled
							color = MAP_SHADE_LT_GREEN;
						}
						else
						{
							// sector visited but not air controlled
							color = MAP_SHADE_LT_RED;
						}
						ShadeMapElem(cnt, cnt2, color);
					}
				}
			}
		}

		/* unfortunately, we can't shade these icons as part of shading the map,
		 * because for airspace, the shade function doesn't merely shade the
		 * existing map surface, but instead grabs the original graphics from
		 * bigmap, and changes their palette.  blitting icons prior to shading would
		 * mean they don't show up in airspace view at all. */

		if (fFoundOrta) DrawOrta();
		if (fFoundTixa) DrawTixa();

		ShowSAMSitesOnStrategicMap();

		// draw mine icons and descriptive text
		for (INT32 i = 0; i < MAX_NUMBER_OF_MINES; ++i)
		{
			const INT16 x = gMineLocation[i].sSectorX;
			const INT16 y = gMineLocation[i].sSectorY;
			BlitMineIcon(x, y);
			// show mine name/production text
			if (fShowMineFlag) BlitMineText(x, y);
		}

		// draw towns names & loyalty ratings, and grey town limit borders
		if (fShowTownFlag)
		{
			BlitTownGridMarkers();
	 	  ShowTownText();
		}

		if (fShowMilitia) DrawTownMilitiaForcesOnMap();

		if (fShowAircraftFlag && !gfInChangeArrivalSectorMode) DrawBullseye();
	}
	else
	{
		HandleLowerLevelMapBlit();
	}

	/* show mine outlines even when viewing underground sublevels - they indicate
	 * where the mine entrances are */
	if (fShowMineFlag) BlitMineGridMarkers();

	if (fShowTeamFlag)
	{
		ShowTeamAndVehicles(SHOW_TEAMMATES | SHOW_VEHICLES);
	}
	else if (fShowMilitia)
	{
		HandleShowingOfEnemiesWithMilitiaOn();
	}

	if (fShowItemsFlag) ShowItemsOnMap();

	DisplayLevelString();
#endif
}


void GetScreenXYFromMapXY( INT16 sMapX, INT16 sMapY, INT16 *psX, INT16 *psY )
{
	INT16 sXTempOff=1;
	INT16 sYTempOff=1;
	if (fZoomFlag)
	{
		*psX = ( (sMapX/2+sXTempOff) * MAP_GRID_ZOOM_X ) + MAP_VIEW_START_X;
		*psY = ( (sMapY/2+sYTempOff) * MAP_GRID_ZOOM_Y ) + MAP_VIEW_START_Y;
  }
	else
	{
		*psX = ( sMapX * MAP_GRID_X ) + MAP_VIEW_START_X;
		*psY = ( sMapY * MAP_GRID_Y ) + MAP_VIEW_START_Y;
	}
}


static void GetScreenXYFromMapXYStationary(INT16 sMapX, INT16 sMapY, INT16* psX, INT16* psY)
{
	INT16 sXTempOff=1;
	INT16 sYTempOff=1;
   //(MAP_VIEW_START_X+((iCount+1)*MAP_GRID_X)*2-iZoomX));
   *psX = ( (sMapX+sXTempOff) * MAP_GRID_X )*2-((INT16)iZoomX)+MAP_VIEW_START_X;
   *psY = ( (sMapY+sYTempOff) * MAP_GRID_Y )*2-((INT16)iZoomY)+MAP_VIEW_START_Y;

}


// display the town names and loyalty on the screen
static void ShowTownText(void)
{
	SetFont(MAP_FONT);
	SetFontBackground(FONT_MCOLOR_BLACK);
	SetFontDestBuffer(guiSAVEBUFFER, MapScreenRect.iLeft + 2, MapScreenRect.iTop, MapScreenRect.iRight, MapScreenRect.iBottom);
	ClipBlitsToMapViewRegion();

	for (INT8 town = FIRST_TOWN; town < NUM_TOWNS; ++town)
	{
		// skip Orta/Tixa until found
		if (town == ORTA && !fFoundOrta) continue;
		if (town == TIXA && !fFoundTixa) continue;

		UINT16 x = MAP_VIEW_START_X + MAP_GRID_X;
		UINT16 y = MAP_VIEW_START_Y + MAP_GRID_Y;
		if (!fZoomFlag)
		{
			x += pTownPoints[town].iX * MAP_GRID_X / 10;
			y += pTownPoints[town].iY * MAP_GRID_Y / 10 + 1;
		}
		else
		{
			x += MAP_GRID_ZOOM_X - iZoomX + pTownPoints[town].iX * MAP_GRID_ZOOM_X / 10;
			y += MAP_GRID_ZOOM_Y - iZoomY + pTownPoints[town].iY * MAP_GRID_ZOOM_Y / 10 + 1;
		}

		// if within view region...render, else don't
		if (x < MAP_VIEW_START_X || MAP_VIEW_START_X + MAP_VIEW_WIDTH  < x) continue;
		if (y < MAP_VIEW_START_Y || MAP_VIEW_START_Y + MAP_VIEW_HEIGHT < y) continue;

		// don't show loyalty string until loyalty tracking for that town has been started
		if (gTownLoyalty[town].fStarted && gfTownUsesLoyalty[town])
		{
			// if loyalty is too low to train militia, and militia training is allowed here
			UINT8 const colour =
				gTownLoyalty[town].ubRating < MIN_RATING_TO_TRAIN_TOWN &&
				MilitiaTrainingAllowedInTown(town) ?
					FONT_MCOLOR_RED : FONT_MCOLOR_LTGREEN;
			SetFontForeground(colour);

			wchar_t loyalty_str[32];
			swprintf(loyalty_str, lengthof(loyalty_str), gsLoyalString, gTownLoyalty[town].ubRating);

			INT16 loyalty_x = x - StringPixLength(loyalty_str, MAP_FONT) / 2;
			if (!fZoomFlag) // make sure we don't go past left edge (Grumm)
			{
				INT16 const min_x = MAP_VIEW_START_X + 23;
				if (loyalty_x < min_x) loyalty_x = min_x;
			}

			GDirtyPrint(loyalty_x, y + GetFontHeight(MAP_FONT), loyalty_str);
		}
		else
		{
			SetFontForeground(FONT_MCOLOR_LTGREEN);
		}

		wchar_t const* const name   = pTownNames[town];
		INT16          const name_x = x - StringPixLength(name, MAP_FONT) / 2;
		GDirtyPrint(name_x, y, name);
	}

	RestoreClipRegionToFullScreen();
}


static void DrawMapBoxIcon(HVOBJECT hIconHandle, UINT16 usVOIndex, INT16 sMapX, INT16 sMapY, UINT8 ubIconPosition);


// "on duty" includes mercs inside vehicles
static INT32 ShowOnDutyTeam(INT16 sMapX, INT16 sMapY)
{
  // run through list
  UINT8 ubIconPosition = 0;
	CFOR_ALL_IN_CHAR_LIST(c)
	{
		const SOLDIERTYPE* const pSoldier = c->merc;
		if( !( pSoldier->uiStatusFlags & SOLDIER_VEHICLE ) &&
				 ( pSoldier->sSectorX == sMapX) &&
				 ( pSoldier->sSectorY == sMapY) &&
				 ( pSoldier->bSectorZ == iCurrentMapSectorZ ) &&
				 ( ( pSoldier->bAssignment < ON_DUTY ) || ( ( pSoldier->bAssignment == VEHICLE ) && ( pSoldier->iVehicleId != iHelicopterVehicleId ) ) ) &&
				 ( pSoldier->bLife > 0) &&
				 ( !PlayerIDGroupInMotion( pSoldier->ubGroupID ) ) )
		{
			DrawMapBoxIcon(guiCHARICONS, SMALL_YELLOW_BOX, sMapX, sMapY, ubIconPosition);
			ubIconPosition++;
		}
	}
	return ubIconPosition;
}


static INT32 ShowAssignedTeam(INT16 sMapX, INT16 sMapY, INT32 iCount)
{
  // run through list
	UINT8 ubIconPosition = iCount;
	CFOR_ALL_IN_CHAR_LIST(c)
	{
		const SOLDIERTYPE* const pSoldier = c->merc;
    // given number of on duty members, find number of assigned chars
		// start at beginning of list, look for people who are in sector and assigned
		if( !( pSoldier->uiStatusFlags & SOLDIER_VEHICLE ) &&
				 ( pSoldier->sSectorX == sMapX) &&
				 ( pSoldier->sSectorY == sMapY) &&
				 ( pSoldier->bSectorZ == iCurrentMapSectorZ ) &&
				 ( pSoldier->bAssignment >= ON_DUTY ) && ( pSoldier->bAssignment != VEHICLE ) &&
				 ( pSoldier->bAssignment != IN_TRANSIT ) &&
				 ( pSoldier->bAssignment != ASSIGNMENT_POW ) &&
				 ( pSoldier->bLife > 0 ) &&
				 ( !PlayerIDGroupInMotion( pSoldier->ubGroupID ) ) )
		{
			// skip mercs inside the helicopter if we're showing airspace level - they show up inside chopper icon instead
			if ( !fShowAircraftFlag || ( pSoldier->bAssignment != VEHICLE ) || ( pSoldier->iVehicleId != iHelicopterVehicleId ) )
			{
				DrawMapBoxIcon(guiCHARICONS, SMALL_DULL_YELLOW_BOX, sMapX, sMapY, ubIconPosition);
				ubIconPosition++;
			}
		}
	}
	return ubIconPosition;
}


static INT32 ShowVehicles(INT16 sMapX, INT16 sMapY, INT32 icon_pos)
{
	CFOR_ALL_VEHICLES(v)
	{
		// skip the chopper, it has its own icon and displays in airspace mode
		if (VEHICLE2ID(v) == iHelicopterVehicleId) continue;

		if (v->sSectorX == sMapX              &&
				v->sSectorY == sMapY              &&
				v->sSectorZ == iCurrentMapSectorZ &&
				!PlayerIDGroupInMotion(v->ubMovementGroup))
		{
			// ATE: Check if this vehicle has a soldier and it's on our team.....
			const SOLDIERTYPE* const pVehicleSoldier = GetSoldierStructureForVehicle(VEHICLE2ID(v));

			// this skips the chopper, which has no soldier
			if (pVehicleSoldier != NULL && pVehicleSoldier->bTeam == gbPlayerNum)
			{
				DrawMapBoxIcon(guiCHARICONS, SMALL_WHITE_BOX, sMapX, sMapY, icon_pos++);
			}
		}
	}

	return icon_pos;
}


static void ShowEnemiesInSector(INT16 sSectorX, INT16 sSectorY, INT16 sNumberOfEnemies, UINT8 ubIconPosition)
{
	UINT8 ubEnemy = 0;

	for( ubEnemy = 0; ubEnemy < sNumberOfEnemies; ubEnemy++ )
	{
		DrawMapBoxIcon(guiCHARICONS, SMALL_RED_BOX, sSectorX, sSectorY, ubIconPosition);
		ubIconPosition++;
	}
}


static void ShowUncertainNumberEnemiesInSector(INT16 sSectorX, INT16 sSectorY)
{
	INT16 sXPosition = 0, sYPosition = 0;

	// grab the x and y postions
	sXPosition = sSectorX;
	sYPosition = sSectorY;

	// check if we are zoomed in...need to offset in case for scrolling purposes
	if(!fZoomFlag)
	{
		sXPosition = ( INT16 )( MAP_X_ICON_OFFSET + ( MAP_VIEW_START_X + ( sSectorX * MAP_GRID_X + 1 )  ) - 1 );
		sYPosition = ( INT16 )( ( ( MAP_VIEW_START_Y + ( sSectorY * MAP_GRID_Y ) + 1 )  ) );
		sYPosition -= 2;

		// small question mark
		BltVideoObject(guiSAVEBUFFER, guiCHARICONS, SMALL_QUESTION_MARK, sXPosition, sYPosition);
		InvalidateRegion( sXPosition ,sYPosition, sXPosition + DMAP_GRID_X, sYPosition + DMAP_GRID_Y );
	}
/*
	else
	{
		INT16 sX = 0, sY = 0;

		GetScreenXYFromMapXYStationary( sSectorX, sSectorY, &sX, &sY );
 	 	sYPosition = sY-MAP_GRID_Y;
		sXPosition = sX-MAP_GRID_X;

		// get the x and y position
		sXPosition = MAP_X_ICON_OFFSET + sXPosition ;
		sYPosition = sYPosition - 1;

		// clip blits to mapscreen region
		ClipBlitsToMapViewRegion( );

		// large question mark
		BltVideoObject(guiSAVEBUFFER, guiCHARICONS, BIG_QUESTION_MARK, sXPosition, sYPosition);

		// restore clip blits
		RestoreClipRegionToFullScreen( );

		InvalidateRegion( sXPosition, sYPosition, sXPosition + DMAP_GRID_ZOOM_X, sYPosition + DMAP_GRID_ZOOM_Y );
	}
*/
}


static void ShowPeopleInMotion(INT16 sX, INT16 sY);


static void ShowTeamAndVehicles(INT32 fShowFlags)
{
	// go through each sector, display the on duty, assigned, and vehicles
	INT32         iIconOffset              = 0;
	const BOOLEAN fContemplatingRetreating = gfDisplayPotentialRetreatPaths && gpBattleGroup;
	for (INT16 sMapX = 1; sMapX < MAP_WORLD_X - 1; ++sMapX)
	{
		for (INT16 sMapY = 1; sMapY < MAP_WORLD_Y - 1; ++sMapY)
		{
			/* don't show mercs/vehicles currently in this sector if player is
			 * contemplating retreating from THIS sector */
			if (!fContemplatingRetreating         ||
					sMapX != gpBattleGroup->ubSectorX ||
					sMapY != gpBattleGroup->ubSectorY)
			{
				if (fShowFlags & SHOW_TEAMMATES)
				{
					iIconOffset = ShowOnDutyTeam(sMapX, sMapY);
					iIconOffset = ShowAssignedTeam(sMapX, sMapY, iIconOffset);
				}

				if (fShowFlags & SHOW_VEHICLES)
				{
					iIconOffset = ShowVehicles(sMapX, sMapY, iIconOffset);
				}
			}

			if (fShowFlags & SHOW_TEAMMATES)
			{
				HandleShowingOfEnemyForcesInSector(sMapX, sMapY, iCurrentMapSectorZ, iIconOffset);
				ShowPeopleInMotion(sMapX, sMapY);
			}
		}
	}
}


#ifndef JA2DEMO
static void ShadeMapElemZoomIn(INT16 sMapX, INT16 sMapY, INT32 iColor);


static void ShadeMapElem(const INT16 sMapX, const INT16 sMapY, const INT32 iColor)
{
	if (fZoomFlag)
	{
		ShadeMapElemZoomIn(sMapX, sMapY, iColor);
	}
	else
	{
		INT16 sScreenX;
		INT16 sScreenY;
		GetScreenXYFromMapXY(sMapX, sMapY, &sScreenX, &sScreenY);

		// compensate for original BIG_MAP blit being done at MAP_VIEW_START_X + 1
		sScreenX += 1;

		SGPRect clip;
	  clip.iLeft   = 2 * (sScreenX - (MAP_VIEW_START_X + 1));
		clip.iTop    = 2 * (sScreenY -  MAP_VIEW_START_Y     );
		clip.iRight  = clip.iLeft + 2 * MAP_GRID_X;
		clip.iBottom = clip.iTop  + 2 * MAP_GRID_Y;

		// non-airspace
		if (iColor == MAP_SHADE_BLACK) sScreenY -= 1;

		UINT16* pal;
		switch (iColor)
		{
		  case MAP_SHADE_BLACK:
				// simply shade darker
				guiSAVEBUFFER->ShadowRect(sScreenX, sScreenY, sScreenX + MAP_GRID_X - 1, sScreenY + MAP_GRID_Y - 1);
				return;

			case MAP_SHADE_LT_GREEN: pal = pMapLTGreenPalette; break;
			case MAP_SHADE_DK_GREEN: pal = pMapDKGreenPalette; break;
			case MAP_SHADE_LT_RED:   pal = pMapLTRedPalette;   break;
			case MAP_SHADE_DK_RED:   pal = pMapDKRedPalette;   break;

			default: return;
		}

		// get original video surface palette
		SGPVSurface* const map = guiBIGMAP;
		//SGPVSurface* const mine = guiMINEICON;
		//SGPVSurface* const sam  = guiSAMICON;

		UINT16* const org_pal = map->p16BPPPalette;
		map->p16BPPPalette = pal;
		//mine->p16BPPPalette = pal;
		//sam->p16BPPPalette  = pal;
		BltVideoSurfaceHalf(guiSAVEBUFFER, guiBIGMAP, sScreenX, sScreenY, &clip);
		map->p16BPPPalette = org_pal;
		//mine->p16BPPPalette = org_pal;
		//sam->p16BPPPalette  = org_pal;
	}
}


static void ShadeMapElemZoomIn(const INT16 sMapX, const INT16 sMapY, INT32 iColor)
{
	// trabslate to screen co-ords for zoomed
	INT16 sScreenX;
	INT16 sScreenY;
	GetScreenXYFromMapXYStationary(sMapX, sMapY, &sScreenX, &sScreenY);

	// shift left by one sector
	const INT32 iX = sScreenX - MAP_GRID_X;
	const INT32 iY = sScreenY - MAP_GRID_Y;

	if (MapScreenRect.iLeft - MAP_GRID_X * 2 < iX && iX < MapScreenRect.iRight &&
			MapScreenRect.iTop  - MAP_GRID_Y * 2 < iY && iY < MapScreenRect.iBottom)
	{
		sScreenX = iX;
		sScreenY = iY;

		SGPRect clip;
		if (iColor == MAP_SHADE_BLACK)
		{
			clip.iLeft   = sScreenX + 1;
			clip.iTop    = sScreenY;
			clip.iRight  = sScreenX + MAP_GRID_X * 2 - 1;
			clip.iBottom = sScreenY + MAP_GRID_Y * 2 - 1;
		}
		else
		{
			clip.iLeft   = iZoomX + sScreenX - MAP_VIEW_START_X - MAP_GRID_X;
			clip.iTop    = iZoomY + sScreenY - MAP_VIEW_START_Y - MAP_GRID_Y;
			clip.iRight  = clip.iLeft + MAP_GRID_X * 2;
			clip.iBottom = clip.iTop  + MAP_GRID_Y * 2;

			if (sScreenY <= MapScreenRect.iTop + 10)
			{
				clip.iTop -= 5;
				sScreenY  -= 5;
			}


			sScreenX += 1;
			if (sMapX == 1)
			{
				clip.iLeft -= 5;
				sScreenX   -= 5;
			}
		}

		if (sScreenX >= MapScreenRect.iRight -  2 * MAP_GRID_X) ++clip.iRight;
		if (sScreenY >= MapScreenRect.iBottom - 2 * MAP_GRID_X) ++clip.iBottom;

		sScreenX += 1;
		sScreenY += 1;

		if (sScreenX > MapScreenRect.iRight || sScreenY > MapScreenRect.iBottom) return;

		UINT16* pal;
		switch (iColor)
		{
			case MAP_SHADE_BLACK:
			// simply shade darker
			guiSAVEBUFFER->ShadowRect(clip.iLeft, clip.iTop, clip.iRight, clip.iBottom);
			return;

			case MAP_SHADE_LT_GREEN: pal = pMapLTGreenPalette; break;
			case MAP_SHADE_DK_GREEN: pal = pMapDKGreenPalette; break;
			case MAP_SHADE_LT_RED:   pal = pMapLTRedPalette;   break;
			case MAP_SHADE_DK_RED:   pal = pMapDKRedPalette;   break;

			default: return;
		}

		SGPVSurface* const map     = guiBIGMAP;
		UINT16*      const org_pal = map->p16BPPPalette;
		map->p16BPPPalette = pal;
		BltVideoSurface(guiSAVEBUFFER, guiBIGMAP, sScreenX, sScreenY, &clip);
		map->p16BPPPalette = org_pal;
	}
}
#endif


void InitializePalettesForMap(void)
{
#ifndef JA2DEMO
	AutoSGPVSurface uiTempMap(AddVideoSurfaceFromFile("INTERFACE/b_map.pcx"));

	SGPPaletteEntry const* const pal = uiTempMap->GetPalette();

	pMapLTRedPalette   = Create16BPPPaletteShaded(pal, 400,   0, 0, TRUE);
	pMapDKRedPalette   = Create16BPPPaletteShaded(pal, 200,   0, 0, TRUE);
	pMapLTGreenPalette = Create16BPPPaletteShaded(pal,   0, 400, 0, TRUE);
	pMapDKGreenPalette = Create16BPPPaletteShaded(pal,   0, 200, 0, TRUE);
#endif
}


void ShutDownPalettesForMap(void)
{
	MemFree(pMapLTRedPalette);
	MemFree(pMapDKRedPalette);
	MemFree(pMapLTGreenPalette);
	MemFree(pMapDKGreenPalette);

	pMapLTRedPalette   = NULL;
	pMapDKRedPalette   = NULL;
	pMapLTGreenPalette = NULL;
	pMapDKGreenPalette = NULL;
}


static void CopyPathToCharactersSquadIfInOne(SOLDIERTYPE* pCharacter);


void PlotPathForCharacter( SOLDIERTYPE *pCharacter, INT16 sX, INT16 sY, BOOLEAN fTacticalTraversal )
{
	// will plot a path for this character

	// is cursor allowed here?..if not..don't build path
	if( !IsTheCursorAllowedToHighLightThisSector( sX, sY ) )
	{
		return;
	}

	// is the character in transit?..then leave
	if( pCharacter->bAssignment == IN_TRANSIT )
	{
		// leave
		return;
	}


	if( pCharacter->bSectorZ != 0 )
	{
		if( pCharacter->bAssignment >= ON_DUTY )
		{
			// not on the surface, character won't move until they reach surface..info player of this fact
			MapScreenMessage(FONT_MCOLOR_DKRED, MSG_INTERFACE, L"%ls %ls", pCharacter->name, gsUndergroundString);
		}
		else	// squad
		{
			MapScreenMessage(FONT_MCOLOR_DKRED, MSG_INTERFACE, L"%ls %ls", pLongAssignmentStrings[pCharacter->bAssignment], gsUndergroundString);
		}
		return;
	}

	if( ( pCharacter->bAssignment == VEHICLE ) || ( pCharacter->uiStatusFlags & SOLDIER_VEHICLE ) )
	{
		SetUpMvtGroupForVehicle( pCharacter );
	}

	// will plot a path from current position to sX, sY
	// get last sector in characters list, build new path, remove tail section, move to beginning of list, and append onto old list
	pCharacter->pMercPath = AppendStrategicPath(BuildAStrategicPath(GetLastSectorIdInCharactersPath(pCharacter), (INT16)(sX + sY * MAP_WORLD_X), GetGroup(GetSoldierGroupId(pCharacter)), fTacticalTraversal), pCharacter->pMercPath);

	// check if in vehicle, if so, copy path to vehicle
	if( ( pCharacter->bAssignment == VEHICLE ) || ( pCharacter->uiStatusFlags & SOLDIER_VEHICLE ) )
	{
		MoveCharactersPathToVehicle( pCharacter );
	}
	else
	{
		CopyPathToCharactersSquadIfInOne( pCharacter );
	}
}


void PlotATemporaryPathForCharacter(const SOLDIERTYPE* const pCharacter, const INT16 sX, const INT16 sY)
{
	// clear old temp path
	pTempCharacterPath = ClearStrategicPathList( pTempCharacterPath, -1 );

	// is cursor allowed here?..if not..don't build temp path
	if( !IsTheCursorAllowedToHighLightThisSector( sX, sY ) )
	{
		return;
	}

	pTempCharacterPath = BuildAStrategicPath(GetLastSectorIdInCharactersPath(pCharacter), sX + sY * MAP_WORLD_X, GetGroup(GetSoldierGroupId(pCharacter)), FALSE);
}



// clear out character path list, after and including this sector
UINT32 ClearPathAfterThisSectorForCharacter( SOLDIERTYPE *pCharacter, INT16 sX, INT16 sY )
{
	INT32 iOrigLength = 0;
	VEHICLETYPE *pVehicle = NULL;


	iOrigLength = GetLengthOfMercPath( pCharacter );

	if( !iOrigLength )
	{
		// no previous path, nothing to do
		return( ABORT_PLOTTING );
	}


	// if we're clearing everything beyond the current sector, that's quite different.  Since we're basically cancelling
	// his movement completely, we must also make sure his next X,Y are changed and he officially "returns" to his sector
	if ( ( sX == pCharacter->sSectorX ) && ( sY == pCharacter->sSectorY ) )
	{
		// if we're in confirm map move mode, cancel that (before new UI messages are issued)
		EndConfirmMapMoveMode( );

		CancelPathsOfAllSelectedCharacters();
		return( PATH_CLEARED );
	}
	else	// click not in the current sector
	{
		// if the clicked sector is along current route, this will repath only as far as it.  If not, the entire path will
		// be canceled.

		// if a vehicle
		if( pCharacter->uiStatusFlags & SOLDIER_VEHICLE )
		{
			pVehicle = &( pVehicleList[ pCharacter->bVehicleID ] );
		}
		// or in a vehicle
		else if( pCharacter->bAssignment == VEHICLE )
		{
			pVehicle = &( pVehicleList[ pCharacter->iVehicleId ] );
		}
		else
		{
			// foot soldier
			pCharacter->pMercPath = ClearStrategicPathListAfterThisSector( pCharacter->pMercPath, sX, sY, pCharacter->ubGroupID );
		}

		// if there's an associated vehicle structure
		if ( pVehicle != NULL )
		{
			// do it for the vehicle, too
			pVehicle->pMercPath = ClearStrategicPathListAfterThisSector( pVehicle->pMercPath, sX, sY, pVehicle->ubMovementGroup );
		}

		if( GetLengthOfMercPath( pCharacter ) < iOrigLength )
		{
			CopyPathToAllSelectedCharacters( pCharacter->pMercPath );
			// path WAS actually shortened
			return( PATH_SHORTENED );
		}
		else
		{
			// same path as before - it's not any shorter
			return ( ABORT_PLOTTING );
		}
	}
}


void CancelPathForCharacter( SOLDIERTYPE *pCharacter )
{
	// clear out character's entire path list, he and his squad will stay/return to his current sector.
	pCharacter->pMercPath = ClearStrategicPathList( pCharacter->pMercPath, pCharacter->ubGroupID );
	// NOTE: This automatically calls RemoveGroupWaypoints() internally for valid movement groups

	// This causes the group to effectively reverse directions (even if they've never actually left), so handle that.
	// They are going to return to their current X,Y sector.
	RebuildWayPointsForGroupPath( pCharacter->pMercPath, pCharacter->ubGroupID );
//	GroupReversingDirectionsBetweenSectors( GetGroup( pCharacter->ubGroupID ), ( UINT8 )( pCharacter->sSectorX ), ( UINT8 )( pCharacter->sSectorY ), FALSE );


	// if he's in a vehicle, clear out the vehicle, too
	if( pCharacter->bAssignment == VEHICLE )
	{
		CancelPathForVehicle( &( pVehicleList[ pCharacter->iVehicleId ] ), TRUE );
	}
	else
	{
		// display "travel route canceled" message
		BeginMapUIMessage(0, pMapPlotStrings[3]);
	}


	CopyPathToCharactersSquadIfInOne( pCharacter );

	fMapPanelDirty = TRUE;
	fTeamPanelDirty = TRUE;
	fCharacterInfoPanelDirty = TRUE;		// to update ETA
}


void CancelPathForVehicle( VEHICLETYPE *pVehicle, BOOLEAN fAlreadyReversed )
{
	// we're clearing everything beyond the *current* sector, that's quite different.  Since we're basically cancelling
	// his movement completely, we must also make sure his next X,Y are changed and he officially "returns" to his sector
	pVehicle->pMercPath = ClearStrategicPathList( pVehicle->pMercPath, pVehicle->ubMovementGroup );
	// NOTE: This automatically calls RemoveGroupWaypoints() internally for valid movement groups

	// if we already reversed one of the passengers, flag will be TRUE,
	// don't do it again or we're headed back where we came from!
	if ( !fAlreadyReversed )
	{
		// This causes the group to effectively reverse directions (even if they've never actually left), so handle that.
		// They are going to return to their current X,Y sector.
		RebuildWayPointsForGroupPath( pVehicle->pMercPath, pVehicle->ubMovementGroup );
//		GroupReversingDirectionsBetweenSectors( GetGroup( pVehicle->ubMovementGroup ), ( UINT8 ) ( pVehicle->sSectorX ), ( UINT8 ) ( pVehicle->sSectorY ), FALSE );
	}

	// display "travel route canceled" message
	BeginMapUIMessage(0, pMapPlotStrings[3]);

	// turn the helicopter flag off here, this prevents the "route aborted" msg from coming up
	fPlotForHelicopter = FALSE;

	fTeamPanelDirty = TRUE;
	fMapPanelDirty = TRUE;
	fCharacterInfoPanelDirty = TRUE;		// to update ETA
}


static void CopyPathToCharactersSquadIfInOne(SOLDIERTYPE* pCharacter)
{
	INT8 bSquad = 0;

	// check if on a squad, if so, do same thing for all characters

	// what squad is character in?
	bSquad = SquadCharacterIsIn( pCharacter );

	// check to see if character is on a squad, if so, copy path to squad
	if( bSquad != -1)
	{
		// copy path over
		CopyPathOfCharacterToSquad( pCharacter, bSquad );
	}
}


static void AnimateRoute(PathSt* pPath);
static void TracePathRoute(PathSt*);


void DisplaySoldierPath( SOLDIERTYPE *pCharacter )
{
	PathSt* const pPath = GetSoldierMercPathPtr(pCharacter);
  // trace real route
	TracePathRoute(pPath);
	AnimateRoute( pPath );
}


void DisplaySoldierTempPath(void)
{
	// now render temp route
	TracePathRoute(pTempCharacterPath);
}



void DisplayHelicopterPath( void )
{
	// clip to map
  ClipBlitsToMapViewRegion( );

	const VEHICLETYPE* const v = GetHelicopter();
	// trace both lists..temp is conditional if cursor has sat in same sector grid long enough
	TracePathRoute(v->pMercPath);
	AnimateRoute(v->pMercPath);

	// restore
	RestoreClipRegionToFullScreen( );
}


void DisplayHelicopterTempPath( void )
{
	//should we draw temp path?
	if (fDrawTempHeliPath) TracePathRoute(pTempHelicopterPath);
}


void PlotPathForHelicopter(const INT16 sX, const INT16 sY)
{
	// will plot the path for the helicopter

	// no heli...go back
	if (!fShowAircraftFlag || !fHelicopterAvailable) return;

	// is cursor allowed here?..if not..don't build path
	if (!IsTheCursorAllowedToHighLightThisSector(sX, sY)) return;

	// set up mvt group for helicopter
	SetUpHelicopterForMovement();

	VEHICLETYPE* const v = GetHelicopter();
	// will plot a path from current position to sX, sY
	// get last sector in helicopters list, build new path, remove tail section, move to beginning of list, and append onto old list
	v->pMercPath = AppendStrategicPath(BuildAStrategicPath(GetLastSectorOfHelicoptersPath(), (INT16)(sX + sY * MAP_WORLD_X), GetGroup(v->ubMovementGroup), FALSE), v->pMercPath);

	fMapPanelDirty = TRUE;
}


void PlotATemporaryPathForHelicopter( INT16 sX, INT16 sY )
{
	// clear old temp path
	pTempHelicopterPath = ClearStrategicPathList(pTempHelicopterPath, 0);

	// is cursor allowed here?..if not..don't build temp path
	if( !IsTheCursorAllowedToHighLightThisSector( sX, sY ) )
	{
		return;
	}

	// build path
	pTempHelicopterPath = BuildAStrategicPath(GetLastSectorOfHelicoptersPath(), sX + sY * MAP_WORLD_X, GetGroup(GetHelicopter()->ubMovementGroup), FALSE);
}


// clear out helicopter path list, after and including this sector
UINT32 ClearPathAfterThisSectorForHelicopter( INT16 sX, INT16 sY )
{
	INT32 iOrigLength = 0;

	// clear out helicopter path list, after and including this sector
	if( iHelicopterVehicleId == -1 ||  !CanHelicopterFly() )
	{
		// abort plotting, shouldn't even be here
		return( ABORT_PLOTTING );
	}

	VEHICLETYPE* const pVehicle = GetHelicopter();

	iOrigLength = GetLengthOfPath( pVehicle->pMercPath );
	if( !iOrigLength )
	{
		// no previous path, nothing to do, and we didn't shorten it
		return( ABORT_PLOTTING );
	}


	// are we clearing everything beyond the helicopter's CURRENT sector?
	if ( ( sX == pVehicle->sSectorX ) && ( sY == pVehicle->sSectorY ) )
	{
		// if we're in confirm map move mode, cancel that (before new UI messages are issued)
		EndConfirmMapMoveMode( );

		CancelPathForVehicle( pVehicle, FALSE );
		return( PATH_CLEARED );
	}
	else	// click not in the current sector
	{
		// if the clicked sector is along current route, this will repath only as far as it.  If not, the entire path will
		// be canceled.
		pVehicle->pMercPath = ClearStrategicPathListAfterThisSector( pVehicle->pMercPath, sX, sY, pVehicle->ubMovementGroup );

		if( GetLengthOfPath( pVehicle->pMercPath ) < iOrigLength )
		{
			// really shortened!
			return( PATH_SHORTENED );
		}
		else
		{
			// same path as before - it's not any shorter
			return ( ABORT_PLOTTING );
		}
	}
}



INT16 GetLastSectorOfHelicoptersPath( void )
{
	const VEHICLETYPE* const v = GetHelicopter();
	// will return the last sector of the helicopter's current path
	INT16 sLastSector = v->sSectorX + v->sSectorY * MAP_WORLD_X;
	PathSt* pNode = v->pMercPath;

	while( pNode )
	{
		sLastSector = ( INT16 ) ( pNode->uiSectorId );
		pNode = pNode->pNext;
	}

	return sLastSector;
}


// trace a route for a passed path...doesn't require dest char - most more general
static void TracePathRoute(PathSt* const pPath)
{
	if (pPath == NULL) return;
	Assert(pPath->pPrev == NULL);

	INT32 iDirection = -1;
	INT32 iArrow     = -1;
	const PathSt* prev = NULL;
	const PathSt* next;
	for (const PathSt* node = pPath; node != NULL; prev = node, node = next)
	{
		next = node->pNext;

		BOOLEAN fUTurnFlag = FALSE;
		INT32   iArrowX;
		INT32   iArrowY;
		INT32   iX;
		INT32   iY;
		if (prev && next)
		{
			const INT32 iDeltaA = (INT16)node->uiSectorId - (INT16)prev->uiSectorId;
			const INT32 iDeltaB = (INT16)node->uiSectorId - (INT16)next->uiSectorId;
			if (iDeltaA == 0) return;
			if (!fZoomFlag)
			{
				iX = node->uiSectorId % MAP_WORLD_X;
				iY = node->uiSectorId / MAP_WORLD_X;
				iX = iX * MAP_GRID_X + MAP_VIEW_START_X;
				iY = iY * MAP_GRID_Y + MAP_VIEW_START_Y;
			}
			else
			{
				INT16 sX;
				INT16 sY;
				GetScreenXYFromMapXYStationary(node->uiSectorId % MAP_WORLD_X, node->uiSectorId / MAP_WORLD_X, &sX, &sY);
				iY = sY - MAP_GRID_Y;
				iX = sX - MAP_GRID_X;
			}
			iArrowX = iX;
			iArrowY = iY;
			if (prev->pPrev && next->pNext)
			{
				// check to see if out-of sector U-turn
				// for placement of arrows
				const INT32 iDeltaB1 = next->uiSectorId - next->pNext->uiSectorId;
				fUTurnFlag =
					(iDeltaB1 == -WORLD_MAP_X && iDeltaA == -WORLD_MAP_X && iDeltaB == -1)           ||
					(iDeltaB1 == -WORLD_MAP_X && iDeltaA == -WORLD_MAP_X && iDeltaB ==  1)           ||
					(iDeltaB1 ==  WORLD_MAP_X && iDeltaA ==  WORLD_MAP_X && iDeltaB ==  1)           ||
					(iDeltaB1 == -WORLD_MAP_X && iDeltaA == -WORLD_MAP_X && iDeltaB ==  1)           ||
					(iDeltaB1 == -1           && iDeltaA == -1           && iDeltaB == -WORLD_MAP_X) ||
					(iDeltaB1 == -1           && iDeltaA == -1           && iDeltaB ==  WORLD_MAP_X) ||
					(iDeltaB1 ==  1           && iDeltaA ==  1           && iDeltaB == -WORLD_MAP_X) ||
					(iDeltaB1 ==  1           && iDeltaA ==  1           && iDeltaB ==  WORLD_MAP_X);
			}

			const BOOLEAN fSpeedFlag = !node->fSpeed;
			if (prev->uiSectorId == next->uiSectorId)
			{
				if (prev->uiSectorId + WORLD_MAP_X == node->uiSectorId)
				{
					if (fZoomFlag)
					{
						iDirection  = S_TO_N_ZOOM_LINE;
						iArrow      = fSpeedFlag ? ZOOM_Y_NORTH_ARROW : ZOOM_NORTH_ARROW;
						iArrowX    += NORTH_OFFSET_X * 2;
						iArrowY    += NORTH_OFFSET_Y * 2;
					}
					else
					{
						iDirection  = S_TO_N_LINE;
						iArrow      = fSpeedFlag ? Y_NORTH_ARROW : NORTH_ARROW;
						iArrowX    += NORTH_OFFSET_X;
						iArrowY    += NORTH_OFFSET_Y;
					}
				}
				else if (prev->uiSectorId - WORLD_MAP_X == node->uiSectorId)
				{
					if (fZoomFlag)
					{
						iDirection  = N_TO_S_ZOOM_LINE;
						iArrow      = fSpeedFlag ? ZOOM_Y_SOUTH_ARROW : ZOOM_SOUTH_ARROW;
						iArrowX    += SOUTH_OFFSET_X * 2;
						iArrowY    += SOUTH_OFFSET_Y * 2;
					}
					else
					{
						iDirection  = N_TO_S_LINE;
						iArrow      = fSpeedFlag ? Y_SOUTH_ARROW : SOUTH_ARROW;
						iArrowX    += SOUTH_OFFSET_X;
						iArrowY    += SOUTH_OFFSET_Y;
					}
				}
				else if (prev->uiSectorId + 1 == node->uiSectorId)
				{
					if (fZoomFlag)
					{
						iDirection  = E_TO_W_ZOOM_LINE;
						iArrow      = fSpeedFlag ? ZOOM_Y_WEST_ARROW : ZOOM_WEST_ARROW;
						iArrowX    += WEST_OFFSET_X * 2;
						iArrowY    += WEST_OFFSET_Y * 2;
					}
					else
					{
						iDirection  = E_TO_W_LINE;
						iArrow      = fSpeedFlag ? Y_WEST_ARROW : WEST_ARROW;
						iArrowX    += WEST_OFFSET_X;
						iArrowY    += WEST_OFFSET_Y;
					}
				}
				else
				{
					if (fZoomFlag)
					{
						iDirection  = W_TO_E_ZOOM_LINE;
						iArrow      = fSpeedFlag ? ZOOM_Y_EAST_ARROW : ZOOM_EAST_ARROW;
						iArrowX    += EAST_OFFSET_X * 2;
						iArrowY    += EAST_OFFSET_Y * 2;
					}
					else
					{
						iDirection  = W_TO_E_LINE;
						iArrow      = fSpeedFlag ? Y_EAST_ARROW : EAST_ARROW;
						iArrowX    += EAST_OFFSET_X;
						iArrowY    += EAST_OFFSET_Y;
					}
				}
			}
			else
			{
				if (iDeltaA == -1 && iDeltaB == 1)
				{
					if (fZoomFlag)
					{
						iDirection  = WEST_ZOOM_LINE;
						iArrow      = fSpeedFlag ? ZOOM_Y_WEST_ARROW : ZOOM_WEST_ARROW;
						iArrowX    += WEST_OFFSET_X * 2;
						iArrowY    += WEST_OFFSET_Y * 2;
					}
					else
					{
						iDirection  = WEST_LINE;
						iArrow      = fSpeedFlag ? Y_WEST_ARROW : WEST_ARROW;
						iArrowX    += WEST_OFFSET_X;
						iArrowY    += WEST_OFFSET_Y;
					}
				}
				else if (iDeltaA == 1 && iDeltaB == -1)
				{
					if (fZoomFlag)
					{
						iDirection  = EAST_ZOOM_LINE;
						iArrow      = fSpeedFlag ? ZOOM_Y_EAST_ARROW : ZOOM_EAST_ARROW;
						iArrowX    += EAST_OFFSET_X * 2;
						iArrowY    += EAST_OFFSET_Y * 2;
					}
					else
					{
						iDirection  = EAST_LINE;
						iArrow      = fSpeedFlag ? Y_EAST_ARROW : EAST_ARROW;
						iArrowX    += EAST_OFFSET_X;
						iArrowY    += EAST_OFFSET_Y;
					}
				}
				else if (iDeltaA == -WORLD_MAP_X && iDeltaB == WORLD_MAP_X)
				{
					if (fZoomFlag)
					{
						iDirection  = NORTH_ZOOM_LINE;
						iArrow      = fSpeedFlag ? ZOOM_Y_NORTH_ARROW : ZOOM_NORTH_ARROW;
						iArrowX    += NORTH_OFFSET_X * 2;
						iArrowY    += NORTH_OFFSET_Y * 2;
					}
					else
					{
						iDirection  = NORTH_LINE;
						iArrow      = fSpeedFlag ? Y_NORTH_ARROW : NORTH_ARROW;
						iArrowX    += NORTH_OFFSET_X;
						iArrowY    += NORTH_OFFSET_Y;
					}
				}
				else if (iDeltaA == WORLD_MAP_X && iDeltaB == -WORLD_MAP_X)
				{
					if (fZoomFlag)
					{
						iDirection  = SOUTH_ZOOM_LINE;
						iArrow      = fSpeedFlag ? ZOOM_Y_SOUTH_ARROW : ZOOM_SOUTH_ARROW;
						iArrowX    += SOUTH_OFFSET_X * 2;
						iArrowY    += SOUTH_OFFSET_Y * 2;
					}
					else
					{
						iDirection  = SOUTH_LINE;
						iArrow      = fSpeedFlag ? Y_SOUTH_ARROW : SOUTH_ARROW;
						iArrowX    += SOUTH_OFFSET_X;
						iArrowY    += SOUTH_OFFSET_Y;
					}
				}
				else if (iDeltaA == -WORLD_MAP_X && iDeltaB == -1)
				{
					if (fZoomFlag)
					{
						iDirection  = N_TO_E_ZOOM_LINE;
						iArrow      = fSpeedFlag ? ZOOM_Y_EAST_ARROW : ZOOM_EAST_ARROW;
						iArrowX    += EAST_OFFSET_X * 2;
						iArrowY    += EAST_OFFSET_Y * 2;
					}
					else
					{
						iDirection  = N_TO_E_LINE;
						iArrow      = fSpeedFlag ? Y_EAST_ARROW : EAST_ARROW;
						iArrowX    += EAST_OFFSET_X;
						iArrowY    += EAST_OFFSET_Y;
					}
				}
				else if (iDeltaA == WORLD_MAP_X && iDeltaB == 1)
				{
					if (fZoomFlag)
					{
						iDirection  = S_TO_W_ZOOM_LINE;
						iArrow      = fSpeedFlag ? ZOOM_Y_WEST_ARROW : ZOOM_WEST_ARROW;
						iArrowX    += WEST_OFFSET_X * 2;
						iArrowY    += WEST_OFFSET_Y * 2;
					}
					else
					{
						iDirection  = S_TO_W_LINE;
						iArrow      = fSpeedFlag ? Y_WEST_ARROW : WEST_ARROW;
						iArrowX    += WEST_OFFSET_X;
						iArrowY    += WEST_OFFSET_Y;
					}
				}
				else if (iDeltaA == 1 && iDeltaB == -WORLD_MAP_X)
				{
					if (fZoomFlag)
					{
						iDirection  = E_TO_S_ZOOM_LINE;
						iArrow      = fSpeedFlag ? ZOOM_Y_SOUTH_ARROW : ZOOM_SOUTH_ARROW;
						iArrowX    += SOUTH_OFFSET_X * 2;
						iArrowY    += SOUTH_OFFSET_Y * 2;
					}
					else
					{
						iDirection  = E_TO_S_LINE;
						iArrow      = fSpeedFlag ? Y_WEST_ARROW : SOUTH_ARROW;
						iArrowX    += SOUTH_OFFSET_X;
						iArrowY    += SOUTH_OFFSET_Y;
					}
				}
				else if (iDeltaA == -1 && iDeltaB == WORLD_MAP_X)
				{
					if (fZoomFlag)
					{
						iDirection  = W_TO_N_ZOOM_LINE;
						iArrow      = fSpeedFlag ? ZOOM_Y_NORTH_ARROW : ZOOM_NORTH_ARROW;
						iArrowX    += NORTH_OFFSET_X * 2;
						iArrowY    += NORTH_OFFSET_Y * 2;
					}
					else
					{
						iDirection  = W_TO_N_LINE;
						iArrow      = fSpeedFlag ? Y_NORTH_ARROW : NORTH_ARROW;
						iArrowX    += NORTH_OFFSET_X;
						iArrowY    += NORTH_OFFSET_Y;
					}
				}
				else if (iDeltaA == -1 && iDeltaB == -WORLD_MAP_X)
				{
					if (fZoomFlag)
					{
						iDirection  = W_TO_S_ZOOM_LINE;
						iArrow      = fSpeedFlag ? ZOOM_Y_SOUTH_ARROW : ZOOM_SOUTH_ARROW;
						iArrowX    += SOUTH_OFFSET_X * 2;
						iArrowY    += (SOUTH_OFFSET_Y + WEST_TO_SOUTH_OFFSET_Y) * 2;
					}
					else
					{
						iDirection  = W_TO_S_LINE;
						iArrow      = fSpeedFlag ? Y_SOUTH_ARROW : SOUTH_ARROW;
						iArrowX    += SOUTH_OFFSET_X;
						iArrowY    += SOUTH_OFFSET_Y + WEST_TO_SOUTH_OFFSET_Y;
					}
				}
				else if (iDeltaA == -WORLD_MAP_X && iDeltaB == 1)
				{
					if (fZoomFlag)
					{
						iDirection  = N_TO_W_ZOOM_LINE;
						iArrow      = fSpeedFlag ? ZOOM_Y_WEST_ARROW : ZOOM_WEST_ARROW;
						iArrowX    += WEST_OFFSET_X * 2;
						iArrowY    += WEST_OFFSET_Y * 2;
					}
					else
					{
						iDirection  = N_TO_W_LINE;
						iArrow      = fSpeedFlag ? Y_WEST_ARROW : WEST_ARROW;
						iArrowX    += WEST_OFFSET_X;
						iArrowY    += WEST_OFFSET_Y;
					}
				}
				else if (iDeltaA == WORLD_MAP_X && iDeltaB == -1)
				{
					if (fZoomFlag)
					{
						iDirection  = S_TO_E_ZOOM_LINE;
						iArrow      = fSpeedFlag ? ZOOM_Y_EAST_ARROW : ZOOM_EAST_ARROW;
						iArrowX    += EAST_OFFSET_X * 2;
						iArrowY    += EAST_OFFSET_Y * 2;
					}
					else
					{
						iDirection  = S_TO_E_LINE;
						iArrow      = fSpeedFlag ? Y_EAST_ARROW : EAST_ARROW;
						iArrowX    += EAST_OFFSET_X;
						iArrowY    += EAST_OFFSET_Y;
					}
				}
				else if (iDeltaA == 1 && iDeltaB == WORLD_MAP_X)
				{
					if (fZoomFlag)
					{
						iDirection   = E_TO_N_ZOOM_LINE;
						iArrow       = fSpeedFlag ? ZOOM_Y_NORTH_ARROW : ZOOM_NORTH_ARROW;
						iArrowX     += NORTH_OFFSET_X * 2;
						iArrowY     += (NORTH_OFFSET_Y + EAST_TO_NORTH_OFFSET_Y) * 2;
					}
					else
					{
						iDirection  = E_TO_N_LINE;
						iArrow      = fSpeedFlag ? Y_NORTH_ARROW : NORTH_ARROW;
						iArrowX    += NORTH_OFFSET_X;
						iArrowY    += NORTH_OFFSET_Y + EAST_TO_NORTH_OFFSET_Y;
					}
				}
			}
		}
		else
		{
			if (!fZoomFlag)
			{
				iX = node->uiSectorId % MAP_WORLD_X;
				iY = node->uiSectorId / MAP_WORLD_X;
				iX = iX * MAP_GRID_X + MAP_VIEW_START_X;
				iY = iY * MAP_GRID_Y + MAP_VIEW_START_Y;
			}
			else
			{
				INT16 sX;
				INT16 sY;
				GetScreenXYFromMapXYStationary(node->uiSectorId % MAP_WORLD_X, node->uiSectorId / MAP_WORLD_X, &sX, &sY);
				iY = sY - MAP_GRID_Y;
				iX = sX - MAP_GRID_X;
			}
			iArrowX = iX;
			iArrowY = iY;
			// display enter and exit 'X's
			if (prev)
			{
				fUTurnFlag = TRUE;
				const INT32 iDeltaA = (INT16)node->uiSectorId - (INT16)prev->uiSectorId;
				if (iDeltaA == -1)
				{
					iDirection = fZoomFlag ? ZOOM_RED_X_WEST : RED_X_WEST;
				}
				else if (iDeltaA == 1)
				{
					iDirection = fZoomFlag ? ZOOM_RED_X_EAST : RED_X_EAST;
				}
				else if (iDeltaA == -WORLD_MAP_X)
				{
					iDirection = fZoomFlag ? ZOOM_RED_X_NORTH : RED_X_NORTH;
				}
				else
				{
					iDirection = fZoomFlag ? ZOOM_RED_X_SOUTH : RED_X_SOUTH;
				}
			}
			if (next)
			{
				fUTurnFlag = FALSE;
				const INT32   iDeltaB    = (INT16)node->uiSectorId - (INT16)next->uiSectorId;
				const BOOLEAN fSpeedFlag = !node->fSpeed;
				if (iDeltaB == -1)
				{
					if (fZoomFlag)
					{
						iDirection  = ZOOM_GREEN_X_EAST;
						iArrow      = fSpeedFlag ? ZOOM_Y_EAST_ARROW : ZOOM_EAST_ARROW;
						iArrowX    += EAST_OFFSET_X * 2;
						iArrowY    += EAST_OFFSET_Y * 2;
					}
					else
					{
						iDirection  = GREEN_X_EAST;
						iArrow      = fSpeedFlag ? Y_EAST_ARROW : EAST_ARROW;
						iArrowX    += EAST_OFFSET_X;
						iArrowY    += EAST_OFFSET_Y;
					}
				}
				else if (iDeltaB == 1)
				{
					if (fZoomFlag)
					{
						iDirection  = ZOOM_GREEN_X_WEST;
						iArrow      = fSpeedFlag ? ZOOM_Y_WEST_ARROW : ZOOM_WEST_ARROW;
						iArrowX    += WEST_OFFSET_X * 2;
						iArrowY    += WEST_OFFSET_Y * 2;
					}
					else
					{
						iDirection  = GREEN_X_WEST;
						iArrow      = fSpeedFlag ? Y_WEST_ARROW : WEST_ARROW;
						iArrowX    += WEST_OFFSET_X;
						iArrowY    += WEST_OFFSET_Y;
					}
				}
				else if (iDeltaB == WORLD_MAP_X)
				{
					if (fZoomFlag)
					{
						iDirection  = ZOOM_GREEN_X_NORTH;
						iArrow      = fSpeedFlag ? ZOOM_Y_NORTH_ARROW : ZOOM_NORTH_ARROW;
						iArrowX    += NORTH_OFFSET_X * 2;
						iArrowY    += NORTH_OFFSET_Y * 2;
					}
					else
					{
						iDirection  = GREEN_X_NORTH;
						iArrow      = fSpeedFlag ? Y_NORTH_ARROW : NORTH_ARROW;
						iArrowX    += NORTH_OFFSET_X;
						iArrowY    += NORTH_OFFSET_Y;
					}
				}
				else
				{
					if (fZoomFlag)
					{
						iDirection  = ZOOM_GREEN_X_SOUTH;
						iArrow      = fSpeedFlag ? ZOOM_Y_SOUTH_ARROW : ZOOM_SOUTH_ARROW;
						iArrowX    += SOUTH_OFFSET_X * 2;
						iArrowY    += SOUTH_OFFSET_Y * 2;
					}
					else
					{
						iDirection  = GREEN_X_SOUTH;
						iArrow      = fSpeedFlag ? Y_SOUTH_ARROW : SOUTH_ARROW;
						iArrowX    += SOUTH_OFFSET_X;
						iArrowY    += SOUTH_OFFSET_Y;
					}
				}
			}
		}

		if (iDirection == -1) continue;

		if (!fZoomFlag ||
				(
					MAP_VIEW_START_X < iX && iX < SCREEN_WIDTH     - MAP_GRID_X * 2 &&
					MAP_VIEW_START_Y < iY && iY < MAP_VIEW_START_Y + MAP_VIEW_HEIGHT
				))
		{
			BltVideoObject(FRAME_BUFFER, guiMAPCURSORS, iDirection, iX, iY);

			if (!fUTurnFlag)
			{
				BltVideoObject(FRAME_BUFFER, guiMAPCURSORS, (UINT16)iArrow, iArrowX, iArrowY);
				InvalidateRegion(iArrowX, iArrowY, iArrowX + 2 * MAP_GRID_X, iArrowY + 2 * MAP_GRID_Y);
			}

			InvalidateRegion(iX, iY, iX + 2 * MAP_GRID_X, iY + 2 * MAP_GRID_Y);
		}
	}
}


static BOOLEAN TraceCharAnimatedRoute(PathSt* pPath, BOOLEAN fForceUpDate);


static void AnimateRoute(PathSt* pPath)
{
	// set buffer
	SetFontDestBuffer(FRAME_BUFFER, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	// the animated path
	if (TraceCharAnimatedRoute(pPath, FALSE))
  {
// ARM? Huh?  Why the same thing twice more?
		TraceCharAnimatedRoute(pPath, TRUE);
		TraceCharAnimatedRoute(pPath, TRUE);
	}
}


// restore backgrounds for arrows
static void RestoreArrowBackgroundsForTrace(INT32 iArrow, INT32 iArrowX, INT32 iArrowY, BOOLEAN fZoom)
{
	INT16 sArrow=0;
  INT32 iX = -1, iY = -1;
	// find location of arrow and restore appropriate background

 if((iArrow==SOUTH_ARROW)||(iArrow==W_SOUTH_ARROW)||(iArrow==ZOOM_W_SOUTH_ARROW)||(ZOOM_SOUTH_ARROW==iArrow))
 {
	 sArrow=SOUTH_ARROW;
 }
 else if((iArrow==NORTH_ARROW)||(iArrow==W_NORTH_ARROW)||(iArrow==ZOOM_W_NORTH_ARROW)||(ZOOM_NORTH_ARROW==iArrow))
 {
   sArrow=NORTH_ARROW;
 }
 else if((iArrow==WEST_ARROW)||(iArrow==W_WEST_ARROW)||(iArrow==ZOOM_W_WEST_ARROW)||(ZOOM_WEST_ARROW==iArrow))
 {
   sArrow=WEST_ARROW;
 }
 else if((iArrow==EAST_ARROW)||(iArrow==W_EAST_ARROW)||(iArrow==ZOOM_W_EAST_ARROW)||(ZOOM_EAST_ARROW==iArrow))
 {
   sArrow=EAST_ARROW;
 }

 switch(sArrow)
 {
	case(SOUTH_ARROW):
	  iX=iArrowX;
		iY=iArrowY;
	break;
	case(NORTH_ARROW):
    iX=iArrowX;
		iY=iArrowY;
	break;
	case(WEST_ARROW):
	  iX=iArrowX;
		iY=iArrowY;
	break;
	case(EAST_ARROW):
	  iX=iArrowX;
		iY=iArrowY;
	break;
 }

 // error check
 if( iX == -1 )
 {
	 return;
 }

 if(!fZoom)
  RestoreExternBackgroundRect(((INT16)iX),((INT16)iY),DMAP_GRID_X/2 ,DMAP_GRID_Y/2 );
 else
  RestoreExternBackgroundRect(((INT16)iX), ((INT16)iY),DMAP_GRID_ZOOM_X, DMAP_GRID_ZOOM_Y);
}


static BOOLEAN TraceCharAnimatedRoute(PathSt* const pPath, const BOOLEAN fForceUpDate)
{
	static PathSt* pCurrentNode = NULL;
 static BOOLEAN fUpDateFlag=FALSE;
 static BOOLEAN fPauseFlag=TRUE;
 static UINT8 ubCounter=1;

 BOOLEAN fSpeedFlag=FALSE;
 INT32 iDifference=0;
 INT32 iArrow=-1;
 INT32 iX = 0, iY = 0;
 INT32 iPastX, iPastY;
 INT16 sX = 0, sY = 0;
 INT32 iArrowX, iArrowY;
 INT32 iDeltaA, iDeltaB, iDeltaB1;
 INT32 iDirection = -1;
 BOOLEAN fUTurnFlag=FALSE;
 BOOLEAN fNextNode=FALSE;
	PathSt* pTempNode = NULL;
	PathSt* pNode     = NULL;
	PathSt* pPastNode = NULL;
	PathSt* pNextNode = NULL;


	// must be plotting movement
	if (bSelectedDestChar == -1 && !fPlotForHelicopter)
 {
	 return FALSE;
 }

 // if any nodes have been deleted, reset current node to beginning of the list
 if( fDeletedNode )
 {
	 fDeletedNode = FALSE;
	 pCurrentNode = NULL;
 }


 // Valid path?
 if ( pPath == NULL )
 {
	 return FALSE;
 }
 else
 {
	 if(pCurrentNode==NULL)
	 {
		 pCurrentNode = pPath;
	 }
 }

 // Check Timer
 if (giAnimateRouteBaseTime==0)
 {
	 giAnimateRouteBaseTime=GetJA2Clock();
	 return FALSE;
 }

 // check difference in time
 iDifference=GetJA2Clock()-giAnimateRouteBaseTime;

 // if pause flag, check time, if time passed, reset, continue on, else return
 if(fPauseFlag)
 {
	 if(iDifference < PAUSE_DELAY)
	 {
		  return FALSE;
	 }
	 else
	 {
		  fPauseFlag=FALSE;
	    giAnimateRouteBaseTime=GetJA2Clock();
	 }
 }


 // if is checkflag and change in status, return TRUE;
 if(!fForceUpDate)
 {
  if(iDifference < ARROW_DELAY)
  {
  	if (!fUpDateFlag)
  		return FALSE;
  }
  else
  {
  	// sufficient time, update base time
    giAnimateRouteBaseTime=GetJA2Clock();
    fUpDateFlag=!fUpDateFlag;

		fNextNode=TRUE;
  }
 }

	// check to see if Current node has not been deleted
	pTempNode = pPath;

  while(pTempNode)
  {
	  if(pTempNode==pCurrentNode)
		{
		  //not deleted
		 //reset pause flag
		 break;
		 }
	 else
		 pTempNode=pTempNode->pNext;
	}

 // if deleted, restart at beginnning
 if(pTempNode==NULL)
 {
	 pCurrentNode = pPath;

	 // set pause flag
	 if(!pCurrentNode)
		 return FALSE;

 }

 // Handle drawing of arrow
 pNode=pCurrentNode;
 if((!pNode->pPrev)&&(ubCounter==1)&&(fForceUpDate))
 {
	 ubCounter=0;
	 return FALSE;
 }
 else if((ubCounter==1)&&(fForceUpDate))
 {
	 pNode=pCurrentNode->pPrev;
 }
 if (pNode->pNext)
	 pNextNode=pNode->pNext;
 else
	 pNextNode=NULL;

 if (pNode->pPrev)
   pPastNode=pNode->pPrev;
 else
	 pPastNode=NULL;

 // go through characters list and display arrows for path
 fUTurnFlag=FALSE;
 if ((pPastNode)&&(pNextNode))
		 {
      iDeltaA=(INT16)pNode->uiSectorId-(INT16)pPastNode->uiSectorId;
		  iDeltaB=(INT16)pNode->uiSectorId-(INT16)pNextNode->uiSectorId;
			if(!pNode->fSpeed)
				fSpeedFlag=TRUE;
			else
				fSpeedFlag=FALSE;
			if (iDeltaA ==0)
		   return FALSE;
      if(!fZoomFlag)
			{
       iX=(pNode->uiSectorId%MAP_WORLD_X);
		   iY=(pNode->uiSectorId/MAP_WORLD_X);
		   iX=(iX*MAP_GRID_X)+MAP_VIEW_START_X;
		   iY=(iY*MAP_GRID_Y)+MAP_VIEW_START_Y;
			}
			else
			{
       GetScreenXYFromMapXYStationary( ((UINT16)(pNode->uiSectorId%MAP_WORLD_X)),((UINT16)(pNode->uiSectorId/MAP_WORLD_X)) , &sX, &sY );
			 iY=sY-MAP_GRID_Y;
			 iX=sX-MAP_GRID_X;
			}
			iArrowX=iX;
			iArrowY=iY;
		  if ((pPastNode->pPrev)&&(pNextNode->pNext))
      {
       fUTurnFlag=FALSE;
				// check to see if out-of sector U-turn
        // for placement of arrows
			 iDeltaB1=pNextNode->uiSectorId-pNextNode->pNext->uiSectorId;
			 if ((iDeltaB1==-WORLD_MAP_X)&&(iDeltaA==-WORLD_MAP_X)&&(iDeltaB==-1))
			 {
				 fUTurnFlag=TRUE;
			 }
			 else if((iDeltaB1==-WORLD_MAP_X)&&(iDeltaA==-WORLD_MAP_X)&&(iDeltaB==1))
			 {
				 fUTurnFlag=TRUE;
			 }
			 else if((iDeltaB1==WORLD_MAP_X)&&(iDeltaA==WORLD_MAP_X)&&(iDeltaB==1))
			 {
				 fUTurnFlag=TRUE;
			 }
			 else if((iDeltaB1==-WORLD_MAP_X)&&(iDeltaA==-WORLD_MAP_X)&&(iDeltaB==1))
			 {
				 fUTurnFlag=TRUE;
			 }
			 else if((iDeltaB1==-1)&&(iDeltaA==-1)&&(iDeltaB==-WORLD_MAP_X))
			 {
				 fUTurnFlag=TRUE;
			 }
			 else if((iDeltaB1==-1)&&(iDeltaA==-1)&&(iDeltaB==WORLD_MAP_X))
			 {
				 fUTurnFlag=TRUE;
			 }
       else if((iDeltaB1==1)&&(iDeltaA==1)&&(iDeltaB==-WORLD_MAP_X))
			 {
				 fUTurnFlag=TRUE;
			 }
        else if((iDeltaB1==1)&&(iDeltaA==1)&&(iDeltaB==WORLD_MAP_X))
			 {
				 fUTurnFlag=TRUE;
			 }
       else
				 fUTurnFlag=FALSE;
			 }


	if ((pPastNode->uiSectorId==pNextNode->uiSectorId))
				{
				 if (pPastNode->uiSectorId+WORLD_MAP_X==pNode->uiSectorId)
				 {
          if(fZoomFlag)
					{
					 iDirection=S_TO_N_ZOOM_LINE;
					 if(!ubCounter)
            iArrow=ZOOM_W_NORTH_ARROW;
					 else if(fSpeedFlag)
            iArrow=ZOOM_Y_NORTH_ARROW;
           else
						iArrow=ZOOM_NORTH_ARROW;
					 iArrowX+=NORTH_OFFSET_X*2;
					 iArrowY+=NORTH_OFFSET_Y*2;
					}
          else
          {
					 iDirection=S_TO_N_LINE;
           if(!ubCounter)
            iArrow=W_NORTH_ARROW;
					 else if(fSpeedFlag)
            iArrow=Y_NORTH_ARROW;
					 else
						iArrow=NORTH_ARROW;

					 iArrowX+=NORTH_OFFSET_X;
					 iArrowY+=NORTH_OFFSET_Y;
					}
				 }
				 else if(pPastNode->uiSectorId-WORLD_MAP_X==pNode->uiSectorId)
				 {
          if(fZoomFlag)
					{
					 iDirection=N_TO_S_ZOOM_LINE;
           if(!ubCounter)
            iArrow=ZOOM_W_SOUTH_ARROW;
					 else if(fSpeedFlag)
            iArrow=ZOOM_Y_SOUTH_ARROW;
					 else
						iArrow=ZOOM_SOUTH_ARROW;
					 iArrowX+=SOUTH_OFFSET_X*2;
					 iArrowY+=SOUTH_OFFSET_Y*2;
					}
          else
					{
           iDirection=N_TO_S_LINE;
           if(!ubCounter)
             iArrow=W_SOUTH_ARROW;
					 else if(fSpeedFlag)
              iArrow=Y_SOUTH_ARROW;
					 else
						 iArrow=SOUTH_ARROW;
					 iArrowX+=SOUTH_OFFSET_X;
					 iArrowY+=SOUTH_OFFSET_Y;
					}
				 }
				 else if (pPastNode->uiSectorId+1==pNode->uiSectorId)
				 {
          if(fZoomFlag)
					{
					 iDirection=E_TO_W_ZOOM_LINE;
           if(!ubCounter)
             iArrow=ZOOM_W_WEST_ARROW;
					 else if (fSpeedFlag)
              iArrow=ZOOM_Y_WEST_ARROW;
					 else
						 iArrow=ZOOM_WEST_ARROW;
					 iArrowX+=WEST_OFFSET_X*2;
					 iArrowY+=WEST_OFFSET_Y*2;
					}
          else
					{
					 iDirection=E_TO_W_LINE;
           if(!ubCounter)
             iArrow=W_WEST_ARROW;
					 else if (fSpeedFlag)
             iArrow=Y_WEST_ARROW;
					 else
						 iArrow=WEST_ARROW;
					 iArrowX+=WEST_OFFSET_X;
					 iArrowY+=WEST_OFFSET_Y;
					}
				 }
				 else
				 {
          if(fZoomFlag)
					{
					 iDirection=W_TO_E_ZOOM_LINE;
           if(!ubCounter)
             iArrow=ZOOM_W_EAST_ARROW;
					 else if (fSpeedFlag)
              iArrow=ZOOM_Y_EAST_ARROW;
					 else
						 iArrow=ZOOM_EAST_ARROW;
					 iArrowX+=EAST_OFFSET_X*2;
					 iArrowY+=EAST_OFFSET_Y*2;
					}
          else
					{
           iDirection=W_TO_E_LINE;
           if(!ubCounter)
            iArrow=W_EAST_ARROW;
					 else if (fSpeedFlag)
            iArrow=Y_EAST_ARROW;
					 else
						iArrow=EAST_ARROW;
					 iArrowX+=EAST_OFFSET_X;
					 iArrowY+=EAST_OFFSET_Y;
					}
				 }
				}
			 else
			 {
        if ((iDeltaA==-1)&&(iDeltaB==1))
				{
          if(fZoomFlag)
					{
					 iDirection=WEST_ZOOM_LINE;
           if(!ubCounter)
            iArrow=ZOOM_W_WEST_ARROW;
					 else if (fSpeedFlag)
            iArrow=ZOOM_Y_WEST_ARROW;
					 else
						iArrow=ZOOM_WEST_ARROW;

	         iArrowX+=WEST_OFFSET_X*2;
					 iArrowY+=WEST_OFFSET_Y*2;
					}
          else
					{
					 iDirection=WEST_LINE;
           if(!ubCounter)
            iArrow=W_WEST_ARROW;
					 else if (fSpeedFlag)
            iArrow=Y_WEST_ARROW;
					 else
						iArrow=WEST_ARROW;


	         iArrowX+=WEST_OFFSET_X;
					 iArrowY+=WEST_OFFSET_Y;
					}
				}
				else if((iDeltaA==1)&&(iDeltaB==-1))
				{
          if(fZoomFlag)
					{
					 iDirection=EAST_ZOOM_LINE;
           if(!ubCounter)
            iArrow=ZOOM_W_EAST_ARROW;
					 else
						iArrow=ZOOM_EAST_ARROW;

					 iArrowX+=EAST_OFFSET_X*2;
					 iArrowY+=EAST_OFFSET_Y*2;
					}
          else
					{
					 iDirection=EAST_LINE;
           if(!ubCounter)
            iArrow=W_EAST_ARROW;
					 else if (fSpeedFlag)
            iArrow=Y_EAST_ARROW;
					 else
						iArrow=EAST_ARROW;

					 iArrowX+=EAST_OFFSET_X;
					 iArrowY+=EAST_OFFSET_Y;
					}
				}
				else if((iDeltaA==-WORLD_MAP_X)&&(iDeltaB==WORLD_MAP_X))
				{
          if(fZoomFlag)
					{
					 iDirection=NORTH_ZOOM_LINE;
           if(!ubCounter)
            iArrow=ZOOM_W_NORTH_ARROW;
					 else if (fSpeedFlag)
            iArrow=ZOOM_Y_NORTH_ARROW;
					 else
					  iArrow=ZOOM_NORTH_ARROW;

					 iArrowX+=NORTH_OFFSET_X*2;
					 iArrowY+=NORTH_OFFSET_Y*2;
					}
          else
					{
					 iDirection=NORTH_LINE;
           if(!ubCounter)
            iArrow=W_NORTH_ARROW;
					 else if (fSpeedFlag)
            iArrow=Y_NORTH_ARROW;
					 else
					  iArrow=NORTH_ARROW;

           iArrowX+=NORTH_OFFSET_X;
					 iArrowY+=NORTH_OFFSET_Y;
					}
				}
				else if((iDeltaA==WORLD_MAP_X)&&(iDeltaB==-WORLD_MAP_X))
				{
         if(fZoomFlag)
				 {
					iDirection=SOUTH_ZOOM_LINE;
          if(!ubCounter)
           iArrow=ZOOM_W_SOUTH_ARROW;
          else if (fSpeedFlag)
            iArrow=ZOOM_Y_SOUTH_ARROW;
					else
					 iArrow=ZOOM_SOUTH_ARROW;

          iArrowX+=SOUTH_OFFSET_X*2;
				  iArrowY+=SOUTH_OFFSET_Y*2;
				 }
         else
				 {
					iDirection=SOUTH_LINE;
          if(!ubCounter)
           iArrow=W_SOUTH_ARROW;
					else if (fSpeedFlag)
           iArrow=Y_SOUTH_ARROW;
					else
					 iArrow=SOUTH_ARROW;

          iArrowX+=SOUTH_OFFSET_X;
				  iArrowY+=SOUTH_OFFSET_Y;
				 }
				}
				else if((iDeltaA==-WORLD_MAP_X)&&(iDeltaB==-1))
				{
          if(fZoomFlag)
					{
					 iDirection=N_TO_E_ZOOM_LINE;
           if(!ubCounter)
            iArrow=ZOOM_W_EAST_ARROW;
					 else if (fSpeedFlag)
           iArrow=ZOOM_Y_EAST_ARROW;
					 else
					  iArrow=ZOOM_EAST_ARROW;

           iArrowX+=EAST_OFFSET_X*2;
					 iArrowY+=EAST_OFFSET_Y*2;
					}
          else
					{
					 iDirection=N_TO_E_LINE;
           if(!ubCounter)
            iArrow=W_EAST_ARROW;
					 else if (fSpeedFlag)
            iArrow=Y_EAST_ARROW;
					 else
					  iArrow=EAST_ARROW;

           iArrowX+=EAST_OFFSET_X;
					 iArrowY+=EAST_OFFSET_Y;
					}
				}
				else if((iDeltaA==WORLD_MAP_X)&&(iDeltaB==1))
				{
          if(fZoomFlag)
					{
					 iDirection=S_TO_W_ZOOM_LINE;
           if(!ubCounter)
             iArrow=ZOOM_W_WEST_ARROW;
					 else if (fSpeedFlag)
             iArrow=ZOOM_Y_WEST_ARROW;
					 else
					   iArrow=ZOOM_WEST_ARROW;

        	 iArrowX+=WEST_OFFSET_X*2;
					 iArrowY+=WEST_OFFSET_Y*2;
					}
          else
					{
					 iDirection=S_TO_W_LINE;
           if(!ubCounter)
             iArrow=W_WEST_ARROW;
					 else if (fSpeedFlag)
              iArrow=Y_WEST_ARROW;
					 else
					   iArrow=WEST_ARROW;


        	 iArrowX+=WEST_OFFSET_X;
					 iArrowY+=WEST_OFFSET_Y;
					}
				}
				else if((iDeltaA==1)&&(iDeltaB==-WORLD_MAP_X))
				{
          if(fZoomFlag)
					{
					 iDirection=E_TO_S_ZOOM_LINE;
           if(!ubCounter)
            iArrow=ZOOM_W_SOUTH_ARROW;
					 else if (fSpeedFlag)
             iArrow=ZOOM_Y_SOUTH_ARROW;
					 else
					  iArrow=ZOOM_SOUTH_ARROW;

           iArrowX+=SOUTH_OFFSET_X*2;
					 iArrowY+=SOUTH_OFFSET_Y*2;
					}
          else
					{
					 iDirection=E_TO_S_LINE;
           if(!ubCounter)
            iArrow=W_SOUTH_ARROW;
					 else if (fSpeedFlag)
             iArrow=Y_SOUTH_ARROW;
					 else
					  iArrow=SOUTH_ARROW;

           iArrowX+=SOUTH_OFFSET_X;
					 iArrowY+=SOUTH_OFFSET_Y;
					}
				}
				else if ((iDeltaA==-1)&&(iDeltaB==WORLD_MAP_X))
				{
          if(fZoomFlag)
					{
					 iDirection=W_TO_N_ZOOM_LINE;
           if(!ubCounter)
            iArrow=ZOOM_W_NORTH_ARROW;
					 else if (fSpeedFlag)
              iArrow=ZOOM_Y_NORTH_ARROW;
					 else
					  iArrow=ZOOM_NORTH_ARROW;

           iArrowX+=NORTH_OFFSET_X*2;
					 iArrowY+=NORTH_OFFSET_Y*2;
					}
          else
					{
					 iDirection=W_TO_N_LINE;
           if(!ubCounter)
            iArrow=W_NORTH_ARROW;
					 else if (fSpeedFlag)
              iArrow=Y_NORTH_ARROW;
					 else
					  iArrow=NORTH_ARROW;

           iArrowX+=NORTH_OFFSET_X;
					 iArrowY+=NORTH_OFFSET_Y;
					}
				}
				else if ((iDeltaA==-1)&&(iDeltaB==-WORLD_MAP_X))
				{
          if(fZoomFlag)
					{
					 iDirection=W_TO_S_ZOOM_LINE;
           if(!ubCounter)
            iArrow=ZOOM_W_SOUTH_ARROW;
					 else if (fSpeedFlag)
              iArrow=ZOOM_Y_SOUTH_ARROW;
					 else
					  iArrow=ZOOM_SOUTH_ARROW;

           iArrowX+=SOUTH_OFFSET_X*2;
					 iArrowY+=(SOUTH_OFFSET_Y+WEST_TO_SOUTH_OFFSET_Y)*2;
					}
          else
					{
					 iDirection=W_TO_S_LINE;
           if(!ubCounter)
            iArrow=W_SOUTH_ARROW;
					 else if (fSpeedFlag)
            iArrow=Y_SOUTH_ARROW;
					 else
					  iArrow=SOUTH_ARROW;
           iArrowX+=SOUTH_OFFSET_X;
					 iArrowY+=(SOUTH_OFFSET_Y+WEST_TO_SOUTH_OFFSET_Y);
					}
				}
				else if ((iDeltaA==-WORLD_MAP_X)&&(iDeltaB==1))
				{
          if(fZoomFlag)
					{
					 iDirection=N_TO_W_ZOOM_LINE;
           if(!ubCounter)
            iArrow=ZOOM_W_WEST_ARROW;
					 else if (fSpeedFlag)
            iArrow=ZOOM_Y_WEST_ARROW;
					 else
					  iArrow=ZOOM_WEST_ARROW;

           iArrowX+=WEST_OFFSET_X*2;
					 iArrowY+=WEST_OFFSET_Y*2;
					}
          else
					{
					 iDirection=N_TO_W_LINE;
           if(!ubCounter)
            iArrow=W_WEST_ARROW;
					 else if (fSpeedFlag)
            iArrow=Y_WEST_ARROW;
					 else
					  iArrow=WEST_ARROW;

           iArrowX+=WEST_OFFSET_X;
					 iArrowY+=WEST_OFFSET_Y;
					}
				}
				else if ((iDeltaA==WORLD_MAP_X)&&(iDeltaB==-1))
				{
          if(fZoomFlag)
					{
					 iDirection=S_TO_E_ZOOM_LINE;
           if(!ubCounter)
            iArrow=ZOOM_W_EAST_ARROW;
					 else if (fSpeedFlag)
            iArrow=ZOOM_Y_EAST_ARROW;
					 else
					  iArrow=ZOOM_EAST_ARROW;
	         iArrowX+=EAST_OFFSET_X*2;
					 iArrowY+=EAST_OFFSET_Y*2;
					}
          else
					{
					 iDirection=S_TO_E_LINE;
           if(!ubCounter)
            iArrow=W_EAST_ARROW;
           else if (fSpeedFlag)
            iArrow=Y_EAST_ARROW;
					 else
					  iArrow=EAST_ARROW;
					 iArrowX+=EAST_OFFSET_X;
					 iArrowY+=EAST_OFFSET_Y;
					}
				}
				else if ((iDeltaA==1)&&(iDeltaB==WORLD_MAP_X))
				{
          if(fZoomFlag)
					{
					 iDirection=E_TO_N_ZOOM_LINE;
           if(!ubCounter)
            iArrow=ZOOM_W_NORTH_ARROW;
					 else if (fSpeedFlag)
             iArrow=ZOOM_Y_NORTH_ARROW;
					 else
					  iArrow=ZOOM_NORTH_ARROW;
           iArrowX+=(NORTH_OFFSET_X*2);
					 iArrowY+=(NORTH_OFFSET_Y+EAST_TO_NORTH_OFFSET_Y)*2;
					}
          else
					{
					 iDirection=E_TO_N_LINE;
           if(!ubCounter)
            iArrow=W_NORTH_ARROW;
					 else if (fSpeedFlag)
             iArrow=Y_NORTH_ARROW;
					 else
					   iArrow=NORTH_ARROW;

           iArrowX+=NORTH_OFFSET_X;
					 iArrowY+=NORTH_OFFSET_Y+EAST_TO_NORTH_OFFSET_Y;
					}
				}
			 }

		 }

     else
		 {
		  iX=(pNode->uiSectorId%MAP_WORLD_X);
		  iY=(pNode->uiSectorId/MAP_WORLD_X);
		  iX=(iX*MAP_GRID_X)+MAP_VIEW_START_X;
		  iY=(iY*MAP_GRID_Y)+MAP_VIEW_START_Y;
			if(pPastNode)
			{
		   iPastX=(pPastNode->uiSectorId%MAP_WORLD_X);
       iPastY=(pPastNode->uiSectorId/MAP_WORLD_X);
       iPastX=(iPastX*MAP_GRID_X)+MAP_VIEW_START_X;
		   iPastY=(iPastY*MAP_GRID_Y)+MAP_VIEW_START_Y;
			}
      if(pNode->fSpeed)
				fSpeedFlag=TRUE;
			else
				fSpeedFlag=FALSE;
			iArrowX=iX;
			iArrowY=iY;
			// display enter and exit 'X's
      if (pPastNode)
			{
				// red 'X'
				fUTurnFlag=TRUE;
        iDeltaA=(INT16)pNode->uiSectorId-(INT16)pPastNode->uiSectorId;
				if (iDeltaA==-1)
				{
					iDirection=RED_X_WEST;
		      //iX+=RED_WEST_OFF_X;
				}
				else if (iDeltaA==1)
				{
					iDirection=RED_X_EAST;
					//iX+=RED_EAST_OFF_X;
				}
				else if(iDeltaA==-WORLD_MAP_X)
				{
					iDirection=RED_X_NORTH;
					//iY+=RED_NORTH_OFF_Y;
				}
				else
				{
					iDirection=RED_X_SOUTH;
				//	iY+=RED_SOUTH_OFF_Y;
				}
			}
			if (pNextNode)
			{
       fUTurnFlag=FALSE;
       iDeltaB=(INT16)pNode->uiSectorId-(INT16)pNextNode->uiSectorId;
       	if (iDeltaB==-1)
				{
					iDirection=GREEN_X_EAST;
          if(!ubCounter)
            iArrow=W_EAST_ARROW;
					else if (fSpeedFlag)
            iArrow=Y_EAST_ARROW;
					else
					  iArrow=EAST_ARROW;

          iArrowX+=EAST_OFFSET_X;
					iArrowY+=EAST_OFFSET_Y;
					//iX+=RED_EAST_OFF_X;
				}
				else if (iDeltaB==1)
				{
					iDirection=GREEN_X_WEST;
           if(!ubCounter)
            iArrow=W_WEST_ARROW;
					 else if (fSpeedFlag)
            iArrow=Y_WEST_ARROW;
					 else
					  iArrow=WEST_ARROW;

          iArrowX+=WEST_OFFSET_X;
					iArrowY+=WEST_OFFSET_Y;
					//iX+=RED_WEST_OFF_X;
				}
				else if(iDeltaB==WORLD_MAP_X)
				{
					iDirection=GREEN_X_NORTH;
          if(!ubCounter)
           iArrow=W_NORTH_ARROW;
					else if (fSpeedFlag)
            iArrow=Y_NORTH_ARROW;
					else
					 iArrow=NORTH_ARROW;

        	iArrowX+=NORTH_OFFSET_X;
					iArrowY+=NORTH_OFFSET_Y;
					//iY+=RED_NORTH_OFF_Y;
				}
				else
				{
					iDirection=GREEN_X_SOUTH;
          if(!ubCounter)
            iArrow=W_SOUTH_ARROW;
					else if (fSpeedFlag)
           iArrow=Y_SOUTH_ARROW;
					else
					 iArrow=SOUTH_ARROW;
          iArrowX+=SOUTH_OFFSET_X;
					iArrowY+=SOUTH_OFFSET_Y;
					//iY+=RED_SOUTH_OFF_Y;
				}


		 }
		 }
	    if(fNextNode)
			{
				if(!ubCounter)
				{
	      pCurrentNode=pCurrentNode->pNext;
		    if(!pCurrentNode)
			   fPauseFlag=TRUE;
				}
			}
		if ((iDirection !=-1)&&(iArrow!=-1))
		{

      if(!fUTurnFlag)
			{
				if (!fZoomFlag ||
						(MAP_VIEW_START_X < iX && iX < SCREEN_WIDTH - MAP_GRID_X * 2 && MAP_VIEW_START_Y < iY && iY < MAP_VIEW_START_Y + MAP_VIEW_HEIGHT))
			 {

         //if(!fZoomFlag)
          //RestoreExternBackgroundRect(((INT16)iArrowX),((INT16)iArrowY),DMAP_GRID_X, DMAP_GRID_Y);
				 //else
          //RestoreExternBackgroundRect(((INT16)iArrowX), ((INT16)iArrowY),DMAP_GRID_ZOOM_X, DMAP_GRID_ZOOM_Y);
				 if( pNode != pPath )
				 {
					BltVideoObject(FRAME_BUFFER, guiMAPCURSORS, (UINT16)iArrow, iArrowX, iArrowY);
					 InvalidateRegion( iArrowX, iArrowY, iArrowX + 2 * MAP_GRID_X, iArrowY + 2 * MAP_GRID_Y );
				 }
			 }
       if(ubCounter==1)
	      ubCounter=0;
	     else
        ubCounter=1;
		   return TRUE;
			}
      if(ubCounter==1)
	     ubCounter=0;
	    else
       ubCounter=1;


		}
		// move to next arrow


//ARM who knows what it should return here?
	return FALSE;
}


// display potential path, yes or no?
void DisplayThePotentialPathForHelicopter(INT16 sMapX, INT16 sMapY )
{
	// simply check if we want to refresh the screen to display path
	static BOOLEAN fOldShowAirCraft = FALSE;
  static INT16  sOldMapX, sOldMapY;
	INT32 iDifference = 0;


	if( fOldShowAirCraft != fShowAircraftFlag )
	{
		fOldShowAirCraft = fShowAircraftFlag;
	  giPotHeliPathBaseTime = GetJA2Clock( );

		sOldMapX = sMapX;
		sOldMapY = sMapY;
		fTempPathAlreadyDrawn = FALSE;
		fDrawTempHeliPath = FALSE;

	}

	if( ( sMapX != sOldMapX) || ( sMapY != sOldMapY ) )
	{
		giPotHeliPathBaseTime = GetJA2Clock( );

		sOldMapX = sMapX;
		sOldMapY = sMapY;

		// path was plotted and we moved, re draw map..to clean up mess
		if( fTempPathAlreadyDrawn )
		{
			fMapPanelDirty = TRUE;
		}

		fTempPathAlreadyDrawn = FALSE;
		fDrawTempHeliPath = FALSE;
	}

	iDifference = GetJA2Clock( ) - giPotHeliPathBaseTime ;

	if( fTempPathAlreadyDrawn )
	{
		return;
	}

	if( iDifference > MIN_WAIT_TIME_FOR_TEMP_PATH )
	{
		fDrawTempHeliPath = TRUE;
		giPotHeliPathBaseTime = GetJA2Clock( );
		fTempPathAlreadyDrawn = TRUE;
	}
}


BOOLEAN IsTheCursorAllowedToHighLightThisSector( INT16 sSectorX, INT16 sSectorY )
{
	// check to see if this sector is a blocked out sector?

	if( sBadSectorsList[ sSectorX ][ sSectorY ] )
	{
		return  ( FALSE );
	}
	else
	{
		// return cursor is allowed to highlight this sector
		return ( TRUE );
	}
}

void SetUpBadSectorsList( void )
{
	// initalizes all sectors to highlighable and then the ones non highlightable are marked as such
	INT8 bY;

	memset( &sBadSectorsList, 0, sizeof( sBadSectorsList ) );

	// the border regions
	for( bY = 0; bY < WORLD_MAP_X; bY++ )
	{
	  sBadSectorsList[ 0 ][ bY ] = sBadSectorsList[ WORLD_MAP_X - 1 ][ bY ] = sBadSectorsList[ bY ][ 0 ] = sBadSectorsList[ bY ][ WORLD_MAP_X - 1 ] = TRUE;
	}


	sBadSectorsList[ 4 ][ 1 ] = TRUE;
	sBadSectorsList[ 5 ][ 1 ] = TRUE;
	sBadSectorsList[ 16 ][ 1 ] = TRUE;
	sBadSectorsList[ 16 ][ 5 ] = TRUE;
	sBadSectorsList[ 16 ][ 6 ] = TRUE;


	sBadSectorsList[ 16 ][ 10 ] = TRUE;
	sBadSectorsList[ 16 ][ 11 ] = TRUE;
	sBadSectorsList[ 16 ][ 12 ] = TRUE;
	sBadSectorsList[ 16 ][ 13 ] = TRUE;
	sBadSectorsList[ 16 ][ 14 ] = TRUE;
	sBadSectorsList[ 16 ][ 15 ] = TRUE;
	sBadSectorsList[ 16 ][ 16 ] = TRUE;

	sBadSectorsList[ 15 ][ 13 ] = TRUE;
	sBadSectorsList[ 15 ][ 14 ] = TRUE;
	sBadSectorsList[ 15 ][ 15 ] = TRUE;
	sBadSectorsList[ 15 ][ 16 ] = TRUE;

	sBadSectorsList[ 14 ][ 14 ] = TRUE;
	sBadSectorsList[ 14 ][ 15 ] = TRUE;
	sBadSectorsList[ 14 ][ 16 ] = TRUE;

	sBadSectorsList[ 13 ][ 14 ] = TRUE;
}



void RestoreBackgroundForMapGrid( INT16 sMapX, INT16 sMapY )
{
	INT16 sX, sY;

	if(!fZoomFlag)
	{
		// screen values
		sX=(sMapX * MAP_GRID_X ) + MAP_VIEW_START_X;
		sY=(sMapY * MAP_GRID_Y ) + MAP_VIEW_START_Y;

		// restore background
		RestoreExternBackgroundRect( sX, sY ,DMAP_GRID_X ,DMAP_GRID_Y );
	}
	else
	{

		// get screen coords from map values
    GetScreenXYFromMapXYStationary( sMapX, sMapY, &sX, &sY );

		// is this on the screen?
		if( ( sX > MapScreenRect.iLeft ) && ( sX < MapScreenRect.iRight ) && ( sY > MapScreenRect.iTop ) && ( sY < MapScreenRect.iBottom ) )
		{
			// offset
			sY=sY-MAP_GRID_Y;
		  sX=sX-MAP_GRID_X;

			// restore
		  RestoreExternBackgroundRect( sX, sY ,DMAP_GRID_ZOOM_X ,DMAP_GRID_ZOOM_Y );
		}
	}

}


void ClipBlitsToMapViewRegion( void )
{
	// the standard mapscreen rectangle doesn't work for clipping while zoomed...
	SGPRect ZoomedMapScreenClipRect={	MAP_VIEW_START_X + MAP_GRID_X, MAP_VIEW_START_Y + MAP_GRID_Y - 1, MAP_VIEW_START_X + MAP_VIEW_WIDTH + MAP_GRID_X, MAP_VIEW_START_Y + MAP_VIEW_HEIGHT + MAP_GRID_Y - 10 };
	SGPRect *pRectToUse;

	if (fZoomFlag)
		pRectToUse = &ZoomedMapScreenClipRect;
	else
		pRectToUse = &MapScreenRect;

	SetClippingRect( pRectToUse );
	gOldClipRect = gDirtyClipRect;
	gDirtyClipRect = *pRectToUse;
}

void RestoreClipRegionToFullScreen( void )
{
	SetClippingRect( &FullScreenRect );
	gDirtyClipRect = gOldClipRect;
}


void ClipBlitsToMapViewRegionForRectangleAndABit( UINT32 uiDestPitchBYTES )
{
	// clip blits to map view region
	// because MC's map coordinates system is so screwy, these had to be hand-tuned to work right...  ARM
	if (fZoomFlag)
		SetClippingRegionAndImageWidth( uiDestPitchBYTES, MapScreenRect.iLeft + 2, MapScreenRect.iTop, MapScreenRect.iRight - MapScreenRect.iLeft, MapScreenRect.iBottom - MapScreenRect.iTop );
	else
		SetClippingRegionAndImageWidth( uiDestPitchBYTES, MapScreenRect.iLeft - 1, MapScreenRect.iTop - 1, MapScreenRect.iRight - MapScreenRect.iLeft + 3, MapScreenRect.iBottom - MapScreenRect.iTop + 2 );
}

void RestoreClipRegionToFullScreenForRectangle( UINT32 uiDestPitchBYTES )
{
	// clip blits to map view region
	SetClippingRegionAndImageWidth(uiDestPitchBYTES, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}


// mvt icon offset defines
#define SOUTH_Y_MVT_OFFSET +10
#define SOUTH_X_MVT_OFFSET 0
#define NORTH_Y_MVT_OFFSET -10
#define NORTH_X_MVT_OFFSET  +10
#define NORTH_SOUTH_CENTER_OFFSET +5


#define SOUTH_Y_MVT_OFFSET_ZOOM +30
#define SOUTH_X_MVT_OFFSET_ZOOM +5
#define NORTH_Y_MVT_OFFSET_ZOOM -8
#define NORTH_X_MVT_OFFSET_ZOOM +25
#define NORTH_SOUTH_CENTER_OFFSET_ZOOM +15

#define EAST_Y_MVT_OFFSET + 8
#define EAST_X_MVT_OFFSET 15
#define WEST_Y_MVT_OFFSET -2
#define WEST_X_MVT_OFFSET -8
#define EAST_WEST_CENTER_OFFSET +2

#define EAST_Y_MVT_OFFSET_ZOOM +24
#define EAST_X_MVT_OFFSET_ZOOM +36
#define WEST_Y_MVT_OFFSET_ZOOM  +5
#define WEST_X_MVT_OFFSET_ZOOM -10
#define EAST_WEST_CENTER_OFFSET_ZOOM +12

#define NORTH_TEXT_X_OFFSET +1
#define NORTH_TEXT_Y_OFFSET +4
#define SOUTH_TEXT_X_OFFSET +1
#define SOUTH_TEXT_Y_OFFSET +2

#define EAST_TEXT_X_OFFSET + 2
#define EAST_TEXT_Y_OFFSET 0
#define WEST_TEXT_X_OFFSET + 4
#define WEST_TEXT_Y_OFFSET 0


#define ICON_WIDTH 8


// show the icons for people in motion
static void ShowPeopleInMotion(INT16 sX, INT16 sY)
{
	INT32 sExiting = 0;
	INT32 sEntering = 0;
	INT16 sDest = 0;
	INT16 sSource = 0;
	INT16 sOffsetX = 0, sOffsetY = 0;
	INT16 iX = sX, iY = sY;
	INT16 sXPosition = 0, sYPosition = 0;
	INT32 iCounter = 0;
	BOOLEAN fAboutToEnter = FALSE;
	CHAR16 sString[ 32 ];
	INT16 sTextXOffset = 0;
	INT16 sTextYOffset = 0;
	INT16 usX, usY;
	INT32 iWidth = 0, iHeight = 0;
	INT32 iDeltaXForError = 0, iDeltaYForError = 0;


	if( iCurrentMapSectorZ != 0 )
	{
		return;
	}


	// show the icons for people in motion from this sector to the next guy over
	for( iCounter = 0; iCounter < 4; iCounter++ )
	{
		// find how many people are coming and going in this sector
		sExiting = 0;
		sEntering = 0;
		sSource = CALCULATE_STRATEGIC_INDEX( sX, sY );
		sOffsetX = 0;
		sOffsetY = 0;
		iX = sX;
		iY = sY;

		// reset fact about to enter
		fAboutToEnter = FALSE;

		sDest = sSource;

		if( ( iCounter == 0 ) && sY > 1 )
		{
			sDest += NORTH_MOVE;
		}
		else if( ( iCounter == 1 ) && ( sX < MAP_WORLD_X - 1 ) )
		{
			sDest += EAST_MOVE;
		}
		else if( ( iCounter == 2 ) && ( sY < MAP_WORLD_Y - 1 ) )
		{
			sDest += SOUTH_MOVE;
		}
		else if( ( iCounter == 3 ) && ( sX > 1 ) )
		{
			sDest += WEST_MOVE;
		}

		// if not at edge of map
		if ( sDest != sSource )
		{
			if( PlayersBetweenTheseSectors( ( INT16 ) SECTOR( sSource % MAP_WORLD_X, sSource / MAP_WORLD_X ) , ( INT16 ) SECTOR( sDest % MAP_WORLD_X, sDest / MAP_WORLD_X ), &sExiting, &sEntering, &fAboutToEnter ) )
			{
				// someone is leaving

				// now find position
				if( !( iCounter % 2 ) )
				{
					// guys going north or south
					if( sEntering > 0 )
					{
						// more than one coming in, offset from middle
						sOffsetX = ( !iCounter ? ( !fZoomFlag ? NORTH_X_MVT_OFFSET: NORTH_X_MVT_OFFSET_ZOOM ) : ( !fZoomFlag ? SOUTH_X_MVT_OFFSET: SOUTH_X_MVT_OFFSET_ZOOM )  );
					}
					else
					{
						sOffsetX = ( !fZoomFlag ?  NORTH_SOUTH_CENTER_OFFSET : NORTH_SOUTH_CENTER_OFFSET_ZOOM );
					}

					if( !iCounter )
					{
						// going north
						sOffsetY = ( !fZoomFlag ? NORTH_Y_MVT_OFFSET : NORTH_Y_MVT_OFFSET_ZOOM );
					}
					else
					{
						// going south
						sOffsetY = ( !fZoomFlag ? SOUTH_Y_MVT_OFFSET : SOUTH_Y_MVT_OFFSET_ZOOM );
					}
				}
				else
				{
					// going east/west

					if( sEntering > 0 )
					{
						// people also entering, offset from middle
						sOffsetY = ( iCounter == 1? ( !fZoomFlag ? EAST_Y_MVT_OFFSET: EAST_Y_MVT_OFFSET_ZOOM ) : ( !fZoomFlag ? WEST_Y_MVT_OFFSET: WEST_Y_MVT_OFFSET_ZOOM )  );
					}
					else
					{
						sOffsetY = ( !fZoomFlag ?  EAST_WEST_CENTER_OFFSET : EAST_WEST_CENTER_OFFSET_ZOOM );
					}

					if( iCounter == 1 )
					{
						// going east
						sOffsetX = ( !fZoomFlag ? EAST_X_MVT_OFFSET : EAST_X_MVT_OFFSET_ZOOM );
					}
					else
					{
						// going west
						sOffsetX = ( !fZoomFlag ? WEST_X_MVT_OFFSET : WEST_X_MVT_OFFSET_ZOOM );
					}
				}

				switch( iCounter )
				{
					case 0:
						sTextXOffset = NORTH_TEXT_X_OFFSET;
						sTextYOffset = NORTH_TEXT_Y_OFFSET;
						break;
					case 1:
						sTextXOffset = EAST_TEXT_X_OFFSET;
						sTextYOffset = EAST_TEXT_Y_OFFSET;
						break;
					case 2:
						sTextXOffset = SOUTH_TEXT_X_OFFSET;
						sTextYOffset = SOUTH_TEXT_Y_OFFSET;
						break;
					case 3:
						sTextXOffset = WEST_TEXT_X_OFFSET;
						sTextYOffset = WEST_TEXT_Y_OFFSET;
						break;
				}


				// blit the text

				SetFont( MAP_MVT_ICON_FONT );

				if( !fAboutToEnter )
				{
					SetFontForeground( FONT_WHITE );
				}
				else
				{
					SetFontForeground( FONT_BLACK );
				}

				SetFontBackground( FONT_BLACK );

				swprintf( sString, lengthof(sString), L"%d", sExiting );

				// if about to enter, draw yellow arrows, blue otherwise
				const SGPVObject* const hIconHandle = (fAboutToEnter ? guiCHARBETWEENSECTORICONSCLOSE : guiCHARBETWEENSECTORICONS);

				// zoomed in or not?
				if(!fZoomFlag)
				{
					iX = MAP_VIEW_START_X+( iX * MAP_GRID_X ) + sOffsetX;
					iY = MAP_Y_ICON_OFFSET + MAP_VIEW_START_Y + ( iY * MAP_GRID_Y ) + sOffsetY;

					BltVideoObject(guiSAVEBUFFER, hIconHandle, ( UINT16 )iCounter , ( INT16 ) iX, ( INT16 ) iY);
				}
				else
				{
					GetScreenXYFromMapXYStationary( ((UINT16)(iX)),((UINT16)(iY)) , &sXPosition, &sYPosition );

					iY=sYPosition-MAP_GRID_Y + sOffsetY;
					iX=sXPosition-MAP_GRID_X + sOffsetX;

					// clip blits to mapscreen region
					ClipBlitsToMapViewRegion( );

					BltVideoObject(guiSAVEBUFFER, hIconHandle,( UINT16 )iCounter , iX   , iY);

					// restore clip blits
					RestoreClipRegionToFullScreen( );
				}

				FindFontCenterCoordinates(iX + sTextXOffset, 0, ICON_WIDTH, 0, sString, MAP_FONT, &usX, &usY);
				SetFontDestBuffer(guiSAVEBUFFER, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
				MPrint(usX, iY + sTextYOffset, sString);

				switch( iCounter % 2 )
				{
					case 0 :
						// north south
						iWidth = 10;
						iHeight = 12;
					break;
					case 1 :
						// east west
						iWidth = 12;
						iHeight = 7;
					break;

				}

				// error correction for scrolling with people on the move
				if( iX < 0 )
				{
					iDeltaXForError = 0 - iX;
					iWidth -= iDeltaXForError;
					iX = 0;
				}

				if( iY < 0 )
				{
					iDeltaYForError = 0 - iY;
					iHeight -= iDeltaYForError;
					iY = 0;
				}

				if( ( iWidth > 0 )&&( iHeight > 0 ) )
				{
					RestoreExternBackgroundRect( iX, iY, ( INT16 )iWidth, ( INT16 )iHeight );
				}
			}
		}
	}

	// restore buffer
	SetFontDestBuffer(FRAME_BUFFER, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}


/* calculate the distance travelled, the proposed distance, and total distance
 * one can go and display these on screen */
void DisplayDistancesForHelicopter()
{
	static INT16 sOldYPosition = 0;

	INT16 sMapX;
	INT16 sMapY;
	INT16 const sYPosition =
		!fZoomFlag && GetMouseMapXY(&sMapX, &sMapY) && sMapY >= 13 ?
			MAP_HELICOPTER_UPPER_ETA_POPUP_Y : MAP_HELICOPTER_ETA_POPUP_Y;

	if (sOldYPosition != 0 && sOldYPosition != sYPosition)
	{
		RestoreExternBackgroundRect(MAP_HELICOPTER_ETA_POPUP_X, sOldYPosition, MAP_HELICOPTER_ETA_POPUP_WIDTH + 20, MAP_HELICOPTER_ETA_POPUP_HEIGHT);
	}
	sOldYPosition = sYPosition;

	BltVideoObject(FRAME_BUFFER, guiMapBorderHeliSectors, 0, MAP_HELICOPTER_ETA_POPUP_X, sYPosition);

	SetFont(MAP_FONT);
	SetFontForeground(FONT_LTGREEN);
	SetFontBackground(FONT_BLACK);

	INT32 const x = MAP_HELICOPTER_ETA_POPUP_X + 5;
	INT32       y = sYPosition + 5;
	INT32 const w = MAP_HELICOPTER_ETA_POPUP_WIDTH;
	INT32 const h = GetFontHeight(MAP_FONT);
	wchar_t     sString[32];
	INT16       sX;
	INT16       sY;

	MPrint(x, y, pHelicopterEtaStrings[0]);
	INT32 const total_distance = DistanceOfIntendedHelicopterPath();
	swprintf(sString, lengthof(sString), L"%d", total_distance);
	FindFontRightCoordinates(x, y, w, 0, sString, MAP_FONT,  &sX, &sY);
	MPrint(sX, y, sString);
	y += h;

	MPrint(x, y, pHelicopterEtaStrings[1]);
  INT16 const n_safe_sectors = GetNumSafeSectorsInPath();
	swprintf(sString, lengthof(sString), L"%d", n_safe_sectors);
	FindFontRightCoordinates(x, y, w, 0, sString, MAP_FONT, &sX, &sY);
	MPrint(sX, y, sString);
	y += h;

	MPrint(x, y, pHelicopterEtaStrings[2]);
  INT16 const n_unsafe_sectors = GetNumUnSafeSectorsInPath();
	swprintf(sString, lengthof(sString), L"%d", n_unsafe_sectors);
	FindFontRightCoordinates(x, y, w, 0, sString, MAP_FONT, &sX, &sY);
	MPrint(sX, y, sString);
	y += h;

	// calculate the cost of the trip based on the number of safe and unsafe sectors it will pass through
	MPrint(x, y, pHelicopterEtaStrings[3]);
	UINT32 const uiTripCost = n_safe_sectors * COST_AIRSPACE_SAFE + n_unsafe_sectors * COST_AIRSPACE_UNSAFE;
	SPrintMoney(sString, uiTripCost);
	FindFontRightCoordinates(x, y, w, 0, sString, MAP_FONT, &sX, &sY);
	MPrint(sX, y, sString);
	y += h;

	MPrint(x, y, pHelicopterEtaStrings[4]);
	// get travel time for the last path segment
	INT32 iTime = GetPathTravelTimeDuringPlotting(pTempHelicopterPath);
	// add travel time for any prior path segments (stored in the helicopter's mercpath, but waypoints aren't built)
	iTime += GetPathTravelTimeDuringPlotting(GetHelicopter()->pMercPath);
	swprintf(sString, lengthof(sString), L"%d%ls %d%ls", iTime / 60, gsTimeStrings[0], iTime % 60, gsTimeStrings[1]);
	FindFontRightCoordinates(x, y, w, 0, sString, MAP_FONT, &sX, &sY);
	MPrint(sX, y, sString);
	y += h;

	// show # of passengers aboard the chopper
	MPrint(x, y, pHelicopterEtaStrings[6]);
	swprintf(sString, lengthof(sString), L"%d", GetNumberOfPassengersInHelicopter());
	FindFontRightCoordinates(x, y, w, 0, sString, MAP_FONT, &sX, &sY);
	MPrint(sX, y, sString);

	InvalidateRegion(MAP_HELICOPTER_ETA_POPUP_X, sOldYPosition, MAP_HELICOPTER_ETA_POPUP_X + MAP_HELICOPTER_ETA_POPUP_WIDTH + 20, sOldYPosition + MAP_HELICOPTER_ETA_POPUP_HEIGHT);
}


static void DisplayDestinationOfHelicopter(void);


// grab position of helicopter and blt to screen
void DisplayPositionOfHelicopter( void )
{
	static INT16 sOldMapX = 0, sOldMapY = 0;
//	INT16 sX =0, sY = 0;
	FLOAT flRatio = 0.0;
	UINT32 x,y;
	UINT16 minX, minY, maxX, maxY;

	AssertMsg(0 <= sOldMapX && sOldMapX < SCREEN_WIDTH,  String("DisplayPositionOfHelicopter: Invalid sOldMapX = %d", sOldMapX));
	AssertMsg(0 <= sOldMapY && sOldMapY < SCREEN_HEIGHT, String("DisplayPositionOfHelicopter: Invalid sOldMapY = %d", sOldMapY));

	// restore background on map where it is
	if( sOldMapX != 0 )
	{
		RestoreExternBackgroundRect( sOldMapX, sOldMapY, HELI_ICON_WIDTH, HELI_ICON_HEIGHT );
		sOldMapX = 0;
	}


	if( iHelicopterVehicleId != -1 )
	{
		// draw the destination icon first, so when they overlap, the real one is on top!
		DisplayDestinationOfHelicopter( );

		const VEHICLETYPE* const v = GetHelicopter();
		// check if mvt group
		if (v->ubMovementGroup != 0)
		{
			const GROUP* const pGroup = GetGroup(v->ubMovementGroup);

			// this came up in one bug report!
			Assert( pGroup->uiTraverseTime != -1 );

			if( ( pGroup->uiTraverseTime > 0 ) && ( pGroup->uiTraverseTime != 0xffffffff ) )
			{
				flRatio = ( ( pGroup->uiTraverseTime + GetWorldTotalMin() ) - pGroup->uiArrivalTime ) / ( float ) pGroup->uiTraverseTime;
			}

/*
			AssertMsg( ( flRatio >= 0 ) && ( flRatio <= 100 ), String( "DisplayPositionOfHelicopter: Invalid flRatio = %6.2f, trav %d, arr %d, time %d",
																				flRatio, pGroup->uiTraverseTime, pGroup->uiArrivalTime, GetWorldTotalMin() ) );
*/

			if ( flRatio < 0 )
			{
				flRatio = 0;
			}
			else if ( flRatio > 100 )
			{
				flRatio = 100;
			}

//			if( !fZoomFlag )
			{
				// grab min and max locations to interpolate sub sector position
				minX = MAP_VIEW_START_X + MAP_GRID_X * ( pGroup->ubSectorX );
				maxX = MAP_VIEW_START_X + MAP_GRID_X * ( pGroup->ubNextX );
				minY = MAP_VIEW_START_Y + MAP_GRID_Y * ( pGroup->ubSectorY );
				maxY = MAP_VIEW_START_Y + MAP_GRID_Y * ( pGroup->ubNextY );
			}
/*
			else
			{

				// grab coords for nextx,y and current x,y

				// zoomed in, takes a little more work
				GetScreenXYFromMapXYStationary( ((UINT16)(pGroup->ubSectorX)),((UINT16)(pGroup->ubSectorY)) , &sX, &sY );
 	 			sY=sY-MAP_GRID_Y;
				sX=sX-MAP_GRID_X;

				minX = ( sX );
				minY = ( sY );

				GetScreenXYFromMapXYStationary( ((UINT16)(pGroup->ubNextX)),((UINT16)(pGroup->ubNextY)) , &sX, &sY );
 	 			sY=sY-MAP_GRID_Y;
				sX=sX-MAP_GRID_X;

				maxX = ( sX );
				maxY = ( sY );
			}
*/

			AssertMsg(minX < 640, String("DisplayPositionOfHelicopter: Invalid minX = %d", minX));
			AssertMsg(maxX < 640, String("DisplayPositionOfHelicopter: Invalid maxX = %d", maxX));
			AssertMsg(minY < 640, String("DisplayPositionOfHelicopter: Invalid minY = %d", minY));
			AssertMsg(maxY < 640, String("DisplayPositionOfHelicopter: Invalid maxY = %d", maxY));

			// IMPORTANT: Since min can easily be larger than max, we gotta cast to as signed value
			x = ( UINT32 )( minX + flRatio * ( ( INT16 ) maxX - ( INT16 ) minX ) );
 			y = ( UINT32 )( minY + flRatio * ( ( INT16 ) maxY - ( INT16 ) minY ) );

/*
			if( fZoomFlag )
			{
				x += 13;
				y += 8;
			}
			else
*/
			{
				x += 1;
				y += 3;
			}


			AssertMsg(0 <= x && x < SCREEN_WIDTH, String("DisplayPositionOfHelicopter: Invalid x = %d.  At %d,%d.  Next %d,%d.  Min/Max X = %d/%d",
							x, pGroup->ubSectorX, pGroup->ubSectorY, pGroup->ubNextX, pGroup->ubNextY, minX, maxX ) );

			AssertMsg(0 <= y && y < SCREEN_HEIGHT, String("DisplayPositionOfHelicopter: Invalid y = %d.  At %d,%d.  Next %d,%d.  Min/Max Y = %d/%d",
							y, pGroup->ubSectorX, pGroup->ubSectorY, pGroup->ubNextX, pGroup->ubNextY, minY, maxY ) );


			// clip blits to mapscreen region
			ClipBlitsToMapViewRegion( );

			BltVideoObject(FRAME_BUFFER, guiHelicopterIcon, HELI_ICON, x, y);

			SetFont( MAP_MVT_ICON_FONT );
			SetFontForeground( FONT_WHITE );
			SetFontBackground( FONT_BLACK );
			mprintf(x + 5, y + 1,  L"%d", GetNumberOfPassengersInHelicopter());

			InvalidateRegion( x, y, x + HELI_ICON_WIDTH, y + HELI_ICON_HEIGHT );

			RestoreClipRegionToFullScreen( );

			// now store the old stuff
			sOldMapX = ( INT16 ) x;
			sOldMapY = ( INT16 ) y;
		}
	}
}


static void DisplayDestinationOfHelicopter(void)
{
	static INT16 sOldMapX = 0, sOldMapY = 0;
	INT16 sMapX, sMapY;
	UINT32 x,y;

	AssertMsg(0 <= sOldMapX && sOldMapX < SCREEN_WIDTH, String("DisplayDestinationOfHelicopter: Invalid sOldMapX = %d", sOldMapX));
	AssertMsg(0 <= sOldMapY && sOldMapY < SCREEN_HEIGHT, String("DisplayDestinationOfHelicopter: Invalid sOldMapY = %d", sOldMapY));

	// restore background on map where it is
	if( sOldMapX != 0 )
	{
		RestoreExternBackgroundRect( sOldMapX, sOldMapY, HELI_SHADOW_ICON_WIDTH, HELI_SHADOW_ICON_HEIGHT );
		sOldMapX = 0;
	}

	// if helicopter is going somewhere
	if (GetLengthOfPath(GetHelicopter()->pMercPath) > 1)
	{
		// get destination
		const INT16 sSector = GetLastSectorOfHelicoptersPath();
		sMapX = sSector % MAP_WORLD_X;
		sMapY = sSector / MAP_WORLD_X;

		x = MAP_VIEW_START_X + ( MAP_GRID_X * sMapX ) + 1;
		y = MAP_VIEW_START_Y + ( MAP_GRID_Y * sMapY ) + 3;

		AssertMsg(0 <= x && x < SCREEN_WIDTH, String("DisplayDestinationOfHelicopter: Invalid x = %d.  Dest %d,%d", x, sMapX, sMapY));
		AssertMsg(0 <= y && y < SCREEN_HEIGHT, String("DisplayDestinationOfHelicopter: Invalid y = %d.  Dest %d,%d", y, sMapX, sMapY));

		// clip blits to mapscreen region
		ClipBlitsToMapViewRegion( );

		BltVideoObject(FRAME_BUFFER, guiHelicopterIcon, HELI_SHADOW_ICON, x, y);
		InvalidateRegion( x, y, x + HELI_SHADOW_ICON_WIDTH, y + HELI_SHADOW_ICON_HEIGHT );

		RestoreClipRegionToFullScreen( );

		// now store the old stuff
		sOldMapX = ( INT16 ) x;
		sOldMapY = ( INT16 ) y;
	}
}



BOOLEAN CheckForClickOverHelicopterIcon( INT16 sClickedSectorX, INT16 sClickedSectorY )
{
	BOOLEAN fHelicopterOverNextSector = FALSE;
	FLOAT flRatio = 0.0;
	INT16 sSectorX;
	INT16 sSectorY;

	if( !fHelicopterAvailable || !fShowAircraftFlag )
	{
		return( FALSE );
	}

	if ( iHelicopterVehicleId == -1 )
	{
		return( FALSE );
	}

	const VEHICLETYPE* const v = GetHelicopter();

	// figure out over which sector the helicopter APPEARS to be to the player (because we slide it smoothly across the
	// map, unlike groups travelling on the ground, it can appear over its next sector while it's not there yet.
	const GROUP* const pGroup = GetGroup(v->ubMovementGroup);
	Assert( pGroup );

	if ( pGroup->fBetweenSectors )
	{
		// this came up in one bug report!
		Assert( pGroup->uiTraverseTime != -1 );

		if( ( pGroup->uiTraverseTime > 0 ) && ( pGroup->uiTraverseTime != 0xffffffff ) )
		{
			flRatio = ( pGroup->uiTraverseTime - pGroup->uiArrivalTime + GetWorldTotalMin() ) / ( float ) pGroup->uiTraverseTime;
		}

		// if more than halfway there, the chopper appears more over the next sector, not over its current one(!)
		if ( flRatio > 0.5 )
		{
			fHelicopterOverNextSector = TRUE;
		}
	}


	if ( fHelicopterOverNextSector )
	{
		// use the next sector's coordinates
		sSectorX = pGroup->ubNextX;
		sSectorY = pGroup->ubNextY;
	}
	else
	{
		// use current sector's coordinates
		sSectorX = v->sSectorX;
		sSectorY = v->sSectorY;
	}

	// check if helicopter appears where he clicked
	if( ( sSectorX != sClickedSectorX ) || ( sSectorY != sClickedSectorY ) )
	{
		return( FALSE );
	}

	return( TRUE );
}


static void DrawSite(const INT16 sector_x, const INT16 sector_y, const SGPVObject* const icon)
{
	INT16  x;
	INT16  y;
	UINT16 max_w;
	UINT16 max_h;
	UINT8  vo_idx;
	if (fZoomFlag)
	{
		{ SGPVSurface::Lock l(guiSAVEBUFFER);
			SetClippingRegionAndImageWidth(l.Pitch(), MAP_VIEW_START_X + MAP_GRID_X - 1, MAP_VIEW_START_Y + MAP_GRID_Y - 1, MAP_VIEW_WIDTH + 1, MAP_VIEW_HEIGHT - 9);
		}

		GetScreenXYFromMapXYStationary(sector_x, sector_y, &x, &y);
		x -= MAP_GRID_X - 1;
		y -= MAP_GRID_Y;
		max_w = MAP_GRID_ZOOM_X - 1;
		max_h = MAP_GRID_ZOOM_Y - 1;
		vo_idx = 0;
	}
	else
	{
		GetScreenXYFromMapXY(sector_x, sector_y, &x, &y);
		++x;
		max_w = MAP_GRID_X - 1;
		max_h = MAP_GRID_Y - 1;
		vo_idx = 1;
	}

	ETRLEObject const* const ETRLEProps = icon->SubregionProperties(vo_idx);
	const UINT16 w = ETRLEProps->usWidth;
	const UINT16 h = ETRLEProps->usHeight;
	x += (max_w - w) / 2;
	/* If the icon is higher than a map cell, align with the bottom of the cell */
	y += (h > max_h ? max_h - h : (max_h - h) / 2);

	BltVideoObject(guiSAVEBUFFER, icon, vo_idx, x, y);
}


static void BlitMineIcon(INT16 sMapX, INT16 sMapY)
{
	DrawSite(sMapX, sMapY, guiMINEICON);
}


static void PrintStringCenteredBoxed(INT32 x, const INT32 y, const wchar_t* const string)
{
	x -= StringPixLength(string, MAP_FONT) / 2;
	if (!fZoomFlag) // it's ok to cut strings off in zoomed mode
	{
		if (x < MAP_VIEW_START_X + 23) x = MAP_VIEW_START_X + 23;
	}
	MPrint(x, y, string);
}


static void BlitMineText(INT16 sMapX, INT16 sMapY)
{
	// set coordinates for start of mine text
	INT16 sScreenX;
	INT16 sScreenY;
	if (fZoomFlag)
	{
		GetScreenXYFromMapXYStationary(sMapX, sMapY, &sScreenX, &sScreenY);
		sScreenY += MAP_GRID_ZOOM_Y / 2 + 1; // slightly below
	}
	else
	{
		GetScreenXYFromMapXY(sMapX, sMapY, &sScreenX, &sScreenY);
		sScreenX += MAP_GRID_X / 2; // centered around middle of mine square
		sScreenY += MAP_GRID_Y + 1; // slightly below
	}

	// show detailed mine info (name, production rate, daily production)

	SetFontDestBuffer(guiSAVEBUFFER, MAP_VIEW_START_X, MAP_VIEW_START_Y, MAP_VIEW_START_X + MAP_VIEW_WIDTH + MAP_GRID_X, MAP_VIEW_START_Y + MAP_VIEW_HEIGHT + 7);

	SetFont(MAP_FONT);
	SetFontForeground(FONT_LTGREEN);
	SetFontBackground(FONT_BLACK);

	UINT8 const mine_idx = GetMineIndexForSector(sMapX, sMapY);
	INT32 const x        = sScreenX;
	INT32       y        = sScreenY;
	INT32 const h        = GetFontHeight(MAP_FONT);
	wchar_t     buf[32];

	// display associated town name, followed by "mine"
	swprintf(buf, lengthof(buf), L"%ls %ls", pTownNames[GetTownAssociatedWithMine(GetMineIndexForSector(sMapX, sMapY))],  pwMineStrings[0]);
	PrintStringCenteredBoxed(x, y, buf);
	y += h;

	// check if mine is empty (abandoned) or running out
	if (gMineStatus[mine_idx].fEmpty)
	{
		PrintStringCenteredBoxed(x, y, pwMineStrings[5]);
		y += h;
	}
	else if (gMineStatus[mine_idx].fShutDown)
	{
		PrintStringCenteredBoxed(x, y, pwMineStrings[6]);
		y += h;
	}
	else if (gMineStatus[mine_idx].fRunningOut)
	{
		PrintStringCenteredBoxed(x, y, pwMineStrings[7]);
		y += h;
	}

	// only show production if player controls it and it's actually producing
	if (PlayerControlsMine(mine_idx) && !gMineStatus[mine_idx].fEmpty)
	{
		// show current production
		SPrintMoney(buf, PredictDailyIncomeFromAMine(mine_idx));

		// if potential is not nil, show percentage of the two
		if (GetMaxPeriodicRemovalFromMine(mine_idx) > 0)
		{
			wchar_t wSubString[32];
			swprintf(wSubString, lengthof(wSubString), L" (%d%%)", PredictDailyIncomeFromAMine(mine_idx) * 100 / GetMaxDailyRemovalFromMine(mine_idx));
			wcscat(buf, wSubString);
		}

		PrintStringCenteredBoxed(x, y, buf);
	}

	SetFontDestBuffer(FRAME_BUFFER, MAP_VIEW_START_X, MAP_VIEW_START_Y, MAP_VIEW_START_X + MAP_VIEW_WIDTH + MAP_GRID_X, MAP_VIEW_START_Y + MAP_VIEW_HEIGHT + 7);
}


static void BlitTownGridMarkers(void)
{
	INT16 sScreenX = 0, sScreenY = 0;
	UINT16 usColor = 0;
	INT32 iCounter = 0;
	INT16 sWidth = 0, sHeight = 0;

	// get 16 bpp color
	usColor = Get16BPPColor( FROMRGB( 100, 100, 100) );

	// blit in the highlighted sector
	SGPVSurface::Lock l(guiSAVEBUFFER);
	UINT8* const pDestBuf         = l.Buffer<UINT8>();
	UINT32 const uiDestPitchBYTES = l.Pitch();

	// clip to view region
	ClipBlitsToMapViewRegionForRectangleAndABit( uiDestPitchBYTES );


	// go through list of towns and place on screen
	while( pTownNamesList[ iCounter ] != 0 )
	{
		// skip Orta/Tixa until found
		if ((pTownNamesList[iCounter] != ORTA || fFoundOrta) && (pTownNamesList[iCounter] != TIXA || fFoundTixa))
		{
			if( fZoomFlag )
			{
				GetScreenXYFromMapXYStationary( ( INT16 )( pTownLocationsList[ iCounter ] % MAP_WORLD_X ), ( INT16 )( pTownLocationsList[ iCounter ] / MAP_WORLD_X ) , &sScreenX, &sScreenY );
				sScreenX -= MAP_GRID_X - 1;
				sScreenY -= MAP_GRID_Y;

				sWidth = 2 * MAP_GRID_X;
				sHeight= 2 * MAP_GRID_Y;
			}
			else
			{
				// get location on screen
				GetScreenXYFromMapXY( ( INT16 )( pTownLocationsList[ iCounter ] % MAP_WORLD_X ), ( INT16 )( pTownLocationsList[ iCounter ] / MAP_WORLD_X ), &sScreenX, &sScreenY );
				sWidth = MAP_GRID_X - 1;
				sHeight= MAP_GRID_Y;

				sScreenX += 2;
			}

			if( StrategicMap[ pTownLocationsList[ iCounter ] - MAP_WORLD_X ].bNameId == BLANK_SECTOR )
			{
				LineDraw( TRUE,  sScreenX - 1, sScreenY - 1, sScreenX + sWidth - 1, sScreenY - 1, usColor, pDestBuf );
			}

			if( ( StrategicMap[ pTownLocationsList[ iCounter ] + MAP_WORLD_X ].bNameId == BLANK_SECTOR ) /* || ( StrategicMap[ pTownLocationsList[ iCounter ] + MAP_WORLD_X ].bNameId == PALACE ) */ )
			{
				LineDraw( TRUE,  sScreenX - 1, sScreenY + sHeight - 1, sScreenX + sWidth - 1, sScreenY + sHeight - 1, usColor, pDestBuf );
			}

			if( StrategicMap[ pTownLocationsList[ iCounter ] - 1 ].bNameId == BLANK_SECTOR )
			{
				LineDraw( TRUE,  sScreenX - 2, sScreenY - 1, sScreenX - 2, sScreenY + sHeight - 1, usColor, pDestBuf );
			}

			if( StrategicMap[ pTownLocationsList[ iCounter ] + 1 ].bNameId == BLANK_SECTOR )
			{
				LineDraw( TRUE,  sScreenX + sWidth - 1, sScreenY - 1, sScreenX + sWidth - 1, sScreenY + sHeight - 1, usColor, pDestBuf );
			}
		}

		iCounter++;
	}

	// restore clips
	RestoreClipRegionToFullScreenForRectangle( uiDestPitchBYTES );
}


static void BlitMineGridMarkers(void)
{
	INT16 sScreenX = 0, sScreenY = 0;
	UINT16 usColor = 0;
	INT32 iCounter = 0;
	INT16 sWidth = 0, sHeight = 0;

	// get 16 bpp color
	usColor = Get16BPPColor( FROMRGB( 100, 100, 100) );


	// blit in the highlighted sector
	SGPVSurface::Lock l(guiSAVEBUFFER);
	UINT32 const uiDestPitchBYTES = l.Pitch();

	// clip to view region
	ClipBlitsToMapViewRegionForRectangleAndABit( uiDestPitchBYTES );

	for( iCounter = 0; iCounter < MAX_NUMBER_OF_MINES; iCounter++ )
	{
		if( fZoomFlag )
		{
			GetScreenXYFromMapXYStationary( ( INT16 )( gMineLocation[ iCounter ].sSectorX), ( INT16 )( gMineLocation[ iCounter ].sSectorY ) , &sScreenX, &sScreenY );
			sScreenX -= MAP_GRID_X;
			sScreenY -= MAP_GRID_Y;

			sWidth = 2 * MAP_GRID_X;
			sHeight= 2 * MAP_GRID_Y;
		}
		else
		{
			// get location on screen
			GetScreenXYFromMapXY( ( INT16 )(  gMineLocation[ iCounter ].sSectorX ), ( INT16 )(  gMineLocation[ iCounter ].sSectorY ), &sScreenX, &sScreenY );
			sWidth = MAP_GRID_X;
			sHeight= MAP_GRID_Y;

		}

		// draw rectangle
		RectangleDraw(TRUE, sScreenX, sScreenY - 1, sScreenX + sWidth, sScreenY + sHeight - 1, usColor, l.Buffer<UINT8>());
	}

	// restore clips
	RestoreClipRegionToFullScreenForRectangle( uiDestPitchBYTES );
}


/*
void CheckIfAnyoneLeftInSector( INT16 sX, INT16 sY, INT16 sNewX, INT16 sNewY, INT8 bZ )
{
	// if this map sector is currently selected
	if( ( sX == sSelMapX ) && ( sY == sSelMapY ) && ( bZ == iCurrentMapSectorZ ) )
	{
		// check if anyone left in the old sector
		if( NumFriendlyInSector( (INT8)sX, (INT8)sY, bZ ) == 0)
		{
			ChangeSelectedMapSector( sNewX, sNewY, bZ );
			fMapScreenBottomDirty = TRUE;
		}
	}
}


UINT8 NumFriendlyInSector( INT16 sX, INT16 sY, INT8 bZ )
{
	UINT8				ubNumFriendlies = 0;

	// Check if the battle is won!
	// Loop through all mercs and make go
	CFOR_ALL_SOLDIERS(pTeamSoldier)
	{
		if (pTeamSoldier->bLife > 0)
		{
			if ( (pTeamSoldier->bSide == gbPlayerNum ) && ( pTeamSoldier->sSectorX == sX ) && ( pTeamSoldier->sSectorY == sY ) && ( pTeamSoldier->bSectorZ == bZ ) )
			{
				ubNumFriendlies++;
			}
		}
	}

	return( ubNumFriendlies );

}
*/


static void DisplayLevelString(void)
{
	// given the current level being displayed on the map, show a sub level message

	// at the surface
	if( !iCurrentMapSectorZ )
	{
		return;
	}

	// otherwise we will have to display the string with the level number

	SetFontDestBuffer(guiSAVEBUFFER, MAP_VIEW_START_X, MAP_VIEW_START_Y, MAP_VIEW_START_X + MAP_VIEW_WIDTH + MAP_GRID_X, MAP_VIEW_START_Y + MAP_VIEW_HEIGHT + 7);

	SetFont( MAP_FONT );
	SetFontForeground( MAP_INDEX_COLOR );
	SetFontBackground( FONT_BLACK );
	mprintf(MAP_LEVEL_STRING_X, MAP_LEVEL_STRING_Y, L"%ls %d", sMapLevelString, iCurrentMapSectorZ);

	SetFontDestBuffer(FRAME_BUFFER, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}


// function to manipulate the number of towns people on the cursor
static BOOLEAN PickUpATownPersonFromSector(UINT8 ubType, INT16 sX, INT16 sY)
{
	// see if there are any militia of this type in this sector
	if( !SectorInfo[ SECTOR( sX, sY ) ].ubNumberOfCivsAtLevel[ ubType ] )
	{
		// failed, no one here
		return( FALSE );
	}

	// are they in the same town as they were pickedup from
	if( GetTownIdForSector( sX, sY ) != sSelectedMilitiaTown )
	{
		return( FALSE );
	}

	if( !SectorOursAndPeaceful( sX, sY, 0 ) )
	{
		return( FALSE );
	}

	if( SECTOR( sX, sY ) == SECTOR( gWorldSectorX, gWorldSectorY ) )
	{
		gfStrategicMilitiaChangesMade = TRUE;
	}

	// otherwise pick this guy up
	switch( ubType )
	{
		case( GREEN_MILITIA ):
			sGreensOnCursor++;
		break;
		case( REGULAR_MILITIA ):
			sRegularsOnCursor++;
		break;
		case( ELITE_MILITIA ):
			sElitesOnCursor++;
		break;
	}

	// reduce number in this sector
	SectorInfo[ SECTOR( sX, sY )].ubNumberOfCivsAtLevel[ ubType ]--;

	fMapPanelDirty = TRUE;

	return( TRUE );
}


static BOOLEAN DropAPersonInASector(UINT8 ubType, INT16 sX, INT16 sY)
{

	// are they in the same town as they were pickedup from
	if( GetTownIdForSector( sX, sY ) != sSelectedMilitiaTown )
	{
		return( FALSE );
	}

	if( SectorInfo[ SECTOR( sX, sY )].ubNumberOfCivsAtLevel[ GREEN_MILITIA ] +  SectorInfo[ SECTOR( sX, sY )].ubNumberOfCivsAtLevel[ REGULAR_MILITIA ]
		+  SectorInfo[ SECTOR( sX, sY )].ubNumberOfCivsAtLevel[ ELITE_MILITIA ] >= MAX_ALLOWABLE_MILITIA_PER_SECTOR )
	{
		return( FALSE );
	}

	if( !SectorOursAndPeaceful(sX, sY, 0 ) )
	{
		return( FALSE );
	}

	if( SECTOR( sX, sY ) == SECTOR( gWorldSectorX, gWorldSectorY ) )
	{
		gfStrategicMilitiaChangesMade = TRUE;
	}

	// drop the guy into this sector
	switch( ubType )
	{
		case( GREEN_MILITIA ):

			if( !sGreensOnCursor )
			{
				return( FALSE );
			}

			sGreensOnCursor--;
		break;
		case( REGULAR_MILITIA ):
			if( !sRegularsOnCursor )
			{
				return( FALSE );
			}
			sRegularsOnCursor--;
		break;
		case( ELITE_MILITIA ):
			if( !sElitesOnCursor )
			{
				return( FALSE );
			}

			sElitesOnCursor--;
		break;
	}

	// up the number in this sector of this type of militia
	SectorInfo[ SECTOR( sX, sY )].ubNumberOfCivsAtLevel[ ubType ]++;

	fMapPanelDirty = TRUE;

	return( TRUE );
}


void LoadMilitiaPopUpBox(void)
{
	// load the militia pop up box
	guiMilitia                = AddVideoObjectFromFile("INTERFACE/Militia.sti");
	guiMilitiaMaps            = AddVideoObjectFromFile("INTERFACE/Militiamaps.sti");
	guiMilitiaSectorHighLight = AddVideoObjectFromFile("INTERFACE/MilitiamapsectorOutline2.sti");
	guiMilitiaSectorOutline   = AddVideoObjectFromFile("INTERFACE/MilitiamapsectorOutline.sti");
}


void RemoveMilitiaPopUpBox( void )
{
	// delete the militia pop up box graphic
	DeleteVideoObject(guiMilitia);
	DeleteVideoObject(guiMilitiaMaps);
	DeleteVideoObject(guiMilitiaSectorHighLight);
	DeleteVideoObject(guiMilitiaSectorOutline);
}


static void CheckAndUpdateStatesOfSelectedMilitiaSectorButtons(void);
static void DisplayUnallocatedMilitia(void);
static void DrawTownMilitiaName(void);
static void RenderIconsPerSectorForSelectedTown(void);
static void RenderShadingForUnControlledSectors(void);
static void SetMilitiaMapButtonsText(void);
static void ShowHighLightedSectorOnMilitiaMap(void);


BOOLEAN DrawMilitiaPopUpBox( void )
{
	if( !fShowMilitia )
	{
		sSelectedMilitiaTown = 0;
	}

	// create buttons
	CreateDestroyMilitiaSectorButtons( );

	// create mouse regions if we need to
	CreateDestroyMilitiaPopUPRegions( );

	if( !sSelectedMilitiaTown )
	{
		return( FALSE );
	}

	// update states of militia selected sector buttons
	CheckAndUpdateStatesOfSelectedMilitiaSectorButtons( );

	BltVideoObject(FRAME_BUFFER, guiMilitia,     0,                        MAP_MILITIA_BOX_POS_X,                     MAP_MILITIA_BOX_POS_Y);
	BltVideoObject(FRAME_BUFFER, guiMilitiaMaps, sSelectedMilitiaTown - 1, MAP_MILITIA_BOX_POS_X + MAP_MILITIA_MAP_X, MAP_MILITIA_BOX_POS_Y + MAP_MILITIA_MAP_Y);

	// set font color for labels and "total militia" counts
	SetFontForeground(FONT_WHITE);

	// draw name of town, and the "unassigned" label
	DrawTownMilitiaName( );

	// render the icons for each sector in the town
	RenderIconsPerSectorForSelectedTown( );

	// shade any sectors not under our control
	RenderShadingForUnControlledSectors( );

	// display anyone picked up
	DisplayUnallocatedMilitia( );

	// draw the highlight last
	ShowHighLightedSectorOnMilitiaMap( );

	ETRLEObject const* const pTrav = guiMilitia->SubregionProperties(0);
	InvalidateRegion( MAP_MILITIA_BOX_POS_X, MAP_MILITIA_BOX_POS_Y, MAP_MILITIA_BOX_POS_X + pTrav->usWidth, MAP_MILITIA_BOX_POS_Y + pTrav->usHeight );

	// set the text for the militia map sector info buttons
	SetMilitiaMapButtonsText( );

	// render buttons
	MarkButtonsDirty( );

	return( TRUE );
}


static void CreateMilitiaPanelBottomButton(void);
static void DeleteMilitiaPanelBottomButton(void);
static void HandleShutDownOfMilitiaPanelIfPeopleOnTheCursor(INT16 sTownValue);
static void MilitiaRegionClickCallback(MOUSE_REGION* pRegion, INT32 iReason);
static void MilitiaRegionMoveCallback(MOUSE_REGION* pRegion, INT32 iReason);


void CreateDestroyMilitiaPopUPRegions( void )
{
	static INT16 sOldTown = 0;
	INT32 iCounter = 0;

	// create destroy militia pop up regions for mapscreen militia pop up box
	if( sSelectedMilitiaTown != 0 )
	{
		sOldTown = sSelectedMilitiaTown;
	}

	if( fShowMilitia && sSelectedMilitiaTown && !gfMilitiaPopupCreated )
	{

		for( iCounter = 0; iCounter < 9; iCounter++ )
		{
			MSYS_DefineRegion( &gMapScreenMilitiaBoxRegions[ iCounter ], ( INT16 ) ( MAP_MILITIA_BOX_POS_X + MAP_MILITIA_MAP_X + ( iCounter % MILITIA_BOX_ROWS ) * MILITIA_BOX_BOX_WIDTH ), ( INT16 )( MAP_MILITIA_BOX_POS_Y + MAP_MILITIA_MAP_Y + ( iCounter / MILITIA_BOX_ROWS ) * MILITIA_BOX_BOX_HEIGHT ), ( INT16 )( MAP_MILITIA_BOX_POS_X + MAP_MILITIA_MAP_X + ( ( ( iCounter  ) % MILITIA_BOX_ROWS ) + 1 ) * MILITIA_BOX_BOX_WIDTH ), ( INT16 )( MAP_MILITIA_BOX_POS_Y + MAP_MILITIA_MAP_Y + ( ( ( iCounter ) / MILITIA_BOX_ROWS ) + 1 ) * MILITIA_BOX_BOX_HEIGHT ), MSYS_PRIORITY_HIGHEST - 3,
							 MSYS_NO_CURSOR, MilitiaRegionMoveCallback, MilitiaRegionClickCallback );

			MSYS_SetRegionUserData( &gMapScreenMilitiaBoxRegions[ iCounter ], 0, iCounter );

		}

		// create militia panel buttons
		CreateMilitiaPanelBottomButton( );


		gfMilitiaPopupCreated = TRUE;
	}
	else if( gfMilitiaPopupCreated  && ( !fShowMilitia || !sSelectedMilitiaTown ) )
	{

		for( iCounter = 0; iCounter < 9; iCounter++ )
		{
			// remove region
			MSYS_RemoveRegion( &gMapScreenMilitiaBoxRegions[ iCounter ] );
		}

		// handle the shutdown of the panel...there maybe people on the cursor, distribute them evenly over all the sectors
		HandleShutDownOfMilitiaPanelIfPeopleOnTheCursor( sOldTown );

		DeleteMilitiaPanelBottomButton( );

		gfMilitiaPopupCreated = FALSE;
	}
}


static INT16 GetBaseSectorForCurrentTown(void);


static void RenderIconsPerSectorForSelectedTown(void)
{
	INT16 sBaseSectorValue = 0;
	INT16 sCurrentSectorValue = 0;
	INT32 iCounter = 0;
	INT32 iNumberOfGreens = 0;
	INT32 iNumberOfRegulars = 0;
	INT32 iNumberOfElites = 0;
	INT32 iTotalNumberOfTroops = 0;
	INT32 iCurrentTroopIcon = 0;
	INT32 iCurrentIcon = 0;
	INT16 sX, sY;
	CHAR16 sString[ 32 ];
	INT16 sSectorX = 0, sSectorY = 0;


	// get the sector value for the upper left corner
	sBaseSectorValue = GetBaseSectorForCurrentTown( );

	// render icons for map
	for( iCounter = 0; iCounter < 9; iCounter++ )
	{
		// grab current sector value
		sCurrentSectorValue = sBaseSectorValue + ( ( iCounter % MILITIA_BOX_ROWS ) + ( iCounter / MILITIA_BOX_ROWS ) * ( 16 ) );

		sSectorX = SECTORX( sCurrentSectorValue );
		sSectorY = SECTORY( sCurrentSectorValue );

		// skip sectors not in the selected town (nearby other towns or wilderness SAM Sites)
		if( GetTownIdForSector( sSectorX, sSectorY ) != sSelectedMilitiaTown )
		{
			continue;
		}

		// get number of each
		iNumberOfGreens = SectorInfo[ sCurrentSectorValue ].ubNumberOfCivsAtLevel[ GREEN_MILITIA ];
		iNumberOfRegulars = SectorInfo[ sCurrentSectorValue ].ubNumberOfCivsAtLevel[ REGULAR_MILITIA ];
		iNumberOfElites = SectorInfo[ sCurrentSectorValue ].ubNumberOfCivsAtLevel[ ELITE_MILITIA ];

		// get total
		iTotalNumberOfTroops = iNumberOfGreens + iNumberOfRegulars + iNumberOfElites;

		// printf number of troops
		SetFont( FONT10ARIAL );
		swprintf( sString, lengthof(sString), L"%d", iTotalNumberOfTroops );
		FindFontRightCoordinates(MAP_MILITIA_BOX_POS_X + MAP_MILITIA_MAP_X + iCounter % MILITIA_BOX_ROWS * MILITIA_BOX_BOX_WIDTH, MAP_MILITIA_BOX_POS_Y + MAP_MILITIA_MAP_Y + iCounter / MILITIA_BOX_ROWS * MILITIA_BOX_BOX_HEIGHT, MILITIA_BOX_BOX_WIDTH, 0, sString, FONT10ARIAL, &sX, &sY);

		if( StrategicMap[ SECTOR_INFO_TO_STRATEGIC_INDEX( sCurrentSectorValue ) ].bNameId != BLANK_SECTOR &&
				!StrategicMap[ SECTOR_INFO_TO_STRATEGIC_INDEX( sCurrentSectorValue ) ].fEnemyControlled )
		{
			INT32 const x = (sSectorMilitiaMapSector != iCounter ? sX : sX - 15);
			MPrint(x, sY + MILITIA_BOX_BOX_HEIGHT - 5, sString);
		}

		// now display
		for( iCurrentTroopIcon = 0; iCurrentTroopIcon < iTotalNumberOfTroops; iCurrentTroopIcon++ )
		{
			// get screen x and y coords
			if( sSectorMilitiaMapSector == iCounter )
			{
				sX =  ( iCurrentTroopIcon % POPUP_MILITIA_ICONS_PER_ROW ) * MEDIUM_MILITIA_ICON_SPACING + MAP_MILITIA_BOX_POS_X + MAP_MILITIA_MAP_X + ( ( iCounter % MILITIA_BOX_ROWS ) * MILITIA_BOX_BOX_WIDTH ) + 2;
				sY =  ( iCurrentTroopIcon / POPUP_MILITIA_ICONS_PER_ROW ) * ( MEDIUM_MILITIA_ICON_SPACING - 1 ) + MAP_MILITIA_BOX_POS_Y + MAP_MILITIA_MAP_Y + ( ( iCounter / MILITIA_BOX_ROWS ) * MILITIA_BOX_BOX_HEIGHT ) + 3;

				if( iCurrentTroopIcon < iNumberOfGreens )
				{
					iCurrentIcon = 5;
				}
				else if( iCurrentTroopIcon < iNumberOfGreens + iNumberOfRegulars )
				{
					iCurrentIcon = 6;
				}
				else
				{
					iCurrentIcon = 7;
				}
			}
			else
			{
				sX =  ( iCurrentTroopIcon % POPUP_MILITIA_ICONS_PER_ROW ) * MEDIUM_MILITIA_ICON_SPACING + MAP_MILITIA_BOX_POS_X + MAP_MILITIA_MAP_X + ( ( iCounter % MILITIA_BOX_ROWS ) * MILITIA_BOX_BOX_WIDTH ) + 3;
				sY =  ( iCurrentTroopIcon / POPUP_MILITIA_ICONS_PER_ROW ) * ( MEDIUM_MILITIA_ICON_SPACING ) + MAP_MILITIA_BOX_POS_Y + MAP_MILITIA_MAP_Y + ( ( iCounter / MILITIA_BOX_ROWS ) * MILITIA_BOX_BOX_HEIGHT ) + 3;

				if( iCurrentTroopIcon < iNumberOfGreens )
				{
					iCurrentIcon = 8;
				}
				else if( iCurrentTroopIcon < iNumberOfGreens + iNumberOfRegulars )
				{
					iCurrentIcon = 9;
				}
				else
				{
					iCurrentIcon = 10;
				}
			}

			BltVideoObject(FRAME_BUFFER, guiMilitia, iCurrentIcon, sX, sY);
		}

	}
}


static INT16 GetBaseSectorForCurrentTown(void)
{
	INT16 sBaseSector = 0;

	// is the current town
	if( sSelectedMilitiaTown != 0 )
	{
		sBaseSector = sBaseSectorList[ ( INT16 )( sSelectedMilitiaTown - 1 ) ];
	}

	// return the current sector value
	return( sBaseSector );
}


static void ShowHighLightedSectorOnMilitiaMap(void)
{
	// show the highlighted sector on the militia map
	INT16 sX = 0, sY = 0;

	if( sSectorMilitiaMapSector != -1 )
	{
		sX = MAP_MILITIA_BOX_POS_X + MAP_MILITIA_MAP_X + ( ( sSectorMilitiaMapSector % MILITIA_BOX_ROWS ) * MILITIA_BOX_BOX_WIDTH );
		sY = MAP_MILITIA_BOX_POS_Y + MAP_MILITIA_MAP_Y + ( ( sSectorMilitiaMapSector / MILITIA_BOX_ROWS ) * MILITIA_BOX_BOX_HEIGHT );
		BltVideoObject(FRAME_BUFFER, guiMilitiaSectorHighLight, 0, sX, sY);
	}

	if( sSectorMilitiaMapSectorOutline != -1 )
	{
		sX = MAP_MILITIA_BOX_POS_X + MAP_MILITIA_MAP_X + ( ( sSectorMilitiaMapSectorOutline % MILITIA_BOX_ROWS ) * MILITIA_BOX_BOX_WIDTH );
		sY = MAP_MILITIA_BOX_POS_Y + MAP_MILITIA_MAP_Y + ( ( sSectorMilitiaMapSectorOutline / MILITIA_BOX_ROWS ) * MILITIA_BOX_BOX_HEIGHT );
		BltVideoObject(FRAME_BUFFER, guiMilitiaSectorOutline, 0, sX, sY);
	}
}


static BOOLEAN IsThisMilitiaTownSectorAllowable(INT16 sSectorIndexValue);


static void MilitiaRegionClickCallback(MOUSE_REGION* pRegion, INT32 iReason)
{
	INT32 iValue = 0;

	iValue = MSYS_GetRegionUserData( pRegion, 0 );

	if( ( iReason & MSYS_CALLBACK_REASON_LBUTTON_UP ) )
	{
		if( IsThisMilitiaTownSectorAllowable( (INT16)iValue ) )
		{
			if( sSectorMilitiaMapSector == (INT16)iValue )
			{
				sSectorMilitiaMapSector = -1;
			}
			else
			{
				sSectorMilitiaMapSector = ( INT16 )iValue;
			}
		}
		else
		{
			sSectorMilitiaMapSector = -1;
		}
	}

	if( ( iReason & MSYS_CALLBACK_REASON_RBUTTON_UP ) )
	{
		sSectorMilitiaMapSector = -1;
	}
}


static void MilitiaRegionMoveCallback(MOUSE_REGION* pRegion, INT32 iReason)
{
	INT32 iValue = 0;

	iValue = MSYS_GetRegionUserData( pRegion, 0 );

	if( ( iReason & MSYS_CALLBACK_REASON_GAIN_MOUSE ) )
	{
		if( IsThisMilitiaTownSectorAllowable( ( INT16 )iValue ) )
		{
			sSectorMilitiaMapSectorOutline = ( INT16 )iValue;
		}
		else
		{
			sSectorMilitiaMapSectorOutline = -1;
		}
	}
	else if( iReason & MSYS_CALLBACK_REASON_LOST_MOUSE )
	{
		sSectorMilitiaMapSectorOutline = -1;
	}
}


static void MilitiaBoxMaskBtnCallback(MOUSE_REGION* pRegion, INT32 iReason);
static void MilitiaButtonCallback(GUI_BUTTON* btn, INT32 reason);


void CreateDestroyMilitiaSectorButtons(void)
{
	static BOOLEAN fCreated = FALSE;
	static INT16 sOldSectorValue = -1;
	INT16 sX = 0, sY = 0;
	INT32 iCounter = 0;

	if( sOldSectorValue == sSectorMilitiaMapSector && fShowMilitia && sSelectedMilitiaTown && !fCreated && sSectorMilitiaMapSector != -1 )
	{
		fCreated = TRUE;

		// given sector..place down the 3 buttons

		for( iCounter = 0; iCounter < 3; iCounter++ )
		{

			// set screen x and y positions
			sX = MAP_MILITIA_BOX_POS_X + MAP_MILITIA_MAP_X + ( ( sSectorMilitiaMapSector % MILITIA_BOX_ROWS ) * MILITIA_BOX_BOX_WIDTH );
			sY = MAP_MILITIA_BOX_POS_Y + MAP_MILITIA_MAP_Y + ( ( sSectorMilitiaMapSector / MILITIA_BOX_ROWS ) * MILITIA_BOX_BOX_HEIGHT );

			// adjust offsets
			sX += MILITIA_BTN_OFFSET_X;
			sY += ( iCounter * ( MILITIA_BTN_HEIGHT ) + 2 );

			giMapMilitiaButton[iCounter] = QuickCreateButtonImg("INTERFACE/militia.sti", -1, 3, -1, 4, -1, sX, sY, MSYS_PRIORITY_HIGHEST - 1, MilitiaButtonCallback);

			// set button user data
			MSYS_SetBtnUserData(giMapMilitiaButton[iCounter], iCounter);
			giMapMilitiaButton[iCounter]->SpecifyGeneralTextAttributes(NULL, FONT10ARIAL, gsMilitiaSectorButtonColors[iCounter], FONT_BLACK);

			giMapMilitiaButton[iCounter]->SetFastHelpText(pMilitiaButtonsHelpText[iCounter]);
		}

		CreateScreenMaskForMoveBox( );

		// ste the fact that the buttons were in fact created
		fMilitiaMapButtonsCreated = TRUE;
	}
	else if( fCreated && ( sOldSectorValue != sSectorMilitiaMapSector || !fShowMilitia || !sSelectedMilitiaTown || sSectorMilitiaMapSector == -1 ) )
	{
		sOldSectorValue = sSectorMilitiaMapSector;
		fCreated = FALSE;

		// the militia box left click region
	//	MSYS_RemoveRegion( &gMapScreenMilitiaRegion );

		// get rid of the buttons
		for( iCounter = 0 ; iCounter < 3; iCounter++ )
		{
			RemoveButton( giMapMilitiaButton[ iCounter ] );
		}

		if( !fShowMilitia || !sSelectedMilitiaTown )
		{
			sSectorMilitiaMapSector = -1;
			sSelectedMilitiaTown = 0;
		}

		RemoveScreenMaskForMoveBox( );

		// set the fact that the buttons were destroyed
		fMilitiaMapButtonsCreated = FALSE;
	}

	sOldSectorValue = sSectorMilitiaMapSector;
}


static void SetMilitiaMapButtonsText(void)
{
	// now set the militia map button text
	CHAR16 sString[ 64 ];
	INT32 iNumberOfGreens = 0, iNumberOfRegulars = 0, iNumberOfElites = 0;
	INT16 sBaseSectorValue = 0, sGlobalMapSector = 0;


	if( !fMilitiaMapButtonsCreated )
	{
		return;
	}

	// grab the appropriate global sector value in the world
	sBaseSectorValue = GetBaseSectorForCurrentTown( );
	sGlobalMapSector = sBaseSectorValue + ( ( sSectorMilitiaMapSector % MILITIA_BOX_ROWS ) + ( sSectorMilitiaMapSector / MILITIA_BOX_ROWS ) * ( 16 ) );

	iNumberOfGreens =  SectorInfo[ sGlobalMapSector ].ubNumberOfCivsAtLevel[ GREEN_MILITIA ];
	iNumberOfRegulars = SectorInfo[ sGlobalMapSector ].ubNumberOfCivsAtLevel[ REGULAR_MILITIA ];
	iNumberOfElites = SectorInfo[ sGlobalMapSector ].ubNumberOfCivsAtLevel[ ELITE_MILITIA ];

	// the greens in this sector
	swprintf( sString, lengthof(sString), L"%d", iNumberOfGreens );
	SpecifyButtonText( giMapMilitiaButton[ 0 ], sString );

	// the regulars in this sector
	swprintf( sString, lengthof(sString), L"%d", iNumberOfRegulars );
	SpecifyButtonText( giMapMilitiaButton[ 1 ], sString );

	// the number of elites in this sector
	swprintf( sString, lengthof(sString), L"%d", iNumberOfElites );
	SpecifyButtonText( giMapMilitiaButton[ 2 ], sString );
}


static void MilitiaButtonCallback(GUI_BUTTON *btn, INT32 reason)
{
	INT32 iValue = MSYS_GetBtnUserData(btn);

	// get the sector value for the upper left corner
	INT16 sBaseSectorValue = GetBaseSectorForCurrentTown();
	INT16 sGlobalMapSector = sBaseSectorValue + sSectorMilitiaMapSector % MILITIA_BOX_ROWS + sSectorMilitiaMapSector / MILITIA_BOX_ROWS * 16;

	if (reason & MSYS_CALLBACK_REASON_LBUTTON_UP)
	{
		DropAPersonInASector((UINT8)iValue, (INT16)(sGlobalMapSector % 16 + 1), (INT16)(sGlobalMapSector / 16 + 1));
	}
	else if (reason & MSYS_CALLBACK_REASON_RBUTTON_UP)
	{
		PickUpATownPersonFromSector((UINT8)iValue, (INT16)(sGlobalMapSector % 16 + 1), (INT16)(sGlobalMapSector / 16 + 1));
	}
}


static void DisplayUnallocatedMilitia(void)
{
	// show the nunber on the cursor
	INT32 iTotalNumberOfTroops =0, iNumberOfGreens = 0, iNumberOfRegulars =0, iNumberOfElites = 0, iCurrentTroopIcon = 0;
	INT32 iCurrentIcon = 0;
	INT16 sX = 0, sY = 0;

	// get number of each
	iNumberOfGreens = sGreensOnCursor;
	iNumberOfRegulars = sRegularsOnCursor;
	iNumberOfElites = sElitesOnCursor;

	// get total
	iTotalNumberOfTroops = iNumberOfGreens + iNumberOfRegulars + iNumberOfElites;

	// now display
	for( iCurrentTroopIcon = 0; iCurrentTroopIcon < iTotalNumberOfTroops; iCurrentTroopIcon++ )
	{
		// get screen x and y coords
		sX =  ( iCurrentTroopIcon % NUMBER_OF_MILITIA_ICONS_PER_LOWER_ROW ) * MEDIUM_MILITIA_ICON_SPACING + MAP_MILITIA_BOX_POS_X + MAP_MILITIA_MAP_X + 1;
		sY =  ( iCurrentTroopIcon / NUMBER_OF_MILITIA_ICONS_PER_LOWER_ROW ) * MEDIUM_MILITIA_ICON_SPACING + MAP_MILITIA_BOX_POS_Y + MAP_MILITIA_LOWER_ROW_Y;

		if( iCurrentTroopIcon < iNumberOfGreens )
		{
			iCurrentIcon = 8;
		}
		else if( iCurrentTroopIcon < iNumberOfGreens + iNumberOfRegulars )
		{
			iCurrentIcon = 9;
		}
		else
		{
			iCurrentIcon = 10;
		}

		BltVideoObject(FRAME_BUFFER, guiMilitia, iCurrentIcon, sX, sY);
	}
}


static BOOLEAN IsThisMilitiaTownSectorAllowable(INT16 sSectorIndexValue)
{
	INT16 sBaseSectorValue = 0, sGlobalMapSector = 0;
	INT16 sSectorX, sSectorY;

	// is this sector allowed to be clicked on?
	sBaseSectorValue = GetBaseSectorForCurrentTown( );
	sGlobalMapSector = sBaseSectorValue + ( ( sSectorIndexValue % MILITIA_BOX_ROWS ) + ( sSectorIndexValue / MILITIA_BOX_ROWS ) * 16 );

	sSectorX = SECTORX( sGlobalMapSector );
	sSectorY = SECTORY( sGlobalMapSector );

	// is this in fact part of a town?
	if( StrategicMap[ CALCULATE_STRATEGIC_INDEX( sSectorX, sSectorY ) ].bNameId == BLANK_SECTOR )
	{
		return( FALSE );
	}

	if( !SectorOursAndPeaceful( sSectorX, sSectorY, 0 ) )
	{
		return( FALSE );
	}

	// valid
	return( TRUE );
}


static void DrawTownMilitiaName(void)
{
	CHAR16 sString[ 64 ];
	INT16 sX, sY;

	// get the name for the current militia town
	swprintf( sString, lengthof(sString), L"%ls %ls", pTownNames[ sSelectedMilitiaTown ], pMilitiaString[ 0 ] );
	FindFontCenterCoordinates( MAP_MILITIA_BOX_POS_X, MAP_MILITIA_BOX_POS_Y + MILITIA_BOX_TEXT_OFFSET_Y, MILITIA_BOX_WIDTH, MILITIA_BOX_TEXT_TITLE_HEIGHT, sString, FONT10ARIAL, &sX, &sY );
	MPrint(sX, sY, sString);

	// might as well show the unassigned string
	swprintf( sString, lengthof(sString), L"%ls %ls", pTownNames[ sSelectedMilitiaTown ], pMilitiaString[ 1 ] );
	FindFontCenterCoordinates( MAP_MILITIA_BOX_POS_X, MAP_MILITIA_BOX_POS_Y + MILITIA_BOX_UNASSIGNED_TEXT_OFFSET_Y, MILITIA_BOX_WIDTH, GetFontHeight( FONT10ARIAL ), sString, FONT10ARIAL, &sX, &sY );
	MPrint(sX, sY, sString);
}


static void HandleShutDownOfMilitiaPanelIfPeopleOnTheCursor(INT16 sTownValue)
{
	INT32 iCounter = 0, iCounterB = 0, iNumberUnderControl = 0, iNumberThatCanFitInSector= 0, iCount = 0;
	BOOLEAN fLastOne = FALSE;

	// check if anyone still on the cursor
	if( !sGreensOnCursor && !sRegularsOnCursor && !sElitesOnCursor )
	{
		return;
	}

	// yep someone left
	iNumberUnderControl = GetTownSectorsUnderControl( ( INT8 ) sTownValue );

	// find number of sectors under player's control
	while( pTownNamesList[ iCounter ] != 0 )
	{
		if( pTownNamesList[ iCounter] == sTownValue )
		{
			if( SectorOursAndPeaceful( ( INT16 )( pTownLocationsList[ iCounter ] % MAP_WORLD_X ) , ( INT16 )( pTownLocationsList[ iCounter ] / MAP_WORLD_X ), 0 ) )
			{
				iCount = 0;
				iNumberThatCanFitInSector = MAX_ALLOWABLE_MILITIA_PER_SECTOR;
				iNumberThatCanFitInSector -= SectorInfo[ STRATEGIC_INDEX_TO_SECTOR_INFO( pTownLocationsList[ iCounter ] ) ].ubNumberOfCivsAtLevel[ GREEN_MILITIA ];
				iNumberThatCanFitInSector -= SectorInfo[ STRATEGIC_INDEX_TO_SECTOR_INFO( pTownLocationsList[ iCounter ] ) ].ubNumberOfCivsAtLevel[ REGULAR_MILITIA ];
				iNumberThatCanFitInSector -= SectorInfo[ STRATEGIC_INDEX_TO_SECTOR_INFO( pTownLocationsList[ iCounter ] ) ].ubNumberOfCivsAtLevel[ ELITE_MILITIA ];

				while( ( iCount < iNumberThatCanFitInSector )&&( ( sGreensOnCursor ) || ( sRegularsOnCursor ) || ( sElitesOnCursor ) ) )
				{
					// green
					if( ( iCount + 1 <= iNumberThatCanFitInSector )&&( sGreensOnCursor ) )
					{
						SectorInfo[ STRATEGIC_INDEX_TO_SECTOR_INFO( pTownLocationsList[ iCounter ] ) ].ubNumberOfCivsAtLevel[ GREEN_MILITIA ]++;
						iCount++;
						sGreensOnCursor--;
					}

					// regular
					if( ( iCount + 1 <= iNumberThatCanFitInSector ) && ( sRegularsOnCursor ) )
					{
						SectorInfo[ STRATEGIC_INDEX_TO_SECTOR_INFO( pTownLocationsList[ iCounter ] ) ].ubNumberOfCivsAtLevel[ REGULAR_MILITIA ]++;
						iCount++;
						sRegularsOnCursor--;
					}

					// elite
					if( ( iCount + 1 <= iNumberThatCanFitInSector ) && ( sElitesOnCursor ) )
					{
						SectorInfo[ STRATEGIC_INDEX_TO_SECTOR_INFO( pTownLocationsList[ iCounter ] ) ].ubNumberOfCivsAtLevel[ ELITE_MILITIA ]++;
						iCount++;
						sElitesOnCursor--;
					}
				}

				if( STRATEGIC_INDEX_TO_SECTOR_INFO( pTownLocationsList[ iCounter ] ) == SECTOR( gWorldSectorX, gWorldSectorY ) )
				{
					gfStrategicMilitiaChangesMade = TRUE;
				}

			}

			fLastOne = TRUE;

			iCounterB = iCounter + 1;

			while( pTownNamesList[ iCounterB ] != 0 )
			{
				if( pTownNamesList[ iCounterB ] ==  sTownValue )
				{
					fLastOne = FALSE;
				}

				iCounterB++;
			}

			if( fLastOne )
			{
				SectorInfo[ STRATEGIC_INDEX_TO_SECTOR_INFO( pTownLocationsList[ iCounter ] ) ].ubNumberOfCivsAtLevel[ GREEN_MILITIA ] +=  ( UINT8 )( sGreensOnCursor % iNumberUnderControl );
				SectorInfo[ STRATEGIC_INDEX_TO_SECTOR_INFO( pTownLocationsList[ iCounter ] ) ].ubNumberOfCivsAtLevel[ REGULAR_MILITIA ] +=  ( UINT8 )( sRegularsOnCursor % iNumberUnderControl );
				SectorInfo[ STRATEGIC_INDEX_TO_SECTOR_INFO( pTownLocationsList[ iCounter ] ) ].ubNumberOfCivsAtLevel[ ELITE_MILITIA ] +=  ( UINT8 )( sElitesOnCursor % iNumberUnderControl );
			}
		}

		iCounter++;
	}

	// zero out numbers on the cursor
	sGreensOnCursor = 0;
	sRegularsOnCursor = 0;
	sElitesOnCursor = 0;
}


static void HandleEveningOutOfTroopsAmongstSectors(void)
{
	// even out troops among the town
	INT32 iCounter = 0, iNumberUnderControl = 0, iNumberOfGreens = 0, iNumberOfRegulars = 0, iNumberOfElites = 0, iTotalNumberOfTroops = 0;
	INT32 iNumberLeftOverGreen = 0, iNumberLeftOverRegular = 0, iNumberLeftOverElite = 0;
	INT16 sBaseSectorValue = 0, sCurrentSectorValue = 0;
	INT16 sSectorX = 0, sSectorY = 0, sSector = 0;
	INT16 sTotalSoFar = 0;


	// how many sectors in the selected town do we control?
	iNumberUnderControl = GetTownSectorsUnderControl( ( INT8 ) sSelectedMilitiaTown );

	// if none, there's nothing to be done!
	if( !iNumberUnderControl )
	{
		return;
	}


	// get the sector value for the upper left corner
	sBaseSectorValue = GetBaseSectorForCurrentTown( );

	// render icons for map
	for( iCounter = 0; iCounter < 9; iCounter++ )
	{
		// grab current sector value
		sCurrentSectorValue = sBaseSectorValue + ( ( iCounter % MILITIA_BOX_ROWS ) + ( iCounter / MILITIA_BOX_ROWS ) * ( 16 ) );

		sSectorX = SECTORX( sCurrentSectorValue );
		sSectorY = SECTORY( sCurrentSectorValue );

		// skip sectors not in the selected town (nearby other towns or wilderness SAM Sites)
		if( GetTownIdForSector( sSectorX, sSectorY ) != sSelectedMilitiaTown )
		{
			continue;
		}

		if( !StrategicMap[ CALCULATE_STRATEGIC_INDEX( sSectorX, sSectorY ) ].fEnemyControlled )
		{
			// get number of each
			iNumberOfGreens += SectorInfo[ sCurrentSectorValue ].ubNumberOfCivsAtLevel[ GREEN_MILITIA ];
			iNumberOfRegulars += SectorInfo[ sCurrentSectorValue ].ubNumberOfCivsAtLevel[ REGULAR_MILITIA ];
			iNumberOfElites += SectorInfo[ sCurrentSectorValue ].ubNumberOfCivsAtLevel[ ELITE_MILITIA ];
		}
	}

	// now grab those on the cursor
	iNumberOfGreens += sGreensOnCursor;
	iNumberOfRegulars += sRegularsOnCursor;
	iNumberOfElites += sElitesOnCursor;

	// now get the left overs
	iNumberLeftOverGreen = iNumberOfGreens % iNumberUnderControl;
	iNumberLeftOverRegular = iNumberOfRegulars % iNumberUnderControl;
	iNumberLeftOverElite = iNumberOfElites % iNumberUnderControl;

	// get total
	iTotalNumberOfTroops = iNumberOfGreens + iNumberOfRegulars + iNumberOfElites;

	if( !iTotalNumberOfTroops )
	{
		return;
	}

	iCounter = 0;

	while( pTownNamesList[ iCounter ] != 0 )
	{
		if( pTownNamesList[ iCounter] == sSelectedMilitiaTown )
		{
			sSectorX = GET_X_FROM_STRATEGIC_INDEX( pTownLocationsList[ iCounter ] );
			sSectorY = GET_Y_FROM_STRATEGIC_INDEX( pTownLocationsList[ iCounter ] );

			if( !StrategicMap[ pTownLocationsList[ iCounter ] ].fEnemyControlled && !NumHostilesInSector( sSectorX, sSectorY, 0 ) )
			{
				sSector = SECTOR( sSectorX, sSectorY );

				// distribute here
				SectorInfo[ sSector ].ubNumberOfCivsAtLevel[ GREEN_MILITIA ] =  ( UINT8 )( iNumberOfGreens / iNumberUnderControl );
				SectorInfo[ sSector ].ubNumberOfCivsAtLevel[ REGULAR_MILITIA ] =  ( UINT8 )( iNumberOfRegulars / iNumberUnderControl );
				SectorInfo[ sSector ].ubNumberOfCivsAtLevel[ ELITE_MILITIA ] =  ( UINT8 )( iNumberOfElites / iNumberUnderControl );
				sTotalSoFar = ( INT8 )( ( iNumberOfGreens / iNumberUnderControl ) + ( iNumberOfRegulars / iNumberUnderControl ) + ( iNumberOfElites / iNumberUnderControl ) );

				// add leftovers that weren't included in the div operation
				if( ( iNumberLeftOverGreen ) && ( sTotalSoFar < MAX_ALLOWABLE_MILITIA_PER_SECTOR ) )
				{
					SectorInfo[ sSector ].ubNumberOfCivsAtLevel[ GREEN_MILITIA ]++;
					sTotalSoFar++;
					iNumberLeftOverGreen--;
				}

				if( ( iNumberLeftOverRegular )&&( sTotalSoFar < MAX_ALLOWABLE_MILITIA_PER_SECTOR ) )
				{
					SectorInfo[ sSector ].ubNumberOfCivsAtLevel[ REGULAR_MILITIA ]++;
					sTotalSoFar++;
					iNumberLeftOverRegular--;
				}

				if( ( iNumberLeftOverElite )&&( sTotalSoFar < MAX_ALLOWABLE_MILITIA_PER_SECTOR ) )
				{
					SectorInfo[ sSector ].ubNumberOfCivsAtLevel[ ELITE_MILITIA ]++;
					sTotalSoFar++;
					iNumberLeftOverElite--;
				}

				// if this sector is currently loaded
				if( sSector == SECTOR( gWorldSectorX, gWorldSectorY ) && gWorldSectorY != 0 )
				{
					gfStrategicMilitiaChangesMade = TRUE;
				}
			}
		}

		iCounter++;
	}


	// zero out numbers on the cursor
	sGreensOnCursor = 0;
	sRegularsOnCursor = 0;
	sElitesOnCursor = 0;
}


static void MakeButton(UINT idx, INT16 x, GUI_CALLBACK click, const wchar_t* text)
{
	GUIButtonRef const btn = QuickCreateButtonImg("INTERFACE/militia.sti", -1, 1, -1, 2, -1, x, MAP_MILITIA_BOX_POS_Y + MAP_MILITIA_BOX_AUTO_BOX_Y, MSYS_PRIORITY_HIGHEST - 1, click);
	giMapMilitiaButton[idx] = btn;
	btn->SpecifyGeneralTextAttributes(text, FONT10ARIAL, FONT_BLACK, FONT_BLACK);
}


static BOOLEAN CanMilitiaAutoDistribute(void);
static void MilitiaAutoButtonCallback(GUI_BUTTON* btn, INT32 reason);
static void MilitiaDoneButtonCallback(GUI_BUTTON* btn, INT32 reason);


static void CreateMilitiaPanelBottomButton(void)
{
	MakeButton(3, MAP_MILITIA_BOX_POS_X + MAP_MILITIA_BOX_AUTO_BOX_X, MilitiaAutoButtonCallback, pMilitiaButtonString[0]);
	MakeButton(4, MAP_MILITIA_BOX_POS_X + MAP_MILITIA_BOX_DONE_BOX_X, MilitiaDoneButtonCallback, pMilitiaButtonString[1]);

	// AUTO button help
	giMapMilitiaButton[3]->SetFastHelpText(pMilitiaButtonsHelpText[3]);


	// if auto-distribution is not possible
	if ( !CanMilitiaAutoDistribute( ) )
	{
		// disable the AUTO button
		DisableButton( giMapMilitiaButton[ 3 ] );
	}
}


static void DeleteMilitiaPanelBottomButton(void)
{

	// delete militia panel bottom
	RemoveButton( giMapMilitiaButton[ 3 ] );
	RemoveButton( giMapMilitiaButton[ 4 ] );

	if( sSelectedMilitiaTown != 0 )
	{
		HandleShutDownOfMilitiaPanelIfPeopleOnTheCursor( sSelectedMilitiaTown );
	}

	// redraw the map
	fMapPanelDirty = TRUE;

}


static void MilitiaAutoButtonCallback(GUI_BUTTON *btn, INT32 reason)
{
	if (reason & MSYS_CALLBACK_REASON_LBUTTON_UP)
	{
		// distribute troops over all the sectors under control
		HandleEveningOutOfTroopsAmongstSectors();
		fMapPanelDirty = TRUE;
	}
}


static void MilitiaDoneButtonCallback(GUI_BUTTON *btn, INT32 reason)
{
	if (reason & MSYS_CALLBACK_REASON_LBUTTON_UP)
	{
		// reset fact we are in the box
		sSelectedMilitiaTown = 0;
		fMapPanelDirty = TRUE;
	}
}


static void RenderShadingForUnControlledSectors(void)
{
	// now render shading over any uncontrolled sectors
	// get the sector value for the upper left corner
	INT16 sBaseSectorValue = 0, sCurrentSectorValue = 0, sX = 0, sY = 0;
	INT32 iCounter = 0;

	// get the base sector value
	sBaseSectorValue = GetBaseSectorForCurrentTown( );

	// render icons for map
	for( iCounter = 0; iCounter < 9; iCounter++ )
	{
		// grab current sector value
		sCurrentSectorValue = sBaseSectorValue + ( ( iCounter % MILITIA_BOX_ROWS ) + ( iCounter / MILITIA_BOX_ROWS ) * ( 16 ) );

		if( ( StrategicMap[ SECTOR_INFO_TO_STRATEGIC_INDEX( sCurrentSectorValue ) ].bNameId != BLANK_SECTOR ) &&
			( ( StrategicMap[ SECTOR_INFO_TO_STRATEGIC_INDEX( sCurrentSectorValue ) ].fEnemyControlled ) ||
				( NumHostilesInSector( ( INT16 ) SECTORX( sCurrentSectorValue ), ( INT16 ) SECTORY( sCurrentSectorValue ), 0 ) ) ) )
		{
			// shade this sector, not under our control
			sX = MAP_MILITIA_BOX_POS_X + MAP_MILITIA_MAP_X + ( ( iCounter % MILITIA_BOX_ROWS ) * MILITIA_BOX_BOX_WIDTH );
			sY = MAP_MILITIA_BOX_POS_Y + MAP_MILITIA_MAP_Y + ( ( iCounter / MILITIA_BOX_ROWS ) * MILITIA_BOX_BOX_HEIGHT );

			FRAME_BUFFER->ShadowRect(sX, sY, sX + MILITIA_BOX_BOX_WIDTH - 1, sY + MILITIA_BOX_BOX_HEIGHT - 1);
		}
	}
}


static void DrawTownMilitiaForcesOnMap(void)
{
	INT32 iCounter = 0, iCounterB = 0, iTotalNumberOfTroops = 0, iIconValue = 0;
	INT32 iNumberOfGreens = 0, iNumberOfRegulars = 0,  iNumberOfElites = 0;
	INT16 sSectorX = 0, sSectorY = 0;

	// clip blits to mapscreen region
	ClipBlitsToMapViewRegion( );

	while( pTownNamesList[ iCounter ] != 0 )
	{
		// run through each town sector and plot the icons for the militia forces in the town
		if( !StrategicMap[ pTownLocationsList[ iCounter ] ].fEnemyControlled )
		{
			sSectorX = GET_X_FROM_STRATEGIC_INDEX( pTownLocationsList[ iCounter ] );
			sSectorY = GET_Y_FROM_STRATEGIC_INDEX( pTownLocationsList[ iCounter ] );

			// get number of each
			iNumberOfGreens =  SectorInfo[ STRATEGIC_INDEX_TO_SECTOR_INFO( pTownLocationsList[ iCounter ] ) ].ubNumberOfCivsAtLevel[ GREEN_MILITIA ];
			iNumberOfRegulars = SectorInfo[ STRATEGIC_INDEX_TO_SECTOR_INFO( pTownLocationsList[ iCounter ] ) ].ubNumberOfCivsAtLevel[ REGULAR_MILITIA ];
			iNumberOfElites = SectorInfo[ STRATEGIC_INDEX_TO_SECTOR_INFO( pTownLocationsList[ iCounter ] ) ].ubNumberOfCivsAtLevel[ ELITE_MILITIA ];

			// set the total for loop upper bound
			iTotalNumberOfTroops = iNumberOfGreens + iNumberOfRegulars + iNumberOfElites;

			for( iCounterB = 0; iCounterB < iTotalNumberOfTroops; iCounterB++ )
			{
				if( fZoomFlag )
				{
					// LARGE icon offset in the .sti
					iIconValue = 11;
				}
				else
				{
					// SMALL icon offset in the .sti
					iIconValue = 5;
				}

				// get the offset further into the .sti
				if( iCounterB < iNumberOfGreens )
				{
					iIconValue += 0;
				}
				else if( iCounterB < iNumberOfGreens + iNumberOfRegulars )
				{
					iIconValue += 1;
				}
				else
				{
					iIconValue += 2;
				}

				DrawMapBoxIcon(guiMilitia, iIconValue, sSectorX, sSectorY, iCounterB);
			}
		}

		iCounter++;
	}

	// now handle militia for sam sectors
	for( iCounter = 0; iCounter < NUMBER_OF_SAMS; iCounter++ )
	{
		sSectorX = SECTORX( pSamList[ iCounter ] );
		sSectorY = SECTORY( pSamList[ iCounter ] );

		if( !StrategicMap[ CALCULATE_STRATEGIC_INDEX( sSectorX, sSectorY ) ].fEnemyControlled )
		{
			// get number of each
			iNumberOfGreens =  SectorInfo[ pSamList[ iCounter ] ].ubNumberOfCivsAtLevel[ GREEN_MILITIA ];
			iNumberOfRegulars = SectorInfo[ pSamList[ iCounter ] ].ubNumberOfCivsAtLevel[ REGULAR_MILITIA ];
			iNumberOfElites = SectorInfo[pSamList[ iCounter ] ].ubNumberOfCivsAtLevel[ ELITE_MILITIA ];

			// ste the total for loop upper bound
			iTotalNumberOfTroops = iNumberOfGreens + iNumberOfRegulars + iNumberOfElites;

			for( iCounterB = 0; iCounterB < iTotalNumberOfTroops; iCounterB++ )
			{
				if( fZoomFlag )
				{
					// LARGE icon offset in the .sti
					iIconValue = 11;
				}
				else
				{
					// SMALL icon offset in the .sti
					iIconValue = 5;
				}

				// get the offset further into the .sti
				if( iCounterB < iNumberOfGreens )
				{
					iIconValue += 0;
				}
				else if( iCounterB < iNumberOfGreens + iNumberOfRegulars )
				{
					iIconValue += 1;
				}
				else
				{
					iIconValue += 2;
				}

				DrawMapBoxIcon(guiMilitia, iIconValue, sSectorX, sSectorY, iCounterB);
			}
		}
	}
	// restore clip blits
	RestoreClipRegionToFullScreen( );
}


static void CheckAndUpdateStatesOfSelectedMilitiaSectorButtons(void)
{
	// now set the militia map button text
	INT32 iNumberOfGreens = 0, iNumberOfRegulars = 0, iNumberOfElites = 0;
	INT16 sBaseSectorValue = 0, sGlobalMapSector = 0;


	if( !fMilitiaMapButtonsCreated )
	{
		EnableButton( giMapMilitiaButton[ 4 ] );
		return;
	}

	// grab the appropriate global sector value in the world
	sBaseSectorValue = GetBaseSectorForCurrentTown( );
	sGlobalMapSector = sBaseSectorValue + ( ( sSectorMilitiaMapSector % MILITIA_BOX_ROWS ) + ( sSectorMilitiaMapSector / MILITIA_BOX_ROWS ) * ( 16 ) );

	iNumberOfGreens =  SectorInfo[ sGlobalMapSector ].ubNumberOfCivsAtLevel[ GREEN_MILITIA ] + sGreensOnCursor;
	iNumberOfRegulars = SectorInfo[ sGlobalMapSector ].ubNumberOfCivsAtLevel[ REGULAR_MILITIA ] + sRegularsOnCursor;
	iNumberOfElites = SectorInfo[ sGlobalMapSector ].ubNumberOfCivsAtLevel[ ELITE_MILITIA ] + sElitesOnCursor;

	if( ( sGreensOnCursor > 0 ) || ( sRegularsOnCursor > 0 ) || ( sElitesOnCursor > 0 ) )
	{
		DisableButton( giMapMilitiaButton[ 4 ] );	// DONE
	}
	else
	{
		EnableButton( giMapMilitiaButton[ 4 ] );	// DONE
	}

	// greens button
	if( !iNumberOfGreens )
	{
		DisableButton( giMapMilitiaButton[ 0 ] );
	}
	else
	{
		EnableButton( giMapMilitiaButton[ 1 ] );
	}

	// regulars button
	if( !iNumberOfRegulars )
	{
		DisableButton( giMapMilitiaButton[ 1 ] );
	}
	else
	{
		EnableButton( giMapMilitiaButton[ 1 ] );
	}

	// elites button
	if( !iNumberOfElites )
	{
		DisableButton( giMapMilitiaButton[ 2 ] );
	}
	else
	{
		EnableButton( giMapMilitiaButton[ 2 ] );
	}
}


static void HideExistenceOfUndergroundMapSector(UINT8 ubSectorX, UINT8 ubSectorY);


static void ShadeSubLevelsNotVisited(void)
{
	UNDERGROUND_SECTORINFO *pNode = gpUndergroundSectorInfoHead;

	// obtain the 16-bit version of the same color used in the mine STIs
 	gusUndergroundNearBlack = Get16BPPColor( FROMRGB( 2, 2, 0 ) );

	// run through all (real & possible) underground sectors
	while( pNode )
	{
		// if the sector is on the currently displayed sublevel, and has never been visited
		if( pNode->ubSectorZ == (UINT8)iCurrentMapSectorZ && !pNode->fVisited )
		{
			// remove that portion of the "mine" graphics from view
			HideExistenceOfUndergroundMapSector( pNode->ubSectorX, pNode->ubSectorY );
		}

		pNode = pNode->next;
	}
}


static void HandleLowerLevelMapBlit(void)
{
	// blits the sub level maps
	const SGPVObject* vo; // XXX HACK000E
	switch( iCurrentMapSectorZ )
	{
		case 1: vo = guiSubLevel1; break;
		case 2: vo = guiSubLevel2; break;
		case 3: vo = guiSubLevel3; break;

		default: abort(); // HACK000E
	}

	// handle the blt of the sublevel
	BltVideoObject(guiSAVEBUFFER, vo, 0, MAP_VIEW_START_X + 21, MAP_VIEW_START_Y + 17);

	// handle shading of sublevels
	ShadeSubLevelsNotVisited( );
}


static void MilitiaBoxMaskBtnCallback(MOUSE_REGION* pRegion, INT32 iReason)
{
	// btn callback handler for assignment screen mask region
	if( ( iReason & MSYS_CALLBACK_REASON_LBUTTON_UP )  )
	{
		sSectorMilitiaMapSector = -1;
	}
	else if( iReason & MSYS_CALLBACK_REASON_RBUTTON_UP )
	{
		sSectorMilitiaMapSector = -1;
	}
}


INT32 GetNumberOfMilitiaInSector( INT16 sSectorX, INT16 sSectorY, INT8 bSectorZ )
{
	INT32 iNumberInSector = 0;

	if( !bSectorZ )
	{
	 iNumberInSector = SectorInfo[ SECTOR( sSectorX, sSectorY )].ubNumberOfCivsAtLevel[ GREEN_MILITIA ]
		+  SectorInfo[ SECTOR( sSectorX, sSectorY )].ubNumberOfCivsAtLevel[ REGULAR_MILITIA ]
		+  SectorInfo[ SECTOR( sSectorX, sSectorY )].ubNumberOfCivsAtLevel[ ELITE_MILITIA ];
	}

	return( iNumberInSector );
}


//There is a special case flag used when players encounter enemies in a sector, then retreat.  The number of enemies
//will display on mapscreen until time is compressed.  When time is compressed, the flag is cleared, and
//a question mark is displayed to reflect that the player no longer knows.  This is the function that clears that
//flag.
void ClearAnySectorsFlashingNumberOfEnemies()
{
	INT32 i;
	for( i = 0; i < 256; i++ )
	{
		SectorInfo[ i ].uiFlags &= ~SF_PLAYER_KNOWS_ENEMIES_ARE_HERE;
	}

	// redraw map
	fMapPanelDirty = TRUE;
}


static BOOLEAN CanMercsScoutThisSector(INT16 sSectorX, INT16 sSectorY, INT8 bSectorZ);


UINT32 WhatPlayerKnowsAboutEnemiesInSector( INT16 sSectorX, INT16 sSectorY )
{
	UINT32 uiSectorFlags = SectorInfo[ SECTOR( sSectorX, sSectorY ) ].uiFlags;


	// if player has militia close enough to scout this sector out, if there are mercs who can scout here, OR
	//Special case flag used when players encounter enemies in a sector, then retreat.  The number of enemies
	//will display on mapscreen until time is compressed.  When time is compressed, the flag is cleared, and
	//a question mark is displayed to reflect that the player no longer knows.
	if ( CanMercsScoutThisSector( sSectorX, sSectorY, 0 ) ||
			 CanNearbyMilitiaScoutThisSector( sSectorX, sSectorY ) ||
			 ( uiSectorFlags & SF_PLAYER_KNOWS_ENEMIES_ARE_HERE ) )
	{
		// if the enemies are stationary (i.e. mercs attacking a garrison)
		if ( NumStationaryEnemiesInSector( sSectorX, sSectorY ) > 0 )
		{
			// inside a garrison - hide their # (show question mark) to match what the PBI is showing
			return KNOWS_THEYRE_THERE;
		}
		else
		{
			// other situations - show exactly how many there are
			return KNOWS_HOW_MANY;
		}
	}

	// if the player has visited the sector during this game
	if (GetSectorFlagStatus(sSectorX, sSectorY, 0, SF_ALREADY_VISITED))
	{
		// then he always knows about any enemy presence for the remainder of the game, but not exact numbers
		return KNOWS_THEYRE_THERE;
	}

	// if Skyrider noticed the enemis in the sector recently
	if ( uiSectorFlags & SF_SKYRIDER_NOTICED_ENEMIES_HERE )
	{
		// and Skyrider is still in this sector, flying
		if( IsSkyriderIsFlyingInSector( sSectorX, sSectorY ) )
		{
			// player remains aware of them as long as Skyrider remains in the sector
			return KNOWS_THEYRE_THERE;
		}
		else
		{
			// Skyrider is gone, reset the flag that he noticed enemies here
			SectorInfo[ SECTOR( sSectorX, sSectorY ) ].uiFlags &= ~SF_SKYRIDER_NOTICED_ENEMIES_HERE;
		}
	}


	// no information available
	return KNOWS_NOTHING;
}


static BOOLEAN CanMercsScoutThisSector(INT16 sSectorX, INT16 sSectorY, INT8 bSectorZ)
{
	CFOR_ALL_IN_TEAM(pSoldier, OUR_TEAM)
	{
		// vehicles can't scout!
		if ( pSoldier->uiStatusFlags & SOLDIER_VEHICLE )
		{
			continue;
		}

		// POWs, dead guys, guys in transit, sleeping, and really hurt guys can't scout!
		if ( ( pSoldier->bAssignment == IN_TRANSIT ) ||
				 ( pSoldier->bAssignment == ASSIGNMENT_POW ) ||
				 ( pSoldier->bAssignment == ASSIGNMENT_DEAD ) ||
				pSoldier->fMercAsleep ||
				 ( pSoldier->bLife < OKLIFE ) )
		{
			continue;
		}

		// don't count mercs aboard Skyrider
		if ( ( pSoldier->bAssignment == VEHICLE ) && ( pSoldier->iVehicleId == iHelicopterVehicleId ) )
		{
			continue;
		}

		// mercs on the move can't scout
		if ( pSoldier->fBetweenSectors )
		{
			continue;
		}

		// is he here?
		if( ( pSoldier->sSectorX == sSectorX ) && ( pSoldier->sSectorY == sSectorY ) && ( pSoldier->bSectorZ == bSectorZ ) )
		{
			return( TRUE );
		}
	}

	// none here who can scout
	return( FALSE );
}


static void HandleShowingOfEnemyForcesInSector(INT16 sSectorX, INT16 sSectorY, INT8 bSectorZ, UINT8 ubIconPosition)
{
	INT16 sNumberOfEnemies = 0;


	// ATE: If game has just started, don't do it!
	if ( DidGameJustStart() )
	{
		return;
	}

	// never display enemies underground - sector info doesn't have support for it!
	if( bSectorZ != 0 )
	{
		return;
	}

	// get total number of badguys here
	sNumberOfEnemies = NumEnemiesInSector( sSectorX, sSectorY );

	// anyone here?
	if( !sNumberOfEnemies )
	{
		// nope - display nothing
		return;
	}


	switch ( WhatPlayerKnowsAboutEnemiesInSector( sSectorX, sSectorY ) )
	{
		case KNOWS_NOTHING:
			// display nothing
			break;

		case KNOWS_THEYRE_THERE:
			// display a question mark
			ShowUncertainNumberEnemiesInSector( sSectorX, sSectorY );
			break;

		case KNOWS_HOW_MANY:
			// display individual icons for each enemy, starting at the received icon position index
			ShowEnemiesInSector( sSectorX, sSectorY, sNumberOfEnemies, ubIconPosition );
			break;
	}
}


static void BlitSAMGridMarkers(void);


static void ShowSAMSitesOnStrategicMap(void)
{
	if( fShowAircraftFlag )
	{
		BlitSAMGridMarkers( );
	}

	for (INT32 iCounter = 0; iCounter < NUMBER_OF_SAM_SITES; ++iCounter)
	{
		// has the sam site here been found?
		if( !fSamSiteFound[ iCounter ] )
		{
			continue;
		}

		const INT16 sSectorX = gpSamSectorX[iCounter];
		const INT16 sSectorY = gpSamSectorY[iCounter];

#ifndef JA2DEMO
		DrawSite(sSectorX, sSectorY, guiSAMICON);
#endif

		if( fShowAircraftFlag )
		{
			// write "SAM Site" centered underneath

			INT16 sX;
			INT16 sY;
			if( fZoomFlag )
			{
				GetScreenXYFromMapXYStationary(sSectorX, sSectorY, &sX, &sY);
				sX += 1;
				sY += 9;
			}
			else
			{
				GetScreenXYFromMapXY(sSectorX, sSectorY, &sX, &sY);
				sX += 11;
				sY += 19;
			}

			const wchar_t* SAMSite = pLandTypeStrings[SAM_SITE];

			// we're CENTERING the first string AROUND sX, so calculate the starting X value
			sX -= StringPixLength(SAMSite, MAP_FONT) / 2;

			// if within view region...render, else don't
			if( ( sX > MAP_VIEW_START_X + MAP_VIEW_WIDTH  ) || ( sX < MAP_VIEW_START_X ) ||
					( sY > MAP_VIEW_START_Y + MAP_VIEW_HEIGHT ) || ( sY < MAP_VIEW_START_Y ) )
			{
				continue;
			}


			SetFontDestBuffer(guiSAVEBUFFER, MapScreenRect.iLeft + 2, MapScreenRect.iTop, MapScreenRect.iRight, MapScreenRect.iBottom);

			// clip blits to mapscreen region
			ClipBlitsToMapViewRegion( );

			SetFont(MAP_FONT);
			// Green on green doesn't contrast well, use Yellow
			SetFontForeground(FONT_MCOLOR_LTYELLOW);
			SetFontBackground(FONT_MCOLOR_BLACK);

			// draw the text
			GDirtyPrint(sX, sY, SAMSite);

			// restore clip blits
			RestoreClipRegionToFullScreen( );
		}
	}
}


static void BlitSAMGridMarkers(void)
{
	INT16 sScreenX = 0, sScreenY = 0;
	UINT16 usColor = 0;
	INT32 iCounter = 0;
	INT16 sWidth = 0, sHeight = 0;


	// get 16 bpp color
	usColor = Get16BPPColor( FROMRGB( 100, 100, 100) );

	SGPVSurface::Lock l(guiSAVEBUFFER);
	UINT32 const uiDestPitchBYTES = l.Pitch();

	// clip to view region
	ClipBlitsToMapViewRegionForRectangleAndABit( uiDestPitchBYTES );

	for( iCounter = 0; iCounter < NUMBER_OF_SAM_SITES; iCounter++ )
	{
		// has the sam site here been found?
		if( !fSamSiteFound[ iCounter ] )
		{
			continue;
		}

		if( fZoomFlag )
		{
			GetScreenXYFromMapXYStationary( gpSamSectorX[ iCounter ], gpSamSectorY[ iCounter ], &sScreenX, &sScreenY );
			sScreenX -= MAP_GRID_X;
			sScreenY -= MAP_GRID_Y;

			sWidth = 2 * MAP_GRID_X;
			sHeight= 2 * MAP_GRID_Y;
		}
		else
		{
			// get location on screen
			GetScreenXYFromMapXY( gpSamSectorX[ iCounter ], gpSamSectorY[ iCounter ], &sScreenX, &sScreenY );
			sWidth = MAP_GRID_X;
			sHeight= MAP_GRID_Y;
		}

		// draw rectangle
		RectangleDraw(TRUE, sScreenX, sScreenY - 1, sScreenX + sWidth, sScreenY + sHeight - 1, usColor, l.Buffer<UINT8>());
	}

	// restore clips
	RestoreClipRegionToFullScreenForRectangle( uiDestPitchBYTES );
}


static BOOLEAN CanMilitiaAutoDistribute(void)
{
	INT32 iCounter = 0;
	INT16 sBaseSectorValue = 0, sCurrentSectorValue = 0;
	INT16 sSectorX = 0, sSectorY = 0;
	INT32 iTotalTroopsInTown = 0;


	// can't auto-distribute if we don't have a town selected (this excludes SAM sites)
	if( sSelectedMilitiaTown == BLANK_SECTOR )
		return( FALSE );

	// can't auto-distribute if we don't control any sectors in the the town
	if( !GetTownSectorsUnderControl( ( INT8 ) sSelectedMilitiaTown ) )
		return( FALSE );


	// get the sector value for the upper left corner
	sBaseSectorValue = GetBaseSectorForCurrentTown( );

	// render icons for map
	for( iCounter = 0; iCounter < 9; iCounter++ )
	{
		// grab current sector value
		sCurrentSectorValue = sBaseSectorValue + ( ( iCounter % MILITIA_BOX_ROWS ) + ( iCounter / MILITIA_BOX_ROWS ) * ( 16 ) );

		sSectorX = SECTORX( sCurrentSectorValue );
		sSectorY = SECTORY( sCurrentSectorValue );

		// skip sectors not in the selected town (nearby other towns or wilderness SAM Sites)
		if( GetTownIdForSector( sSectorX, sSectorY ) != sSelectedMilitiaTown )
		{
			continue;
		}

		if( !StrategicMap[ CALCULATE_STRATEGIC_INDEX( sSectorX, sSectorY ) ].fEnemyControlled )
		{
			// get number of each
			iTotalTroopsInTown += SectorInfo[ sCurrentSectorValue ].ubNumberOfCivsAtLevel[ GREEN_MILITIA ] +
															SectorInfo[ sCurrentSectorValue ].ubNumberOfCivsAtLevel[ REGULAR_MILITIA ] +
															SectorInfo[ sCurrentSectorValue ].ubNumberOfCivsAtLevel[ ELITE_MILITIA ];
		}
	}

	// can't auto-distribute if we don't have any militia in the town
	if( !iTotalTroopsInTown )
		return( FALSE );

	// can auto-distribute
	return( TRUE );
}


static void ShowItemsOnMap(void)
{
	INT16 sMapX, sMapY;
	INT16 sXCorner, sYCorner;
	UINT32 uiItemCnt;
	CHAR16 sString[ 10 ];


	// clip blits to mapscreen region
	ClipBlitsToMapViewRegion( );

	SetFontDestBuffer(guiSAVEBUFFER, MapScreenRect.iLeft + 2, MapScreenRect.iTop, MapScreenRect.iRight, MapScreenRect.iBottom);

	SetFont(MAP_FONT);
  SetFontForeground(FONT_MCOLOR_LTGREEN);
  SetFontBackground(FONT_MCOLOR_BLACK);

	// run through sectors
	for( sMapX = 1; sMapX < ( MAP_WORLD_X - 1 ); sMapX++ )
	{
		for( sMapY = 1; sMapY < ( MAP_WORLD_Y - 1 ); sMapY++ )
		{
			// to speed this up, only look at sector that player has visited
			if( GetSectorFlagStatus( sMapX, sMapY, ( UINT8 ) iCurrentMapSectorZ, SF_ALREADY_VISITED ) )
			{
				uiItemCnt = GetNumberOfVisibleWorldItemsFromSectorStructureForSector( sMapX, sMapY, ( UINT8 ) iCurrentMapSectorZ );

				if ( uiItemCnt > 0 )
				{
					sXCorner = ( INT16 )( MAP_VIEW_START_X + ( sMapX * MAP_GRID_X ) );
					sYCorner = ( INT16 )( MAP_VIEW_START_Y + ( sMapY * MAP_GRID_Y ) );

					swprintf( sString, lengthof(sString), L"%d", uiItemCnt );

					INT16 usXPos;
					INT16 usYPos;
					FindFontCenterCoordinates( sXCorner, sYCorner, MAP_GRID_X, MAP_GRID_Y, sString, MAP_FONT, &usXPos, &usYPos );
	//				sXPos -= StringPixLength( sString, MAP_FONT ) / 2;
					GDirtyPrint(usXPos, usYPos, sString);
				}
			}
		}
	}


	// restore clip blits
	RestoreClipRegionToFullScreen( );
}


static void DrawMapBoxIcon(HVOBJECT hIconHandle, UINT16 usVOIndex, INT16 sMapX, INT16 sMapY, UINT8 ubIconPosition)
{
  INT32 iRowNumber, iColumnNumber;
	INT32 iX, iY;


	// don't show any more icons than will fit into one sector, to keep them from spilling into sector(s) beneath
	if ( ubIconPosition >= ( MERC_ICONS_PER_LINE * ROWS_PER_SECTOR ) )
	{
		return;
	}


	iColumnNumber = ubIconPosition % MERC_ICONS_PER_LINE;
	iRowNumber    = ubIconPosition / MERC_ICONS_PER_LINE;

	if ( !fZoomFlag )
	{
		iX = MAP_VIEW_START_X + ( sMapX * MAP_GRID_X ) + MAP_X_ICON_OFFSET + ( 3 * iColumnNumber );
		iY = MAP_VIEW_START_Y + ( sMapY * MAP_GRID_Y ) + MAP_Y_ICON_OFFSET + ( 3 * iRowNumber );

		BltVideoObject( guiSAVEBUFFER, hIconHandle, usVOIndex , iX, iY);
		InvalidateRegion( iX, iY, iX + DMAP_GRID_X, iY + DMAP_GRID_Y );
	}
/*
	else
	{
		INT sX, sY;

		GetScreenXYFromMapXYStationary( ( UINT16 ) sX,( UINT16 ) sY, &sX, &sY );
 	 	iY = sY-MAP_GRID_Y;
		iX = sX-MAP_GRID_X;

		// clip blits to mapscreen region
		ClipBlitsToMapViewRegion( );

		BltVideoObject(guiSAVEBUFFER, hIconHandle,BIG_YELLOW_BOX,MAP_X_ICON_OFFSET+iX+6*iColumnNumber+2,MAP_Y_ICON_OFFSET+iY+6*iRowNumber);

		// restore clip blits
		RestoreClipRegionToFullScreen( );

		InvalidateRegion(MAP_X_ICON_OFFSET+iX+6*iColumnNumber+2, MAP_Y_ICON_OFFSET+iY+6*iRowNumber,MAP_X_ICON_OFFSET+iX+6*iColumnNumber+2+ DMAP_GRID_ZOOM_X, MAP_Y_ICON_OFFSET+iY+6*iRowNumber + DMAP_GRID_ZOOM_Y );
	}
*/
}


static void DrawOrta(void)
{
	DrawSite(ORTA_SECTOR_X, ORTA_SECTOR_Y, guiORTAICON);
}


static void DrawTixa(void)
{
	DrawSite(TIXA_SECTOR_X, TIXA_SECTOR_Y, guiTIXAICON);
}


static void DrawBullseye(void)
{
#ifndef JA2DEMO
	INT16 sX, sY;

	GetScreenXYFromMapXY( gsMercArriveSectorX, gsMercArriveSectorY, &sX, &sY );
	sY -= 2;

	BltVideoObject(guiSAVEBUFFER, guiBULLSEYE, 0, sX, sY);
#endif
}


static void HideExistenceOfUndergroundMapSector(UINT8 ubSectorX, UINT8 ubSectorY)
{
	INT16 sScreenX;
	INT16 sScreenY;

	GetScreenXYFromMapXY( ubSectorX, ubSectorY, &sScreenX, &sScreenY );

	// fill it with near black
	ColorFillVideoSurfaceArea( guiSAVEBUFFER, sScreenX + 1, sScreenY, sScreenX + MAP_GRID_X,	sScreenY + MAP_GRID_Y - 1, gusUndergroundNearBlack );
}


void InitMapSecrets( void )
{
	UINT8 ubSamIndex;

	fFoundTixa = FALSE;
	fFoundOrta = FALSE;

	for( ubSamIndex = 0; ubSamIndex < NUMBER_OF_SAMS; ubSamIndex++ )
	{
		fSamSiteFound[ ubSamIndex ] = FALSE;
	}
}


BOOLEAN CanRedistributeMilitiaInSector(INT16 sClickedSectorX, INT16 sClickedSectorY, INT8 bClickedTownId)
{
	INT32 iCounter = 0;
	INT16 sBaseSectorValue = 0, sCurrentSectorValue = 0;
	INT16 sSectorX = 0, sSectorY = 0;

	// if no world is loaded, we can't be in combat (PBI/Auto-resolve locks out normal mapscreen interface for this)
	if( !gfWorldLoaded )
	{
		// ok to redistribute
		return( TRUE );
	}

	// if tactically not in combat, hostile sector, or air-raid
	if( !( gTacticalStatus.uiFlags & INCOMBAT ) && !( gTacticalStatus.fEnemyInSector ) && !InAirRaid( ) )
	{
		// ok to redistribute
		return( TRUE );
	}

	// if the fight is underground
	if ( gbWorldSectorZ != 0 )
	{
		// ok to redistribute
		return( TRUE );
	}


	// currently loaded surface sector IS hostile - so we must check if it's also one of the sectors in this "militia map"

	// get the sector value for the upper left corner
	sBaseSectorValue = sBaseSectorList[ bClickedTownId - 1 ];

	// render icons for map
	for( iCounter = 0; iCounter < 9; iCounter++ )
	{
		// grab current sector value
		sCurrentSectorValue = sBaseSectorValue + ( ( iCounter % MILITIA_BOX_ROWS ) + ( iCounter / MILITIA_BOX_ROWS ) * ( 16 ) );

		sSectorX = SECTORX( sCurrentSectorValue );
		sSectorY = SECTORY( sCurrentSectorValue );

		// not in the same town?
		if( StrategicMap[ CALCULATE_STRATEGIC_INDEX( sSectorX, sSectorY ) ].bNameId != bClickedTownId )
		{
			continue;
		}

		// if this is the loaded sector that is currently hostile
		if ( ( sSectorX == gWorldSectorX ) && ( sSectorY == gWorldSectorY ) )
		{
			// the fight is within this town!  Can't redistribute.
			return( FALSE );
		}
	}


	// the fight is elsewhere - ok to redistribute
	return( TRUE );
}
