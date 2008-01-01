#ifndef _MAP_SCREEN_INTERFACE_MAP_H
#define _MAP_SCREEN_INTERFACE_MAP_H

#include "JA2Types.h"


// functions
void DrawMapIndexBigMap( BOOLEAN fSelectedCursorIsYellow );
//void DrawMapIndexSmallMap( BOOLEAN fSelectedCursorIsYellow );

UINT32 DrawMap( void );

void GetScreenXYFromMapXY( INT16 sMapX, INT16 sMapY, INT16 *psX, INT16 *psY );

BOOLEAN InitializePalettesForMap( void );
void ShutDownPalettesForMap( void );

// plot path for helicopter
void PlotPathForHelicopter( INT16 sX, INT16 sY );

// the temp path, where the helicopter could go
void PlotATemporaryPathForHelicopter( INT16 sX, INT16 sY );

// show arrows for this char
void DisplayPathArrows(UINT16 usCharNum, HVOBJECT hMapHandle);

// build path for character
void PlotPathForCharacter( SOLDIERTYPE *pCharacter, INT16 sX, INT16 sY, BOOLEAN fTacticalTraversal );

// build temp path for character
void PlotATemporaryPathForCharacter(const SOLDIERTYPE* s, INT16 sX, INT16 sY);


// display current/temp paths
void DisplaySoldierPath( SOLDIERTYPE *pCharacter );
void DisplaySoldierTempPath(void);
void DisplayHelicopterPath( void );
void DisplayHelicopterTempPath( void );


// clear path after this sector
UINT32 ClearPathAfterThisSectorForCharacter( SOLDIERTYPE *pCharacter, INT16 sX, INT16 sY );

// cancel path : clear the path completely and gives player feedback message that the route was canceled
void CancelPathForCharacter( SOLDIERTYPE *pCharacter );
void CancelPathForVehicle( VEHICLETYPE *pVehicle, BOOLEAN fAlreadyReversed );

// check if we have waited long enought o update temp path
void DisplayThePotentialPathForHelicopter(INT16 sMapX, INT16 sMapY );

// clear out helicopter list after this sector
UINT32 ClearPathAfterThisSectorForHelicopter( INT16 sX, INT16 sY );


// check to see if sector is highlightable
BOOLEAN IsTheCursorAllowedToHighLightThisSector( INT16 sSectorX, INT16 sSectorY );

// set bad sectors list
void SetUpBadSectorsList( void );

// restore background for map grids
void RestoreBackgroundForMapGrid( INT16 sMapX, INT16 sMapY );

// clip blits to map view region
void ClipBlitsToMapViewRegion( void );
void ClipBlitsToMapViewRegionForRectangleAndABit( UINT32 uiDestPitchBYTES );

// clip blits to full screen....restore after use of ClipBlitsToMapViewRegion( )
void RestoreClipRegionToFullScreen( void );
void RestoreClipRegionToFullScreenForRectangle( UINT32 uiDestPitchBYTES );

// last sector in helicopter's path
INT16 GetLastSectorOfHelicoptersPath( void );

// display info about helicopter path
void DisplayDistancesForHelicopter( void );

// display where hei is
void DisplayPositionOfHelicopter( void );

// check for click
BOOLEAN CheckForClickOverHelicopterIcon( INT16 sX, INT16 sY );

// load and delete the graphics for the map screen map militia
BOOLEAN LoadMilitiaPopUpBox( void );
void RemoveMilitiaPopUpBox( void );


// check if anyone left behind, if not, move selected cursor along with movement group
//void CheckIfAnyoneLeftInSector( INT16 sX, INT16 sY, INT16 sNewX, INT16 sNewY, INT8 bZ );

// grab the total number of militia in sector
INT32 GetNumberOfMilitiaInSector( INT16 sSectorX, INT16 sSectorY, INT8 bSectorZ );


// create destroy
void CreateDestroyMilitiaPopUPRegions( void );

// draw the militia box
BOOLEAN DrawMilitiaPopUpBox( void );


//Returns true if the player knows how many enemies are in the sector if that number is greater than 0.
//Returns false for all other cases.
UINT32 WhatPlayerKnowsAboutEnemiesInSector( INT16 sSectorX, INT16 sSectorY );

//There is a special case flag used when players encounter enemies in a sector, then retreat.  The number of enemies
//will display on mapscreen until time is compressed.  When time is compressed, the flag is cleared, and
//a question mark is displayed to reflect that the player no longer knows.  This is the function that clears that
//flag.
void ClearAnySectorsFlashingNumberOfEnemies(void);

void InitMapSecrets( void );



enum {
	ABORT_PLOTTING = 0,
	PATH_CLEARED,
	PATH_SHORTENED,
};

// what the player knows about the enemies in a given sector
enum {
	KNOWS_NOTHING = 0,
	KNOWS_THEYRE_THERE,
	KNOWS_HOW_MANY,
};


// size of squares on the map
#define			MAP_GRID_X						21
#define			MAP_GRID_Y						18


// scroll bounds
#define EAST_ZOOM_BOUND 378
#define WEST_ZOOM_BOUND 42
#define SOUTH_ZOOM_BOUND 324
#define NORTH_ZOOM_BOUND 36

// map view region
#define			MAP_VIEW_START_X			270
#define			MAP_VIEW_START_Y			10
#define			MAP_VIEW_WIDTH				336
#define			MAP_VIEW_HEIGHT				298

// zoomed in grid sizes
#define     MAP_GRID_ZOOM_X       MAP_GRID_X*2
#define     MAP_GRID_ZOOM_Y       MAP_GRID_Y*2

// number of units wide
#define     WORLD_MAP_X           18

// dirty regions for the map
#define			DMAP_GRID_X						( MAP_GRID_X + 1 )
#define			DMAP_GRID_Y						( MAP_GRID_Y + 1 )
#define     DMAP_GRID_ZOOM_X      ( MAP_GRID_ZOOM_X+1)
#define     DMAP_GRID_ZOOM_Y      ( MAP_GRID_ZOOM_Y+1)


// Orta position on the map
#define ORTA_SECTOR_X 4
#define ORTA_SECTOR_Y 11

#define TIXA_SECTOR_X 9
#define TIXA_SECTOR_Y 10

// what are we showing?..teams/vehicles
// Show values
#define SHOW_TEAMMATES 1
#define SHOW_VEHICLES  2


// wait time until temp path is drawn, from placing cursor on a map grid
#define MIN_WAIT_TIME_FOR_TEMP_PATH 200


// zoom UL coords
extern INT32 iZoomX;
extern INT32 iZoomY;

// the number of militia on the cursor
extern INT16 sGreensOnCursor;
extern INT16 sRegularsOnCursor;
extern INT16 sElitesOnCursor;

// highlighted sectors
extern INT16 gsHighlightSectorX;
extern INT16 gsHighlightSectorY;

#ifndef JA2DEMO
// the big map
extern SGPVSurface* guiBIGMAP;
#endif

// the orta icon
extern SGPVObject* guiORTAICON;

extern SGPVObject* guiTIXAICON;

// the character icons
extern SGPVObject* guiCHARICONS;

#ifndef JA2DEMO
// the merc arrival sector landing zone icon
extern SGPVObject* guiBULLSEYE;
#endif

// character between sector icons
extern SGPVObject* guiCHARBETWEENSECTORICONS;
extern SGPVObject* guiCHARBETWEENSECTORICONSCLOSE;

// the viewable map bound region
extern SGPRect MapScreenRect;

// draw temp path
extern BOOLEAN fDrawTempHeliPath;

// selected destination char
extern INT8 bSelectedDestChar;

// current assignment character
extern INT8 bSelectedAssignChar;

// the info character
extern INT8 bSelectedInfoChar;

// the contract char
extern INT8 bSelectedContractChar;

// map arrows graphical index value
extern SGPVObject* guiMAPCURSORS;

// has temp path for character path or helicopter been already drawn
extern BOOLEAN  fTempPathAlreadyDrawn;

// the map border eta pop up
extern SGPVObject* guiMapBorderEtaPopUp;

// heli pop up
extern SGPVObject* guiMapBorderHeliSectors;

// the currently selected town militia
extern INT16 sSelectedMilitiaTown;

// the selected sectors
extern UINT16	sSelMapX;
extern UINT16	sSelMapY;


extern BOOLEAN fFoundTixa;

extern SGPVObject* guiSubLevel1;
extern SGPVObject* guiSubLevel2;
extern SGPVObject* guiSubLevel3;

void    CreateDestroyMilitiaSectorButtons(void);
BOOLEAN CanRedistributeMilitiaInSector(INT16 sClickedSectorX, INT16 sClickedSectorY, INT8 bClickedTownId);

extern SGPVObject* guiMINEICON;

#endif
