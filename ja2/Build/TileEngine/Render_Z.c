static inline INT16 GetMapXYWorldY(INT32 WorldCellX, INT32 WorldCellY)
{
	INT16 RDistToCenterX = WorldCellX * CELL_X_SIZE - gCenterWorldX;
	INT16 RDistToCenterY = WorldCellY * CELL_Y_SIZE - gCenterWorldY;
	INT16 RScreenCenterY = RDistToCenterX + RDistToCenterY;
	return RScreenCenterY + gsCY - gsTLY;
}


#define LandZLevel()      \
{                         \
	sZLevel = LAND_Z_LEVEL; \
}


#define ObjectZLevel(sMapX, sMapY)                     \
{                                                      \
	if ( uiTileElemFlags & CLIFFHANG_TILE )              \
	{                                                    \
		sZLevel=LAND_Z_LEVEL;                              \
	}                                                    \
	else if ( uiTileElemFlags & OBJECTLAYER_USEZHEIGHT ) \
	{                                                    \
		INT16 sWorldY = GetMapXYWorldY(sMapX, sMapY);      \
		sZLevel=( ( sWorldY ) *Z_SUBLAYERS)+LAND_Z_LEVEL;  \
	}                                                    \
	else                                                 \
	{                                                    \
		sZLevel=OBJECT_Z_LEVEL;                            \
	}                                                    \
}


#define	StructZLevel( sMapX, sMapY )                                                     \
{                                                                                        \
	INT16 sWorldY = GetMapXYWorldY(sMapX, sMapY);                                          \
                                                                                         \
	if ( ( uiLevelNodeFlags & LEVELNODE_ROTTINGCORPSE ) )                                  \
	{                                                                                      \
		if ( pCorpse->def.usFlags & ROTTING_CORPSE_VEHICLE )                                 \
		{                                                                                    \
			if ( pNode->pStructureData != NULL )                                               \
			{                                                                                  \
				sZOffsetX = pNode->pStructureData->pDBStructureRef->pDBStructure->bZTileOffsetX; \
				sZOffsetY = pNode->pStructureData->pDBStructureRef->pDBStructure->bZTileOffsetY; \
			}                                                                                  \
			sWorldY = GetMapXYWorldY(sMapX + sZOffsetX, sMapY + sZOffsetY);                    \
			sZLevel = STRUCT_Z_LEVEL;                                                          \
		}                                                                                    \
		else                                                                                 \
		{                                                                                    \
			sZOffsetX = -1;                                                                    \
			sZOffsetY = -1;                                                                    \
			sWorldY = GetMapXYWorldY(sMapX + sZOffsetX, sMapY + sZOffsetY);                    \
      sWorldY += 20;                                                                     \
			sZLevel = LAND_Z_LEVEL;                                                            \
		}                                                                                    \
	}                                                                                      \
	else if ( uiLevelNodeFlags & LEVELNODE_PHYSICSOBJECT )                                 \
	{                                                                                      \
		sWorldY += pNode->sRelativeZ;                                                        \
		sZLevel = ONROOF_Z_LEVEL;                                                            \
	}                                                                                      \
	else if ( uiLevelNodeFlags & LEVELNODE_ITEM )                                          \
	{                                                                                      \
		if ( pNode->pItemPool->bRenderZHeightAboveLevel > 0 )                                \
		{                                                                                    \
			sZLevel = STRUCT_Z_LEVEL;                                                          \
			sZLevel+=( pNode->pItemPool->bRenderZHeightAboveLevel );                           \
		}                                                                                    \
		else                                                                                 \
		{                                                                                    \
			sZLevel = OBJECT_Z_LEVEL;                                                          \
		}                                                                                    \
	}                                                                                      \
  else if ( uiAniTileFlags & ANITILE_SMOKE_EFFECT )                                      \
  {                                                                                      \
		sZLevel = OBJECT_Z_LEVEL;                                                            \
  }                                                                                      \
	else if ( ( uiLevelNodeFlags & LEVELNODE_USEZ ) )                                      \
	{                                                                                      \
		if ( ( uiLevelNodeFlags & LEVELNODE_NOZBLITTER ) )                                   \
		{                                                                                    \
			sWorldY += 40;                                                                     \
		}                                                                                    \
		else                                                                                 \
		{                                                                                    \
			sWorldY += pNode->sRelativeZ;                                                      \
		}                                                                                    \
		sZLevel = ONROOF_Z_LEVEL;                                                            \
	}                                                                                      \
	else                                                                                   \
	{                                                                                      \
		sZLevel = STRUCT_Z_LEVEL;                                                            \
	}                                                                                      \
  sZLevel += sWorldY * Z_SUBLAYERS;                                                      \
}

#define	RoofZLevel( sMapX, sMapY )              \
{                                               \
	INT16 sWorldY = GetMapXYWorldY(sMapX, sMapY); \
	sWorldY += WALL_HEIGHT;                       \
	sZLevel=(sWorldY*Z_SUBLAYERS)+ROOF_Z_LEVEL;   \
}

#define	OnRoofZLevel( sMapX, sMapY )                \
{                                                   \
	INT16 sWorldY = GetMapXYWorldY(sMapX, sMapY);     \
                                                    \
	if ( uiLevelNodeFlags & LEVELNODE_ROTTINGCORPSE ) \
	{                                                 \
		sWorldY += ( WALL_HEIGHT + 40 );                \
	}                                                 \
	if ( uiLevelNodeFlags & LEVELNODE_ROTTINGCORPSE ) \
	{                                                 \
		sWorldY += ( WALL_HEIGHT + 40 );                \
	}                                                 \
	else                                              \
	{                                                 \
		sWorldY += WALL_HEIGHT;                         \
	}                                                 \
                                                    \
	sZLevel=(sWorldY*Z_SUBLAYERS)+ONROOF_Z_LEVEL;     \
}

#define	TopmostZLevel()    \
{                          \
	sZLevel=TOPMOST_Z_LEVEL; \
}

#define	ShadowZLevel( sMapX, sMapY )                                   \
{                                                                      \
	INT16 sWorldY = GetMapXYWorldY(sMapX, sMapY);                        \
	sZLevel=__max( ( (sWorldY - 80 ) *Z_SUBLAYERS )+SHADOW_Z_LEVEL, 0 ); \
}

#define SoldierZLevel( pSoldier, sMapX, sMapY )                                        \
{                                                                                      \
	INT16 sWorldY;                                                                       \
	if ( ( pSoldier->uiStatusFlags & SOLDIER_MULTITILE ) )                               \
	{                                                                                    \
		if ( pNode->pStructureData != NULL )                                               \
		{                                                                                  \
			sZOffsetX = pNode->pStructureData->pDBStructureRef->pDBStructure->bZTileOffsetX; \
			sZOffsetY = pNode->pStructureData->pDBStructureRef->pDBStructure->bZTileOffsetY; \
		}                                                                                  \
		sWorldY = GetMapXYWorldY(sMapX + sZOffsetX, sMapY + sZOffsetY);                    \
	}                                                                                    \
	else                                                                                 \
	{                                                                                    \
		sWorldY = GetMapXYWorldY(sMapX, sMapY);                                            \
	}                                                                                    \
                                                                                       \
	if ( pSoldier->uiStatusFlags & SOLDIER_VEHICLE )                                     \
	{                                                                                    \
		sZLevel=(sWorldY*Z_SUBLAYERS)+STRUCT_Z_LEVEL;                                      \
	}                                                                                    \
	else                                                                                 \
	{                                                                                    \
		if ( pSoldier->dHeightAdjustment > 0 )                                             \
		{                                                                                  \
			sWorldY += ( WALL_HEIGHT + 20 );                                                 \
			sZLevel=(sWorldY*Z_SUBLAYERS)+ONROOF_Z_LEVEL;                                    \
		}                                                                                  \
		else                                                                               \
		{                                                                                  \
			sZLevel = (sWorldY * Z_SUBLAYERS) + MERC_Z_LEVEL;                                \
		}                                                                                  \
	                                                                                     \
		if ( pSoldier->sZLevelOverride != -1 )                                             \
		{                                                                                  \
			sZLevel = pSoldier->sZLevelOverride;                                             \
		}                                                                                  \
	                                                                                     \
		if ( gsForceSoldierZLevel != 0 )                                                   \
		{                                                                                  \
			sZLevel = gsForceSoldierZLevel;                                                  \
		}                                                                                  \
	}                                                                                    \
}
