#include "Physics.h"
#include "Soldier_Find.h"
#include "Timer_Control.h"
#include "WorldDef.h"
#include "Debug.h"
#include "Smooth.h"
#include "WorldMan.h"
#include "Lighting.h"
#include "RenderWorld.h"
#include "Overhead.h"
#include "AI.h"
#include "Sound_Control.h"
#include "Animation_Control.h"
#include "Isometric_Utils.h"
#include "Tile_Animation.h"
#include "Tile_Cache.h"
#include "Explosion_Control.h"
#include "Weapons.h"
#include "Keys.h"
#include "Bullets.h"
#include "Rotting_Corpses.h"
#include "SmokeEffects.h"
#include "MemMan.h"


static ANITILE* pAniTileHead = NULL;


static UINT16 SetFrameByDir(UINT16 frame, const ANITILE* const a)
{
	if (a->uiFlags & ANITILE_USE_DIRECTION_FOR_START_FRAME)
	{
		// Our start frame is actually a direction indicator
		const UINT8 ubTempDir = OneCDirection(a->v.user.uiData3);
		frame = a->usNumFrames * ubTempDir;
	}
	else if (a->uiFlags & ANITILE_USE_4DIRECTION_FOR_START_FRAME)
	{
		// Our start frame is actually a direction indicator
		const UINT8 ubTempDir = gb4DirectionsFrom8[a->v.user.uiData3];
		frame = a->usNumFrames * ubTempDir;
	}
	return frame;
}


ANITILE* CreateAnimationTile(const ANITILE_PARAMS* const parms)
{
	ANITILE* const a = MemAlloc(sizeof(*a));

	INT32        cached_tile = -1;
	const INT16  gridno      = parms->sGridNo;
	const UINT8  ubLevel     = parms->ubLevelID;
	INT16        tile_index  = parms->usTileIndex;
	const UINT32 flags       = parms->uiFlags;
	LEVELNODE*   l           = parms->pGivenLevelNode;
	if (flags & ANITILE_EXISTINGTILE)
	{
		Assert(parms->zCachedFile == NULL);
		l->pAniTile = a;
	}
	else
	{
		if (parms->zCachedFile != NULL)
		{
			cached_tile = GetCachedTile(parms->zCachedFile);
			if (cached_tile == -1) return NULL;
			tile_index = cached_tile + TILE_CACHE_START_INDEX;
		}

		// allocate new tile
		switch (ubLevel)
		{
			case ANI_STRUCT_LEVEL:  l = ForceStructToTail(gridno, tile_index); break;
			case ANI_SHADOW_LEVEL:  l = AddShadowToHead(  gridno, tile_index); break;
			case ANI_OBJECT_LEVEL:  l = AddObjectToHead(  gridno, tile_index); break;
			case ANI_ROOF_LEVEL:    l = AddRoofToHead(    gridno, tile_index); break;
			case ANI_ONROOF_LEVEL:  l = AddOnRoofToHead(  gridno, tile_index); break;
			case ANI_TOPMOST_LEVEL: l = AddTopmostToHead( gridno, tile_index); break;
			default:                return NULL;
		}

		// set new tile values
		l->ubShadeLevel        = DEFAULT_SHADE_LEVEL;
		l->ubNaturalShadeLevel = DEFAULT_SHADE_LEVEL;

		if (cached_tile != -1)
		{
			l->uiFlags    |= LEVELNODE_CACHEDANITILE;
			l->pAniTile    = a;
			a->sRelativeX  = parms->sX;
			a->sRelativeY  = parms->sY;
			l->sRelativeZ  = parms->sZ;
		}
	}
	a->pLevelNode    = l;
	a->sCachedTileID = cached_tile;

	switch (ubLevel)
	{
		case ANI_STRUCT_LEVEL:  ResetSpecificLayerOptimizing(TILES_DYNAMIC_STRUCTURES); break;
		case ANI_SHADOW_LEVEL:  ResetSpecificLayerOptimizing(TILES_DYNAMIC_SHADOWS);    break;
		case ANI_OBJECT_LEVEL:  ResetSpecificLayerOptimizing(TILES_DYNAMIC_OBJECTS);    break;
		case ANI_ROOF_LEVEL:    ResetSpecificLayerOptimizing(TILES_DYNAMIC_ROOF);       break;
		case ANI_ONROOF_LEVEL:  ResetSpecificLayerOptimizing(TILES_DYNAMIC_ONROOF);     break;
		case ANI_TOPMOST_LEVEL: ResetSpecificLayerOptimizing(TILES_DYNAMIC_TOPMOST);    break;
	}

	// set flags for levelnode
	UINT32 lflags = l->uiFlags | LEVELNODE_ANIMATION | LEVELNODE_USEZ;
	lflags |= (flags & ANITILE_PAUSED ? LEVELNODE_LASTDYNAMIC | LEVELNODE_UPDATESAVEBUFFERONCE : LEVELNODE_DYNAMIC);
	if (flags & ANITILE_NOZBLITTER)             lflags |= LEVELNODE_NOZBLITTER;
	if (flags & ANITILE_ALWAYS_TRANSLUCENT)     lflags |= LEVELNODE_REVEAL;
	if (flags & ANITILE_USEBEST_TRANSLUCENT)    lflags |= LEVELNODE_USEBESTTRANSTYPE;
	if (flags & ANITILE_ANIMATE_Z)              lflags |= LEVELNODE_DYNAMICZ;
	if (flags & ANITILE_OPTIMIZEFORSMOKEEFFECT) lflags |= LEVELNODE_NOWRITEZ;
	l->uiFlags = lflags;

	// set anitile values
	if (cached_tile != -1)
	{
		a->usNumFrames = gpTileCache[cached_tile].ubNumFrames;
	}
	else
	{
		a->usNumFrames = gTileDatabase[tile_index].pAnimData->ubNumFrames;
	}
	a->ubLevelID        = ubLevel;
	a->usTileIndex      = tile_index;
	a->uiFlags          = flags;
	a->sDelay           = parms->sDelay;
	a->uiTimeLastUpdate = GetJA2Clock();
	a->sGridNo          = gridno;
	a->ubKeyFrame1      = parms->ubKeyFrame1;
	a->uiKeyFrame1Code  = parms->uiKeyFrame1Code;
	a->ubKeyFrame2      = parms->ubKeyFrame2;
	a->uiKeyFrame2Code  = parms->uiKeyFrame2Code;
	a->v                = parms->v;
	const INT16 start_frame = parms->sStartFrame + SetFrameByDir(0, a);
	a->sCurrentFrame    = start_frame;
	a->sStartFrame      = start_frame;
	a->pNext            = pAniTileHead;
	pAniTileHead = a;
	return a;
}


// Loop throug all ani tiles and remove...
void DeleteAniTiles( )
{
	ANITILE *pAniNode			= NULL;
	ANITILE *pNode				= NULL;

	// LOOP THROUGH EACH NODE
	// And call delete function...
	pAniNode = pAniTileHead;

	while( pAniNode != NULL )
	{
		pNode = pAniNode;
		pAniNode = pAniNode->pNext;

		DeleteAniTile( pNode );
	}
}


void DeleteAniTile( ANITILE *pAniTile )
{
	ANITILE				*pAniNode				= NULL;
	ANITILE				*pOldAniNode		= NULL;
	TILE_ELEMENT	*TileElem;

	pAniNode = pAniTileHead;

	while( pAniNode!= NULL )
	{
		if ( pAniNode == pAniTile )
		{
			// OK, set links
			// Check for head or tail
			if ( pOldAniNode == NULL )
			{
				// It's the head
				pAniTileHead = pAniTile->pNext;
			}
			else
			{
				pOldAniNode->pNext = pAniNode->pNext;
			}

			if ( !(pAniNode->uiFlags & ANITILE_EXISTINGTILE  ) )
			{

				// Delete memory assosiated with item
				switch( pAniNode->ubLevelID )
				{
					case ANI_STRUCT_LEVEL:

						RemoveStructFromLevelNode( pAniNode->sGridNo, pAniNode->pLevelNode );
						break;

					case ANI_SHADOW_LEVEL:

						RemoveShadowFromLevelNode( pAniNode->sGridNo, pAniNode->pLevelNode );
						break;

					case ANI_OBJECT_LEVEL:

						RemoveObject( pAniNode->sGridNo, pAniNode->usTileIndex );
						break;

					case ANI_ROOF_LEVEL:

						RemoveRoof( pAniNode->sGridNo, pAniNode->usTileIndex );
						break;

					case ANI_ONROOF_LEVEL:

						RemoveOnRoof( pAniNode->sGridNo, pAniNode->usTileIndex );
						break;

					case ANI_TOPMOST_LEVEL:

						RemoveTopmostFromLevelNode( pAniNode->sGridNo, pAniNode->pLevelNode );
						break;

				}

				if (pAniNode->sCachedTileID != -1)
				{
					RemoveCachedTile(pAniNode->sCachedTileID);
				}

				if ( pAniNode->uiFlags & ANITILE_EXPLOSION )
				{
					// Talk to the explosion data...
					EXPLOSIONTYPE* const e     = pAniNode->v.explosion;
					SOLDIERTYPE*   const owner = e->owner;
					RemoveExplosionData(e);

					if ( !gfExplosionQueueActive )
					{
						// turn on sighting again
						// the explosion queue handles all this at the end of the queue
						gTacticalStatus.uiFlags &= (~DISALLOW_SIGHT);
					}

          // Freeup attacker from explosion
					DebugMsg(TOPIC_JA2, DBG_LEVEL_3, "@@@@@@@ Reducing attacker busy count..., EXPLOSION effect gone off");
					ReduceAttackBusyCount(owner, FALSE);
				}


				if ( pAniNode->uiFlags & ANITILE_RELEASE_ATTACKER_WHEN_DONE )
				{
					BULLET*      const bullet   = pAniNode->v.bullet;
					SOLDIERTYPE* const attacker = bullet->pFirer;
					// First delete the bullet!
					RemoveBullet(bullet);

					DebugMsg(TOPIC_JA2, DBG_LEVEL_3, "@@@@@@@ Freeing up attacker - miss finished animation");
					FreeUpAttacker(attacker);
				}
			}
			else
			{
				TileElem = &( gTileDatabase[ pAniNode->usTileIndex ] );

				// OK, update existing tile usIndex....
				Assert( TileElem->pAnimData != NULL );
				pAniNode->pLevelNode->usIndex = TileElem->pAnimData->pusFrames[ pAniNode->pLevelNode->sCurrentFrame ];

				// OK, set our frame data back to zero....
				pAniNode->pLevelNode->sCurrentFrame = 0;

				// Set some flags to write to Z / update save buffer
				// pAniNode->pLevelNode->uiFlags |=( LEVELNODE_LASTDYNAMIC | LEVELNODE_UPDATESAVEBUFFERONCE );
				pAniNode->pLevelNode->uiFlags &= ~( LEVELNODE_DYNAMIC | LEVELNODE_USEZ | LEVELNODE_ANIMATION );

				if (pAniNode->uiFlags & ANITILE_DOOR)
				{
					// unset door busy!
					DOOR_STATUS * pDoorStatus;

					pDoorStatus = GetDoorStatus( pAniNode->sGridNo );
					if (pDoorStatus)
					{
						pDoorStatus->ubFlags &= ~(DOOR_BUSY);
					}

					if ( GridNoOnScreen( pAniNode->sGridNo ) )
					{
						SetRenderFlags(RENDER_FLAG_FULL);
					}

				}
			}

			MemFree( pAniNode );
			return;
		}

		pOldAniNode = pAniNode;
		pAniNode		= pAniNode->pNext;

	}


}



void UpdateAniTiles( )
{
	ANITILE *pAniNode			= NULL;
	ANITILE *pNode				= NULL;
	UINT32	uiClock				= GetJA2Clock( );

	// LOOP THROUGH EACH NODE
	pAniNode = pAniTileHead;

	while( pAniNode != NULL )
	{
		pNode = pAniNode;
		pAniNode = pAniNode->pNext;

		if ( (uiClock - pNode->uiTimeLastUpdate ) > (UINT32)pNode->sDelay && !( pNode->uiFlags & ANITILE_PAUSED ) )
		{
			pNode->uiTimeLastUpdate = GetJA2Clock( );

			if ( pNode->uiFlags & ( ANITILE_OPTIMIZEFORSLOWMOVING ) )
			{
				pNode->pLevelNode->uiFlags |= (LEVELNODE_DYNAMIC );
				pNode->pLevelNode->uiFlags &= (~LEVELNODE_LASTDYNAMIC);
			}
			else if ( pNode->uiFlags & ( ANITILE_OPTIMIZEFORSMOKEEFFECT ) )
			{
			//	pNode->pLevelNode->uiFlags |= LEVELNODE_DYNAMICZ;
				ResetSpecificLayerOptimizing( TILES_DYNAMIC_STRUCTURES );
				pNode->pLevelNode->uiFlags &= (~LEVELNODE_LASTDYNAMIC);
				pNode->pLevelNode->uiFlags |= (LEVELNODE_DYNAMIC );
			}

			if ( pNode->uiFlags & ANITILE_FORWARD )
			{
				const UINT16 usMaxFrames = pNode->usNumFrames + SetFrameByDir(0, pNode);
				if ( ( pNode->sCurrentFrame + 1 ) < usMaxFrames )
				{
					pNode->sCurrentFrame++;
					pNode->pLevelNode->sCurrentFrame = pNode->sCurrentFrame;

					if ( pNode->uiFlags & ANITILE_EXPLOSION )
					{
						// Talk to the explosion data...
						UpdateExplosionFrame(pNode->v.explosion, pNode->sCurrentFrame);
					}

					// CHECK IF WE SHOULD BE DISPLAYING TRANSLUCENTLY!
					if ( pNode->sCurrentFrame == pNode->ubKeyFrame1 )
					{
						switch( pNode->uiKeyFrame1Code )
						{
							case ANI_KEYFRAME_BEGIN_TRANSLUCENCY:

								pNode->pLevelNode->uiFlags |= LEVELNODE_REVEAL;
								break;

							case ANI_KEYFRAME_CHAIN_WATER_EXPLOSION:
							{
								const REAL_OBJECT* const o = pNode->v.object;
								IgniteExplosionXY(o->owner, pNode->pLevelNode->sRelativeX, pNode->pLevelNode->sRelativeY, 0, pNode->sGridNo, o->Obj.usItem, 0);
								break;
							}

              case ANI_KEYFRAME_DO_SOUND:
                PlayLocationJA2Sample(pNode->sGridNo, pNode->v.sound, MIDVOLUME, 1);
                break;
						}

					}

					// CHECK IF WE SHOULD BE DISPLAYING TRANSLUCENTLY!
					if ( pNode->sCurrentFrame == pNode->ubKeyFrame2 )
					{
           	switch( pNode->uiKeyFrame2Code )
						{
							case ANI_KEYFRAME_BEGIN_DAMAGE:
							{
                Assert(pNode->uiFlags & ANITILE_EXPLOSION);
                const EXPLOSIONTYPE* const e    = pNode->v.explosion;
								const UINT16               item = e->usItem;
                const UINT8 ubExpType = Explosive[Item[item].ubClassIndex].ubType;

                if ( ubExpType == EXPLOSV_TEARGAS || ubExpType == EXPLOSV_MUSTGAS ||
                     ubExpType == EXPLOSV_SMOKE )
                {
                  // Do sound....
                  // PlayLocationJA2Sample(pNode->sGridNo, AIR_ESCAPING_1, HIGHVOLUME, 1);
									NewSmokeEffect(pNode->sGridNo, item, e->bLevel, e->owner);
                }
                else
                {
									SpreadEffect(pNode->sGridNo, Explosive[Item[item].ubClassIndex].ubRadius, item, e->owner, FALSE, e->bLevel, NULL);
                }
								// Forfait any other animations this frame....
								return;
							}
						}

					}

				}
				else
				{
					// We are done!
					if ( pNode->uiFlags & ANITILE_LOOPING )
					{
						pNode->sCurrentFrame = SetFrameByDir(pNode->sStartFrame, pNode);
					}
					else if ( pNode->uiFlags & ANITILE_REVERSE_LOOPING )
					{
						// Turn off backwards flag
						pNode->uiFlags &= (~ANITILE_FORWARD );

						// Turn onn forwards flag
						pNode->uiFlags |= ANITILE_BACKWARD;
					}
					else
					{
						// Delete from world!
						DeleteAniTile( pNode );

						// Turn back on redunency checks!
						gTacticalStatus.uiFlags &= (~NOHIDE_REDUNDENCY);

						return;
					}
				}
			}

			if ( pNode->uiFlags & ANITILE_BACKWARD )
			{
				if ( pNode->uiFlags & ANITILE_ERASEITEMFROMSAVEBUFFFER )
				{
					// ATE: Check if bounding box is on the screen...
					if ( pNode->bFrameCountAfterStart == 0 )
					{
						pNode->bFrameCountAfterStart = 1;
						pNode->pLevelNode->uiFlags |= (LEVELNODE_DYNAMIC );

						// Dangerous here, since we may not even be on the screen...
						SetRenderFlags( RENDER_FLAG_FULL );

						continue;
					}
				}

				const UINT16 usMinFrames = SetFrameByDir(0, pNode);
				if ( ( pNode->sCurrentFrame - 1 ) >= usMinFrames )
				{
					pNode->sCurrentFrame--;
					pNode->pLevelNode->sCurrentFrame = pNode->sCurrentFrame;

					if ( pNode->uiFlags & ANITILE_EXPLOSION )
					{
						// Talk to the explosion data...
						UpdateExplosionFrame(pNode->v.explosion, pNode->sCurrentFrame);
					}

				}
				else
				{
					// We are done!
					if ( pNode->uiFlags & ANITILE_PAUSE_AFTER_LOOP )
					{
						// Turn off backwards flag
						pNode->uiFlags &= (~ANITILE_BACKWARD );

						// Pause
						pNode->uiFlags |= ANITILE_PAUSED;

					}
					else if ( pNode->uiFlags & ANITILE_LOOPING )
					{
						pNode->sCurrentFrame = SetFrameByDir(pNode->sStartFrame, pNode);
					}
					else if ( pNode->uiFlags & ANITILE_REVERSE_LOOPING )
					{
						// Turn off backwards flag
						pNode->uiFlags &= (~ANITILE_BACKWARD );

						// Turn onn forwards flag
						pNode->uiFlags |= ANITILE_FORWARD;
					}
					else
					{
						// Delete from world!
						DeleteAniTile( pNode );

						return;
					}

					if ( pNode->uiFlags & ANITILE_ERASEITEMFROMSAVEBUFFFER )
					{
						// ATE: Check if bounding box is on the screen...
						pNode->bFrameCountAfterStart = 0;
						//pNode->pLevelNode->uiFlags |= LEVELNODE_UPDATESAVEBUFFERONCE;

						// Dangerous here, since we may not even be on the screen...
						SetRenderFlags( RENDER_FLAG_FULL );

					}

				}

			}

		}
		else
		{
			if ( pNode->uiFlags & ( ANITILE_OPTIMIZEFORSLOWMOVING ) )
			{
				// ONLY TURN OFF IF PAUSED...
				if ( ( pNode->uiFlags & ANITILE_ERASEITEMFROMSAVEBUFFFER ) )
				{
					if ( pNode->uiFlags & ANITILE_PAUSED )
					{
						if ( pNode->pLevelNode->uiFlags & LEVELNODE_DYNAMIC )
						{
							pNode->pLevelNode->uiFlags &= (~LEVELNODE_DYNAMIC );
							pNode->pLevelNode->uiFlags |= (LEVELNODE_LASTDYNAMIC);
							SetRenderFlags( RENDER_FLAG_FULL );
						}
					}
				}
				else
				{
					pNode->pLevelNode->uiFlags &= (~LEVELNODE_DYNAMIC );
					pNode->pLevelNode->uiFlags |= (LEVELNODE_LASTDYNAMIC);
				}
			}
			else if ( pNode->uiFlags & ( ANITILE_OPTIMIZEFORSMOKEEFFECT ) )
			{
				pNode->pLevelNode->uiFlags |= (LEVELNODE_LASTDYNAMIC);
				pNode->pLevelNode->uiFlags &= (~LEVELNODE_DYNAMIC );
			}
		}
	}
}


ANITILE *GetCachedAniTileOfType( INT16 sGridNo, UINT8 ubLevelID, UINT32 uiFlags )
{
	LEVELNODE *pNode = NULL;

	switch( ubLevelID )
	{
		case ANI_STRUCT_LEVEL:

			pNode = gpWorldLevelData[ sGridNo ].pStructHead;
			break;

		case ANI_SHADOW_LEVEL:

			pNode = gpWorldLevelData[ sGridNo ].pShadowHead;
			break;

		case ANI_OBJECT_LEVEL:

			pNode = gpWorldLevelData[ sGridNo ].pObjectHead;
			break;

		case ANI_ROOF_LEVEL:

			pNode = gpWorldLevelData[ sGridNo ].pRoofHead;
			break;

		case ANI_ONROOF_LEVEL:

			pNode = gpWorldLevelData[ sGridNo ].pOnRoofHead;
			break;

		case ANI_TOPMOST_LEVEL:

			pNode = gpWorldLevelData[ sGridNo ].pTopmostHead;
			break;

		default:

			return( NULL );
	}

	while( pNode != NULL )
	{
		if ( pNode->uiFlags & LEVELNODE_CACHEDANITILE )
		{
			if ( pNode->pAniTile->uiFlags & uiFlags )
			{
				return( pNode->pAniTile );
			}

		}

		pNode = pNode->pNext;
	}

	return( NULL );
}


void HideAniTile( ANITILE *pAniTile, BOOLEAN fHide )
{
	if ( fHide )
	{
		pAniTile->pLevelNode->uiFlags |= LEVELNODE_HIDDEN;
	}
	else
	{
		pAniTile->pLevelNode->uiFlags &= (~LEVELNODE_HIDDEN );
	}
}
