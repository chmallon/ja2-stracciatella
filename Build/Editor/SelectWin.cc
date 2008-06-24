#ifdef JA2EDITOR

#include "Button_System.h"
#include "Font.h"
#include "HImage.h"
#include "Input.h"
#include "Overhead_Map.h"
#include "TileDef.h"
#include "VSurface.h"
#include "WorldDat.h"
#include "Random.h"
#include "SysUtil.h"
#include "Font_Control.h"
#include "SelectWin.h"
#include "EditorDefines.h"
#include "Editor_Taskbar_Utils.h"
#include "MemMan.h"
#include "VObject.h"
#include "VObject_Blitters.h"
#include "WorldDef.h"


// defines for DisplaySpec.ubType
#define DISPLAY_TEXT    1
#define DISPLAY_GRAPHIC 2

enum DisplayWindowFlags
{
	ONE_COLUMN       = 0x0001,
	ONE_ROW          = 0x0002,
	CLEAR_BACKGROUND = 0x0004
};
ENUM_BITSET(DisplayWindowFlags)

#define DISPLAY_ALL_OBJECTS 0xFFFF


typedef struct DisplaySpec
{
	UINT8 ubType;
	union
	{
		struct
		{
			HVOBJECT hVObject;
			UINT16   usStart;
			UINT16   usEnd;
			UINT32   uiObjIndx;
		};
		struct
		{
			UINT16* pString;
		};
	};
} DisplaySpec;


typedef struct DisplayList DisplayList;
struct DisplayList
{
	HVOBJECT     hObj;
	UINT16       uiIndex;
	INT16        iX;
	INT16        iY;
	INT16        iWidth;
	INT16        iHeight;
	UINT32       uiObjIndx;
	BOOLEAN      fChosen;
	DisplayList* pNext;
};


extern BOOLEAN fDontUseRandom;


static BOOLEAN gfRenderSquareArea = FALSE;
static INT16 iStartClickX;
static INT16 iStartClickY;
static INT16 iEndClickX;
static INT16 iEndClickY;


#define CANCEL_ICON		0
#define UP_ICON				1
#define DOWN_ICON			2
#define OK_ICON				3

static INT32 iButtonIcons[4];
static GUIButtonRef iSelectWin;
static GUIButtonRef iCancelWin;
static GUIButtonRef iScrollUp;
static GUIButtonRef iScrollDown;
static GUIButtonRef iOkWin;

BOOLEAN fAllDone=FALSE;
static BOOLEAN fButtonsPresent = FALSE;

SGPPoint SelWinSpacing, SelWinStartPoint, SelWinEndPoint;

//These definitions help define the start and end of the various wall indices.
//This needs to be maintained if the walls change.
#define WALL_LAST_WALL_OFFSET						 30
#define	WALL_FIRST_AFRAME_OFFSET				 31
#define WALL_LAST_AFRAME_OFFSET					 34
#define	WALL_FIRST_WINDOW_OFFSET				 35
#define WALL_LAST_WINDOW_OFFSET					 46
#define WALL_FIRST_BROKEN_WALL_OFFSET		 47
#define WALL_LAST_BROKEN_WALL_OFFSET		 54
#define WALL_FIRST_APPENDED_WALL_OFFSET	 55
#define WALL_LAST_APPENDED_WALL_OFFSET	 56
#define WALL_FIRST_WEATHERED_WALL_OFFSET 57
#define WALL_LAST_WEATHERED_WALL_OFFSET  64

//I've added these definitions to add readability, and minimize conversion time for changes
//incase there are new values, etc.
#define OSTRUCTS_NUMELEMENTS		( LASTOSTRUCT - FIRSTFULLSTRUCT + 22 )
#define OSTRUCTS1_NUMELEMENTS					5
#define OSTRUCTS2_NUMELEMENTS					12
#define BANKSLIST_NUMELEMENTS					5
#define ROADSLIST_NUMELEMENTS					1
#define DEBRISLIST_NUMELEMENTS				( LASTDEBRIS - DEBRISROCKS + 2 + 1 ) //+1 for ANOTHERDEBRIS

#define SINGLEWALL_NUMELEMENTS				( (LASTWALL-FIRSTWALL+1) * 2 )
#define SINGLEDOOR_NUMELEMENTS				(( LASTDOOR - FIRSTDOOR + 1 ) * 5)
#define SINGLEWINDOW_NUMELEMENTS			( LASTWALL - FIRSTWALL + 1 )
#define SINGLEROOF_NUMELEMENTS				( (LASTROOF-FIRSTROOF+1) + (LASTSLANTROOF-FIRSTSLANTROOF+1) + \
																				(LASTWALL-FIRSTWALL+1) + (SECONDONROOF-FIRSTONROOF+1) )
#define SINGLENEWROOF_NUMELEMENTS			(LASTROOF-FIRSTROOF+1)
#define SINGLEBROKENWALL_NUMELEMENTS	( (LASTDECORATIONS-FIRSTDECORATIONS+1) + (LASTWALL-FIRSTWALL+1)*2 )
#define SINGLEDECOR_NUMELEMENTS				( LASTISTRUCT - FIRSTISTRUCT + 1 )
#define SINGLEDECAL_NUMELEMENTS				( LASTWALLDECAL - FIRSTWALLDECAL + EIGTHWALLDECAL - FIFTHWALLDECAL + 3 )
#define SINGLEFLOOR_NUMELEMENTS				( LASTFLOOR - FIRSTFLOOR + 1 )
#define SINGLETOILET_NUMELEMENTS			( EIGHTISTRUCT - FIFTHISTRUCT + 1 )
//#define ROOM_NUMELEMENTS							( (LASTWALL-FIRSTWALL+1) + (LASTFLOOR-FIRSTFLOOR+1) + \
//																				(LASTROOF-FIRSTROOF+1) + (LASTSLANTROOF-FIRSTSLANTROOF+1) )
#define ROOM_NUMELEMENTS							( (LASTWALL-FIRSTWALL+1) + (LASTFLOOR-FIRSTFLOOR+1) + \
																				(LASTROOF-FIRSTROOF+1) + (2 ))


//This is a special case for trees which may have varying numbers.  There was a problem
//in which we loaded a new tileset which had one less tree in it.  When we called BuildSelectionWindow(),
//it would crash because it thought there was an extra tree which was now invalid.
static UINT16 gusNumOStructs = 0;

// List of objects to display in the selection window
static DisplaySpec OStructs[OSTRUCTS_NUMELEMENTS];
static DisplaySpec OStructs1[OSTRUCTS1_NUMELEMENTS];
static DisplaySpec OStructs2[OSTRUCTS2_NUMELEMENTS];
static DisplaySpec BanksList[BANKSLIST_NUMELEMENTS];
static DisplaySpec RoadsList[ROADSLIST_NUMELEMENTS];
static DisplaySpec DebrisList[DEBRISLIST_NUMELEMENTS];
static DisplaySpec SingleWall[SINGLEWALL_NUMELEMENTS];
static DisplaySpec SingleDoor[SINGLEDOOR_NUMELEMENTS];
static DisplaySpec SingleWindow[SINGLEWINDOW_NUMELEMENTS];
static DisplaySpec SingleRoof[SINGLEROOF_NUMELEMENTS];
static DisplaySpec SingleNewRoof[SINGLENEWROOF_NUMELEMENTS];
static DisplaySpec SingleBrokenWall[SINGLEBROKENWALL_NUMELEMENTS];
static DisplaySpec SingleDecor[SINGLEDECOR_NUMELEMENTS];
static DisplaySpec SingleDecal[SINGLEDECAL_NUMELEMENTS];
static DisplaySpec SingleFloor[SINGLEFLOOR_NUMELEMENTS];
static DisplaySpec SingleToilet[SINGLETOILET_NUMELEMENTS];
static DisplaySpec Room[ROOM_NUMELEMENTS];

//These are all of the different selection lists.  Changing the max_selections will
//change the number of selections values you can have at a time.  This is Bret's gay code,
//though I've cleaned it up a lot.
Selections	SelOStructs[MAX_SELECTIONS]				=		{ {FIRSTFULLSTRUCT, 0, 1} };	// Default selections
Selections	SelOStructs1[MAX_SELECTIONS]				=		{ {FOURTHOSTRUCT, 0, 1} };	// Default selections
Selections	SelOStructs2[MAX_SELECTIONS]				=		{ {THIRDOSTRUCT, 0, 1} };	// Default selections
Selections	SelBanks[MAX_SELECTIONS]					=		{ {FIRSTCLIFF, 0, 1} };
Selections	SelRoads[MAX_SELECTIONS]					=		{ {FIRSTROAD, 0, 1} };
Selections	SelDebris[MAX_SELECTIONS]					=		{ {DEBRISROCKS, 0, 1} };
Selections	SelSingleWall[MAX_SELECTIONS]			=		{ {FIRSTWALL, 0, 1} };
Selections	SelSingleDoor[MAX_SELECTIONS]			=		{ {FIRSTDOOR, 0, 1} };
Selections	SelSingleWindow[MAX_SELECTIONS]		=		{ {FIRSTWALL, 44, 1} };
Selections  SelSingleRoof[MAX_SELECTIONS] =				{ {FIRSTROOF, 0, 1} };
Selections  SelSingleNewRoof[MAX_SELECTIONS] =		{ {FIRSTROOF, 0, 1} };
Selections  SelSingleBrokenWall[MAX_SELECTIONS] = { {FIRSTDECORATIONS, 0, 1} };
Selections	SelSingleDecor[MAX_SELECTIONS]=				{ {FIRSTISTRUCT, 0, 1} };
Selections	SelSingleDecal[MAX_SELECTIONS]	=		{ {FIRSTWALLDECAL, 0, 1} };
Selections  SelSingleFloor[MAX_SELECTIONS] =			{ {FIRSTFLOOR, 0, 1} };
Selections  SelSingleToilet[MAX_SELECTIONS] =			{ {FIFTHISTRUCT, 0, 1} };
Selections	SelRoom[MAX_SELECTIONS]			=					{ {FIRSTWALL, 0, 1} };

// Number of objects currently in the selection list
INT32				iNumOStructsSelected = 1;
INT32				iNumOStructs1Selected = 1;
INT32				iNumOStructs2Selected = 1;
INT32				iNumBanksSelected = 1;
INT32				iNumRoadsSelected = 1;
INT32				iNumDebrisSelected = 1;

INT32				iNumWallsSelected = 1;
INT32				iNumDoorsSelected = 1;
INT32				iNumWindowsSelected = 1;
INT32				iNumDecorSelected = 1;
INT32				iNumDecalsSelected = 1;
INT32				iNumBrokenWallsSelected = 1;
INT32				iNumFloorsSelected = 1;
INT32       iNumToiletsSelected = 1;
INT32				iNumRoofsSelected = 1;
INT32				iNumNewRoofsSelected = 1;
INT32				iNumRoomsSelected = 1;

// Holds the previous selection list when a selection window is up. Used for canceling the selection window
Selections	OldSelList[MAX_SELECTIONS];
INT32				iOldNumSelList;

// Global pointers for selection list
Selections  *pSelList;
INT32				*pNumSelList;

// Global used to indicate which selection to use (changes with the PGUP/PGDWN keys in editor)
INT32				iCurBank = 0;

static DisplayList* pDispList;
static INT16 iTopWinCutOff;

UINT16 SelWinFillColor = 0x0000;					// Black
UINT16 SelWinHilightFillColor = 0x000d;		// a kind of medium dark blue


static BOOLEAN BuildDisplayWindow(DisplaySpec const*, UINT16 usNumSpecs, DisplayList** pDisplayList, SGPPoint const* pUpperLeft, SGPPoint const* pBottomRight, SGPPoint const* pSpacing, DisplayWindowFlags);
static void CnclClkCallback(GUI_BUTTON* button, INT32 reason);
static void DwnClkCallback(GUI_BUTTON* button, INT32 reason);
static void OkClkCallback(GUI_BUTTON* button, INT32 reason);
static void SelWinClkCallback(GUI_BUTTON* button, INT32 reason);
static void UpClkCallback(GUI_BUTTON* button, INT32 reason);


static GUIButtonRef MakeButton(UINT idx, const char* gfx, INT16 y, INT16 h, GUI_CALLBACK click, const wchar_t* help)
{
	INT32 img = LoadGenericButtonIcon(gfx);
	iButtonIcons[idx] = img;
	GUIButtonRef const btn = CreateIconButton(img, 0, SCREEN_WIDTH - 40, y, 40, h, MSYS_PRIORITY_HIGH, click);
	btn->SetFastHelpText(help);
	return btn;
}


// Create a selection window of the given type.
void CreateJA2SelectionWindow(SelectWindow const sWhat)
{
	DisplaySpec *pDSpec; // XXX HACK000E
	UINT16 usNSpecs;

	fAllDone = FALSE;

	DisableEditorTaskbar( );

	iSelectWin = CreateHotSpot(0, 0, SCREEN_WIDTH - 40, TASKBAR_Y, MSYS_PRIORITY_HIGH, SelWinClkCallback);

	INT16 y;
	iOkWin      = MakeButton(OK_ICON,     "EDITOR/checkmark.sti",   y  =  0, 40, OkClkCallback,   L"Accept selections");
	iCancelWin  = MakeButton(CANCEL_ICON, "EDITOR/bigX.sti",        y += 40, 40, CnclClkCallback, L"Cancel selections");
	INT16 const h = (TASKBAR_Y - 40 - 40) / 2;
	iScrollUp   = MakeButton(UP_ICON,     "EDITOR/lgUpArrow.sti",   y += 40,  h, UpClkCallback,   L"Scroll window up");
	iScrollDown = MakeButton(DOWN_ICON,   "EDITOR/lgDownArrow.sti", y += h,   h, DwnClkCallback,  L"Scroll window down");

	fButtonsPresent = TRUE;

	SelWinSpacing.iX = 2;
	SelWinSpacing.iY = 2;

	SelWinStartPoint.iX = 1;
	SelWinStartPoint.iY = 15;

	iTopWinCutOff = 15;

	SelWinEndPoint.iX = 599;
	SelWinEndPoint.iY = 359;

	switch( sWhat )
	{
		case SELWIN_OSTRUCTS:
			pDSpec = OStructs;
			usNSpecs = gusNumOStructs;//OSTRUCTS_NUMELEMENTS;
			pSelList = SelOStructs;
			pNumSelList = &iNumOStructsSelected;
			break;

		case SELWIN_OSTRUCTS1:
			pDSpec = OStructs1;
			usNSpecs = OSTRUCTS1_NUMELEMENTS;
			pSelList = SelOStructs1;
			pNumSelList = &iNumOStructs1Selected;
			break;

		case SELWIN_OSTRUCTS2:
			pDSpec = OStructs2;
			usNSpecs = OSTRUCTS2_NUMELEMENTS;
			pSelList = SelOStructs2;
			pNumSelList = &iNumOStructs2Selected;
			break;

		case SELWIN_BANKS:
			pDSpec = BanksList;
			usNSpecs = BANKSLIST_NUMELEMENTS;
			pSelList = SelBanks;
			pNumSelList = &iNumBanksSelected;
			break;

		case SELWIN_ROADS:
			pDSpec = RoadsList;
			usNSpecs = ROADSLIST_NUMELEMENTS;
			pSelList = SelRoads;
			pNumSelList = &iNumRoadsSelected;
			break;

		case SELWIN_DEBRIS:
			pDSpec = DebrisList;
			usNSpecs = DEBRISLIST_NUMELEMENTS;
			pSelList = SelDebris;
			pNumSelList = &iNumDebrisSelected;
			break;

		case SELWIN_SINGLEWALL:
			pDSpec = SingleWall;
			usNSpecs = SINGLEWALL_NUMELEMENTS;
			pSelList = SelSingleWall;
			pNumSelList = &iNumWallsSelected;
			break;
		case SELWIN_SINGLEDOOR:
			pDSpec = SingleDoor;
			usNSpecs = SINGLEDOOR_NUMELEMENTS;
			pSelList = SelSingleDoor;
			pNumSelList = &iNumDoorsSelected;
			break;
		case SELWIN_SINGLEWINDOW:
			pDSpec = SingleWindow;
			usNSpecs = SINGLEWINDOW_NUMELEMENTS;
			pSelList = SelSingleWindow;
			pNumSelList = &iNumWindowsSelected;
			break;
		case SELWIN_SINGLEROOF:
			pDSpec = SingleRoof;
			usNSpecs = SINGLEROOF_NUMELEMENTS;
			pSelList = SelSingleRoof;
			pNumSelList = &iNumRoofsSelected;
			break;
		case SELWIN_SINGLENEWROOF:
			pDSpec = SingleNewRoof;
			usNSpecs = SINGLENEWROOF_NUMELEMENTS;
			pSelList = SelSingleNewRoof;
			pNumSelList = &iNumNewRoofsSelected;
			break;
		case SELWIN_SINGLEBROKENWALL:
			pDSpec = SingleBrokenWall;
			usNSpecs = SINGLEBROKENWALL_NUMELEMENTS;
			pSelList = SelSingleBrokenWall;
			pNumSelList = &iNumBrokenWallsSelected;
			break;
		case SELWIN_SINGLEDECOR:
			pDSpec = SingleDecor;
			usNSpecs = SINGLEDECOR_NUMELEMENTS;
			pSelList = SelSingleDecor;
			pNumSelList = &iNumDecorSelected;
			break;
		case SELWIN_SINGLEDECAL:
			pDSpec = SingleDecal;
			usNSpecs = SINGLEDECAL_NUMELEMENTS;
			pSelList = SelSingleDecal;
			pNumSelList = &iNumDecalsSelected;
			break;
		case SELWIN_SINGLEFLOOR:
			pDSpec = SingleFloor;
			usNSpecs = SINGLEFLOOR_NUMELEMENTS;
			pSelList = SelSingleFloor;
			pNumSelList = &iNumFloorsSelected;
			break;
		case SELWIN_SINGLETOILET:
			pDSpec = SingleToilet;
			usNSpecs = SINGLETOILET_NUMELEMENTS;
			pSelList = SelSingleToilet;
			pNumSelList = &iNumToiletsSelected;
			break;
		case SELWIN_ROOM:
			pDSpec = Room;
			usNSpecs = ROOM_NUMELEMENTS;
			pSelList = SelRoom;
			pNumSelList = &iNumRoomsSelected;
			break;

		default: abort(); // HACK000E
	}

	BuildDisplayWindow( pDSpec, usNSpecs, &pDispList, &SelWinStartPoint, &SelWinEndPoint,
										&SelWinSpacing, CLEAR_BACKGROUND);
}


static void InitDisplayGfx(DisplaySpec* const ds, const HVOBJECT vo, const UINT16 start, const UINT16 end, const UINT32 obj_idx)
{
	ds->ubType    = DISPLAY_GRAPHIC;
	ds->hVObject  = vo;
	ds->usStart   = start;
	ds->usEnd     = end;
	ds->uiObjIndx = obj_idx;
}


static void InitDisplayGfxFromTileData(DisplaySpec* const ds, const UINT16 start, const UINT16 end, const UINT32 obj_idx)
{
	InitDisplayGfx(ds, TileElemFromTileType(obj_idx)->hTileSurface, start, end, obj_idx);
}


static void InitDisplayGfxAllFromTileData(DisplaySpec* const ds, const UINT32 obj_idx)
{
	InitDisplayGfxFromTileData(ds, DISPLAY_ALL_OBJECTS, 0, obj_idx);
}


//The selection window method is initialized here.  This is where all the graphics for all
//the categories are organized and loaded.  If you wish to move things around, then this is
//where the initialization part is done.  I have also changed this from previously being loaded
//every single time you go into a selection window which was redundant and CPU consuming.
void InitJA2SelectionWindow( void )
{
	INT32 iCount;
	INT32 iCount2;
	INT32 iCount3;

	pDispList = NULL;

	// Init the display spec lists for the types of selection windows

	// Trees & bushes (The tree button in the "terrain" toolbar)
	for ( iCount3 = 0, iCount = 0; iCount < (LASTOSTRUCT - FIRSTFULLSTRUCT + 1); iCount++ )
	{
		const HVOBJECT hVObject = TileElemFromTileType(FIRSTFULLSTRUCT + iCount)->hTileSurface;
		UINT16 const usETRLEObjects = hVObject->SubregionCount();

		for ( iCount2 = 0; iCount2 < usETRLEObjects; iCount2 += 3, iCount3++)
		{
			InitDisplayGfx(&OStructs[iCount3], hVObject, iCount2, iCount2, FIRSTFULLSTRUCT + iCount);
		}
	}

	InitDisplayGfxAllFromTileData(&OStructs[iCount3], SIXTHOSTRUCT);

	gusNumOStructs = (UINT16)iCount3 + 1;

	// Rocks & barrels! (the "1" button in the "terrain" toolbar)
	InitDisplayGfxAllFromTileData(&OStructs1[0], FOURTHOSTRUCT);

	for ( iCount = 0; iCount < (THIRDOSTRUCT - FIRSTOSTRUCT); iCount++ )
	{
		InitDisplayGfxAllFromTileData(&OStructs1[iCount +  1], FIRSTOSTRUCT + iCount);
	}

	// Other junk! (the "2" button in the "terrain" toolbar)
	InitDisplayGfxAllFromTileData(&OStructs2[ 0], THIRDOSTRUCT);
	InitDisplayGfxAllFromTileData(&OStructs2[ 1], FIFTHOSTRUCT);
	InitDisplayGfxAllFromTileData(&OStructs2[ 2], SEVENTHOSTRUCT);
	InitDisplayGfxAllFromTileData(&OStructs2[ 3], EIGHTOSTRUCT);
	InitDisplayGfxAllFromTileData(&OStructs2[ 4], FIRSTVEHICLE);
	InitDisplayGfxAllFromTileData(&OStructs2[ 5], SECONDVEHICLE);
	InitDisplayGfxAllFromTileData(&OStructs2[ 6], FIRSTDEBRISSTRUCT);
	InitDisplayGfxAllFromTileData(&OStructs2[ 7], SECONDDEBRISSTRUCT);
	InitDisplayGfxAllFromTileData(&OStructs2[ 8], FIRSTLARGEEXPDEBRIS);
	InitDisplayGfxAllFromTileData(&OStructs2[ 9], SECONDLARGEEXPDEBRIS);
	InitDisplayGfxAllFromTileData(&OStructs2[10], NINTHOSTRUCT);
	InitDisplayGfxAllFromTileData(&OStructs2[11], TENTHOSTRUCT);

	// River banks and cliffs (the "river" button on the "terrain" toolbar)
	InitDisplayGfxAllFromTileData(&BanksList[0], ANIOSTRUCT);
	InitDisplayGfxAllFromTileData(&BanksList[0], FIRSTCLIFF);
	InitDisplayGfxAllFromTileData(&BanksList[0], FIRSTCLIFFHANG);
	InitDisplayGfxAllFromTileData(&BanksList[0], FIRSTROAD);
	InitDisplayGfxAllFromTileData(&BanksList[0], FENCESTRUCT);

	InitDisplayGfxAllFromTileData(&RoadsList[0], FIRSTROAD);

	// Debris (the "bent can" button on the "terrain", and "buildings" toolbars)
	for ( iCount = 0; iCount < (LASTDEBRIS - DEBRISROCKS + 1); iCount++ )
	{
		InitDisplayGfxAllFromTileData(&DebrisList[iCount], DEBRISROCKS + iCount);
	}
	// Add one more for new misc debris
	InitDisplayGfxAllFromTileData(&DebrisList[iCount++], DEBRIS2MISC);
	//Add yet another one...
	InitDisplayGfxAllFromTileData(&DebrisList[iCount],   ANOTHERDEBRIS);

	// Rooms
	for ( iCount = 0; iCount < (LASTWALL - FIRSTWALL + 1); iCount++ )
	{
		InitDisplayGfxFromTileData(&Room[iCount], 0, 0, FIRSTWALL + iCount);
	}
	for ( iCount2 = 0; iCount2 < (LASTFLOOR - FIRSTFLOOR + 1); iCount2++, iCount++ )
	{
		InitDisplayGfxFromTileData(&Room[iCount], 0, 0, FIRSTFLOOR + iCount2);
	}
	for ( iCount2 = 0; iCount2 < (LASTROOF - FIRSTROOF + 1); iCount2++, iCount++ )
	{
		InitDisplayGfxFromTileData(&Room[iCount], 0, 0, FIRSTROOF + iCount2);
	}
	for ( iCount2 = 0; iCount2 < 2/*(LASTSLANTROOF - FIRSTSLANTROOF + 1)*/; iCount2++, iCount++ )
	{
		InitDisplayGfxFromTileData(&Room[iCount], 0, 0, FIRSTSLANTROOF + iCount2);
	}

	//Walls
	for ( iCount = 0, iCount2 = 0; iCount < (LASTWALL - FIRSTWALL + 1); iCount++, iCount2+=2 )
	{
		InitDisplayGfxFromTileData(&SingleWall[iCount2],     0,                               WALL_LAST_WALL_OFFSET,          FIRSTWALL + iCount);
		//New appended walls
		InitDisplayGfxFromTileData(&SingleWall[iCount2 + 1], WALL_FIRST_APPENDED_WALL_OFFSET, WALL_LAST_APPENDED_WALL_OFFSET, FIRSTWALL + iCount);
	}

	//Doors
	for ( iCount = 0, iCount2 = 0; iCount < (LASTDOOR - FIRSTDOOR + 1); iCount++, iCount2+=5)
	{
		//closed
		InitDisplayGfxFromTileData(&SingleDoor[iCount2],      0,  0, FIRSTDOOR + iCount);
		//open, closed
		InitDisplayGfxFromTileData(&SingleDoor[iCount2 + 1],  4,  5, FIRSTDOOR + iCount);
		//open, closed
		InitDisplayGfxFromTileData(&SingleDoor[iCount2 + 2],  9, 10, FIRSTDOOR + iCount);
		//open, closed
		InitDisplayGfxFromTileData(&SingleDoor[iCount2 + 3], 14, 15, FIRSTDOOR + iCount);
		//open
		InitDisplayGfxFromTileData(&SingleDoor[iCount2 + 4], 19, 19, FIRSTDOOR + iCount);
	}
	//Windows
	for ( iCount = 0; iCount < (LASTWALL - FIRSTWALL + 1); iCount++ )
	{
		InitDisplayGfxFromTileData(&SingleWindow[iCount], WALL_FIRST_WINDOW_OFFSET, WALL_LAST_WINDOW_OFFSET, FIRSTWALL + iCount);
	}
	//Roofs and slant roofs
	for ( iCount = 0; iCount < (LASTROOF - FIRSTROOF + 1); iCount++ )
	{	//Flat roofs
		InitDisplayGfxAllFromTileData(&SingleRoof[iCount], FIRSTROOF + iCount);
	}
	for ( iCount2 = 0; iCount2 < (LASTSLANTROOF - FIRSTSLANTROOF + 1); iCount2++, iCount++ )
	{	//Slanted roofs
		InitDisplayGfxAllFromTileData(&SingleRoof[iCount], FIRSTSLANTROOF + iCount2);
	}
	for( iCount2 = 0; iCount2 < (LASTWALL - FIRSTWALL + 1); iCount2++, iCount++ )
	{	//A-Frames
		InitDisplayGfxFromTileData(&SingleRoof[iCount], WALL_FIRST_AFRAME_OFFSET, WALL_LAST_AFRAME_OFFSET, FIRSTWALL + iCount2);
	}
	for( iCount2 = 0; iCount2 < (SECONDONROOF - FIRSTONROOF + 1); iCount2++, iCount++ )
	{	//On roofs
		InitDisplayGfxAllFromTileData(&SingleRoof[iCount], FIRSTONROOF + iCount2);
	}

	//New replacement roofs
	for ( iCount = 0; iCount < (LASTROOF - FIRSTROOF + 1); iCount++ )
	{	//Flat roofs
		InitDisplayGfxFromTileData(&SingleNewRoof[iCount], 9, 9, FIRSTROOF + iCount);
	}

	//Broken walls
	for ( iCount = 0; iCount < (LASTDECORATIONS - FIRSTDECORATIONS + 1); iCount++ )
	{	//Old obsolete wall decals, but should be replaced with multitiled decals such as banners, etc.
		InitDisplayGfxAllFromTileData(&SingleBrokenWall[iCount], FIRSTDECORATIONS + iCount);
	}
	for( iCount2 = 0; iCount2 < (LASTWALL - FIRSTWALL + 1); iCount2++, iCount++ )
	{	//Broken walls
		InitDisplayGfxFromTileData(&SingleBrokenWall[iCount], WALL_FIRST_BROKEN_WALL_OFFSET, WALL_LAST_BROKEN_WALL_OFFSET, FIRSTWALL + iCount2);
	}
	for( iCount2 = 0; iCount2 < (LASTWALL - FIRSTWALL + 1); iCount2++, iCount++ )
	{	//Cracked and smudged walls
		InitDisplayGfxFromTileData(&SingleBrokenWall[iCount], WALL_FIRST_WEATHERED_WALL_OFFSET, WALL_LAST_WEATHERED_WALL_OFFSET, FIRSTWALL + iCount2);
	}

	// Decorations
	for ( iCount = 0; iCount < (LASTISTRUCT - FIRSTISTRUCT + 1); iCount++ )
	{
		InitDisplayGfxAllFromTileData(&SingleDecor[iCount], FIRSTISTRUCT + iCount);
	}

	// Wall decals
	for ( iCount = 0; iCount < (LASTWALLDECAL - FIRSTWALLDECAL + 1); iCount++ )
	{
		InitDisplayGfxAllFromTileData(&SingleDecal[iCount], FIRSTWALLDECAL + iCount);
	}
	for ( iCount2 = 0; iCount2 < (EIGTHWALLDECAL - FIFTHWALLDECAL + 1); iCount++, iCount2++ )
	{
		InitDisplayGfxAllFromTileData(&SingleDecal[iCount], FIFTHWALLDECAL + iCount2);
	}
	InitDisplayGfxAllFromTileData(&SingleDecal[iCount], FIRSTSWITCHES);

	//Floors
	for ( iCount = 0; iCount < (LASTFLOOR - FIRSTFLOOR + 1); iCount++ )
	{
		InitDisplayGfxFromTileData(&SingleFloor[iCount], 0, 7, FIRSTFLOOR + iCount);
	}

	//Toilets
	for ( iCount = 0; iCount < (EIGHTISTRUCT - FIFTHISTRUCT + 1); iCount++ )
	{
		InitDisplayGfxAllFromTileData(&SingleToilet[iCount], FIFTHISTRUCT + iCount);
	}
}


static DisplayList* TrashList(DisplayList* pNode);


//----------------------------------------------------------------------------------------------
//	ShutdownJA2SelectionWindow
//
//	Unloads selection window button images and makes sure any display list that may remain in memory
//	is removed.
//
void ShutdownJA2SelectionWindow( void )
{
	INT16 x;

	for (x = 0; x < 4; x++)
		UnloadGenericButtonIcon( (INT16)iButtonIcons[x] );

	if (pDispList != NULL)
	{
		pDispList = TrashList(pDispList);
	}
	gfRenderWorld = TRUE;
}


//----------------------------------------------------------------------------------------------
//	RemoveJA2SelectionWindow
//
//	Removes the selection window from the screen.
//
void RemoveJA2SelectionWindow( void )
{
	RemoveButton(iSelectWin);
	RemoveButton(iCancelWin);
	RemoveButton(iScrollUp);
	RemoveButton(iScrollDown);
	RemoveButton(iOkWin);

	gfRenderSquareArea = FALSE;

	if (pDispList != NULL)
	{
		pDispList = TrashList(pDispList);
	}
	gfRenderTaskbar = TRUE;

	gfOverheadMapDirty = TRUE;
	EnableEditorTaskbar( );
}


//	Free the current display list for the selection window.
static DisplayList* TrashList(DisplayList* pNode)
{
	if (pNode == NULL)
		return(NULL);

	if (pNode->pNext != NULL)
		pNode->pNext = TrashList(pNode->pNext);

	if (pNode->pNext == NULL)
		MemFree(pNode);

	return(NULL);
}


static void DrawSelections(void);


//----------------------------------------------------------------------------------------------
//	RenderSelectionWindow
//
//	Displays the current selection window
//
void RenderSelectionWindow( void )
{
	INT32 iSX,iSY,iEX,iEY;
	UINT16 usFillColor;
	static UINT8 usFillGreen = 0;
	static UINT8 usDir = 5;


	if (!fButtonsPresent)
		return;

	ColorFillVideoSurfaceArea(FRAME_BUFFER, 0, 0, SCREEN_WIDTH - 40, TASKBAR_Y, GetGenericButtonFillColor());
	DrawSelections( );
	MarkButtonsDirty();
	RenderButtons( );

	if ( gfRenderSquareArea )
	{
		GUIButtonRef const button = iSelectWin;
		if (!button) return;

		if ( (abs( iStartClickX - button->Area.MouseXPos ) > 9) ||
			   (abs( iStartClickY - (button->Area.MouseYPos + iTopWinCutOff - (INT16)SelWinStartPoint.iY)) > 9) )
		{
//			iSX = (INT32)iStartClickX;
//			iEX = (INT32)button->Area.MouseXPos;
//			iSY = (INT32)iStartClickY;
//			iEY = (INT32)(button->Area.MouseYPos + iTopWinCutOff - (INT16)SelWinStartPoint.iY);

			iSX = iStartClickX;
			iSY = iStartClickY - iTopWinCutOff + SelWinStartPoint.iY;
			iEX = gusMouseXPos;
			iEY = gusMouseYPos;


			if (iEX < iSX)
			{
				iEX ^= iSX;
				iSX ^= iEX;
				iEX ^= iSX;
			}

			if (iEY < iSY)
			{
				iEY ^= iSY;
				iSY ^= iEY;
				iEY ^= iSY;
			}

			iEX = MIN( iEX, 600 );
			iSY = MAX( SelWinStartPoint.iY, iSY );
			iEY = MIN( 359, iEY );
			iEY = MAX( SelWinStartPoint.iY, iEY );

			usFillColor = Get16BPPColor(FROMRGB(255, usFillGreen, 0));
			usFillGreen += usDir;
			if( usFillGreen > 250 )
				usDir = 251;
			else if( usFillGreen < 5 )
				usDir = 5;

			ColorFillVideoSurfaceArea(FRAME_BUFFER, iSX, iSY, iEX, iSY+1, usFillColor );
			ColorFillVideoSurfaceArea(FRAME_BUFFER, iSX, iEY, iEX, iEY+1, usFillColor );
			ColorFillVideoSurfaceArea(FRAME_BUFFER, iSX, iSY, iSX+1, iEY, usFillColor );
			ColorFillVideoSurfaceArea(FRAME_BUFFER, iEX, iSY, iEX+1, iEY, usFillColor );
		}
	}
}


static void AddToSelectionList(DisplayList* pNode);
static BOOLEAN RemoveFromSelectionList(DisplayList* pNode);


//	Button callback function for the main selection window. Checks if user clicked on an image,
//	if so selects or de-selects that object. Also handles the multi-object selection (left-click
//	and drag to get the selection rectangle)
static void SelWinClkCallback(GUI_BUTTON* button, INT32 reason)
{
	DisplayList *pNode;
	BOOLEAN fDone;
	INT16 iClickX,iClickY, iYInc, iXInc;


	if (!(button->uiFlags & BUTTON_ENABLED))
		return;

	iClickX = button->Area.MouseXPos;
	iClickY = button->Area.MouseYPos + iTopWinCutOff - (INT16)SelWinStartPoint.iY;

	if (reason & MSYS_CALLBACK_REASON_LBUTTON_DWN)
	{
		button->uiFlags |= BUTTON_CLICKED_ON;
		iStartClickX = iClickX;
		iStartClickY = iClickY;
		gfRenderSquareArea = TRUE;
	}
	else if (reason & MSYS_CALLBACK_REASON_RBUTTON_DWN)
	{
		button->uiFlags |= BUTTON_CLICKED_ON;

		if ( gfRenderSquareArea )
		{
			gfRenderSquareArea = FALSE;
			return;
		}

		// Code to figure out what image user wants goes here
		pNode = pDispList;

		fDone = FALSE;
		while( (pNode != NULL) && !fDone )
		{
			if ( (iClickX >= pNode->iX) && (iClickX < (pNode->iX + pNode->iWidth)) &&
					(iClickY >= pNode->iY) && (iClickY < (pNode->iY + pNode->iHeight)) )
			{
				fDone = TRUE;
				if ( RemoveFromSelectionList(pNode) )
					pNode->fChosen = FALSE;
			}
			else
				pNode = pNode->pNext;
		}
	}
	else if (reason & MSYS_CALLBACK_REASON_RBUTTON_UP )
	{
		button->uiFlags &= (~BUTTON_CLICKED_ON);
	}
	else if (reason & MSYS_CALLBACK_REASON_LBUTTON_UP )
	{
		button->uiFlags &= (~BUTTON_CLICKED_ON);

		if( !gfRenderSquareArea )
			return;

		iEndClickX = iClickX;
		iEndClickY = iClickY;

		gfRenderSquareArea = FALSE;

		if( iEndClickX < iStartClickX )
		{
			iStartClickX ^= iEndClickX;
			iEndClickX ^= iStartClickX;
			iStartClickX ^= iEndClickX;
		}

		if( iEndClickY < iStartClickY )
		{
			iStartClickY ^= iEndClickY;
			iEndClickY ^= iStartClickY;
			iStartClickY ^= iEndClickY;
		}

		iXInc = iYInc = 1;
		for( iClickY = iStartClickY; iClickY <= iEndClickY; iClickY += iYInc )
		{
			iYInc = 1;
			for( iClickX = iStartClickX; iClickX <= iEndClickX; iClickX += iXInc )
			{
				iXInc = 1;
				// Code to figure out what image user wants goes here
				pNode = pDispList;

				fDone = FALSE;
				while( (pNode != NULL) && !fDone )
				{
					if ( (iClickX >= pNode->iX) && (iClickX < (pNode->iX + pNode->iWidth)) &&
							(iClickY >= pNode->iY) && (iClickY < (pNode->iY + pNode->iHeight)) )
					{
						fDone = TRUE;
						AddToSelectionList(pNode);
						pNode->fChosen = TRUE;
						iXInc = (pNode->iX + pNode->iWidth) - iClickX;
						if ( iYInc < ((pNode->iY + pNode->iHeight) - iClickY) )
							iYInc = (pNode->iY + pNode->iHeight) - iClickY;
					}
					else
						pNode = pNode->pNext;
				}
			}
		}
	}
	else if (reason & MSYS_CALLBACK_REASON_WHEEL_UP)
	{
		ScrollSelWinUp();
	}
	else if (reason & MSYS_CALLBACK_REASON_WHEEL_DOWN)
	{
		ScrollSelWinDown();
	}
}

//When a selection window is up, the file information of the picture will display
//at the top of the screen.
void DisplaySelectionWindowGraphicalInformation()
{
	DisplayList *pNode;
	BOOLEAN fDone;
	//UINT16 usObjIndex, usIndex;
	UINT16 y;
	//Determine if there is a valid picture at cursor position.
	//iRelX = gusMouseXPos;
	//iRelY = gusMouseYPos + iTopWinCutOff - (INT16)SelWinStartPoint.iY;

	y = gusMouseYPos + iTopWinCutOff - (UINT16)SelWinStartPoint.iY;
	pNode = pDispList;
	fDone = FALSE;
	while( (pNode != NULL) && !fDone )
	{
		if ( (gusMouseXPos >= pNode->iX) && (gusMouseXPos < (pNode->iX + pNode->iWidth)) &&
				(y >= pNode->iY) && (y < (pNode->iY + pNode->iHeight)) )
		{
			fDone = TRUE;
			//pNode->fChosen = TRUE;
			//iXInc = (pNode->iX + pNode->iWidth) - iClickX;
			//if ( iYInc < ((pNode->iY + pNode->iHeight) - iClickY) )
			//	iYInc = (pNode->iY + pNode->iHeight) - iClickY;
		}
		else
			pNode = pNode->pNext;
	}
	SetFont( FONT12POINT1 );
	SetFontForeground( FONT_WHITE );
	if( pNode )
	{
		//usObjIndex = (UINT16)pNode->uiObjIndx;
		//usIndex = pNode->uiIndex;
		if( !gTilesets[ giCurrentTilesetID].TileSurfaceFilenames[ pNode->uiObjIndx ][0] )
		{
			mprintf(2, 2, L"%hs[%d] is from default tileset %ls (%hs)",
				gTilesets[0].TileSurfaceFilenames[ pNode->uiObjIndx ],
				pNode->uiIndex, gTilesets[0].zName,
				gTileSurfaceName[ pNode->uiObjIndx ] );
		}
		else
		{
			mprintf(2, 2, L"File:  %hs, subindex:  %d (%hs)",
				gTilesets[ giCurrentTilesetID ].TileSurfaceFilenames[ pNode->uiObjIndx ],
				pNode->uiIndex, gTileSurfaceName[ pNode->uiObjIndx ] );
		}
	}
	mprintf(350, 2, L"Current Tileset:  %ls", gTilesets[giCurrentTilesetID].zName);
}

//----------------------------------------------------------------------------------------------
//	AddToSelectionList
//
//	Add an object in the display list to the selection list. If the object already exists in the
//	selection list, then it's count is incremented.
//
static void AddToSelectionList(DisplayList* pNode)
{
	for (INT32 iIndex = 0; iIndex < *pNumSelList; ++iIndex)
	{
		if ( pNode->uiObjIndx == pSelList[ iIndex ].uiObject &&
				 pNode->uiIndex == pSelList[ iIndex ].usIndex )
		{
			// Was already in the list, so bump up the count
			++pSelList[iIndex].sCount;
			return;
		}
	}

	// Wasn't in the list, so add to end (if space available)
	if ( (*pNumSelList) < MAX_SELECTIONS )
	{
		pSelList[ (*pNumSelList) ].uiObject = pNode->uiObjIndx;
		pSelList[ (*pNumSelList) ].usIndex = pNode->uiIndex;
		pSelList[ (*pNumSelList) ].sCount = 1;

		(*pNumSelList)++;
	}
}



//----------------------------------------------------------------------------------------------
//	ClearSelectionList
//
//	Removes everything from the current selection list
//
BOOLEAN ClearSelectionList( void )
{
	INT32 iIndex;
	DisplayList *pNode;


	if (pNumSelList == NULL )
		return( FALSE );

	pNode = pDispList;
	while ( pNode != NULL )
	{
		pNode->fChosen = FALSE;
		pNode = pNode->pNext;
	}

	for (iIndex = 0; iIndex < (*pNumSelList); iIndex++ )
		pSelList[ iIndex ].sCount = 0;

	(*pNumSelList) = 0;
	return( TRUE );
}


//	Removes the object given n a display list from the selection list. If the objects count is
//	greater than one, then the count is decremented and the object remains in the list.
static BOOLEAN RemoveFromSelectionList(DisplayList* pNode)
{
	// Abort if no entries in list (pretend we removed a node)
	if ( (*pNumSelList) <= 0 )
		return( TRUE );

	for (INT32 iIndex = 0; iIndex < *pNumSelList; ++iIndex)
	{
		if ( pNode->uiObjIndx == pSelList[ iIndex ].uiObject &&
				 pNode->uiIndex == pSelList[ iIndex ].usIndex )
		{
			if (--pSelList[iIndex].sCount <= 0)
			{
				// Squash the list to remove old entry
				for ( iIndex = iIndex; iIndex < ((*pNumSelList) - 1); iIndex++ )
					pSelList[ iIndex ] = pSelList[ iIndex + 1 ];

				(*pNumSelList)--;
				return TRUE;
			}
			break;
		}
	}

	return FALSE;
}


//----------------------------------------------------------------------------------------------
//	GetRandomSelection
//
//	Randomly selects an item in the selection list. The object counts are taken into account so
//	that objects with higher counts will be chosen more often.
//
INT32 GetRandomSelection( void )
{
	INT32 iRandNum, iTotalCounts;
	INT32 iIndex, iSelectedIndex, iNextCount;

	if ( fDontUseRandom )
	{
		fDontUseRandom = FALSE;
		return( iCurBank );
	}

	iTotalCounts = 0;
	for (iIndex = 0; iIndex < (*pNumSelList); iIndex++ )
		iTotalCounts += (INT32)pSelList[ iIndex ].sCount;

	iRandNum = Random( iTotalCounts );

	iSelectedIndex = -1;
	iNextCount = 0;
	for (iIndex = 0; iIndex < (*pNumSelList) && iSelectedIndex == -1; iIndex++ )
	{
		iNextCount += (INT32)pSelList[ iIndex ].sCount;
		if ( iRandNum < iNextCount )
			iSelectedIndex = iIndex;
	}

	return ( iSelectedIndex );
}


//	Verifies if a particular display list object exists in the current selection list.
static BOOLEAN IsInSelectionList(const DisplayList* pNode)
{
	for (INT32 iIndex = 0; iIndex < *pNumSelList; iIndex++)
	{
		if (pNode->uiObjIndx == pSelList[iIndex].uiObject &&
				pNode->uiIndex   == pSelList[iIndex].usIndex)
		{
			return TRUE;
		}
	}

	return FALSE;
}


//	Finds an occurance of a particular display list object in the current selection list.
//	if found, returns the selection list's index where it can be found. otherwise it
//	returns -1
static INT32 FindInSelectionList(DisplayList* pNode)
{
	INT32 iIndex,iUseIndex;
	BOOLEAN fFound;

	fFound = FALSE;
	iUseIndex = -1;
	for (iIndex = 0; iIndex < (*pNumSelList) && !fFound; iIndex++ )
	{
		if ( pNode->uiObjIndx == pSelList[ iIndex ].uiObject &&
				 pNode->uiIndex == pSelList[ iIndex ].usIndex )
		{
			fFound = TRUE;
			iUseIndex = iIndex;
		}
	}

	return ( iUseIndex );
}


//	Copies the current selection list to a save buffer. Used in case we want to cancel a
//	selection window.
static void SaveSelectionList(void)
{
	INT32 iIndex;

	for (iIndex = 0; iIndex < MAX_SELECTIONS; iIndex++ )
		OldSelList[ iIndex ] = pSelList[ iIndex ];

	iOldNumSelList = (*pNumSelList);
}


//----------------------------------------------------------------------------------------------
//	RestoreSelectionList
//
//	Copies the selection list in the save buffer back to the current selection list.
//
void RestoreSelectionList( void )
{
	INT32 iIndex;

	for (iIndex = 0; iIndex < MAX_SELECTIONS; iIndex++ )
		pSelList[ iIndex ] = OldSelList[ iIndex ];

	(*pNumSelList) = iOldNumSelList;
}


//	Button callback function for the selection window's OK button
static void OkClkCallback(GUI_BUTTON* button, INT32 reason)
{
	if (reason & MSYS_CALLBACK_REASON_LBUTTON_UP)
	{
		fAllDone = TRUE;
	}
}


//----------------------------------------------------------------------------------------------
//	CnclClkCallback
//
//	Button callback function for the selection window's CANCEL button
//
static void CnclClkCallback(GUI_BUTTON* button, INT32 reason)
{
	if (reason & MSYS_CALLBACK_REASON_LBUTTON_UP)
	{
		fAllDone = TRUE;
		RestoreSelectionList();
	}
}


//	Button callback function for scrolling the selection window up
static void UpClkCallback(GUI_BUTTON* button, INT32 reason)
{
	if( reason & MSYS_CALLBACK_REASON_LBUTTON_DWN )
	{
		button->uiFlags |= BUTTON_CLICKED_ON;
	}
	else if (reason & MSYS_CALLBACK_REASON_LBUTTON_UP )
	{
		button->uiFlags &= (~BUTTON_CLICKED_ON );
		ScrollSelWinUp();
	}
}


//----------------------------------------------------------------------------------------------
//	ScrollSelWinUp
//
//	Performs the calculations required to actually scroll a selection window up by one line.
//
void ScrollSelWinUp(void)
{
	DisplayList *pNode;
	INT16 iCutOff;
	BOOLEAN fDone;

	// Code to scroll window up!
	pNode = pDispList;
	iCutOff = iTopWinCutOff;

	fDone = FALSE;
	while( (pNode != NULL) && !fDone )
	{
		if (pNode->iY >= iTopWinCutOff)
		{
			iCutOff = pNode->iY;
			pNode = pNode->pNext;
		}
		else
		{
			iCutOff = pNode->iY;
			fDone = TRUE;
		}
	}

	iTopWinCutOff = iCutOff;
}


//----------------------------------------------------------------------------------------------
//	ScrollSelWinDown
//
//	Performs the actual calculations for scrolling a selection window down.
//
void ScrollSelWinDown(void)
{
	DisplayList *pNode;
	INT16 iCutOff;
	BOOLEAN fDone;

	pNode = pDispList;
	iCutOff = iTopWinCutOff;

	fDone = FALSE;
	while( (pNode != NULL) && !fDone )
	{
		if (pNode->iY > iTopWinCutOff)
		{
			iCutOff = pNode->iY;
			pNode = pNode->pNext;
		}
		else
			fDone = TRUE;
	}

	iTopWinCutOff = iCutOff;
}


//	Button callback function to scroll the selection window down.
static void DwnClkCallback(GUI_BUTTON* button, INT32 reason)
{
	if (reason & MSYS_CALLBACK_REASON_LBUTTON_DWN)
	{
		button->uiFlags |= BUTTON_CLICKED_ON;
	}
	else if( reason & MSYS_CALLBACK_REASON_LBUTTON_UP )
	{
		button->uiFlags &= (~BUTTON_CLICKED_ON);
		ScrollSelWinDown();
	}
}


static BOOLEAN DisplayWindowFunc(DisplayList* pNode, INT16 iTopCutOff, INT16 iBottomCutOff, SGPPoint* pUpperLeft, UINT16 fFlags);


//	Displays the objects in the display list to the selection window.
static void DrawSelections(void)
{
	SGPRect					ClipRect, NewRect;

	NewRect.iLeft = SelWinStartPoint.iX;
	NewRect.iTop = SelWinStartPoint.iY;
	NewRect.iRight = SelWinEndPoint.iX;
	NewRect.iBottom = SelWinEndPoint.iY;

	GetClippingRect(&ClipRect);
	SetClippingRect(&NewRect);

	SetFont( gpLargeFontType1 );
	SetFontShade(LARGEFONT1, FONT_SHADE_GREY_165);

	DisplayWindowFunc(pDispList, iTopWinCutOff, SelWinEndPoint.iY, &SelWinStartPoint, CLEAR_BACKGROUND);

	SetFontShade(LARGEFONT1, FONT_SHADE_NEUTRAL);

	SetClippingRect(&ClipRect);
}


/* Create a display list from a display specification list.  Also set variables
 * up for properly scrolling the window etc. */
static BOOLEAN BuildDisplayWindow(DisplaySpec const* const pDisplaySpecs, UINT16 const usNumSpecs, DisplayList** const pDisplayList, SGPPoint const* const pUpperLeft, SGPPoint const* const pBottomRight, SGPPoint const* const pSpacing, DisplayWindowFlags const flags)
try
{
	SaveSelectionList();

	INT32  x     = pUpperLeft->iX;
	INT32  y     = pUpperLeft->iY;
	UINT16 max_h = 0; // Maximum height in current row
	for (DisplaySpec const* ds = pDisplaySpecs; ds != pDisplaySpecs + usNumSpecs; ++ds)
	{
		if (ds->ubType != DISPLAY_GRAPHIC) continue;

		SGPVObject* const vo = ds->hVObject;
		if (!vo) return FALSE;

		UINT16 usETRLEStart = ds->usStart;
		UINT16 usETRLEEnd   = ds->usEnd;
		if (usETRLEStart == DISPLAY_ALL_OBJECTS)
		{
			usETRLEStart = 0;
			usETRLEEnd   = vo->SubregionCount() - 1;
		}

		if (usETRLEStart         >  usETRLEEnd) return FALSE;
		if (vo->SubregionCount() <= usETRLEEnd) return FALSE;

		for (UINT16 usETRLELoop = usETRLEStart; usETRLELoop <= usETRLEEnd; ++usETRLELoop)
		{
			ETRLEObject const* const e = vo->SubregionProperties(usETRLELoop);

			if (x + e->usWidth > pBottomRight->iX || flags & ONE_COLUMN)
			{
				if (flags & ONE_ROW) break;
				x  = pUpperLeft->iX;
				y += max_h + pSpacing->iY;
				max_h = 0;
			}

			DisplayList* const n = MALLOC(DisplayList);
			n->hObj      = vo;
			n->uiIndex   = usETRLELoop;
			n->iX        = x;
			n->iY        = y;
			n->iWidth    = e->usWidth;
			n->iHeight   = e->usHeight;
			n->pNext     = *pDisplayList;
			n->uiObjIndx = ds->uiObjIndx;
			n->fChosen   = IsInSelectionList(n);

			*pDisplayList = n;

			if (max_h < e->usHeight) max_h = e->usHeight;

			x += e->usWidth + pSpacing->iX;
		}
	}

	return TRUE;
}
catch (...) { return FALSE; }


//----------------------------------------------------------------------------------------------
//	DisplayWindowFunc
//
//	Blits the actual object images in the display list on the selection window. The objects that
//	have been selected (in the selection list) are highlighted and the count placed in the upper
//	left corner of the image.
//
static BOOLEAN DisplayWindowFunc(DisplayList* pNode, INT16 iTopCutOff, INT16 iBottomCutOff, SGPPoint* pUpperLeft, UINT16 fFlags)
{
	INT16						iCurrY;
	BOOLEAN					fReturnVal;
	UINT16					usFillColor;
	INT16						sCount;

	if ( pNode == NULL )
		return(TRUE);

	if ( pNode->iY < iTopCutOff )
		return(TRUE);

	fReturnVal = FALSE;
	if (DisplayWindowFunc(pNode->pNext, iTopCutOff, iBottomCutOff, pUpperLeft, fFlags))
	{
		iCurrY = pUpperLeft->iY + pNode->iY - iTopCutOff;

		if ( iCurrY > iBottomCutOff )
			return(TRUE);

		if (fFlags & CLEAR_BACKGROUND)
		{
			usFillColor = SelWinFillColor;
			if (pNode->fChosen)
				usFillColor = SelWinHilightFillColor;

			ColorFillVideoSurfaceArea( FRAME_BUFFER, pNode->iX, iCurrY,
												pNode->iX + pNode->iWidth,
												iCurrY + pNode->iHeight, usFillColor);
		}

		sCount = 0;
		if (pNode->fChosen)
			sCount = pSelList[ FindInSelectionList( pNode ) ].sCount;

		SGPVObject*        const vo = pNode->hObj;
		ETRLEObject const* const e  = vo->SubregionProperties(pNode->uiIndex);
		vo->CurrentShade(DEFAULT_SHADE_LEVEL);
		fReturnVal = BltVideoObject(FRAME_BUFFER, vo, pNode->uiIndex, pNode->iX - e->sOffsetX, iCurrY - e->sOffsetY);

		if ( sCount != 0)
		{
			gprintf( pNode->iX, iCurrY, L"%d", sCount );
		}
	}

	return(fReturnVal);
}





#endif
