#ifndef __ISOMETRIC_UTILSH
#define __ISOMETRIC_UTILSH

#include "WorldDef.h"


#define MAXCOL					WORLD_COLS
#define MAXROW					WORLD_ROWS
#define GRIDSIZE        (MAXCOL * MAXROW)
#define RIGHTMOSTGRID   (MAXCOL - 1)
#define LASTROWSTART    (GRIDSIZE - MAXCOL)
#define NOWHERE         (GRIDSIZE + 1)
#define	NO_MAP_POS			NOWHERE
#define MAPWIDTH			(WORLD_COLS)
#define MAPHEIGHT			(WORLD_ROWS)
#define MAPLENGTH			(MAPHEIGHT*MAPWIDTH)


#define	ADJUST_Y_FOR_HEIGHT( pos, y )				( y -= gpWorldLevelData[ pos ].sHeight )


static inline UINT8 OppositeDirection(UINT dir) { return (dir + 4) % NUM_WORLD_DIRECTIONS; }
static inline UINT8 TwoCCDirection(UINT dir)    { return (dir + 6) % NUM_WORLD_DIRECTIONS; }
static inline UINT8 TwoCDirection(UINT dir)     { return (dir + 2) % NUM_WORLD_DIRECTIONS; }
static inline UINT8 OneCCDirection(UINT dir)    { return (dir + 7) % NUM_WORLD_DIRECTIONS; }
static inline UINT8 OneCDirection(UINT dir)     { return (dir + 1) % NUM_WORLD_DIRECTIONS; }

extern const UINT8 gPurpendicularDirection[NUM_WORLD_DIRECTIONS][NUM_WORLD_DIRECTIONS];

// Macros


//                                                |Check for map bounds------------------------------------------|   |Invalid-|   |Valid-------------------|
#define MAPROWCOLTOPOS( r, c )									( ( (r < 0) || (r >= WORLD_ROWS) || (c < 0) || (c >= WORLD_COLS) ) ? ( 0xffff ) : ( (r) * WORLD_COLS + (c) ) )

#define GETWORLDINDEXFROMWORLDCOORDS( r, c )		( (INT16) ( r / CELL_X_SIZE ) ) * WORLD_COLS + ( (INT16) ( c / CELL_Y_SIZE ) )

void ConvertGridNoToXY( INT16 sGridNo, INT16 *sXPos, INT16 *sYPos );
void ConvertGridNoToCellXY( INT16 sGridNo, INT16 *sXPos, INT16 *sYPos );
void ConvertGridNoToCenterCellXY( INT16 sGridNo, INT16 *sXPos, INT16 *sYPos );


// GRID NO MANIPULATION FUNCTIONS
INT16 NewGridNo(INT16 sGridno, INT16 sDirInc);
INT16 DirectionInc(INT16 sDirection);
INT32 OutOfBounds(INT16 sGridno, INT16 sProposedGridno);


BOOLEAN GetMouseXY( INT16 *psMouseX, INT16 *psMouseY );
BOOLEAN GetMouseWorldCoords( INT16 *psMouseX, INT16 *psMouseY );

/* Returns the GridNo of the tile the mouse cursor is currently over or NOWHERE
 * if the cursor is not over any tile. */
GridNo GetMouseMapPos(void);


void GetWorldXYAbsoluteScreenXY( INT32 sWorldCellX, INT32 sWorldCellY, INT16 *psWorldScreenX, INT16 *psWorldScreenY );
GridNo GetMapPosFromAbsoluteScreenXY(INT16 sWorldScreenX, INT16 sWorldScreenY);


void FromCellToScreenCoordinates( INT16 sCellX, INT16 sCellY, INT16 *psScreenX, INT16 *psScreenY );
void FromScreenToCellCoordinates( INT16 sScreenX, INT16 sScreenY, INT16 *psCellX, INT16 *psCellY );

// Higher resolution convertion functions
void FloatFromCellToScreenCoordinates( FLOAT dCellX, FLOAT dCellY, FLOAT *pdScreenX, FLOAT *pdScreenY );

BOOLEAN GridNoOnVisibleWorldTile( INT16 sGridNo );
BOOLEAN GridNoOnEdgeOfMap( INT16 sGridNo, INT8 * pbDirection );

BOOLEAN CellXYToScreenXY(INT16 sCellX, INT16 sCellY, INT16 *sScreenX, INT16 *sScreenY);

INT32 GetRangeFromGridNoDiff( INT16 sGridNo1, INT16 sGridNo2 );
INT32 GetRangeInCellCoordsFromGridNoDiff( INT16 sGridNo1, INT16 sGridNo2 );

BOOLEAN IsPointInScreenRect( INT16 sXPos, INT16 sYPos, SGPRect *pRect );
BOOLEAN IsPointInScreenRectWithRelative( INT16 sXPos, INT16 sYPos, SGPRect *pRect, INT16 *sXRel, INT16 *sRelY );


INT16 PythSpacesAway(INT16 sOrigin, INT16 sDest);
INT16 SpacesAway(INT16 sOrigin, INT16 sDest);
INT16 CardinalSpacesAway(INT16 sOrigin, INT16 sDest);
BOOLEAN FindHeigherLevel(const SOLDIERTYPE* pSoldier, INT16 sGridNo, INT8 bStartingDir, INT8* pbDirection);
BOOLEAN FindLowerLevel(const SOLDIERTYPE* pSoldier, INT16 sGridNo, INT8 bStartingDir, INT8* pbDirection);

INT16 QuickestDirection(INT16 origin, INT16 dest);
INT16 ExtQuickestDirection(INT16 origin, INT16 dest);


// Returns the (center ) cell coordinates in X
INT16 CenterX( INT16 sGridno );

// Returns the (center ) cell coordinates in Y
INT16 CenterY( INT16 sGridno );

BOOLEAN FindFenceJumpDirection(const SOLDIERTYPE* pSoldier, INT16 sGridNo, INT8 bStartingDir, INT8* pbDirection);

//Simply chooses a random gridno within valid boundaries (for dropping things in unloaded sectors)
INT16 RandomGridNo(void);

extern UINT32 guiForceRefreshMousePositionCalculation;

extern const INT16 DirIncrementer[8];

#endif
