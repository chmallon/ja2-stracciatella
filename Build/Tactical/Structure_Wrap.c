#include "Debug.h"
#include "WCheck.h"
#include "WorldDef.h"
#include "WorldMan.h"
#include "Structure_Wrap.h"
#include "Isometric_Utils.h"
#include "Overhead.h"
#include "RenderWorld.h"
#include "StrategicMap.h"
#include "Rotting_Corpses.h"


extern BOOLEAN DoesSAMExistHere( INT16 sSectorX, INT16 sSectorY, INT16 sSectorZ, INT16 sGridNo );



BOOLEAN	IsFencePresentAtGridno( INT16 sGridNo )
{
	if ( FindStructure( sGridNo, STRUCTURE_ANYFENCE ) != NULL )
	{
		return( TRUE );
	}

	return( FALSE );
}

BOOLEAN	IsRoofPresentAtGridno( INT16 sGridNo )
{
	if ( FindStructure( sGridNo, STRUCTURE_ROOF ) != NULL )
	{
		return( TRUE );
	}

	return( FALSE );
}


BOOLEAN	IsJumpableFencePresentAtGridno( INT16 sGridNo )
{
	STRUCTURE * pStructure;

	pStructure = FindStructure( sGridNo, STRUCTURE_OBSTACLE );

	if ( pStructure )
	{
		if ( pStructure->fFlags & STRUCTURE_FENCE && !(pStructure->fFlags & STRUCTURE_SPECIAL) )
		{
			return( TRUE );
		}
		if ( pStructure->pDBStructureRef->pDBStructure->ubArmour == MATERIAL_SANDBAG && StructureHeight( pStructure ) < 2 )
		{
			return( TRUE );
		}
	}

	return( FALSE );
}


static BOOLEAN IsDoorPresentAtGridno(INT16 sGridNo)
{
	if ( FindStructure( sGridNo, STRUCTURE_ANYDOOR ) != NULL )
	{
		return( TRUE );
	}

	return( FALSE );
}


BOOLEAN	IsTreePresentAtGridno( INT16 sGridNo )
{
	if ( FindStructure( sGridNo, STRUCTURE_TREE ) != NULL )
	{
		return( TRUE );
	}

	return( FALSE );
}


static LEVELNODE* IsWallPresentAtGridno(INT16 sGridNo)
{
	LEVELNODE *pNode = NULL;
	STRUCTURE * pStructure;

	pStructure = FindStructure( sGridNo, STRUCTURE_WALLSTUFF );

	if ( pStructure != NULL )
	{
		pNode = FindLevelNodeBasedOnStructure( sGridNo, pStructure );
	}

	return( pNode );
}


// OK, THIS FUNCTION RETURNS A LEVELNODE POINTER TO A WALL OF THE SAME ORIENTATION
// AS WAS GIVEN. RETURNS NULL IF NONE FOUND.
static LEVELNODE* GetWallLevelNodeOfSameOrientationAtGridno(INT16 sGridNo, INT8 ubOrientation)
{
	LEVELNODE *pNode = NULL;
	STRUCTURE * pStructure;

	pStructure = FindStructure( sGridNo, STRUCTURE_WALLSTUFF );

	while ( pStructure != NULL )
	{
		// Check orientation
		if ( pStructure->ubWallOrientation == ubOrientation )
		{
			pNode = FindLevelNodeBasedOnStructure( sGridNo, pStructure );
			return( pNode );
		}
		pStructure = FindNextStructure( pStructure, STRUCTURE_WALLSTUFF );
	}

	return( NULL );
}


LEVELNODE	*GetWallLevelNodeAndStructOfSameOrientationAtGridno( INT16 sGridNo, INT8 ubOrientation, STRUCTURE **ppStructure )
{
	LEVELNODE *pNode = NULL;
	STRUCTURE * pStructure, * pBaseStructure;

	(*ppStructure) = NULL;

	pStructure = FindStructure( sGridNo, STRUCTURE_WALLSTUFF );

	while ( pStructure != NULL )
	{
		// Check orientation
		if ( pStructure->ubWallOrientation == ubOrientation )
		{
			pBaseStructure = FindBaseStructure( pStructure );
			if (pBaseStructure)
			{
				pNode = FindLevelNodeBasedOnStructure( pBaseStructure->sGridNo, pBaseStructure );
				(*ppStructure) = pBaseStructure;
				return( pNode );
			}
		}
		pStructure = FindNextStructure( pStructure, STRUCTURE_WALLSTUFF );
	}

	return( NULL );
}


BOOLEAN IsDoorVisibleAtGridNo( INT16 sGridNo )
{
	STRUCTURE * pStructure;
	INT16				sNewGridNo;

	pStructure = FindStructure( sGridNo, STRUCTURE_ANYDOOR );

	if ( pStructure != NULL )
	{
		// Check around based on orientation
		switch( pStructure->ubWallOrientation )
		{
			case INSIDE_TOP_LEFT:
			case OUTSIDE_TOP_LEFT:

				// Here, check north direction
				sNewGridNo = NewGridNo( sGridNo, DirectionInc( NORTH ) );

				if ( IsRoofVisible2( sNewGridNo ) )
				{
					// OK, now check south, if true, she's not visible
					sNewGridNo = NewGridNo( sGridNo, DirectionInc( SOUTH ) );

					if ( IsRoofVisible2( sNewGridNo ) )
					{
						return( FALSE );
					}
				}
				break;

			case INSIDE_TOP_RIGHT:
			case OUTSIDE_TOP_RIGHT:

				// Here, check west direction
				sNewGridNo = NewGridNo( sGridNo, DirectionInc( WEST ) );

				if ( IsRoofVisible2( sNewGridNo ) )
				{
					// OK, now check south, if true, she's not visible
					sNewGridNo = NewGridNo( sGridNo, DirectionInc( EAST ) );

					if ( IsRoofVisible2( sNewGridNo ) )
					{
						return( FALSE );
					}
				}
				break;

		}

	}

	// Return true here, even if she does not exist
	return( TRUE );
}


BOOLEAN DoesGridnoContainHiddenStruct( INT16 sGridNo, BOOLEAN *pfVisible )
{
	// ATE: These are ignored now - always return false

	//STRUCTURE *pStructure;

	//pStructure = FindStructure( sGridNo, STRUCTURE_HIDDEN );

	//if ( pStructure != NULL )
	//{
	//	if ( !(gpWorldLevelData[ sGridNo ].uiFlags & MAPELEMENT_REVEALED ) && !(gTacticalStatus.uiFlags&SHOW_ALL_MERCS)  )
	//	{
	//		*pfVisible = FALSE;
	//	}
	//	else
	//	{
	//		*pfVisible = TRUE;
	//	}//
//
	//	return( TRUE );
	//}

	return( FALSE );
}


BOOLEAN IsHiddenStructureVisible( INT16 sGridNo, UINT16 usIndex )
{
	// Check if it's a hidden struct and we have not revealed anything!
	if ( gTileDatabase[ usIndex ].uiFlags & HIDDEN_TILE )
	{
		if ( !(gpWorldLevelData[ sGridNo ].uiFlags & MAPELEMENT_REVEALED ) && !(gTacticalStatus.uiFlags&SHOW_ALL_MERCS) )
		{
			// Return false
			return( FALSE );
		}
	}

	return( TRUE );
}


BOOLEAN	WallExistsOfTopLeftOrientation( INT16 sGridNo )
{
	// CJC: changing to search only for normal walls, July 16, 1998
	STRUCTURE * pStructure;

	pStructure = FindStructure( sGridNo, STRUCTURE_WALL );

	while ( pStructure != NULL )
	{
		// Check orientation
		if ( pStructure->ubWallOrientation == INSIDE_TOP_LEFT || pStructure->ubWallOrientation == OUTSIDE_TOP_LEFT )
		{
			return( TRUE );
		}

		pStructure = FindNextStructure( pStructure, STRUCTURE_WALL );

	}

	return( FALSE );
}

BOOLEAN	WallExistsOfTopRightOrientation( INT16 sGridNo )
{
	// CJC: changing to search only for normal walls, July 16, 1998
	STRUCTURE * pStructure;

	pStructure = FindStructure( sGridNo, STRUCTURE_WALL );

	while ( pStructure != NULL )
	{
		// Check orientation
		if ( pStructure->ubWallOrientation == INSIDE_TOP_RIGHT || pStructure->ubWallOrientation == OUTSIDE_TOP_RIGHT )
		{
			return( TRUE );
		}

		pStructure = FindNextStructure( pStructure, STRUCTURE_WALL );

	}

	return( FALSE );
}

BOOLEAN WallOrClosedDoorExistsOfTopLeftOrientation( INT16 sGridNo )
{
	STRUCTURE * pStructure;

	pStructure = FindStructure( sGridNo, STRUCTURE_WALLSTUFF );

	while ( pStructure != NULL )
	{
		// skip it if it's an open door
		if ( ! ( ( pStructure->fFlags & STRUCTURE_ANYDOOR ) && ( pStructure->fFlags & STRUCTURE_OPEN ) ) )
		{
			// Check orientation
			if ( pStructure->ubWallOrientation == INSIDE_TOP_LEFT || pStructure->ubWallOrientation == OUTSIDE_TOP_LEFT )
			{
				return( TRUE );
			}
		}

		pStructure = FindNextStructure( pStructure, STRUCTURE_WALLSTUFF );

	}

	return( FALSE );
}

BOOLEAN WallOrClosedDoorExistsOfTopRightOrientation( INT16 sGridNo )
{
	STRUCTURE * pStructure;

	pStructure = FindStructure( sGridNo, STRUCTURE_WALLSTUFF );

	while ( pStructure != NULL )
	{
		// skip it if it's an open door
		if ( ! ( ( pStructure->fFlags & STRUCTURE_ANYDOOR ) && ( pStructure->fFlags & STRUCTURE_OPEN ) ) )
		{
			// Check orientation
			if ( pStructure->ubWallOrientation == INSIDE_TOP_RIGHT || pStructure->ubWallOrientation == OUTSIDE_TOP_RIGHT )
			{
				return( TRUE );
			}
		}

		pStructure = FindNextStructure( pStructure, STRUCTURE_WALLSTUFF );

	}

	return( FALSE );
}

BOOLEAN OpenRightOrientedDoorWithDoorOnRightOfEdgeExists( INT16 sGridNo )
{
	STRUCTURE * pStructure;

	pStructure = FindStructure( sGridNo, STRUCTURE_ANYDOOR );

	while ( pStructure != NULL && (pStructure->fFlags & STRUCTURE_OPEN) )
	{
		// Check orientation
		if ( pStructure->ubWallOrientation == INSIDE_TOP_RIGHT || pStructure->ubWallOrientation == OUTSIDE_TOP_RIGHT )
		{
			if ( (pStructure->fFlags & STRUCTURE_DOOR) || (pStructure->fFlags & STRUCTURE_DDOOR_RIGHT) )
			{
				return( TRUE );
			}
		}

		pStructure = FindNextStructure( pStructure, STRUCTURE_ANYDOOR );

	}

	return( FALSE );
}

BOOLEAN OpenLeftOrientedDoorWithDoorOnLeftOfEdgeExists( INT16 sGridNo )
{
	STRUCTURE * pStructure;

	pStructure = FindStructure( sGridNo, STRUCTURE_ANYDOOR );

	while ( pStructure != NULL && (pStructure->fFlags & STRUCTURE_OPEN) )
	{
		// Check orientation
		if ( pStructure->ubWallOrientation == INSIDE_TOP_LEFT || pStructure->ubWallOrientation == OUTSIDE_TOP_LEFT )
		{
			if ( (pStructure->fFlags & STRUCTURE_DOOR) || (pStructure->fFlags & STRUCTURE_DDOOR_LEFT) )
			{
				return( TRUE );
			}
		}

		pStructure = FindNextStructure( pStructure, STRUCTURE_ANYDOOR );

	}

	return( FALSE );
}


static STRUCTURE* FindCuttableWireFenceAtGridNo(INT16 sGridNo)
{
	STRUCTURE * pStructure;

	pStructure = FindStructure( sGridNo, STRUCTURE_WIREFENCE );
	if (pStructure != NULL && pStructure->ubWallOrientation != NO_ORIENTATION && !(pStructure->fFlags & STRUCTURE_OPEN) )
	{
		return( pStructure );
	}
	return( NULL );
}

BOOLEAN CutWireFence( INT16 sGridNo )
{
	STRUCTURE * pStructure;

	pStructure = FindCuttableWireFenceAtGridNo( sGridNo );
	if (pStructure)
	{
		pStructure = SwapStructureForPartnerAndStoreChangeInMap( sGridNo, pStructure );
		if (pStructure)
		{
			RecompileLocalMovementCosts( sGridNo );
			SetRenderFlags( RENDER_FLAG_FULL );
			return( TRUE );
		}
	}
	return( FALSE );
}

BOOLEAN IsCuttableWireFenceAtGridNo( INT16 sGridNo )
{
	return( FindCuttableWireFenceAtGridNo( sGridNo ) != NULL );
}


BOOLEAN IsRepairableStructAtGridNo(const INT16 sGridNo, SOLDIERTYPE** const tgt)
{
	UINT8 ubMerc;

	// OK, first look for a vehicle....
	ubMerc = WhoIsThere2( sGridNo, 0 );

	if ( ubMerc != NOBODY )
	{
		SOLDIERTYPE* const s = GetMan(ubMerc);
		if (tgt != NULL) *tgt = s;
		if (s->uiStatusFlags & SOLDIER_VEHICLE) return 2;
	}
	else
	{
		if (tgt != NULL) *tgt = NULL;
	}
	// Then for over a robot....

	// then for SAM site....
	if ( DoesSAMExistHere( gWorldSectorX, gWorldSectorY, gbWorldSectorZ, sGridNo ) )
	{
		return( 3 );
	}


	return( FALSE );
}


SOLDIERTYPE* GetRefuelableStructAtGridNo(INT16 sGridNo)
{
	// OK, first look for a vehicle....
	const UINT8 tgt_id = WhoIsThere2(sGridNo, 0);
	if (tgt_id != NOBODY)
	{
		SOLDIERTYPE* const tgt = GetMan(tgt_id);
		if (tgt->uiStatusFlags & SOLDIER_VEHICLE) return tgt;
	}
	return NULL;
}


static BOOLEAN IsCutWireFenceAtGridNo(INT16 sGridNo)
{
	STRUCTURE * pStructure;

	pStructure = FindStructure( sGridNo, STRUCTURE_WIREFENCE );
	if (pStructure != NULL && (pStructure->ubWallOrientation != NO_ORIENTATION) && (pStructure->fFlags & STRUCTURE_OPEN) )
	{
		return( TRUE );
	}
	return( FALSE );
}



INT16 FindDoorAtGridNoOrAdjacent( INT16 sGridNo )
{
	STRUCTURE * pStructure;
	STRUCTURE * pBaseStructure;
	INT16				sTestGridNo;

	sTestGridNo = sGridNo;
	pStructure = FindStructure( sTestGridNo, STRUCTURE_ANYDOOR );
	if (pStructure)
	{
		pBaseStructure = FindBaseStructure( pStructure );
		return( pBaseStructure->sGridNo );
	}

	sTestGridNo = sGridNo + DirectionInc( NORTH );
	pStructure = FindStructure( sTestGridNo, STRUCTURE_ANYDOOR );
	if (pStructure)
	{
		pBaseStructure = FindBaseStructure( pStructure );
		return( pBaseStructure->sGridNo );
	}

	sTestGridNo = sGridNo + DirectionInc( WEST );
	pStructure = FindStructure( sTestGridNo, STRUCTURE_ANYDOOR );
	if (pStructure)
	{
		pBaseStructure = FindBaseStructure( pStructure );
		return( pBaseStructure->sGridNo );
	}

	return( NOWHERE );
}



BOOLEAN IsCorpseAtGridNo( INT16 sGridNo, UINT8 ubLevel )
{
	if ( GetCorpseAtGridNo( sGridNo , ubLevel ) != NULL )
	{
		return( TRUE );
	}
	else
	{
		return( FALSE );
	}
}


BOOLEAN SetOpenableStructureToClosed( INT16 sGridNo, UINT8 ubLevel )
{
	STRUCTURE *		pStructure;
	STRUCTURE *		pNewStructure;

	pStructure = FindStructure( sGridNo, STRUCTURE_OPENABLE );
	if ( !pStructure )
	{
		return( FALSE );
	}

	if ( pStructure->fFlags & STRUCTURE_OPEN )
	{
		pNewStructure = SwapStructureForPartner( sGridNo, pStructure );
		if ( pNewStructure != NULL)
		{
			RecompileLocalMovementCosts( sGridNo );
			SetRenderFlags( RENDER_FLAG_FULL );
		}
	}
	// else leave it as is!
	return( TRUE );
}
