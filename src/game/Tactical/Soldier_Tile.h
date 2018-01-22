#ifndef __SOLDIER_TILE_H
#define __SOLDIER_TILE_H

#include "JA2Types.h"


#define			MOVE_TILE_CLEAR										1
#define			MOVE_TILE_TEMP_BLOCKED					 -1
#define			MOVE_TILE_STATIONARY_BLOCKED		 -2


void UnMarkMovementReserved( SOLDIERTYPE *pSoldier );

BOOLEAN HandleNextTile( SOLDIERTYPE *pSoldier, INT8 bDirection, INT16 sGridNo, INT16 sFinalDestTile );

void HandleNextTileWaiting(SOLDIERTYPE* pSoldier);

BOOLEAN TeleportSoldier( SOLDIERTYPE *pSoldier, INT16 sGridNo, BOOLEAN fForce );

void SwapMercPositions( SOLDIERTYPE *pSoldier1, SOLDIERTYPE *pSoldier2 );

void SetDelayedTileWaiting( SOLDIERTYPE *pSoldier, INT16 sCauseGridNo, INT8 bValue );

BOOLEAN CanExchangePlaces( SOLDIERTYPE *pSoldier1, SOLDIERTYPE *pSoldier2, BOOLEAN fShow );

#endif
