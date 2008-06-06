#include "LoadSaveSmokeEffect.h"
#include "Overhead.h"
#include "Debug.h"
#include "Soldier_Control.h"
#include "TileDef.h"
#include "Weapons.h"
#include "Handle_Items.h"
#include "WorldDef.h"
#include "WorldMan.h"
#include "Tile_Animation.h"
#include "SmokeEffects.h"
#include "Isometric_Utils.h"
#include "RenderWorld.h"
#include "Explosion_Control.h"
#include "Random.h"
#include "Game_Clock.h"
#include "OppList.h"
#include "Tactical_Save.h"
#include "Campaign_Types.h"
#include "FileMan.h"
#include "SaveLoadGame.h"


#define		NUM_SMOKE_EFFECT_SLOTS					25


// GLOBAL FOR SMOKE LISTING
static SMOKEEFFECT gSmokeEffectData[NUM_SMOKE_EFFECT_SLOTS];
static UINT32      guiNumSmokeEffects = 0;


#define BASE_FOR_ALL_SMOKE_EFFECTS(type, iter)                    \
	for (type* iter        = gSmokeEffectData,                      \
	         * iter##__end = gSmokeEffectData + guiNumSmokeEffects; \
	     iter != iter##__end;                                       \
	     ++iter)                                                    \
		if (!iter->fAllocated) continue; else
#define FOR_ALL_SMOKE_EFFECTS(iter)  BASE_FOR_ALL_SMOKE_EFFECTS(      SMOKEEFFECT, iter)
#define CFOR_ALL_SMOKE_EFFECTS(iter) BASE_FOR_ALL_SMOKE_EFFECTS(const SMOKEEFFECT, iter)


static SMOKEEFFECT* GetFreeSmokeEffect(void)
{
	for (SMOKEEFFECT* s = gSmokeEffectData; s != gSmokeEffectData + guiNumSmokeEffects; ++s)
	{
		if (!s->fAllocated) return s;
	}
	if (guiNumSmokeEffects < NUM_SMOKE_EFFECT_SLOTS)
	{
		return &gSmokeEffectData[guiNumSmokeEffects++];
	}
	return NULL;
}


static INT8 FromWorldFlagsToSmokeType(UINT8 ubWorldFlags);


// Returns NO_SMOKE_EFFECT if none there...
INT8 GetSmokeEffectOnTile( INT16 sGridNo, INT8 bLevel )
{
	UINT8		ubExtFlags;

	ubExtFlags = gpWorldLevelData[ sGridNo ].ubExtFlags[ bLevel ];

	// Look at worldleveldata to find flags..
	if ( ubExtFlags & ANY_SMOKE_EFFECT )
	{
		// Which smoke am i?
		return( FromWorldFlagsToSmokeType( ubExtFlags ) );
	}

	return( NO_SMOKE_EFFECT );
}


static INT8 FromWorldFlagsToSmokeType(UINT8 ubWorldFlags)
{
	if ( ubWorldFlags & MAPELEMENT_EXT_SMOKE )
	{
		return( NORMAL_SMOKE_EFFECT );
	}
	else if ( ubWorldFlags & MAPELEMENT_EXT_TEARGAS )
	{
		return( TEARGAS_SMOKE_EFFECT );
	}
	else if ( ubWorldFlags & MAPELEMENT_EXT_MUSTARDGAS )
	{
		return( MUSTARDGAS_SMOKE_EFFECT );
	}
	else if ( ubWorldFlags & MAPELEMENT_EXT_CREATUREGAS )
	{
		return( CREATURE_SMOKE_EFFECT );
	}
	else
	{
		return( NO_SMOKE_EFFECT );
	}
}


static UINT8 FromSmokeTypeToWorldFlags(INT8 bType)
{
	switch( bType )
	{
		case NORMAL_SMOKE_EFFECT:     return MAPELEMENT_EXT_SMOKE;
		case TEARGAS_SMOKE_EFFECT:    return MAPELEMENT_EXT_TEARGAS;
		case MUSTARDGAS_SMOKE_EFFECT: return MAPELEMENT_EXT_MUSTARDGAS;
		case CREATURE_SMOKE_EFFECT:   return MAPELEMENT_EXT_CREATUREGAS;
		default:                      return 0;
	}
}


void NewSmokeEffect(const INT16 sGridNo, const UINT16 usItem, const INT8 bLevel, SOLDIERTYPE* const owner)
{
	INT8				bSmokeEffectType=0;
	UINT8				ubDuration=0;
	UINT8				ubStartRadius=0;

	SMOKEEFFECT* const pSmoke = GetFreeSmokeEffect();
	if (pSmoke == NULL) return;

	memset(pSmoke, 0, sizeof(*pSmoke));

	// Set some values...
	pSmoke->sGridNo									= sGridNo;
	pSmoke->usItem									= usItem;
	pSmoke->uiTimeOfLastUpdate			= GetWorldTotalSeconds( );

	// Are we indoors?
	if ( GetTerrainType( sGridNo ) == FLAT_FLOOR )
	{
		pSmoke->bFlags |= SMOKE_EFFECT_INDOORS;
	}


  switch( usItem )
  {
		case MUSTARD_GRENADE:

			bSmokeEffectType	=	MUSTARDGAS_SMOKE_EFFECT;
			ubDuration				= 5;
			ubStartRadius			= 1;
			break;

		case TEARGAS_GRENADE:
		case GL_TEARGAS_GRENADE:
			bSmokeEffectType	=	TEARGAS_SMOKE_EFFECT;
			ubDuration				= 5;
			ubStartRadius			= 1;
			break;

		case BIG_TEAR_GAS:
			bSmokeEffectType	=	TEARGAS_SMOKE_EFFECT;
			ubDuration				= 5;
			ubStartRadius			= 1;
			break;

		case SMOKE_GRENADE:
		case GL_SMOKE_GRENADE:

			bSmokeEffectType	=	NORMAL_SMOKE_EFFECT;
			ubDuration				= 5;
			ubStartRadius			= 1;
			break;

    case SMALL_CREATURE_GAS:
			bSmokeEffectType	=	CREATURE_SMOKE_EFFECT;
			ubDuration				= 3;
			ubStartRadius			= 1;
			break;

    case LARGE_CREATURE_GAS:
			bSmokeEffectType	=	CREATURE_SMOKE_EFFECT;
			ubDuration				= 3;
			ubStartRadius			= Explosive[ Item[ LARGE_CREATURE_GAS ].ubClassIndex ].ubRadius;
			break;

    case VERY_SMALL_CREATURE_GAS:

			bSmokeEffectType	=	CREATURE_SMOKE_EFFECT;
			ubDuration				= 2;
			ubStartRadius			= 0;
      break;
  }



	pSmoke->ubDuration	= ubDuration;
	pSmoke->ubRadius    = ubStartRadius;
	pSmoke->bAge				= 0;
	pSmoke->fAllocated  = TRUE;
	pSmoke->bType				= bSmokeEffectType;
	pSmoke->owner       = owner;

  if ( pSmoke->bFlags & SMOKE_EFFECT_INDOORS )
  {
		// Duration is increased by 2 turns...indoors
		pSmoke->ubDuration += 3;
  }

  if ( bLevel )
  {
    pSmoke->bFlags |= SMOKE_EFFECT_ON_ROOF;
  }

  // ATE: FALSE into subsequent-- it's the first one!
  SpreadEffectSmoke(pSmoke, FALSE, bLevel);
}


// Add smoke to gridno
// ( Replacement algorithm uses distance away )
void AddSmokeEffectToTile(const SMOKEEFFECT* const smoke, const INT8 bType, const INT16 sGridNo, const INT8 bLevel)
{
	ANITILE_PARAMS	AniParams;
	ANITILE					*pAniTile;
  BOOLEAN         fDissipating = FALSE;

	if (smoke->ubDuration - smoke->bAge < 2)
  {
    fDissipating = TRUE;
    // Remove old one...
    RemoveSmokeEffectFromTile( sGridNo, bLevel );
  }


	// If smoke effect exists already.... stop
	if ( gpWorldLevelData[ sGridNo ].ubExtFlags[ bLevel ] & ANY_SMOKE_EFFECT )
	{
		return;
	}

	// OK,  Create anitile....
	memset( &AniParams, 0, sizeof( ANITILE_PARAMS ) );
	AniParams.sGridNo							= sGridNo;

  if ( bLevel == 0 )
  {
	  AniParams.ubLevelID						= ANI_STRUCT_LEVEL;
  }
  else
  {
	  AniParams.ubLevelID						= ANI_ONROOF_LEVEL;
  }


	AniParams.sDelay							= (INT16)( 300 + Random( 300 ) );

  if ( !( gGameSettings.fOptions[ TOPTION_ANIMATE_SMOKE ] ) )
  {
	  AniParams.sStartFrame					= (INT16)0;
  }
  else
  {
	  AniParams.sStartFrame					= (INT16)Random( 5 );
  }


	// Bare bones flags are...
	//AniParams.uiFlags = ANITILE_FORWARD | ANITILE_SMOKE_EFFECT | ANITILE_LOOPING;


  if ( !( gGameSettings.fOptions[ TOPTION_ANIMATE_SMOKE ] ) )
  {
    AniParams.uiFlags = ANITILE_PAUSED | ANITILE_FORWARD | ANITILE_SMOKE_EFFECT | ANITILE_LOOPING;
  }
  else
  {
    AniParams.uiFlags = ANITILE_FORWARD | ANITILE_SMOKE_EFFECT | ANITILE_LOOPING | ANITILE_ALWAYS_TRANSLUCENT;
  }

	AniParams.sX									= CenterX( sGridNo );
	AniParams.sY									= CenterY( sGridNo );
	AniParams.sZ									= (INT16)0;

	// Use the right graphic based on type..
	switch( bType )
	{
		case NORMAL_SMOKE_EFFECT:

      if ( !( gGameSettings.fOptions[ TOPTION_ANIMATE_SMOKE ] ) )
      {
			   AniParams.zCachedFile = "TILECACHE/smkechze.sti";
      }
      else
      {
        if ( fDissipating )
        {
			    AniParams.zCachedFile = "TILECACHE/smalsmke.sti";
        }
        else
        {
			    AniParams.zCachedFile = "TILECACHE/smoke.sti";
        }
      }
			break;

		case TEARGAS_SMOKE_EFFECT:

      if ( !( gGameSettings.fOptions[ TOPTION_ANIMATE_SMOKE ] ) )
      {
			   AniParams.zCachedFile = "TILECACHE/tearchze.sti";
      }
      else
      {
        if ( fDissipating )
        {
			    AniParams.zCachedFile = "TILECACHE/smaltear.sti";
        }
        else
        {
			    AniParams.zCachedFile = "TILECACHE/teargas.sti";
        }
      }
			break;

		case MUSTARDGAS_SMOKE_EFFECT:

      if ( !( gGameSettings.fOptions[ TOPTION_ANIMATE_SMOKE ] ) )
      {
			   AniParams.zCachedFile = "TILECACHE/mustchze.sti";
      }
      else
      {
        if ( fDissipating )
        {
			    AniParams.zCachedFile = "TILECACHE/smalmust.sti";
        }
        else
        {
			    AniParams.zCachedFile = "TILECACHE/mustard2.sti";
        }
      }
			break;

		case CREATURE_SMOKE_EFFECT:

      if ( !( gGameSettings.fOptions[ TOPTION_ANIMATE_SMOKE ] ) )
      {
			   AniParams.zCachedFile = "TILECACHE/spit_gas.sti";
      }
      else
      {
        if ( fDissipating )
        {
			    AniParams.zCachedFile = "TILECACHE/spit_gas.sti";
        }
        else
        {
			    AniParams.zCachedFile = "TILECACHE/spit_gas.sti";
        }
      }
			break;

	}

	// Create tile...
	pAniTile = CreateAnimationTile( &AniParams );

	// Set world flags
	gpWorldLevelData[ sGridNo ].ubExtFlags[ bLevel ] |= FromSmokeTypeToWorldFlags( bType );

	// All done...

	// Re-draw..... :(
	SetRenderFlags(RENDER_FLAG_FULL);
}

void RemoveSmokeEffectFromTile( INT16 sGridNo, INT8 bLevel )
{
	ANITILE *pAniTile;
	UINT8		ubLevelID;

	// Get ANI tile...
	if ( bLevel == 0 )
	{
		ubLevelID = ANI_STRUCT_LEVEL;
	}
	else
	{
		ubLevelID = ANI_ONROOF_LEVEL;
	}

	pAniTile = GetCachedAniTileOfType( sGridNo, ubLevelID, ANITILE_SMOKE_EFFECT );

	if ( pAniTile != NULL )
	{
		DeleteAniTile( pAniTile );

		SetRenderFlags( RENDER_FLAG_FULL );
	}

	// Unset flags in world....
	// ( // check to see if we are the last one....
	if ( GetCachedAniTileOfType( sGridNo, ubLevelID, ANITILE_SMOKE_EFFECT ) == NULL )
	{
		gpWorldLevelData[ sGridNo ].ubExtFlags[ bLevel ] &= ( ~ANY_SMOKE_EFFECT );
	}
}


void DecaySmokeEffects( UINT32 uiTime )
{
  BOOLEAN fUpdate = FALSE;
  BOOLEAN fSpreadEffect;
  INT8    bLevel;
  UINT16   usNumUpdates = 1;

	// reset 'hit by gas' flags
	FOR_ALL_MERCS(i) (*i)->fHitByGasFlags = 0;

  // ATE: 1 ) make first pass and delete/mark any smoke effect for update
  // all the deleting has to be done first///

  // age all active tear gas clouds, deactivate those that are just dispersing
	FOR_ALL_SMOKE_EFFECTS(pSmoke)
  {
		fSpreadEffect = TRUE;

		if ( pSmoke->bFlags & SMOKE_EFFECT_ON_ROOF )
		{
			bLevel = 1;
		}
		else
		{
			bLevel = 0;
		}


		// Do things differently for combat /vs realtime
		// always try to update during combat
		if (gTacticalStatus.uiFlags & INCOMBAT )
		{
			fUpdate = TRUE;
		}
		else
		{
			// ATE: Do this every so ofte, to acheive the effect we want...
			if ( ( uiTime - pSmoke->uiTimeOfLastUpdate ) > 10 )
			{
				fUpdate = TRUE;

				usNumUpdates = ( UINT16 ) ( ( uiTime - pSmoke->uiTimeOfLastUpdate ) / 10 );
			}
		}

		if ( fUpdate )
		{
			pSmoke->uiTimeOfLastUpdate = uiTime;

			for (UINT32 cnt2 = 0; cnt2 < usNumUpdates; ++cnt2)
			{
				pSmoke->bAge++;

				if ( pSmoke->bAge == 1 )
				{
					// ATE: At least mark for update!
					pSmoke->bFlags |= SMOKE_EFFECT_MARK_FOR_UPDATE;
					fSpreadEffect = FALSE;
				}
				else
				{
					fSpreadEffect = TRUE;
				}

				if ( fSpreadEffect )
				{
					// if this cloud remains effective (duration not reached)
					if ( pSmoke->bAge <= pSmoke->ubDuration)
					{
						// ATE: Only mark now and increse radius - actual drawing is done
						// in another pass cause it could
						// just get erased...
						pSmoke->bFlags |= SMOKE_EFFECT_MARK_FOR_UPDATE;

						// calculate the new cloud radius
						// cloud expands by 1 every turn outdoors, and every other turn indoors

						// ATE: If radius is < maximun, increase radius, otherwise keep at max
						if ( pSmoke->ubRadius < Explosive[ Item[ pSmoke->usItem ].ubClassIndex ].ubRadius )
						{
							pSmoke->ubRadius++;
						}
					}
					else
					{
						// deactivate tear gas cloud (use last known radius)
						SpreadEffectSmoke(pSmoke, ERASE_SPREAD_EFFECT, bLevel);
						pSmoke->fAllocated = FALSE;
						break;
					}
				}
			}
		}
		else
		{
			// damage anyone standing in cloud
			SpreadEffectSmoke(pSmoke, REDO_SPREAD_EFFECT, 0);
		}
  }

	FOR_ALL_SMOKE_EFFECTS(pSmoke)
	{
		if ( pSmoke->bFlags & SMOKE_EFFECT_ON_ROOF )
		{
			bLevel = 1;
		}
		else
		{
			bLevel = 0;
		}

		// if this cloud remains effective (duration not reached)
		if ( pSmoke->bFlags & SMOKE_EFFECT_MARK_FOR_UPDATE )
		{
			SpreadEffectSmoke(pSmoke, TRUE, bLevel);
			pSmoke->bFlags &= (~SMOKE_EFFECT_MARK_FOR_UPDATE);
		}
  }

	AllTeamsLookForAll( TRUE );
}


void LoadSmokeEffectsFromLoadGameFile(HWFILE const hFile)
{
	UINT32	uiCnt=0;

	//no longer need to load smoke effects.  They are now in temp files
	if( guiSaveGameVersion < 75 )
	{
		//Clear out the old list
		memset( gSmokeEffectData, 0, sizeof( SMOKEEFFECT ) * NUM_SMOKE_EFFECT_SLOTS );

		//Load the Number of Smoke Effects
		FileRead(hFile, &guiNumSmokeEffects, sizeof(UINT32));

		//This is a TEMP hack to allow us to use the saves
		if( guiSaveGameVersion < 37 && guiNumSmokeEffects == 0 )
		{
			ExtractSmokeEffectFromFile(hFile, &gSmokeEffectData[0]);
		}


		//loop through and load the list
		for( uiCnt=0; uiCnt<guiNumSmokeEffects;uiCnt++)
		{
			ExtractSmokeEffectFromFile(hFile, &gSmokeEffectData[uiCnt]);
			//This is a TEMP hack to allow us to use the saves
			if( guiSaveGameVersion < 37 )
				break;
		}

		//loop through and apply the smoke effects to the map
		FOR_ALL_SMOKE_EFFECTS(s)
		{
			const INT8 bLevel = (s->bFlags & SMOKE_EFFECT_ON_ROOF ? 1 : 0);
			SpreadEffectSmoke(s, TRUE, bLevel);
		}
	}
}


BOOLEAN SaveSmokeEffectsToMapTempFile( INT16 sMapX, INT16 sMapY, INT8 bMapZ )
try
{
	UINT32	uiNumSmokeEffects=0;
	CHAR8		zMapName[ 128 ];

	//get the name of the map
	GetMapTempFileName( SF_SMOKE_EFFECTS_TEMP_FILE_EXISTS, zMapName, sMapX, sMapY, bMapZ );

	//delete file the file.
	FileDelete( zMapName );

	//loop through and count the number of smoke effects
	CFOR_ALL_SMOKE_EFFECTS(s) ++uiNumSmokeEffects;

	//if there are no smoke effects
	if( uiNumSmokeEffects == 0 )
	{
		//set the fact that there are no smoke effects for this sector
		ReSetSectorFlag( sMapX, sMapY, bMapZ, SF_SMOKE_EFFECTS_TEMP_FILE_EXISTS );

		return( TRUE );
	}

	AutoSGPFile hFile(FileOpen(zMapName, FILE_ACCESS_WRITE | FILE_CREATE_ALWAYS));

	//Save the Number of Smoke Effects
	FileWrite(hFile, &uiNumSmokeEffects, sizeof(UINT32));

	CFOR_ALL_SMOKE_EFFECTS(s)
	{
		InjectSmokeEffectIntoFile(hFile, s);
	}

	SetSectorFlag( sMapX, sMapY, bMapZ, SF_SMOKE_EFFECTS_TEMP_FILE_EXISTS );
	return TRUE;
}
catch (...) { return FALSE; }


BOOLEAN LoadSmokeEffectsFromMapTempFile( INT16 sMapX, INT16 sMapY, INT8 bMapZ )
try
{
	UINT32	uiCnt=0;
	CHAR8		zMapName[ 128 ];

	GetMapTempFileName( SF_SMOKE_EFFECTS_TEMP_FILE_EXISTS, zMapName, sMapX, sMapY, bMapZ );

	AutoSGPFile hFile(FileOpen(zMapName, FILE_ACCESS_READ));

	//Clear out the old list
	ResetSmokeEffects();


	//Load the Number of Smoke Effects
	FileRead(hFile, &guiNumSmokeEffects, sizeof(UINT32));

	//loop through and load the list
	for( uiCnt=0; uiCnt<guiNumSmokeEffects;uiCnt++)
	{
		ExtractSmokeEffectFromFile(hFile, &gSmokeEffectData[uiCnt]);
	}

	//loop through and apply the smoke effects to the map
	FOR_ALL_SMOKE_EFFECTS(s)
	{
		const INT8 bLevel = (s->bFlags & SMOKE_EFFECT_ON_ROOF ? 1 : 0);
		SpreadEffectSmoke(s, TRUE, bLevel);
	}

	return TRUE;
}
catch (...) { return FALSE; }


void ResetSmokeEffects()
{
	//Clear out the old list
	memset( gSmokeEffectData, 0, sizeof( SMOKEEFFECT ) * NUM_SMOKE_EFFECT_SLOTS );
	guiNumSmokeEffects = 0;
}


void UpdateSmokeEffectGraphics( )
{
	FOR_ALL_SMOKE_EFFECTS(s)
	{
		const INT8 bLevel = (s->bFlags & SMOKE_EFFECT_ON_ROOF ? 1 : 0);
		SpreadEffectSmoke(s, ERASE_SPREAD_EFFECT, bLevel);
		SpreadEffectSmoke(s, TRUE,                bLevel);
  }
}
