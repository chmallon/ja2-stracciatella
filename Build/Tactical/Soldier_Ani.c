#include "Font_Control.h"
#include "Merc_Hiring.h"
#include "Overhead.h"
#include "Soldier_Find.h"
#include "WCheck.h"
#include "Debug.h"
#include "MemMan.h"
#include "Overhead_Types.h"
#include "Soldier_Control.h"
#include "Animation_Cache.h"
#include "Animation_Data.h"
#include "Animation_Control.h"
#include "Weapons.h"
#include "Soldier_Ani.h"
#include "Random.h"
#include "Sound_Control.h"
#include "Isometric_Utils.h"
#include "Handle_UI.h"
#include "Event_Pump.h"
#include "OppList.h"
#include "Lighting.h"
#include "AI.h"
#include "RenderWorld.h"
#include "Interactive_Tiles.h"
#include "Points.h"
#include "Message.h"
#include "World_Items.h"
#include "Physics.h"
#include "Soldier_Create.h"
#include "Dialogue_Control.h"
#include "Rotting_Corpses.h"
#include "Merc_Entering.h"
#include "Soldier_Add.h"
#include "Soldier_Profile.h"
#include "Soldier_Functions.h"
#include "Interface.h"
#include "QArray.h"
#include "Soldier_Macros.h"
#include "Strategic_Town_Loyalty.h"
#include "Squads.h"
#include "WorldMan.h"
#include "Structure_Wrap.h"
#include "PathAI.h"
#include "Pits.h"
#include "Text.h"
#include "NPC.h"
#include "Meanwhile.h"
#include "Explosion_Control.h"
#include "FOV.h"
#include "Campaign.h"
#include "LOS.h"
#include "GameSettings.h"
#include "Boxing.h"
#include "Drugs_And_Alcohol.h"
#include "Smell.h"
#include "Strategic_Status.h"
#include "Interface_Dialogue.h"
#include "SoundMan.h"
#include "JAScreens.h"
#include "ScreenIDs.h"
#include "Items.h"
#include "Shading.h"


#define		NO_JUMP											0
#define		MAX_ANIFRAMES_PER_FLASH			2
//#define		TIME_FOR_RANDOM_ANIM_CHECK	10
#define		TIME_FOR_RANDOM_ANIM_CHECK	2

const SOLDIERTYPE* gLastMercTalkedAboutKilling = NULL;


static const DOUBLE gHopFenceForwardSEDist[NUMSOLDIERBODYTYPES]     = { 2.2, 0.7, 3.2, 0.7 };
static const DOUBLE gHopFenceForwardNWDist[NUMSOLDIERBODYTYPES]     = { 2.7, 1.0, 2.7, 1.0 };
static const DOUBLE gHopFenceForwardFullSEDist[NUMSOLDIERBODYTYPES] = { 1.1, 1.0, 2.1, 1.1 };
static const DOUBLE gHopFenceForwardFullNWDist[NUMSOLDIERBODYTYPES] = { 0.8, 0.2, 2.7, 0.8 };
static const DOUBLE gFalloffBackwardsDist[NUMSOLDIERBODYTYPES]      = { 1, 0.8, 1, 1 };
static const DOUBLE gClimbUpRoofDist[NUMSOLDIERBODYTYPES]           = { 2, 0.1, 2, 2 };
static const DOUBLE gClimbUpRoofLATDist[NUMSOLDIERBODYTYPES]        = { 0.7, 0.5, 0.7, 0.5 };
static const DOUBLE gClimbDownRoofStartDist[NUMSOLDIERBODYTYPES]    = { 5.0, 1.0, 1, 1 };
static const DOUBLE gClimbUpRoofDistGoingLower[NUMSOLDIERBODYTYPES] = { 0.9, 0.1, 1, 1 };


static void CheckForAndHandleSoldierIncompacitated(SOLDIERTYPE* pSoldier);
static BOOLEAN CheckForImproperFireGunEnd(SOLDIERTYPE* pSoldier);
static BOOLEAN HandleUnjamAnimation(SOLDIERTYPE* pSoldier);


BOOLEAN AdjustToNextAnimationFrame( SOLDIERTYPE *pSoldier )
{
	EV_S_FIREWEAPON			SFireWeapon;

	UINT16				sNewAniFrame, anAniFrame;
	INT8					ubCurrentHeight;
	UINT16				usOldAnimState;
	static UINT32 uiJumpAddress = NO_JUMP;
	INT16					sNewGridNo;
	INT16					sX, sY;
	BOOLEAN				fStop;
	UINT32				cnt;
	UINT8					ubDiceRoll;						// Percentile dice roll
	UINT8					ubRandomHandIndex;		// Index value into random animation table to use base don what is in the guys hand...
	UINT16				usItem;
	RANDOM_ANI_DEF	*pAnimDef;
	UINT8					ubNewDirection;
	UINT8					ubDesiredHeight;
	BOOLEAN				bOKFireWeapon;
	BOOLEAN				bWeaponJammed;
	BOOLEAN				fFreeUpAttacker=FALSE;
  UINT16        usUIMovementMode;

  do
  {
		// Get new frame code
		sNewAniFrame = gusAnimInst[ pSoldier->usAnimState ][ pSoldier->usAniCode ];

		// Handle muzzel flashes
		if ( pSoldier->bMuzFlashCount > 0 )
		{
			// FLash for about 3 frames
			if ( pSoldier->bMuzFlashCount > MAX_ANIFRAMES_PER_FLASH )
			{
				pSoldier->bMuzFlashCount = 0;
				if ( pSoldier->iMuzFlash != -1 )
				{
					LightSpriteDestroy( pSoldier->iMuzFlash );
          pSoldier->iMuzFlash = -1;
				}
			}
			else
			{
				pSoldier->bMuzFlashCount++;
			}

		}

		if ( pSoldier->bBreathCollapsed )
		{
			// ATE: If we have fallen, and we can't get up... no
			// really, if we were told to collapse but have been hit after, don't
			// do anything...
			if ( gAnimControl[ pSoldier->usAnimState ].uiFlags & ( ANIM_HITSTOP | ANIM_HITFINISH ) )
			{
				pSoldier->bBreathCollapsed = FALSE;
			}
			else if ( pSoldier->bLife == 0 )
			{
				// Death takes precedence...
				pSoldier->bBreathCollapsed = FALSE;
			}
			else if ( pSoldier->usPendingAnimation == FALLFORWARD_ROOF || pSoldier->usPendingAnimation == FALLOFF || pSoldier->usAnimState == FALLFORWARD_ROOF || pSoldier->usAnimState == FALLOFF )
			{
				pSoldier->bBreathCollapsed = FALSE;
			}
			else
			{
				// Wait here until we are free....
				if ( !pSoldier->fInNonintAnim )
				{
					// UNset UI
					UnSetUIBusy( pSoldier->ubID );

					SoldierCollapse( pSoldier );

					pSoldier->bBreathCollapsed = FALSE;

					return( TRUE );
				}
			}
		}

		// Check for special code
		if ( sNewAniFrame < 399 )
		{

			// Adjust / set true ani frame
			// Use -1 because ani files are 1-based, these are 0-based
			ConvertAniCodeToAniFrame( pSoldier, (INT16)( sNewAniFrame - 1 ) );

			// Adjust frame control pos, and try again
			pSoldier->usAniCode++;
			break;
		}
		else if ( sNewAniFrame < 500 )
		{
			// Switch on special code
			switch( sNewAniFrame )
			{
				case 402:

					// DO NOT MOVE FOR THIS FRAME
					pSoldier->fPausedMove = TRUE;
					break;

				case 403:

					// MOVE GUY FORWARD SOME VALUE
					MoveMercFacingDirection( pSoldier, FALSE, (FLOAT)0.7 );

					break;

				case 404:

					// MOVE GUY BACKWARD SOME VALUE
					// Use same function as forward, but is -ve values!
					MoveMercFacingDirection( pSoldier , TRUE, (FLOAT)1 );
					break;

				case 405:

					return( TRUE );

				case 406:

					// Move merc up
					if ( pSoldier->bDirection == NORTH )
					{
						SetSoldierHeight( pSoldier, (FLOAT)(pSoldier->sHeightAdjustment + 2 )  );
					}
					else
					{
						SetSoldierHeight( pSoldier, (FLOAT)( pSoldier->sHeightAdjustment + 3 ) );
					}
					break;

				case 408:

					// CODE: SPECIAL MOVE CLIMB UP ROOF EVENT

		      // re-enable sight
		      gTacticalStatus.uiFlags &= (~DISALLOW_SIGHT);
					{
						INT16		sXPos, sYPos;

						//usNewGridNo = NewGridNo( (UINT16)pSoldier->sGridNo, (UINT16)DirectionInc( pSoldier->bDirection ) );
						ConvertMapPosToWorldTileCenter( pSoldier->sTempNewGridNo, &sXPos, &sYPos );
						EVENT_SetSoldierPosition( pSoldier, (FLOAT)sXPos, (FLOAT)sYPos );
					}
					// Move two CC directions
					EVENT_SetSoldierDirection(pSoldier, TwoCCDirection(pSoldier->bDirection));

					EVENT_SetSoldierDesiredDirection( pSoldier, pSoldier->bDirection );

					// Set desired anim height!
					pSoldier->ubDesiredHeight = ANIM_CROUCH;

					// Move merc up specific height
					SetSoldierHeight( pSoldier, (FLOAT)50 );

					// ATE: Change interface level.....
					// CJC: only if we are a player merc
					if (pSoldier->bTeam == gbPlayerNum)
					{

						if (gTacticalStatus.fAutoBandageMode)
						{
							// in autobandage, handle as AI, but add roof marker too
							FreeUpNPCFromRoofClimb( pSoldier );
							HandlePlacingRoofMarker( pSoldier, pSoldier->sGridNo, TRUE, TRUE );
						}
						else
						{
							// OK, UNSET INTERFACE FIRST
							UnSetUIBusy( pSoldier->ubID );

							if ( pSoldier->ubID == gusSelectedSoldier )
							{
								ChangeInterfaceLevel( 1 );
							}
							HandlePlacingRoofMarker( pSoldier, pSoldier->sGridNo, TRUE, TRUE );
						}
					}
					else
					{
						FreeUpNPCFromRoofClimb( pSoldier );
					}

					// ATE: Handle sight...
					HandleSight( pSoldier,SIGHT_LOOK | SIGHT_RADIO | SIGHT_INTERRUPT );
					break;

				case 409:

					//CODE: MOVE DOWN
					SetSoldierHeight( pSoldier, (FLOAT)( pSoldier->sHeightAdjustment - 2 ) );
					break;

				case 410:

					// Move merc down specific height
					SetSoldierHeight( pSoldier, (FLOAT)0 );
					break;

				case 411:

					// CODE: SPECIALMOVE CLIMB DOWN EVENT
					// Move two C directions
					EVENT_SetSoldierDirection(pSoldier, TwoCDirection(pSoldier->bDirection));

					EVENT_SetSoldierDesiredDirection( pSoldier, pSoldier->bDirection );
					// Adjust height
					SetSoldierHeight( pSoldier, (FLOAT)gClimbDownRoofStartDist[ pSoldier->ubBodyType ] );
					// Adjust position
					MoveMercFacingDirection( pSoldier , TRUE, (FLOAT)3.5 );
					break;

				case 412:

					// CODE: HANDLING PRONE DOWN - NEED TO MOVE GUY BACKWARDS!
					MoveMercFacingDirection( pSoldier , FALSE, (FLOAT).2 );
					break;

				case 413:

					// CODE: HANDLING PRONE UP - NEED TO MOVE GUY FORWARDS!
					MoveMercFacingDirection( pSoldier , TRUE, (FLOAT).2 );
					break;

				case 430:

					// SHOOT GUN
					// MAKE AN EVENT, BUT ONLY DO STUFF IF WE OWN THE GUY!
					SFireWeapon.usSoldierID			= pSoldier->ubID;
					SFireWeapon.uiUniqueId			= pSoldier->uiUniqueSoldierIdValue;
					SFireWeapon.sTargetGridNo		= pSoldier->sTargetGridNo;
					SFireWeapon.bTargetLevel		= pSoldier->bTargetLevel;
					SFireWeapon.bTargetCubeLevel= pSoldier->bTargetCubeLevel;
					AddGameEvent( S_FIREWEAPON, 0, &SFireWeapon );
					break;

				case 431:

					// FLASH FRAME WHITE
					pSoldier->pForcedShade = White16BPPPalette;
					break;

				case 432:

					// PLAY RANDOM IMPACT SOUND!
				//	PlayJA2Sample(BULLET_IMPACT_1 + Random(3), MIDVOLUME, 1, MIDDLEPAN);

					// PLAY RANDOM GETTING HIT SOUND
				//	DoMercBattleSound( pSoldier, BATTLE_SOUND_HIT1 );

					break;

				case 433:

					// CODE: GENERIC HIT!

					CheckForAndHandleSoldierIncompacitated( pSoldier );


					break;

				case 434:

					// JUMP TO ANOTHER ANIMATION ( BLOOD ) IF WE WANT BLOOD
					uiJumpAddress = pSoldier->usAnimState;
					ChangeSoldierState( pSoldier, FLYBACK_HIT_BLOOD_STAND, 0, FALSE );
					return( TRUE );

				case 435:

					// HOOK FOR A RETURN JUMP
					break;

				case 436:

					// Loop through script to find entry address
					if ( uiJumpAddress == NO_JUMP )
					{
						break;
					}
					usOldAnimState = pSoldier->usAnimState;
					pSoldier->usAniCode  = 0;

					do
					{
						// Get new frame code
						anAniFrame = gusAnimInst[ uiJumpAddress ][ pSoldier->usAniCode ];

						if ( anAniFrame == 435 )
						{
							// START PROCESSING HERE
							ChangeSoldierState( pSoldier, (UINT16)uiJumpAddress, pSoldier->usAniCode, FALSE );
							return( TRUE );
						}
						// Adjust frame control pos, and try again
						pSoldier->usAniCode++;
					}
					while( anAniFrame != 999 );

					uiJumpAddress = NO_JUMP;

					if ( anAniFrame == 999 )
					{
						// Fail jump, re-load old anim
						ChangeSoldierState( pSoldier, usOldAnimState, 0, FALSE );
						return( TRUE );
					}
					break;

				case 437:

					// CHANGE DIRECTION AND GET-UP
					//sGridNo = NewGridNo( (UINT16)pSoldier->sGridNo, (UINT16)(-1 * DirectionInc( pSoldier->bDirection ) ) );
					//ConvertMapPosToWorldTileCenter( pSoldier->sGridNo, &sXPos, &sYPos );
					//SetSoldierPosition( pSoldier, (FLOAT)sXPos, (FLOAT)sYPos );


					// Reverse direction
					EVENT_SetSoldierDirection(pSoldier, OppositeDirection(pSoldier->bDirection));
					EVENT_SetSoldierDesiredDirection( pSoldier, pSoldier->bDirection );

					ChangeSoldierState( pSoldier, GETUP_FROM_ROLLOVER, 0 , FALSE );
					return( TRUE );

				case 438:

					//CODE: START HOLD FLASH
					pSoldier->fForceShade = TRUE;
					break;

				case 439:

					//CODE: END HOLD FLASH
					pSoldier->fForceShade = FALSE;
					break;

				case 440:
					//CODE: Set buddy as dead!
					{
						BOOLEAN fMadeCorpse;

						// ATE: Piggyback here on stopping the burn sound...
						if ( pSoldier->usAnimState == CHARIOTS_OF_FIRE ||
								 pSoldier->usAnimState == BODYEXPLODING )
						{
							SoundStop( pSoldier->uiPendingActionData1 );
						}


						CheckForAndHandleSoldierDeath( pSoldier, &fMadeCorpse );

						if ( fMadeCorpse )
						{
							return( FALSE );
						}
						else
						{
							return( TRUE );
						}
					}

				case 441:
					// CODE: Show mussel flash
					if ( pSoldier->bVisible == -1 )
					{
						break;
					}

					// DO ONLY IF WE'RE AT A GOOD LEVEL
					if ( ubAmbientLightLevel < MIN_AMB_LEVEL_FOR_MERC_LIGHTS )
					{
						break;
					}

					if( ( pSoldier->iMuzFlash=LightSpriteCreate("L-R03.LHT", 0 ) )==(-1))
					{
						return( TRUE );
					}

					LightSpritePower(pSoldier->iMuzFlash, TRUE);
					// Get one move forward
					{
						UINT16	usNewGridNo;
						INT16 sXPos, sYPos;

						usNewGridNo = NewGridNo( (UINT16)pSoldier->sGridNo, DirectionInc( pSoldier->bDirection ) );
						ConvertGridNoToCenterCellXY( usNewGridNo, &sXPos, &sYPos );
						LightSpritePosition( pSoldier->iMuzFlash, (INT16)(sXPos/CELL_X_SIZE), (INT16)(sYPos/CELL_Y_SIZE));

						// Start count
						pSoldier->bMuzFlashCount = 1;
					}
					break;

				case 442:

				  //CODE: FOR A NON-INTERRUPTABLE SCRIPT - SIGNAL DONE
					pSoldier->fInNonintAnim = FALSE;

          // ATE: if it's the begin cower animation, unset ui, cause it could
          // be from player changin stance
          if ( pSoldier->usAnimState == START_COWER )
          {
            UnSetUIBusy( pSoldier->ubID );
          }
					break;

				case 443:

					// MOVE GUY FORWARD FOR FENCE HOP ANIMATION
					switch( pSoldier->bDirection )
					{
						case SOUTH:
						case EAST:

							MoveMercFacingDirection( pSoldier, FALSE, (FLOAT)gHopFenceForwardSEDist[ pSoldier->ubBodyType ] );
							break;

						case NORTH:
						case WEST:
							MoveMercFacingDirection( pSoldier, FALSE, (FLOAT)gHopFenceForwardNWDist[ pSoldier->ubBodyType ] );
							break;
					}
					break;

				case 444:

					// CODE: End Hop Fence
					// MOVE TO FORCASTED GRIDNO
					sX = CenterX( pSoldier->sForcastGridno );
					sY = CenterY( pSoldier->sForcastGridno );

					EVENT_InternalSetSoldierPosition( pSoldier, (FLOAT) sX, (FLOAT) sY, FALSE, FALSE, FALSE );
					EVENT_SetSoldierDirection(pSoldier, TwoCDirection(pSoldier->bDirection));
					pSoldier->sZLevelOverride = -1;
					EVENT_SetSoldierDesiredDirection( pSoldier, pSoldier->bDirection );


					if ( gTacticalStatus.bBoxingState == BOXING_WAITING_FOR_PLAYER || gTacticalStatus.bBoxingState == PRE_BOXING || gTacticalStatus.bBoxingState == BOXING )
					{
						BoxingMovementCheck( pSoldier );
					}

					if (SetOffBombsInGridNo(pSoldier, pSoldier->sGridNo, FALSE, pSoldier->bLevel))
					{
						EVENT_StopMerc( pSoldier, pSoldier->sGridNo, pSoldier->bDirection );
						return( TRUE );
					}

					// If we are at our final destination, goto stance of our movement stance...
					// Only in realtime...
					//if ( !( gTacticalStatus.uiFlags & INCOMBAT ) )
					// This has to be here to make guys continue along fence path
					if ( pSoldier->sGridNo == pSoldier->sFinalDestination )
					{
						if ( gAnimControl[ pSoldier->usAnimState ].ubEndHeight != gAnimControl[ pSoldier->usUIMovementMode ].ubEndHeight )
						{
							// Goto Stance...
							pSoldier->fDontChargeAPsForStanceChange = TRUE;
							ChangeSoldierStance( pSoldier, gAnimControl[ pSoldier->usUIMovementMode ].ubEndHeight );
							return( TRUE );
						}
						else
						{
							SoldierGotoStationaryStance( pSoldier );

							// Set UI Busy
							UnSetUIBusy( pSoldier->ubID );
							return( TRUE );
						}
					}
					break;

				case 445:

					// CODE: MOVE GUY FORWARD ONE TILE, BASED ON WHERE WE ARE FACING
					switch( pSoldier->bDirection )
					{
						case SOUTH:
						case EAST:

							MoveMercFacingDirection( pSoldier, FALSE, (FLOAT)gHopFenceForwardFullSEDist[ pSoldier->ubBodyType ] );
							break;

						case NORTH:
						case WEST:

							MoveMercFacingDirection( pSoldier, FALSE, (FLOAT)gHopFenceForwardFullNWDist[ pSoldier->ubBodyType ] );
							break;

					}
					break;

				case 446:

					// CODE: Turn pause move flag on
					pSoldier->uiStatusFlags |= SOLDIER_PAUSEANIMOVE;
					break;

				case 447:

					// TRY TO FALL!!!
					if ( pSoldier ->fTryingToFall )
					{
						INT16 sLastAniFrame;

						// TRY FORWARDS...
						// FIRST GRIDNO
						sNewGridNo = NewGridNo( (UINT16)pSoldier->sGridNo, (UINT16)( DirectionInc( pSoldier->bDirection ) ) );

						if ( OKFallDirection( pSoldier, sNewGridNo, pSoldier->bLevel, pSoldier->bDirection, FALLFORWARD_HITDEATH_STOP ) )
						{
							// SECOND GRIDNO
							// sNewGridNo = NewGridNo( (UINT16)sNewGridNo, (UINT16)( DirectionInc( pSoldier->bDirection ) ) );

							// if ( OKFallDirection( pSoldier, sNewGridNo, pSoldier->bLevel, pSoldier->bDirection, FALLFORWARD_HITDEATH_STOP ) )
							{
								// ALL'S OK HERE...
								pSoldier->fTryingToFall = FALSE;
								break;
							}
						}


						// IF HERE, INCREMENT DIRECTION
            // ATE: Added Feb1 - can be either direction....
            if ( pSoldier->fFallClockwise )
            {
						  EVENT_SetSoldierDirection(pSoldier, OneCDirection(pSoldier->bDirection));
            }
            else
            {
						  EVENT_SetSoldierDirection(pSoldier, OneCCDirection(pSoldier->bDirection));
            }
						EVENT_SetSoldierDesiredDirection( pSoldier, pSoldier->bDirection );
						sLastAniFrame = gusAnimInst[ pSoldier->usAnimState ][ ( pSoldier->usAniCode - 2 ) ];
						ConvertAniCodeToAniFrame( pSoldier, (INT16)( sLastAniFrame ) );

						if ( pSoldier->bDirection == pSoldier->bStartFallDir )
						{
							// GO FORWARD HERE...
							pSoldier ->fTryingToFall = FALSE;
							break;;
						}
						// IF HERE, RETURN SO WE DONOT INCREMENT DIR
						return( TRUE );
					}
					break;


				case 448:

					// CODE: HANDLE BURST
					// FIRST CHECK IF WE'VE REACHED MAX FOR GUN
					fStop = FALSE;

					if ( pSoldier->bDoBurst > Weapon[ pSoldier->usAttackingWeapon ].ubShotsPerBurst )
					{
						fStop = TRUE;
						fFreeUpAttacker = TRUE;
					}

					// CHECK IF WE HAVE AMMO LEFT, IF NOT, END ANIMATION!
					if ( !EnoughAmmo( pSoldier, FALSE, pSoldier->ubAttackingHand ) )
					{
						fStop = TRUE;
						fFreeUpAttacker = TRUE;
						if ( pSoldier->bTeam == gbPlayerNum	 )
						{
							ScreenMsg( FONT_MCOLOR_LTYELLOW, MSG_INTERFACE, TacticalStr[ BURST_FIRE_DEPLETED_CLIP_STR ] );
						}
					}
					else if (pSoldier->bDoBurst == 1)
					{
						// CHECK FOR GUN JAM
						bWeaponJammed = CheckForGunJam( pSoldier );
						if ( bWeaponJammed == TRUE )
						{
							fStop = TRUE;
							fFreeUpAttacker = TRUE;
							// stop shooting!
							pSoldier->bBulletsLeft = 0;

							// OK, Stop burst sound...
							if ( pSoldier->iBurstSoundID != NO_SAMPLE )
							{
								SoundStop( pSoldier->iBurstSoundID );
							}

							if ( pSoldier->bTeam == gbPlayerNum	 )
							{
								PlayLocationJA2Sample(pSoldier->sGridNo, S_DRYFIRE1, MIDVOLUME, 1);
								//ScreenMsg( FONT_MCOLOR_LTYELLOW, MSG_INTERFACE, L"Gun jammed!" );
							}

							DebugMsg(TOPIC_JA2, DBG_LEVEL_3, "@@@@@@@ Freeing up attacker - aborting start of attack due to burst gun jam");
							FreeUpAttacker(pSoldier);
						}
						else if ( bWeaponJammed == 255 )
						{
							// Play intermediate animation...
							if ( HandleUnjamAnimation( pSoldier ) )
							{
								return( TRUE );
							}
						}
					}

					if ( fStop )
					{
						pSoldier->fDoSpread = FALSE;
						pSoldier->bDoBurst = 1;
//						pSoldier->fBurstCompleted = TRUE;
						if ( fFreeUpAttacker )
						{
//							DebugMsg(TOPIC_JA2, DBG_LEVEL_3, "@@@@@@@ Freeing up attacker - aborting start of attack");
//							FreeUpAttacker(pSoldier);
						}

						// ATE; Reduce it due to animation being stopped...
						DebugMsg(TOPIC_JA2, DBG_LEVEL_3, "@@@@@@@ Freeing up attacker - Burst animation ended");
						ReduceAttackBusyCount(pSoldier, FALSE);


						if ( CheckForImproperFireGunEnd( pSoldier ) )
						{
							return( TRUE );
						}

						// END: GOTO AIM STANCE BASED ON HEIGHT
						// If we are a robot - we need to do stuff different here
						if ( AM_A_ROBOT( pSoldier ) )
						{
							ChangeSoldierState( pSoldier, STANDING, 0 , FALSE );
						}
						else
						{
							switch ( gAnimControl[ pSoldier->usAnimState ].ubEndHeight )
							{
								case ANIM_STAND:
									ChangeSoldierState( pSoldier, AIM_RIFLE_STAND, 0 , FALSE );
									break;

								case ANIM_PRONE:
									ChangeSoldierState( pSoldier, AIM_RIFLE_PRONE, 0 , FALSE );
									break;

								case ANIM_CROUCH:
									ChangeSoldierState( pSoldier, AIM_RIFLE_CROUCH, 0 , FALSE );
									break;

							}
						}
						return( TRUE );
					}

					// MOVETO CURRENT SPREAD LOCATION
					if ( pSoldier->fDoSpread )
					{
						if ( pSoldier->sSpreadLocations[ pSoldier->fDoSpread - 1 ] != 0 )
						{
							EVENT_SetSoldierDirection( pSoldier, (INT8)GetDirectionToGridNoFromGridNo( pSoldier->sGridNo, pSoldier->sSpreadLocations[ pSoldier->fDoSpread - 1 ] ) );
							EVENT_SetSoldierDesiredDirection( pSoldier, pSoldier->bDirection );
						}
					}
					break;

				case 449:

					//CODE: FINISH BURST
					pSoldier->fDoSpread = FALSE;
					pSoldier->bDoBurst = 1;
//				pSoldier->fBurstCompleted = TRUE;
					break;

				case 450:

					//CODE: BEGINHOPFENCE
					// MOVE TWO FACGIN GRIDNOS
					sNewGridNo = NewGridNo( (UINT16)pSoldier->sGridNo, (UINT16)( DirectionInc( pSoldier->bDirection ) ) );
					sNewGridNo = NewGridNo( (UINT16)sNewGridNo, (UINT16)( DirectionInc( pSoldier->bDirection ) ) );
					pSoldier->sForcastGridno = sNewGridNo;
					break;


				case 451:

					// CODE: MANAGE START z-buffer override
					switch( pSoldier->bDirection )
					{
						case NORTH:
						case WEST:

							pSoldier->sZLevelOverride = TOPMOST_Z_LEVEL;
							break;
					}
					break;

				case 452:

					// CODE: MANAGE END z-buffer override
					switch( pSoldier->bDirection )
					{
						case SOUTH:
						case EAST:

							pSoldier->sZLevelOverride = TOPMOST_Z_LEVEL;
							break;

						case NORTH:
						case WEST:

							pSoldier->sZLevelOverride = -1;
							break;

					}
					break;

				case 453:

					//CODE: FALLOFF ROOF ( BACKWARDS ) - MOVE BACK SOME!
					// Use same function as forward, but is -ve values!
					MoveMercFacingDirection( pSoldier , TRUE, (FLOAT)gFalloffBackwardsDist[ pSoldier->ubBodyType ] );
					break;

				case 454:

					// CODE: HANDLE CLIMBING ROOF,
					// Move merc up
					if ( pSoldier->bDirection == NORTH )
					{
						SetSoldierHeight( pSoldier, (FLOAT)(pSoldier->dHeightAdjustment + gClimbUpRoofDist[ pSoldier->ubBodyType ] ) );
					}
					else
					{
						SetSoldierHeight( pSoldier, (FLOAT)(pSoldier->dHeightAdjustment + gClimbUpRoofDist[ pSoldier->ubBodyType ] ) );
					}
					break;

				case 455:

					// MOVE GUY FORWARD SOME VALUE
					MoveMercFacingDirection( pSoldier, FALSE, (FLOAT)gClimbUpRoofLATDist[ pSoldier->ubBodyType ] );

					// MOVE DOWN SOME VALUE TOO!
					SetSoldierHeight( pSoldier, (FLOAT)(pSoldier->dHeightAdjustment - gClimbUpRoofDistGoingLower[ pSoldier->ubBodyType ] ) );

					break;

				case 456:

					// CODE: HANDLE CLIMBING ROOF,
					// Move merc DOWN
					if ( pSoldier->bDirection == NORTH )
					{
						SetSoldierHeight( pSoldier, (FLOAT)(pSoldier->dHeightAdjustment - gClimbUpRoofDist[ pSoldier->ubBodyType ] ) );
					}
					else
					{
						SetSoldierHeight( pSoldier, (FLOAT)(pSoldier->dHeightAdjustment - gClimbUpRoofDist[ pSoldier->ubBodyType ] ) );
					}
					break;

				case 457:

					// CODE: CHANGCE STANCE TO STANDING
					ChangeSoldierStance(pSoldier, ANIM_STAND);
					break;

				case 459:

					// CODE: CHANGE ATTACKING TO FIRST HAND
					pSoldier->ubAttackingHand = HANDPOS;
					pSoldier->usAttackingWeapon = pSoldier->inv[pSoldier->ubAttackingHand].usItem;
					// Adjust fReloading to FALSE
					pSoldier->fReloading = FALSE;
					break;

				case 458:

					// CODE: CHANGE ATTACKING TO SECOND HAND
					pSoldier->ubAttackingHand = SECONDHANDPOS;
					pSoldier->usAttackingWeapon = pSoldier->inv[pSoldier->ubAttackingHand].usItem;
					// Adjust fReloading to FALSE
					pSoldier->fReloading = FALSE;
					break;

				case 460:
				case 461:

					//CODE: THORW ITEM
					// Launch ITem!
					if ( pSoldier->pTempObject != NULL && pSoldier->pThrowParams != NULL )
					{
						// ATE: If we are armmed...
						if ( pSoldier->pThrowParams->ubActionCode == THROW_ARM_ITEM )
						{
							// ATE: Deduct points!
							DeductPoints( pSoldier, MinAPsToThrow( pSoldier, pSoldier->sTargetGridNo, FALSE ), 0 );
						}
						else
						{
							// ATE: Deduct points!
							DeductPoints( pSoldier, AP_TOSS_ITEM, 0 );
						}

						CreatePhysicalObject( pSoldier->pTempObject, pSoldier->pThrowParams->dLifeSpan,  pSoldier->pThrowParams->dX, pSoldier->pThrowParams->dY, pSoldier->pThrowParams->dZ, pSoldier->pThrowParams->dForceX, pSoldier->pThrowParams->dForceY, pSoldier->pThrowParams->dForceZ, pSoldier->ubID, pSoldier->pThrowParams->ubActionCode, pSoldier->pThrowParams->uiActionData );

						// Remove object
						//RemoveObjFrom( &(pSoldier->inv[ HANDPOS ] ), 0 );

						// Update UI
						DirtyMercPanelInterface( pSoldier, DIRTYLEVEL2 );

						MemFree( pSoldier->pTempObject );
						pSoldier->pTempObject = NULL;

						MemFree( pSoldier->pThrowParams );
						pSoldier->pThrowParams = NULL;
					}
					break;

				case 462:

					// CODE: MOVE UP FROM CLIFF CLIMB
					pSoldier->dHeightAdjustment += (float)2.1;
					pSoldier->sHeightAdjustment = (INT16)pSoldier->dHeightAdjustment;
					// Move over some...
					//MoveMercFacingDirection( pSoldier , FALSE, (FLOAT)0.5 );
					break;

				case 463:

					// MOVE GUY FORWARD SOME VALUE
					// Creature move
					MoveMercFacingDirection( pSoldier, FALSE, (FLOAT)1.5 );
					break;

				case 464:

					// CODE: END CLIFF CLIMB
					pSoldier->dHeightAdjustment = (float)0;
					pSoldier->sHeightAdjustment = (INT16)pSoldier->dHeightAdjustment;

					// Set new gridno
					{
						INT16 sTempGridNo, sNewX, sNewY;

						//Get Next GridNo;
						sTempGridNo = NewGridNo( (UINT16)pSoldier->sGridNo, (INT16)( DirectionInc(pSoldier->bDirection ) ) );

						// Get center XY
						ConvertGridNoToCenterCellXY( sTempGridNo, &sNewX, &sNewY );

						// Set position
						EVENT_SetSoldierPosition( pSoldier, sNewX, sNewY );

						// Move two CC directions
						EVENT_SetSoldierDirection(pSoldier, TwoCCDirection(pSoldier->bDirection));
						EVENT_SetSoldierDesiredDirection( pSoldier, pSoldier->bDirection );

						// Set desired anim height!
						pSoldier->ubDesiredHeight = ANIM_CROUCH;
						pSoldier->sFinalDestination = pSoldier->sGridNo;

					}
					break;

				case 465:

					// CODE: SET GUY TO LIFE OF 0
					pSoldier->bLife = 0;
					break;

				case 466:

					// CODE: ADJUST TO OUR DEST HEIGHT
					if ( pSoldier->sHeightAdjustment != pSoldier->sDesiredHeight )
					{
						INT16 sDiff = pSoldier->sHeightAdjustment - pSoldier->sDesiredHeight;

						if ( abs( sDiff ) > 4 )
						{
							if ( sDiff > 0 )
							{
								// Adjust!
								SetSoldierHeight( pSoldier, (FLOAT)(pSoldier->dHeightAdjustment - 2 ) );
							}
							else
							{
								// Adjust!
								SetSoldierHeight( pSoldier, (FLOAT)(pSoldier->dHeightAdjustment + 2 ) );
							}
						}
						else
						{
							// Adjust!
							SetSoldierHeight( pSoldier, (FLOAT)(pSoldier->sDesiredHeight) );
						}
					}
					else
					{
						// Goto eating animation
						if ( pSoldier->sDesiredHeight == 0 )
						{
							ChangeSoldierState( pSoldier, CROW_EAT, 0 , FALSE );
						}
						else
						{
							// We should leave now!
							TacticalRemoveSoldier(pSoldier);
							return( FALSE );
						}
						return( TRUE );
					}
					break;

				case 467:

					///CODE: FOR HELIDROP, SET DIRECTION
					EVENT_SetSoldierDirection( pSoldier, EAST );
					EVENT_SetSoldierDesiredDirection( pSoldier, pSoldier->bDirection );

					gfIngagedInDrop = FALSE;

					// OK, now get a sweetspot ( not the place we are now! )
				//	sNewGridNo =  FindGridNoFromSweetSpotExcludingSweetSpot( pSoldier, pSoldier->sGridNo, 5, &ubNewDirection );
					//sNewGridNo =  FindRandomGridNoFromSweetSpotExcludingSweetSpot( pSoldier, pSoldier->sGridNo, 3, &ubNewDirection );

					sNewGridNo =  FindGridNoFromSweetSpotExcludingSweetSpotInQuardent( pSoldier, pSoldier->sGridNo, 3, &ubNewDirection, SOUTHEAST );

					// Check for merc arrives quotes...
					HandleMercArrivesQuotes( pSoldier );

					// Find a path to it!
					EVENT_GetNewSoldierPath( pSoldier, sNewGridNo, WALKING );

					return( TRUE );

				case 468:

					// CODE: End PUNCH
					{
						BOOLEAN fNPCPunch = FALSE;

						// ATE: Put some code in for NPC punches...
						if ( pSoldier->uiStatusFlags & SOLDIER_NPC_DOING_PUNCH )
						{
							fNPCPunch = TRUE;

							// Turn off
							pSoldier->uiStatusFlags &= (~SOLDIER_NPC_DOING_PUNCH );

							// Trigger approach...
							TriggerNPCWithGivenApproach( pSoldier->ubProfile, (UINT8)pSoldier->uiPendingActionData4, FALSE );
						}


						// Are we a martial artist?
						{
							BOOLEAN fMartialArtist = FALSE;

							if ( pSoldier->ubProfile != NO_PROFILE )
							{
								if ( gMercProfiles[ pSoldier->ubProfile ].bSkillTrait == MARTIALARTS || gMercProfiles[ pSoldier->ubProfile ].bSkillTrait2 == MARTIALARTS )
								{
									fMartialArtist = TRUE;
								}
							}

							if ( gAnimControl[ pSoldier->usAnimState ].ubHeight == ANIM_CROUCH )
							{
								if ( fNPCPunch )
								{
									ChangeSoldierStance( pSoldier, ANIM_STAND );
									return( TRUE );
								}
								else
								{
									ChangeSoldierState( pSoldier, CROUCHING, 0, FALSE );
									return( TRUE );
								}
							}
							else
							{
								if ( fMartialArtist && !AreInMeanwhile( ) )
								{
									ChangeSoldierState( pSoldier, NINJA_BREATH, 0, FALSE );
									return( TRUE );
								}
								else
								{
									ChangeSoldierState( pSoldier, PUNCH_BREATH, 0, FALSE );
									return( TRUE );
								}
							}
						}
					}

				case 469:

					// CODE: Begin martial artist attack
					DoNinjaAttack( pSoldier );
					return( TRUE );

				case 470:

					// CODE: CHECK FOR OK WEAPON SHOT!
					bOKFireWeapon =  OKFireWeapon( pSoldier );

					if ( bOKFireWeapon == FALSE )
					{
						DebugMsg(TOPIC_JA2, DBG_LEVEL_3, "Fire Weapon: Gun Cannot fire, code 470");

						// OK, SKIP x # OF FRAMES
						// Skip 3 frames, ( a third ia added at the end of switch.. ) For a total of 4
						pSoldier->usAniCode += 4;

						// Reduce by a bullet...
						pSoldier->bBulletsLeft--;

						PlayLocationJA2Sample(pSoldier->sGridNo, S_DRYFIRE1, MIDVOLUME, 1);

						// Free-up!
						DebugMsg(TOPIC_JA2, DBG_LEVEL_3, "@@@@@@@ Freeing up attacker - gun failed to fire");
						FreeUpAttacker(pSoldier);
					}
					else if ( bOKFireWeapon == 255 )
					{
						// Play intermediate animation...
						if ( HandleUnjamAnimation( pSoldier ) )
						{
							return( TRUE );
						}
					}
					break;

				case 471:

					// CODE: Turn pause move flag off
					pSoldier->uiStatusFlags &= (~SOLDIER_PAUSEANIMOVE);
					break;

				case 472:

					{
						BOOLEAN fGoBackToAimAfterHit;

						// Save old flag, then reset. If we do nothing special here, at least go back
						// to aim if we were.
						fGoBackToAimAfterHit = pSoldier->fGoBackToAimAfterHit;
						pSoldier->fGoBackToAimAfterHit = FALSE;

						if ( !( pSoldier->uiStatusFlags & SOLDIER_TURNINGFROMHIT ) )
						{
							switch( gAnimControl[ pSoldier->usAnimState ].ubEndHeight )
							{
								case ANIM_STAND:

									// OK, we can do some cool things here - first is to make guy walkl back a bit...
								//	ChangeSoldierState( pSoldier, STANDING_HIT_BEGINCROUCHDOWN, 0, FALSE );
								//	return( TRUE );
									break;

							}
						}

						// CODE: HANDLE ANY RANDOM HIT VARIATIONS WE WISH TO DO.....
						if ( fGoBackToAimAfterHit )
						{
							 if ( pSoldier->bLife >= OKLIFE )
							 {
									InternalSoldierReadyWeapon( pSoldier, pSoldier->bDirection, FALSE );
							 }
							 return( TRUE );
						}
					}
					break;

				case 473:

					// CODE: CHECK IF WE HAVE JUST JAMMED / OUT OF AMMO, DONOT CONTINUE, BUT
					// GOTO STATIONARY ANIM
					if ( CheckForImproperFireGunEnd( pSoldier ) )
					{
						return( TRUE );
					}
					break;

				case 474:

					// CODE: GETUP FROM SLEEP
					ChangeSoldierStance( pSoldier, ANIM_STAND );
					return( TRUE );

				case 475:

					// CODE: END CLIMB DOWN ROOF
					pSoldier->ubDesiredHeight = ANIM_STAND;
					pSoldier->sFinalDestination = pSoldier->sGridNo;

		      // re-enable sight
		      gTacticalStatus.uiFlags &= (~DISALLOW_SIGHT);

					// ATE: Change interface level.....
					// CJC: only if we are a player merc
					if ( (pSoldier->bTeam == gbPlayerNum) && !gTacticalStatus.fAutoBandageMode)
					{
						if ( pSoldier->ubID == gusSelectedSoldier )
						{
							ChangeInterfaceLevel( 0 );
						}
						// OK, UNSET INTERFACE FIRST
						UnSetUIBusy( pSoldier->ubID );
					}
					else
					{
						FreeUpNPCFromRoofClimb( pSoldier );
					}
          pSoldier->usUIMovementMode = WALKING;

					// ATE: Handle sight...
					HandleSight( pSoldier,SIGHT_LOOK | SIGHT_RADIO | SIGHT_INTERRUPT );
					break;

				case 476:

					// CODE: GOTO PREVIOUS ANIMATION
					ChangeSoldierState( pSoldier, ( pSoldier->sPendingActionData2 ), (UINT8)( pSoldier->uiPendingActionData1 + 1 ), FALSE );
					return( TRUE );

				case 477:

					// CODE: Locate to target ( if an AI guy.. )
					if ( gTacticalStatus.uiFlags & TURNBASED && (gTacticalStatus.uiFlags & INCOMBAT ) )
					{
						if ( pSoldier->bTeam != gbPlayerNum )
						{
							// only locate if the enemy is visible or he's aiming at a player
							if (pSoldier->bVisible != -1 || (pSoldier->target != NULL && pSoldier->target->bTeam == gbPlayerNum))
							{
								LocateGridNo( pSoldier->sTargetGridNo );
							}
						}
					}
					break;

				case 478:

					// CODE: Decide to turn from hit.......
					{
						INT8		bNewDirection;
						UINT32	uiChance;

						// ONLY DO THIS IF CERTAIN CONDITIONS ARISE!
						// For one, only do for mercs!
						if ( pSoldier->ubBodyType <= REGFEMALE )
						{
							// Secondly, don't if we are going to collapse
							if ( pSoldier->bLife >= OKLIFE && pSoldier->bBreath > 0 && pSoldier->bLevel == 0 )
							{
								// Save old direction
								pSoldier->uiPendingActionData1 = pSoldier->bDirection;

								// If we got a head shot...more chance of turning...
								if ( pSoldier->ubHitLocation != AIM_SHOT_HEAD )
								{
									uiChance = Random( 100 );

									// 30 % chance to change direction one way
									if ( uiChance  < 30 )
									{
										bNewDirection = OneCDirection(pSoldier->bDirection);
									}
									// 30 % chance to change direction the other way
									else if ( uiChance >= 30 && uiChance < 60 )
									{
										bNewDirection = OneCCDirection(pSoldier->bDirection);
									}
									// 30 % normal....
									else
									{
										bNewDirection = pSoldier->bDirection;
									}

									EVENT_SetSoldierDirection( pSoldier, bNewDirection );
									EVENT_SetSoldierDesiredDirection( pSoldier, pSoldier->bDirection );

								}
								else
								{
									// OK, 50% chance here to turn...
									uiChance = Random( 100 );

									if ( uiChance < 50 )
									{
										// OK, pick a larger direction to goto....
										pSoldier->uiStatusFlags |= SOLDIER_TURNINGFROMHIT;

										// Pick evenly between both
										if ( Random( 50 ) < 25 )
										{
											bNewDirection = OneCDirection(pSoldier->bDirection);
											bNewDirection = OneCDirection(bNewDirection);
											bNewDirection = OneCDirection(bNewDirection);
										}
										else
										{
											bNewDirection = OneCCDirection(pSoldier->bDirection);
											bNewDirection = OneCCDirection(bNewDirection);
											bNewDirection = OneCCDirection(bNewDirection);
										}

										EVENT_SetSoldierDesiredDirection( pSoldier, bNewDirection );
									}
								}
							}
						}
						break;
					}

				case 479:

						// CODE: Return to old direction......
						if ( pSoldier->ubBodyType <= REGFEMALE )
						{
							// Secondly, don't if we are going to collapse
							//if ( pSoldier->bLife >= OKLIFE && pSoldier->bBreath > 0 )
							//{
							///	if ( !( pSoldier->uiStatusFlags & SOLDIER_TURNINGFROMHIT ) )
							//	{
							///		pSoldier->bDirection				= (INT8)pSoldier->uiPendingActionData1;
							//		pSoldier->bDesiredDirection = (INT8)pSoldier->uiPendingActionData1;
							//	}
							//}
						}
						break;

				case 480:

					// CODE: FORCE FREE ATTACKER
					// Release attacker
					DebugMsg(TOPIC_JA2, DBG_LEVEL_3, "@@@@@@@ Releasesoldierattacker, code 480");

					ReleaseSoldiersAttacker( pSoldier );

					//FREEUP GETTING HIT FLAG
					pSoldier->fGettingHit = FALSE;
					break;

				case 481:

					// CODE: CUT FENCE...
					CutWireFence( pSoldier->sTargetGridNo );
					break;

				case 482:

					// CODE: END CRIPPLE KICKOUT...
					KickOutWheelchair( pSoldier );
					break;

				case 483:

					// CODE: HANDLE DROP BOMB...
					HandleSoldierDropBomb( pSoldier, pSoldier->sPendingActionData2 );
					break;

				case 484:

					// CODE: HANDLE REMOTE...
					HandleSoldierUseRemote( pSoldier, pSoldier->sPendingActionData2 );
					break;

				case 485:

					// CODE: Try steal.....
					UseHandToHand( pSoldier, pSoldier->sPendingActionData2, TRUE );
					break;

				case 486:

					// CODE: GIVE ITEM
					SoldierGiveItemFromAnimation( pSoldier );
					if (pSoldier->ubProfile != NO_PROFILE && pSoldier->ubProfile >= FIRST_NPC )
					{
						TriggerNPCWithGivenApproach( pSoldier->ubProfile, APPROACH_DONE_GIVING_ITEM, FALSE );
					}
					break;

				case 487:

					// CODE: DROP ITEM
					SoldierHandleDropItem( pSoldier );
					break;

				case 489:


					//CODE: REMOVE GUY FRMO WORLD DUE TO EXPLOSION
					//ChangeSoldierState( pSoldier, RAISE_RIFLE, 0 , FALSE );
					//return( TRUE );
					//Delete guy
					//TacticalRemoveSoldier(pSoldier);
					//return( FALSE );
					break;

				case 490:

					// CODE: HANDLE END ITEM PICKUP
					//LOOK INTO HAND, RAISE RIFLE IF WE HAVE ONE....
					/*
					if ( pSoldier->inv[ HANDPOS ].usItem != NOTHING )
					{
						// CHECK IF GUN
						if ( Item[ pSoldier->inv[ HANDPOS ].usItem ].usItemClass == IC_GUN )
						{
							if ( Weapon[ pSoldier->inv[ HANDPOS ].usItem ].ubWeaponClass != HANDGUNCLASS )
							{
								// RAISE
								ChangeSoldierState( pSoldier, RAISE_RIFLE, 0 , FALSE );
								return( TRUE );
							}

						}

					}
					*/
					break;

				case 491:

					// CODE: HANDLE RANDOM BREATH ANIMATION
					//if ( pSoldier->bLife > INJURED_CHANGE_THREASHOLD )
					if ( pSoldier->bLife >= OKLIFE )
					{
						// Increment time from last update
						pSoldier->uiTimeOfLastRandomAction++;

						if ( pSoldier->uiTimeOfLastRandomAction > TIME_FOR_RANDOM_ANIM_CHECK || pSoldier->bLife < INJURED_CHANGE_THREASHOLD || GetDrunkLevel( pSoldier ) >= BORDERLINE )
						{
							pSoldier->uiTimeOfLastRandomAction = 0;

							// Don't do any in water!
							if ( !MercInWater( pSoldier ) )
							{
								// OK, make a dice roll
								ubDiceRoll = (UINT8)Random( 100 );

								// Determine what is in our hand;
								usItem = pSoldier->inv[ HANDPOS ].usItem;

								// Default to nothing in hand ( nothing in quotes, we do have something but not just visible )
								ubRandomHandIndex = RANDOM_ANIM_NOTHINGINHAND;

								if ( usItem != NOTHING )
								{
									if ( Item[ usItem ].usItemClass == IC_GUN )
									{
										if ( (Item[ usItem ].fFlags & ITEM_TWO_HANDED) )
										{
											// Set to rifle
											ubRandomHandIndex = RANDOM_ANIM_RIFLEINHAND;
										}
										else
										{
											// Don't EVER do a trivial anim...
											break;
										}
									}
								}

								// Check which animation to play....
								for ( cnt = 0; cnt < MAX_RANDOM_ANIMS_PER_BODYTYPE; cnt++ )
								{
									pAnimDef = &( gRandomAnimDefs[ pSoldier->ubBodyType ][ cnt ] );

									if ( pAnimDef->sAnimID	!= 0 )
									{
										// If it's an injured animation and we are not in the threashold....
										if ( ( pAnimDef->ubFlags & RANDOM_ANIM_INJURED ) && pSoldier->bLife >= INJURED_CHANGE_THREASHOLD )
										{
											continue;
										}

										// If we need to do an injured one, don't do any others...
										if ( !( pAnimDef->ubFlags & RANDOM_ANIM_INJURED ) && pSoldier->bLife < INJURED_CHANGE_THREASHOLD )
										{
											continue;
										}

										// If it's a drunk animation and we are not in the threashold....
										if ( ( pAnimDef->ubFlags & RANDOM_ANIM_DRUNK ) && GetDrunkLevel( pSoldier ) < BORDERLINE )
										{
											continue;
										}

										// If we need to do an injured one, don't do any others...
										if ( !( pAnimDef->ubFlags & RANDOM_ANIM_DRUNK ) && GetDrunkLevel( pSoldier ) >= BORDERLINE )
										{
											continue;
										}

										// Check if it's our hand
										if ( pAnimDef->ubHandRestriction != RANDOM_ANIM_IRRELEVENTINHAND && pAnimDef->ubHandRestriction != ubRandomHandIndex )
										{
											continue;
										}

										// Check if it's casual and we're in combat and it's not our guy
										if ( ( pAnimDef->ubFlags & RANDOM_ANIM_CASUAL ) )
										{
											// If he's a bad guy, do not do it!
											if ( pSoldier->bTeam != gbPlayerNum  || ( gTacticalStatus.uiFlags & INCOMBAT ) )
											{
												continue;
											}
										}

										// If we are an alternate big guy and have been told to use a normal big merc ani...
										if ( ( pAnimDef->ubFlags & RANDOM_ANIM_FIRSTBIGMERC ) && ( pSoldier->uiAnimSubFlags & SUB_ANIM_BIGGUYTHREATENSTANCE ) )
										{
											continue;
										}

										// If we are a normal big guy and have been told to use an alternate big merc ani...
										if ( ( pAnimDef->ubFlags & RANDOM_ANIM_SECONDBIGMERC ) && !( pSoldier->uiAnimSubFlags & SUB_ANIM_BIGGUYTHREATENSTANCE ) )
										{
											continue;
										}

										// Check if it's the proper height
										if ( pAnimDef->ubAnimHeight == gAnimControl[ pSoldier->usAnimState ].ubEndHeight )
										{
											// OK, If we rolled a value that lies within the range for this random animation, use this one!
											if ( ubDiceRoll >= pAnimDef->ubStartRoll && ubDiceRoll <= pAnimDef->ubEndRoll )
											{
												// Are we playing a sound
												if ( pAnimDef->sAnimID == RANDOM_ANIM_SOUND )
												{
													if (pSoldier->ubBodyType != COW ||
															Random(2) != 0 && (!(gTacticalStatus.uiFlags & INCOMBAT) || pSoldier->bVisible != -1 || Random(100) < 10))
													{
														PlayLocationJA2SampleFromFile(pSoldier->sGridNo, pAnimDef->zSoundFile, MIDVOLUME, 1);
													}
												}
												else
												{
													ChangeSoldierState( pSoldier, pAnimDef->sAnimID, 0 , FALSE );
												}
												return( TRUE );
											}
										}
									}
								}

							}
						}
					}
					break;

				case 492:


					// SIGNAL DODGE!
					// ATE: Only do if we're not inspecial case...
					if ( !( pSoldier->uiStatusFlags & SOLDIER_NPC_DOING_PUNCH ) )
					{
						SOLDIERTYPE* const pTSoldier = FindSoldier(pSoldier->sTargetGridNo, FIND_SOLDIER_GRIDNO);
						if (pTSoldier != NULL)
						{
							// IF WE ARE AN ANIMAL, CAR, MONSTER, DONT'T DODGE
							if ( IS_MERC_BODY_TYPE( pTSoldier ) )
							{
								// ONLY DODGE IF WE ARE SEEN
								if ( pTSoldier->bOppList[ pSoldier->ubID ] != 0 || pTSoldier->bTeam == pSoldier->bTeam )
								{
									if ( gAnimControl[ pTSoldier->usAnimState ].ubHeight == ANIM_STAND )
									{
										// OK, stop merc....
										EVENT_StopMerc( pTSoldier, pTSoldier->sGridNo, pTSoldier->bDirection );

										if ( pTSoldier->bTeam != gbPlayerNum )
										{
											CancelAIAction(pTSoldier);
										}

										// Turn towards the person!
										EVENT_SetSoldierDesiredDirection( pTSoldier, GetDirectionFromGridNo( pSoldier->sGridNo, pTSoldier ) );

										// PLAY SOLDIER'S DODGE ANIMATION
										ChangeSoldierState( pTSoldier, DODGE_ONE, 0 , FALSE );
									}
								}
							}
						}
					}
					break;

				case 493:

					//CODE: PICKUP ITEM!
					// CHECK IF THIS EVENT HAS BEEN SETUP
					//if ( pSoldier->ubPendingAction == MERC_PICKUPITEM )
					//{
						// DROP ITEM
						HandleSoldierPickupItem( pSoldier, pSoldier->uiPendingActionData1, (INT16)(pSoldier->uiPendingActionData4 ), pSoldier->bPendingActionData3 );
						// EVENT HAS BEEN HANDLED
						pSoldier->ubPendingAction		 = NO_PENDING_ACTION;

					//}
					//else
					//{
					//	 DebugMsg( TOPIC_JA2, DBG_LEVEL_3, "Soldier Ani: CODE 493 Error, Pickup item action called but not setup" );
					//}
					break;

				case 494:

					//CODE: OPEN STRUCT!
					// CHECK IF THIS EVENT HAS BEEN SETUP
					//if ( pSoldier->ubPendingAction == MERC_OPENSTRUCT )
					//{
						SoldierHandleInteractiveObject( pSoldier );

						// EVENT HAS BEEN HANDLED
						pSoldier->ubPendingAction		 = NO_PENDING_ACTION;

					//}
					//else
					//{
					//	 DebugMsg( TOPIC_JA2, DBG_LEVEL_3, "Soldier Ani: CODE 494 Error, OPen door action called but not setup" );
					//}
					break;

				case 495:

						if (pSoldier->bAction == AI_ACTION_UNLOCK_DOOR || (pSoldier->bAction == AI_ACTION_LOCK_DOOR && !(pSoldier->fAIFlags & AI_LOCK_DOOR_INCLUDES_CLOSE) ) )
						{
							// EVENT HAS BEEN HANDLED
							pSoldier->ubPendingAction		 = NO_PENDING_ACTION;

							// do nothing here
						}
						else
						{

							pSoldier->fAIFlags &= ~(AI_LOCK_DOOR_INCLUDES_CLOSE);

							pSoldier->ubDoorOpeningNoise = DoorOpeningNoise( pSoldier );

							if ( SoldierHandleInteractiveObject( pSoldier ) )
              {
							  // HANDLE SIGHT!
							  //HandleSight(pSoldier,SIGHT_LOOK | SIGHT_RADIO | SIGHT_INTERRUPT );

							  InitOpplistForDoorOpening();

							  MakeNoise( pSoldier->ubID, pSoldier->sGridNo, pSoldier->bLevel, gpWorldLevelData[pSoldier->sGridNo].ubTerrainID, pSoldier->ubDoorOpeningNoise, NOISE_CREAKING );
						  //	gfDelayResolvingBestSighting = FALSE;

								gInterruptProvoker = pSoldier;
							  AllTeamsLookForAll( TRUE );

							  // ATE: Now, check AI guy to cancel what he was going....
							  HandleSystemNewAISituation( pSoldier, TRUE );
              }

							// EVENT HAS BEEN HANDLED
							pSoldier->ubPendingAction		 = NO_PENDING_ACTION;

						}


					break;

				case 496:
					// CODE: GOTO PREVIOUS ANIMATION
					ChangeSoldierState( pSoldier, pSoldier->usOldAniState, pSoldier->sOldAniCode, FALSE );
					return( TRUE );

				case 497:

					// CODE: CHECK FOR UNCONSCIOUS OR DEATH
					// IF 496 - GOTO PREVIOUS ANIMATION, OTHERWISE PAUSE ANIMATION
					if ( pSoldier->bLife == 0 )
					{

						//HandleSoldierDeath( pSoldier );

						// If guy is now dead, and we have not played death sound before, play
						if ( pSoldier->bLife == 0 && !pSoldier->fDeadSoundPlayed  )
						{
							if ( pSoldier->usAnimState != JFK_HITDEATH )
							{
								DoMercBattleSound( pSoldier, BATTLE_SOUND_DIE1 );
  							pSoldier->fDeadSoundPlayed = TRUE;
							}
						}

						if ( gGameSettings.fOptions[ TOPTION_BLOOD_N_GORE ] )
						{
							// If we are dead, play some death animations!!
							switch( pSoldier->usAnimState )
							{
								case FLYBACK_HIT:
									ChangeSoldierState( pSoldier, FLYBACK_HIT_DEATH, 0, FALSE );
									break;

								case GENERIC_HIT_DEATHTWITCHNB:
								case FALLFORWARD_FROMHIT_STAND:
								case ENDFALLFORWARD_FROMHIT_CROUCH:

									ChangeSoldierState( pSoldier, GENERIC_HIT_DEATH, 0, FALSE );
									break;

								case FALLBACK_HIT_DEATHTWITCHNB:
								case FALLBACK_HIT_STAND:
									ChangeSoldierState( pSoldier, FALLBACK_HIT_DEATH, 0, FALSE );
									break;

								case PRONE_HIT_DEATHTWITCHNB:
								case PRONE_LAY_FROMHIT:

									ChangeSoldierState( pSoldier, PRONE_HIT_DEATH, 0, FALSE );
									break;

								case FALLOFF:
									ChangeSoldierState( pSoldier, FALLOFF_DEATH, 0, FALSE );
									break;

								case FALLFORWARD_ROOF:
									ChangeSoldierState( pSoldier, FALLOFF_FORWARD_DEATH, 0, FALSE);
									break;

								case ADULTMONSTER_DYING:
									ChangeSoldierState( pSoldier, ADULTMONSTER_DYING_STOP, 0, FALSE);
									break;

								case LARVAE_DIE:
									ChangeSoldierState( pSoldier, LARVAE_DIE_STOP, 0, FALSE);
									break;

								case QUEEN_DIE:
									ChangeSoldierState( pSoldier, QUEEN_DIE_STOP, 0, FALSE);
									break;

								case INFANT_DIE:
									ChangeSoldierState( pSoldier, INFANT_DIE_STOP, 0, FALSE);
									break;

								case CRIPPLE_DIE:
									ChangeSoldierState( pSoldier, CRIPPLE_DIE_STOP, 0, FALSE);
									break;

								case ROBOTNW_DIE:
									ChangeSoldierState( pSoldier, ROBOTNW_DIE_STOP, 0, FALSE);
									break;

								case CRIPPLE_DIE_FLYBACK:
									ChangeSoldierState( pSoldier, CRIPPLE_DIE_FLYBACK_STOP, 0, FALSE);
									break;

								default:
									// IF we are here - something is wrong - we should have a death animation here
									DebugMsg( TOPIC_JA2, DBG_LEVEL_3, String( "Soldier Ani: Death sequence needed for animation %d", pSoldier->usAnimState ) );

							}
						}
						else
						{
							BOOLEAN fMadeCorpse;

							CheckForAndHandleSoldierDeath( pSoldier, &fMadeCorpse );

							// ATE: Needs to be FALSE!
							return( FALSE );
						}
						return( TRUE );

					}
					else
					{
						// We can safely be here as well.. ( ie - next turn we may be able to get up )
						// DO SOME CHECKS HERE TO FREE UP ATTACKERS IF WE ARE WAITING AT SPECIFIC ANIMATIONS
						if ( ( gAnimControl[ pSoldier->usAnimState ].uiFlags & ANIM_HITFINISH ) )
						{
								gfPotentialTeamChangeDuringDeath = TRUE;

								// Release attacker
								DebugMsg(TOPIC_JA2, DBG_LEVEL_3, "@@@@@@@ Releasesoldierattacker, code 497 = check for death");
								ReleaseSoldiersAttacker( pSoldier );

								// ATE: OK - the above call can potentially
								// render the soldier bactive to false - check heare
								if ( !pSoldier->bActive )
								{
									return( FALSE );
								}

								gfPotentialTeamChangeDuringDeath = FALSE;

								// FREEUP GETTING HIT FLAG
								pSoldier->fGettingHit = FALSE;
						}

						HandleCheckForDeathCommonCode( pSoldier );

						return( TRUE );
					}

				case 498:

					// CONDITONAL JUMP
					// If we have a pending animation, play it, else continue
					if ( pSoldier->usPendingAnimation != NO_PENDING_ANIMATION )
					{
						ChangeSoldierState( pSoldier, pSoldier->usPendingAnimation, 0, FALSE );
						pSoldier->usPendingAnimation = NO_PENDING_ANIMATION;
						return( TRUE );
					}
					break;

				// JUMP TO NEXT STATIONARY ANIMATION ACCORDING TO HEIGHT
				case 499:

					if (!(pSoldier->uiStatusFlags & SOLDIER_PC))
					{
						if ( pSoldier->bAction == AI_ACTION_PULL_TRIGGER )
						{
							if ( pSoldier->usAnimState == AI_PULL_SWITCH && gTacticalStatus.ubAttackBusyCount == 0 && gubElementsOnExplosionQueue == 0 )
							{
								FreeUpNPCFromPendingAction( pSoldier );
							}
						}
						else if ( pSoldier->bAction == AI_ACTION_PENDING_ACTION
						|| pSoldier->bAction == AI_ACTION_OPEN_OR_CLOSE_DOOR
						|| pSoldier->bAction == AI_ACTION_YELLOW_ALERT
						|| pSoldier->bAction == AI_ACTION_RED_ALERT
						|| pSoldier->bAction == AI_ACTION_PULL_TRIGGER
						|| pSoldier->bAction == AI_ACTION_CREATURE_CALL
						|| pSoldier->bAction == AI_ACTION_UNLOCK_DOOR
						|| pSoldier->bAction == AI_ACTION_LOCK_DOOR	)
						{
							if ( pSoldier->usAnimState == PICKUP_ITEM || pSoldier->usAnimState == ADJACENT_GET_ITEM || pSoldier->usAnimState == DROP_ITEM || pSoldier->usAnimState == END_OPEN_DOOR || pSoldier->usAnimState == END_OPEN_DOOR_CROUCHED || pSoldier->usAnimState == CLOSE_DOOR || pSoldier->usAnimState == MONSTER_UP || pSoldier->usAnimState == AI_RADIO || pSoldier->usAnimState == AI_CR_RADIO || pSoldier->usAnimState == END_OPENSTRUCT || pSoldier->usAnimState == END_OPENSTRUCT_CROUCHED || pSoldier->usAnimState == QUEEN_CALL )
							{
								FreeUpNPCFromPendingAction( pSoldier );
							}
						}
					}

					ubDesiredHeight = pSoldier->ubDesiredHeight;

					// Check if we are at the desired height
					if ( pSoldier->ubDesiredHeight == gAnimControl[ pSoldier->usAnimState ].ubEndHeight || pSoldier->ubDesiredHeight == NO_DESIRED_HEIGHT )
					{
						// Adjust movement mode......
						if ( pSoldier->bTeam == gbPlayerNum && !pSoldier->fContinueMoveAfterStanceChange )
						{
							usUIMovementMode =  GetMoveStateBasedOnStance( pSoldier, gAnimControl[ pSoldier->usAnimState ].ubEndHeight );

              // ATE: if we are currently running but have been told to walk, don't!
              if ( pSoldier->usUIMovementMode == RUNNING && usUIMovementMode == WALKING )
              {
                // No!
              }
              else
              {
                pSoldier->usUIMovementMode = usUIMovementMode;
              }
						}


						pSoldier->ubDesiredHeight = NO_DESIRED_HEIGHT;

						if (pSoldier->fChangingStanceDueToSuppression)
						{
							pSoldier->fChangingStanceDueToSuppression = FALSE;
							DebugMsg(TOPIC_JA2, DBG_LEVEL_3, "@@@@@@@ Freeing up attacker - end of suppression stance change");
							ReduceAttackBusyCount(ID2SOLDIER(pSoldier->ubSuppressorID), FALSE);
						}

						if ( pSoldier->usPendingAnimation == NO_PENDING_ANIMATION && ( pSoldier->fTurningFromPronePosition != 3 ) && ( pSoldier->fTurningFromPronePosition != 1 ) )
						{
              if ( gTacticalStatus.ubAttackBusyCount == 0 )
              {
							  // OK, UNSET INTERFACE FIRST
							  UnSetUIBusy( pSoldier->ubID );
							  // ( before we could get interrupted potentially by an interrupt )
              }
						}

						// Check to see if we have changed stance and need to update visibility
						if ( gAnimControl[ pSoldier->usAnimState ].uiFlags & ANIM_STANCECHANGEANIM)
						{
							if ( pSoldier->usPendingAnimation == NO_PENDING_ANIMATION && gTacticalStatus.ubAttackBusyCount == 0 && pSoldier->fTurningFromPronePosition != 3 && pSoldier->fTurningFromPronePosition != 1 )
							{
								HandleSight(pSoldier,SIGHT_LOOK | SIGHT_RADIO | SIGHT_INTERRUPT );
							}
							else
							{
								HandleSight(pSoldier,SIGHT_LOOK | SIGHT_RADIO );
							}

              // Keep ui busy if we are now in a hidden interrupt
              // say we're prone and we crouch, we may get a hidden
              // interrupt and in such a case we'd really like the UI
              // still locked
              if ( gfHiddenInterrupt )
              {
			          guiPendingOverrideEvent	= LA_BEGINUIOURTURNLOCK;
			          HandleTacticalUI( );
              }

							// ATE: Now, check AI guy to cancel what he was going....
							HandleSystemNewAISituation( pSoldier, TRUE );
						}

						// Have we finished opening doors?
						if ( pSoldier->usAnimState == END_OPEN_DOOR || pSoldier->usAnimState == END_OPEN_DOOR_CROUCHED || pSoldier->usAnimState == CRIPPLE_CLOSE_DOOR || pSoldier->usAnimState == CRIPPLE_END_OPEN_DOOR )
						{
							// Are we told to continue movement...?
							if ( pSoldier->bEndDoorOpenCode == 1 )
							{
								// OK, set this value to 2 such that once we are into a new gridno,
								// we close the door!
								pSoldier->bEndDoorOpenCode = 2;

								// yes..
								EVENT_GetNewSoldierPath( pSoldier, pSoldier->sFinalDestination, pSoldier->usUIMovementMode );

								if ( !( gAnimControl[ pSoldier->usAnimState ].uiFlags & ( ANIM_MOVING ) ) )
								{
									if ( pSoldier->sAbsoluteFinalDestination != NOWHERE )
									{
										CancelAIAction(pSoldier);
									}
								}

								// OK, this code, pSoldier->bEndDoorOpenCode will be set to 0 if anythiing
								// cuases guy to stop - StopMerc() will set it...

								return( TRUE );
							}
						}

						// Check if we should contine into a moving animation
						if ( pSoldier->usPendingAnimation != NO_PENDING_ANIMATION )
						{
							UINT16 usPendingAnimation = pSoldier->usPendingAnimation;

							pSoldier->usPendingAnimation = NO_PENDING_ANIMATION;
							ChangeSoldierState( pSoldier, usPendingAnimation, 0, FALSE );
							return( TRUE );
						}

						// Alrighty, do we wish to continue
						if ( pSoldier->fContinueMoveAfterStanceChange )
						{
							// OK, if the code is == 2, get the path and try to move....
							if ( pSoldier->fContinueMoveAfterStanceChange == 2 )
							{
								pSoldier->usPathIndex++;

								if ( pSoldier->usPathIndex > pSoldier->usPathDataSize )
								{
									pSoldier->usPathIndex = pSoldier->usPathDataSize;
								}

								if ( pSoldier->usPathIndex == pSoldier->usPathDataSize )
								{
									// Stop, don't do anything.....
								}
								else
								{
									EVENT_InitNewSoldierAnim( pSoldier, pSoldier->usUIMovementMode, 0 , FALSE );

									// UNSET LOCK PENDING ACTION COUNTER FLAG
									pSoldier->uiStatusFlags &= ( ~SOLDIER_LOCKPENDINGACTIONCOUNTER );

								}
							}
							else
							{
								SelectMoveAnimationFromStance( pSoldier );
							}

							pSoldier->fContinueMoveAfterStanceChange = FALSE;
							return( TRUE );
						}
						SoldierGotoStationaryStance( pSoldier );
						return( TRUE );
					}
					else
					{
						ubCurrentHeight = gAnimControl[ pSoldier->usAnimState ].ubEndHeight;

						// We need to go more, continue
						if ( ubDesiredHeight == ANIM_STAND && ubCurrentHeight == ANIM_CROUCH )
						{
							 // Return here because if now, we will skipp a few frames
							 ChangeSoldierState( pSoldier, KNEEL_UP, 0 , FALSE );
							 return( TRUE );
						}
						if ( ubDesiredHeight == ANIM_CROUCH && ubCurrentHeight == ANIM_STAND )
						{
							 // Return here because if now, we will skipp a few frames
							 ChangeSoldierState( pSoldier, KNEEL_DOWN, 0 , FALSE );
							 return( TRUE );
						}
						else if ( ubDesiredHeight == ANIM_PRONE && ubCurrentHeight == ANIM_CROUCH )
						{
							 // Return here because if now, we will skipp a few frames
							 ChangeSoldierState( pSoldier, PRONE_DOWN, 0 , FALSE );
							 return( TRUE );
						}
						else if ( ubDesiredHeight == ANIM_CROUCH && ubCurrentHeight == ANIM_PRONE )
						{
							 // Return here because if now, we will skipp a few frames
							 ChangeSoldierState( pSoldier, PRONE_UP, 0 , FALSE );
							 return( TRUE );
						}
					}
					// IF we are here - something is wrong - we should have a death animation here
					#ifdef JA2BETAVERSION
						ScreenMsg( FONT_ORANGE, MSG_BETAVERSION, L"Soldier Ani: GOTO Stance not chained properly: %d %d %d", ubDesiredHeight, ubCurrentHeight, pSoldier->usAnimState );
					#endif

					SoldierGotoStationaryStance( pSoldier );
					return( TRUE );
			}

			// Adjust frame control pos, and try again
			pSoldier->usAniCode++;

		}
		else if ( sNewAniFrame > 499 && sNewAniFrame < 599 )
		{
			// Jump,
			// Do not adjust, just try again
			pSoldier->usAniCode = sNewAniFrame - 501;
		}
		else if ( sNewAniFrame > 599 && sNewAniFrame <= 699 )
		{
			// Jump, to animation script
			EVENT_InitNewSoldierAnim( pSoldier, (UINT16)(sNewAniFrame - 600 ), 0 , FALSE );
			return( TRUE );
		}
		else if ( sNewAniFrame > 799 && sNewAniFrame <= 899 )
		{
			// Jump, to animation script ( But in the 100's range )
			EVENT_InitNewSoldierAnim( pSoldier, (UINT16)(sNewAniFrame - 700 ), 0 , FALSE );
			return( TRUE );
		}
		else if ( sNewAniFrame > 899 && sNewAniFrame <= 999 )
		{
			// Jump, to animation script ( But in the 200's range )
			EVENT_InitNewSoldierAnim( pSoldier, (UINT16)(sNewAniFrame - 700 ), 0 , FALSE );
			return( TRUE );
		}
		else if ( sNewAniFrame > 699 && sNewAniFrame < 799 )
		{
			switch( sNewAniFrame )
			{
				case 702:
					// Play fall to knees sound
					PlaySoldierJA2Sample(pSoldier, FALL_1 + Random(2), HIGHVOLUME, 1, FALSE);
					break;

				case 703:
				case 704:

					// Play footprints
					PlaySoldierFootstepSound( pSoldier );
					break;

				case 705:
					// PLay body splat sound
					PlaySoldierJA2Sample(pSoldier, BODY_SPLAT_1, HIGHVOLUME, 1, TRUE);
					break;

				case 706:
					// PLay head splat
					PlaySoldierJA2Sample(pSoldier, HEADSPLAT_1, HIGHVOLUME, 1, TRUE);
					break;

				case 707:
					// PLay creature battle cry
					PlayLocationJA2StreamingSample(pSoldier->sGridNo, CREATURE_BATTLECRY_1, HIGHVOLUME, 1);
					break;

				case 708:

					// PLay lock n' load sound for gun....
					// Get LNL sound for current gun
					{
						UINT16	usItem;
						UINT16	usSoundID;

						usItem = pSoldier->inv[ HANDPOS ].usItem;

						if ( usItem != NOTHING )
						{
							usSoundID = Weapon[ usItem ].sLocknLoadSound;

							if ( usSoundID != 0 )
							{
								PlayLocationJA2Sample(pSoldier->sGridNo, usSoundID, HIGHVOLUME, 1);
							}
						}
					}
					break;

				case 709:
					// Knife throw sound...
					PlayLocationJA2Sample(pSoldier->sGridNo, Weapon[THROWING_KNIFE].sSound, HIGHVOLUME, 1);
					break;

				case 710:
					// Monster footstep in
					if (SoldierOnScreen(pSoldier))
					{
						PlaySoldierJA2Sample(pSoldier, ACR_STEP_1, MIDVOLUME, 1, TRUE);
					}
					break;

				case 711:
					// Monster footstep in
					if (SoldierOnScreen(pSoldier))
					{
						PlaySoldierJA2Sample(pSoldier, ACR_STEP_2, MIDVOLUME, 1, TRUE);
					}
					break;

				case 712:
					// Monster footstep in
					if (SoldierOnScreen(pSoldier))
					{
						PlaySoldierJA2Sample(pSoldier, LCR_MOVEMENT, MIDVOLUME, 1, TRUE);
					}
					break;

				case 713:
					// Monster footstep in
					if ( pSoldier->ubBodyType == INFANT_MONSTER )
					{
						if (SoldierOnScreen(pSoldier))
						{
							PlaySoldierJA2Sample(pSoldier, BCR_DRAGGING, MIDVOLUME, 1, TRUE);
						}
					}
					break;

				case 714:

					// Lunges....
					PlaySoldierJA2Sample(pSoldier, ACR_LUNGE, HIGHVOLUME, 1, TRUE);
					break;

				case 715:

					// Swipe
					PlaySoldierJA2Sample(pSoldier, ACR_SWIPE, HIGHVOLUME, 1, TRUE);
					break;

				case 716:

					// Eat flesh
					PlaySoldierJA2Sample(pSoldier, ACR_EATFLESH, HIGHVOLUME, 1, TRUE);
					break;

				case 717:

					// Battle cry
					{
						INT32			iSoundID=0;
						BOOLEAN		fDoCry = FALSE;

						//if (SoldierOnScreen(pSoldier))
						{
							switch( pSoldier->usActionData )
							{
								case CALL_1_PREY:

									if ( pSoldier->ubBodyType == QUEENMONSTER )
									{
										iSoundID = LQ_SMELLS_THREAT;
									}
									else
									{
										iSoundID = ACR_SMEEL_PREY;
									}
									fDoCry = TRUE;
									break;

								case CALL_MULTIPLE_PREY:

									if ( pSoldier->ubBodyType == QUEENMONSTER )
									{
										iSoundID = LQ_SMELLS_THREAT;
									}
									else
									{
										iSoundID = ACR_SMELL_THREAT;
									}
									fDoCry = TRUE;
									break;

								case CALL_ATTACKED:

									if ( pSoldier->ubBodyType == QUEENMONSTER )
									{
										iSoundID = LQ_ENRAGED_ATTACK;
									}
									else
									{
										iSoundID = ACR_SMELL_THREAT;
									}
									fDoCry = TRUE;
									break;

								case CALL_CRIPPLED:

									if ( pSoldier->ubBodyType == QUEENMONSTER )
									{
										iSoundID = LQ_CRIPPLED;
									}
									else
									{
										iSoundID = ACR_CRIPPLED;
									}
									fDoCry = TRUE;
									break;
							}

							if ( fDoCry )
							{
								PlayLocationJA2Sample(pSoldier->sGridNo, iSoundID, HIGHVOLUME, 1);
							}
						}
					}
					break;

				case 718:


					PlaySoldierJA2Sample(pSoldier, LQ_RUPTURING, HIGHVOLUME, 1, TRUE);
					break;

				case 719:

					// Spit attack start sound...
					PlaySoldierJA2Sample(pSoldier, LQ_ENRAGED_ATTACK, HIGHVOLUME, 1, TRUE);
					break;

				case 720:

					// Spit attack start sound...
					PlaySoldierJA2Sample(pSoldier, LQ_WHIP_ATTACK, HIGHVOLUME, 1, TRUE);
					break;

				case 721:
					// Play fall from knees to ground...
					PlaySoldierJA2Sample(pSoldier, FALL_TO_GROUND_1 + Random(3), HIGHVOLUME, 1, FALSE);
					if ( pSoldier->usAnimState == FALLFORWARD_FROMHIT_STAND )
					{
						CheckEquipmentForFragileItemDamage( pSoldier, 20 );
					}
					break;

				case 722:
					// Play fall heavy
					PlaySoldierJA2Sample(pSoldier, HEAVY_FALL_1, HIGHVOLUME, 1, FALSE);
					if ( pSoldier->usAnimState == FALLFORWARD_FROMHIT_CROUCH )
					{
						CheckEquipmentForFragileItemDamage( pSoldier, 15 );
					}
					break;

				case 723:

					// Play armpit noise...
					PlaySoldierJA2Sample(pSoldier, IDLE_ARMPIT, LOWVOLUME, 1, TRUE);
					break;

				case 724:

					// Play ass scratch
					// PlaySoldierJA2Sample(pSoldier, IDLE_SCRATCH, HIGHVOLUME, 1, TRUE);
					break;

				case 725:

					// Play back crack
					PlaySoldierJA2Sample(pSoldier, IDLE_BACKCRACK, LOWVOLUME, 1, TRUE);
					break;

				case 726:

					// Kickin door
					PlaySoldierJA2Sample(pSoldier, KICKIN_DOOR, HIGHVOLUME, 1, TRUE);
					break;

				case 727:

					// Swoosh
					PlaySoldierJA2Sample(pSoldier, SWOOSH_1 + Random(6), HIGHVOLUME, 1, TRUE);
					break;

				case 728:

					// Creature fall
					PlaySoldierJA2Sample(pSoldier, ACR_FALL_1, HIGHVOLUME, 1, TRUE);
					break;

				case 729:

					// grab roof....
					PlaySoldierJA2Sample(pSoldier, GRAB_ROOF, HIGHVOLUME, 1, TRUE);
					break;

				case 730:

					// end climb roof....
					PlaySoldierJA2Sample(pSoldier, LAND_ON_ROOF, HIGHVOLUME, 1, TRUE);
					break;

				case 731:

					// Stop climb roof..
					PlaySoldierJA2Sample(pSoldier, FALL_TO_GROUND_1 + Random(3), HIGHVOLUME, 1, TRUE);
					break;

				case 732:

					// Play die sound
					DoMercBattleSound( pSoldier, BATTLE_SOUND_DIE1 );
					pSoldier->fDeadSoundPlayed = TRUE;
					break;

				case 750:

					// CODE: Move Vehicle UP
					if ( pSoldier->uiStatusFlags & SOLDIER_VEHICLE )
					{
					//	SetSoldierHeight( pSoldier, (FLOAT)( pSoldier->sHeightAdjustment + 1 ) );
					}
					break;

				case 751:

					// CODE: Move vehicle down
					if ( pSoldier->uiStatusFlags & SOLDIER_VEHICLE )
					{
				//		SetSoldierHeight( pSoldier, (FLOAT)( pSoldier->sHeightAdjustment - 1 ) );
					}
					break;

				case 752:

					// Code: decapitate
					DecapitateCorpse( pSoldier, pSoldier->sTargetGridNo, pSoldier->bTargetLevel );
					break;

				case 753:

					// code: freeup attcker
					DebugMsg(TOPIC_JA2, DBG_LEVEL_3, String("@@@@@@@ Reducing attacker busy count..., CODE FROM ANIMATION %hs ( %d )", gAnimControl[pSoldier->usAnimState].zAnimStr, pSoldier->usAnimState));
					ReduceAttackBusyCount(pSoldier, FALSE);

          // ATE: Here, reduce again if creaturequeen tentical attack...
          if ( pSoldier->usAnimState == QUEEN_SWIPE )
          {
					  DebugMsg(TOPIC_JA2, DBG_LEVEL_3, "@@@@@@@ Reducing attacker busy count for end of queen swipe");
					  ReduceAttackBusyCount(pSoldier, FALSE);
          }
					break;

				case 754:
					HandleFallIntoPitFromAnimation(pSoldier);
					break;

				case 755 :

					DishoutQueenSwipeDamage( pSoldier );
					break;

				case 756:

					// Reload robot....
					{
						SOLDIERTYPE* const pRobot = WhoIsThere2(pSoldier->sPendingActionData2, pSoldier->bLevel);
						if (pRobot != NULL && pRobot->uiStatusFlags & SOLDIER_ROBOT)
						{
							ReloadGun( pRobot, &(pRobot->inv[ HANDPOS ] ), pSoldier->pTempObject );

							// OK, check what was returned and place in inventory if it's non-zero
							if ( pSoldier->pTempObject->usItem != NOTHING )
							{
								// Add to inv..
								AutoPlaceObject( pSoldier, pSoldier->pTempObject, TRUE );
							}

							MemFree( pSoldier->pTempObject );
							pSoldier->pTempObject = NULL;
						}
					}
					break;

				case 757:

					// INcrement attacker busy count....
					gTacticalStatus.ubAttackBusyCount++;
					DebugMsg(TOPIC_JA2, DBG_LEVEL_3, String("!!!!! Incrementing attacker busy count..., CODE FROM ANIMATION %hs ( %d ) : Count now %d", gAnimControl[pSoldier->usAnimState].zAnimStr, pSoldier->usAnimState, gTacticalStatus.ubAttackBusyCount));
					break;

				case 758:

					// Trigger after slap...
					TriggerNPCWithGivenApproach( QUEEN, APPROACH_DONE_SLAPPED , TRUE );
					break;

				case 759:

					// Getting hit by slap
					{
						SOLDIERTYPE *pTarget;

						pTarget = FindSoldierByProfileID( ELLIOT, FALSE );

						if ( pTarget )
						{
							EVENT_InitNewSoldierAnim( pTarget, SLAP_HIT, 0 , FALSE );

							// Play noise....
							//PlaySoldierJA2Sample(pTarget, S_SLAP_IMPACT, HIGHVOLUME, 1, TRUE);

							//DoMercBattleSound( pTarget, (INT8)( BATTLE_SOUND_HIT1 + Random( 2 ) ) );

						}
					}
					break;

				case 760:

					// Get some blood.....
					// Corpse Id is from pending action data
					GetBloodFromCorpse( pSoldier );
					// Dirty interface....
					DirtyMercPanelInterface( pSoldier, DIRTYLEVEL2 );
					break;

				case 761:
					// Dish out damage!
					EVENT_SoldierGotHit(MercPtrs[pSoldier->uiPendingActionData4], TAKE_DAMAGE_BLADE, 25, 25, OppositeDirection(pSoldier->bDirection), 50, pSoldier, 0, ANIM_PRONE, 0);
					break;

				case 762:
					{
						// CODE: Set off Trigger
						INT8	bPanicTrigger;

						bPanicTrigger = ClosestPanicTrigger( pSoldier );
						SetOffPanicBombs(pSoldier, bPanicTrigger);
						// any AI guy has been specially given keys for this, now take them
						// away
						pSoldier->bHasKeys = pSoldier->bHasKeys >> 1;

					}
					break;

				case 763:

					// CODE: Drop item at gridno
					if ( pSoldier->pTempObject != NULL )
					{
						if ( pSoldier->bVisible != -1 )
						{
							PlayLocationJA2Sample(pSoldier->sGridNo, THROW_IMPACT_2, MIDVOLUME, 1);
						}

						AddItemToPool( pSoldier->sPendingActionData2, pSoldier->pTempObject, 1, pSoldier->bLevel, 0 , -1 );
						NotifySoldiersToLookforItems( );

						MemFree( pSoldier->pTempObject );
						pSoldier->pTempObject = NULL;
					}
					break;

				case 764:

					PlaySoldierJA2Sample(pSoldier, PICKING_LOCK, HIGHVOLUME, 1, TRUE);
					break;

				case 765:

					// Flyback hit - do blood!
					// PLace in existing tile and one back...
					{
						INT16 sNewGridNo;

						InternalDropBlood( pSoldier->sGridNo, pSoldier->bLevel, 0, (UINT8)(MAXBLOODQUANTITY), 1 );

						// Move forward one gridno....
						sNewGridNo = NewGridNo(pSoldier->sGridNo, DirectionInc(OppositeDirection(pSoldier->bDirection)));

						InternalDropBlood( sNewGridNo, pSoldier->bLevel, 0, (UINT8)(MAXBLOODQUANTITY), 1 );

					}
					break;

				case 766:

					// Say COOL quote
					DoMercBattleSound( pSoldier, BATTLE_SOUND_COOL1 );
					break;

				case 767:

					// Slap sound effect
					PlaySoldierJA2Sample(pSoldier, SLAP_2, HIGHVOLUME, 1, FALSE);
					break;

				case 768:

					// OK, after ending first aid, stand up if not in combat....
					if ( NumCapableEnemyInSector( ) == 0 )
					{
						// Stand up...
						ChangeSoldierStance( pSoldier, ANIM_STAND );
						return( FALSE );
					}
          break;

        case 769:

          // ATE: LOOK HERE FOR CODE IN INTERNALS FOR
          // REFUELING A VEHICLE
          // THE GAS_CAN IS IN THE MERCS MAIN HAND AT THIS TIME
          {
						SOLDIERTYPE* const pVehicle = WhoIsThere2(pSoldier->sPendingActionData2, pSoldier->bLevel);

            // this is a ubID for soldiertype....
						AddFuelToVehicle( pSoldier, pVehicle );

            fInterfacePanelDirty = DIRTYLEVEL2;
          }
					break;

        case 770:

				  PlayJA2Sample(USE_WIRE_CUTTERS, HIGHVOLUME, 1, MIDDLEPAN);
          break;

        case 771:

				  PlayJA2Sample(BLOODCAT_ATTACK, HIGHVOLUME, 1, MIDDLEPAN);
          break;

				case 772:

				  //CODE: FOR A REALTIME NON-INTERRUPTABLE SCRIPT - SIGNAL DONE
					pSoldier->fRTInNonintAnim = FALSE;
					break;

        case 773:

          // Kneel up...
					if ( !pSoldier->bStealthMode )
					{
						PlaySoldierJA2Sample(pSoldier, KNEEL_UP_SOUND, MIDVOLUME, 1, TRUE);
					}
          break;

        case 774:

          // Kneel down..
					if ( !pSoldier->bStealthMode )
					{
						PlaySoldierJA2Sample(pSoldier, KNEEL_DOWN_SOUND, MIDVOLUME, 1, TRUE);
					}
          break;

        case 775:

          // prone up..
					if ( !pSoldier->bStealthMode )
					{
						PlaySoldierJA2Sample(pSoldier, PRONE_UP_SOUND, MIDVOLUME, 1, TRUE);
					}
          break;

        case 776:

          // prone down..
					if ( !pSoldier->bStealthMode )
					{
						PlaySoldierJA2Sample(pSoldier, PRONE_DOWN_SOUND, MIDVOLUME, 1, TRUE);
					}
          break;

        case 777:

          // picking something up
					if ( !pSoldier->bStealthMode )
					{
						PlaySoldierJA2Sample(pSoldier, PICKING_SOMETHING_UP, HIGHVOLUME, 1, TRUE);
					}
          break;

        case 778:
					if ( !pSoldier->bStealthMode )
					{
						PlaySoldierJA2Sample(pSoldier, ENTER_DEEP_WATER_1, HIGHVOLUME, 1, TRUE);
					}
          break;

        case 779:

					PlaySoldierJA2Sample(pSoldier, COW_FALL, HIGHVOLUME, 1, TRUE);
          break;

        case 780:

					PlaySoldierJA2Sample(pSoldier, COW_HIT_SND, HIGHVOLUME, 1, TRUE);
          break;

        case 781:

					PlaySoldierJA2Sample(pSoldier, ACR_DIE_PART2, HIGHVOLUME, 1, FALSE);
          break;

        case 782:

					PlaySoldierJA2Sample(pSoldier, CREATURE_DISSOLVE_1, HIGHVOLUME, 1, FALSE);
          break;

        case 784:

					PlaySoldierJA2Sample(pSoldier, CREATURE_FALL, HIGHVOLUME, 1, FALSE);
          break;

        case 785:

          if ( Random( 5 ) == 0 )
          {
						PlaySoldierJA2Sample(pSoldier, CROW_PECKING_AT_FLESH, MIDVOLUME, 1, TRUE);
          }
          break;

        case 786:

					PlaySoldierJA2Sample(pSoldier, CROW_FLYING_AWAY, MIDVOLUME, 1, TRUE);
          break;

        case 787:

					PlaySoldierJA2Sample(pSoldier, SLAP_1, HIGHVOLUME, 1, FALSE);
          break;

        case 788:

					PlaySoldierJA2Sample(pSoldier, MORTAR_START, HIGHVOLUME, 1, TRUE);
          break;

        case 789:

					PlaySoldierJA2Sample(pSoldier, MORTAR_LOAD, HIGHVOLUME, 1, TRUE);
          break;

        case 790:

					PlaySoldierJA2Sample(pSoldier, COW_FALL_2, HIGHVOLUME, 1, TRUE);
          break;

        case 791:

					PlaySoldierJA2Sample(pSoldier, FENCE_OPEN, HIGHVOLUME, 1, TRUE);
          break;

			}
			// Adjust frame control pos, and try again
			pSoldier->usAniCode++;
		}
		else if ( sNewAniFrame == 999 )
		{

			// Go to start, by default
			pSoldier->usAniCode = 0;

		}

 // Loop here until we break on a real item!
 } while ( TRUE );

 // We're done
	return( TRUE );
}

#define MIN_DEADLINESS_FOR_LIKE_GUN_QUOTE			20


static BOOLEAN ShouldMercSayHappyWithGunQuote(SOLDIERTYPE* pSoldier)
{
	// How do we do this....

	if ( QuoteExp_GotGunOrUsedGun[ pSoldier->ubProfile ] == QUOTE_SATISFACTION_WITH_GUN_AFTER_KILL )
	{
		// For one, only once a day...
		if ( pSoldier->usQuoteSaidFlags & SOLDIER_QUOTE_SAID_LIKESGUN )
		{
			return( FALSE );
		}

    // is it a gun?
    if ( Item[ pSoldier->usAttackingWeapon ].usItemClass & IC_GUN )
    {
  		// Is our weapon powerfull enough?
		  if ( Weapon[ pSoldier->usAttackingWeapon ].ubDeadliness > MIN_DEADLINESS_FOR_LIKE_GUN_QUOTE )
		  {
			  // 20 % chance?
			  if ( Random( 100 ) < 20 )
			  {
				  return( TRUE );
			  }
		  }
    }
	}

	return( FALSE );
}


static void SayBuddyWitnessedQuoteFromKill(SOLDIERTYPE* pKillerSoldier, INT16 sGridNo, INT8 bLevel)
{
	INT8	bBuddyIndex[ 20 ] = { -1 };
  INT8  bTempBuddyIndex;
	UINT8	ubNumMercs = 0;
	UINT8	ubChosenMerc;
	SOLDIERTYPE *pTeamSoldier;
	INT32 cnt;
	INT16		sDistVisible = FALSE;
  UINT16  usQuoteNum;

	// Loop through all our guys and randomly say one from someone in our sector

	// set up soldier ptr as first element in mercptrs list
	cnt = gTacticalStatus.Team[ gbPlayerNum ].bFirstID;

	// run through list
	SOLDIERTYPE* mercs_in_sector[20];
	for ( pTeamSoldier = MercPtrs[ cnt ]; cnt <= gTacticalStatus.Team[ gbPlayerNum ].bLastID; cnt++,pTeamSoldier++ )
	{
		// Add guy if he's a candidate...
		if ( OK_INSECTOR_MERC( pTeamSoldier ) && !AM_AN_EPC( pTeamSoldier ) && !( pTeamSoldier->uiStatusFlags & SOLDIER_GASSED ) && !(AM_A_ROBOT( pTeamSoldier )) && !pTeamSoldier->fMercAsleep && pTeamSoldier->sGridNo != NOWHERE )
		{
      // Are we a buddy of killer?
			bTempBuddyIndex = WhichBuddy( pTeamSoldier->ubProfile, pKillerSoldier->ubProfile );

      if ( bTempBuddyIndex != -1 )
      {
        switch( bTempBuddyIndex )
        {
          case 0:
            if ( pTeamSoldier->usQuoteSaidExtFlags & SOLDIER_QUOTE_SAID_BUDDY_1_WITNESSED )
            {
              continue;
            }
            break;

          case 1:
            if ( pTeamSoldier->usQuoteSaidExtFlags & SOLDIER_QUOTE_SAID_BUDDY_2_WITNESSED )
            {
              continue;
            }
            break;

          case 2:
            if ( pTeamSoldier->usQuoteSaidExtFlags & SOLDIER_QUOTE_SAID_BUDDY_3_WITNESSED )
            {
              continue;
            }
            break;
        }

        // TO LOS check to killed
	      // Can we see location of killer?
	      sDistVisible = DistanceVisible( pTeamSoldier, DIRECTION_IRRELEVANT, DIRECTION_IRRELEVANT, pKillerSoldier->sGridNo, pKillerSoldier->bLevel );
        if ( SoldierTo3DLocationLineOfSightTest( pTeamSoldier, pKillerSoldier->sGridNo,  pKillerSoldier->bLevel, (UINT8)3, (UINT8) sDistVisible, TRUE ) == 0 )
        {
          continue;
        }


	      // Can we see location of killed?
	      sDistVisible = DistanceVisible( pTeamSoldier, DIRECTION_IRRELEVANT, DIRECTION_IRRELEVANT, sGridNo, bLevel );
        if ( SoldierTo3DLocationLineOfSightTest( pTeamSoldier, sGridNo,  bLevel, (UINT8)3, (UINT8) sDistVisible, TRUE ) == 0 )
        {
          continue;
        }

        // OK, a good candidate...
				mercs_in_sector[ubNumMercs] = pTeamSoldier;
			  bBuddyIndex[ ubNumMercs ]     = bTempBuddyIndex;
			  ubNumMercs++;
      }
		}
	}

	// If we are > 0
	if ( ubNumMercs > 0 )
	{
    // Do random check here...
    if ( Random( 100 ) < 20 )
    {
		  ubChosenMerc = (UINT8)Random( ubNumMercs );
			SOLDIERTYPE* const chosen = mercs_in_sector[ubChosenMerc];
      switch( bBuddyIndex[ ubChosenMerc ] )
      {
        case 0:
          usQuoteNum = QUOTE_BUDDY_1_GOOD;
					chosen->usQuoteSaidExtFlags |= SOLDIER_QUOTE_SAID_BUDDY_1_WITNESSED;
          break;

        case 1:
          usQuoteNum = QUOTE_BUDDY_2_GOOD;
					chosen->usQuoteSaidExtFlags |= SOLDIER_QUOTE_SAID_BUDDY_2_WITNESSED;
          break;

        case 2:
          usQuoteNum = QUOTE_LEARNED_TO_LIKE_WITNESSED;
					chosen->usQuoteSaidExtFlags |= SOLDIER_QUOTE_SAID_BUDDY_3_WITNESSED;
          break;
      }
			TacticalCharacterDialogue(chosen, usQuoteNum);
    }
	}
}


void HandleKilledQuote(SOLDIERTYPE* pKilledSoldier, SOLDIERTYPE* pKillerSoldier, INT16 sGridNo, INT8 bLevel)
{
	SOLDIERTYPE *pTeamSoldier;
	INT32 cnt;
	UINT8	ubNumMercs = 0;
	BOOLEAN fDoSomeoneElse = FALSE;
	BOOLEAN	fCanWeSeeLocation = FALSE;
	INT16		sDistVisible = FALSE;

	gLastMercTalkedAboutKilling = pKilledSoldier;

	// Can we see location?
	sDistVisible = DistanceVisible( pKillerSoldier, DIRECTION_IRRELEVANT, DIRECTION_IRRELEVANT, sGridNo, bLevel );

	fCanWeSeeLocation = ( SoldierTo3DLocationLineOfSightTest( pKillerSoldier, sGridNo,  bLevel, (UINT8)3, (UINT8) sDistVisible, TRUE ) != 0 );


	// Are we killing mike?
	if (pKilledSoldier->ubProfile == MIKE && pKillerSoldier->ubWhatKindOfMercAmI == MERC_TYPE__AIM_MERC)
	{
		// Can we see?
		if ( fCanWeSeeLocation )
		{
			TacticalCharacterDialogue( pKillerSoldier, QUOTE_AIM_KILLED_MIKE );
		}
	}
	// Are we killing factory mamager?
	else if (pKilledSoldier->ubProfile == DOREEN)
	{
		// Can we see?
		//f ( fCanWeSeeLocation )
		{
			TacticalCharacterDialogue( pKillerSoldier, QUOTE_KILLED_FACTORY_MANAGER );
		}
	}
	else
	{
		// Steps here...

		// If not head shot, just say killed quote

		// If head shot...

		// If we have a head shot saying,, randomly try that one

		// If not doing that one, search for anybody who can see person

		// If somebody did, play his quote plus attackers killed quote.

		// Checkf for headhot!
		if ( pKilledSoldier->usAnimState == JFK_HITDEATH )
		{
			//Randomliy say it!
			if ( Random( 100 ) < 40 )
			{
				 TacticalCharacterDialogue( pKillerSoldier, QUOTE_HEADSHOT );
			}
			else
			{
				fDoSomeoneElse = TRUE;
			}

			if ( fDoSomeoneElse )
			{
				// Check if a person is here that has this quote....
				cnt = gTacticalStatus.Team[ gbPlayerNum ].bFirstID;

				// run through list
				SOLDIERTYPE* mercs_in_sector[20];
				for ( pTeamSoldier = MercPtrs[ cnt ]; cnt <= gTacticalStatus.Team[ gbPlayerNum ].bLastID; cnt++,pTeamSoldier++ )
				{
					if ( cnt != pKillerSoldier->ubID )
					{
						if ( OK_INSECTOR_MERC( pTeamSoldier ) && !( pTeamSoldier->uiStatusFlags & SOLDIER_GASSED ) && !AM_AN_EPC( pTeamSoldier ) )
						{
							// Can we see location?
							sDistVisible = DistanceVisible( pTeamSoldier, DIRECTION_IRRELEVANT, DIRECTION_IRRELEVANT, sGridNo, bLevel );

							if ( SoldierTo3DLocationLineOfSightTest( pTeamSoldier, sGridNo,  bLevel, 3, (UINT8) sDistVisible, TRUE ) )
							{
								mercs_in_sector[ubNumMercs++] = pTeamSoldier;
							}
						}
					}
				}

				// Did we find anybody?
				if ( ubNumMercs > 0 )
				{
					SOLDIERTYPE* const chosen = mercs_in_sector[Random(ubNumMercs)];

					// We have a random chance of not saying our we killed a guy quote
					if ( Random( 100 ) < 50 )
					{
						// Say this guys quote but the killer's quote as well....
						// if killed was not a plain old civ, say quote
						if (pKilledSoldier->bTeam != CIV_TEAM || pKilledSoldier->ubCivilianGroup != 0)
						{
							TacticalCharacterDialogue( pKillerSoldier, QUOTE_KILLED_AN_ENEMY );
						}
					}

					TacticalCharacterDialogue(chosen, QUOTE_HEADSHOT);
				}
				else
				{
					// Can we see?
					if ( fCanWeSeeLocation )
					{
						// Say this guys quote but the killer's quote as well....
						// if killed was not a plain old civ, say quote
						if (pKilledSoldier->bTeam != CIV_TEAM || pKilledSoldier->ubCivilianGroup != 0)
						{
							TacticalCharacterDialogue( pKillerSoldier, QUOTE_KILLED_AN_ENEMY );
						}
					}
				}
			}
		}
		else
		{
			// Can we see?
			if ( fCanWeSeeLocation )
			{
				// if killed was not a plain old civ, say quote
				if (pKilledSoldier->bTeam != CIV_TEAM || pKilledSoldier->ubCivilianGroup != 0)
				{
					// Are we happy with our gun?
					if ( ShouldMercSayHappyWithGunQuote( pKillerSoldier )  )
					{
						 TacticalCharacterDialogue( pKillerSoldier, QUOTE_SATISFACTION_WITH_GUN_AFTER_KILL );
						 pKillerSoldier->usQuoteSaidFlags |= SOLDIER_QUOTE_SAID_LIKESGUN;
					}
					else
					// Randomize between laugh, quote...
					{
						if ( Random( 100 ) < 33 && pKilledSoldier->ubBodyType != BLOODCAT )
						{
	            // If it's a creature......
	            if ( pKilledSoldier->uiStatusFlags & SOLDIER_MONSTER )
	            {
        				 TacticalCharacterDialogue( pKillerSoldier, QUOTE_KILLED_A_CREATURE );
              }
              else
              {
							  TacticalCharacterDialogue( pKillerSoldier, QUOTE_KILLED_AN_ENEMY );
              }
						}
						else
						{
							if ( Random( 50 ) == 25 )
							{
								DoMercBattleSound( pKillerSoldier, (INT8)( BATTLE_SOUND_LAUGH1 ) );
							}
							else
							{
								DoMercBattleSound( pKillerSoldier, (INT8)( BATTLE_SOUND_COOL1 ) );
							}
						}
					}

          // Buddy witnessed?
          SayBuddyWitnessedQuoteFromKill( pKillerSoldier, sGridNo, bLevel );

				}
			}
		}
	}
}



BOOLEAN HandleSoldierDeath( SOLDIERTYPE *pSoldier , BOOLEAN *pfMadeCorpse )
{
	BOOLEAN fBuddyJustDead = FALSE;

	*pfMadeCorpse = FALSE;

	if ( pSoldier->bLife == 0 && !( pSoldier->uiStatusFlags & SOLDIER_DEAD )  )
	{
		// Cancel services here...
		ReceivingSoldierCancelServices( pSoldier );
		GivingSoldierCancelServices( pSoldier );

		if ( pSoldier->iMuzFlash != -1 )
		{
			LightSpriteDestroy( pSoldier->iMuzFlash );
      pSoldier->iMuzFlash = -1;
		}
		if ( pSoldier->iLight != -1 )
		{
			LightSpriteDestroy( pSoldier->iLight );
		}

		//FREEUP GETTING HIT FLAG
		pSoldier->fGettingHit = FALSE;

		// Find next closest team member!
		if ( pSoldier->bTeam == gbPlayerNum )
		{
			// Set guy to close panel!
			// ONLY IF VISIBLE ON SCREEN
			if (IsMercPortraitVisible(pSoldier)) fInterfacePanelDirty = DIRTYLEVEL2;
			pSoldier->fUIdeadMerc = TRUE;

			if ( !gfKillingGuysForLosingBattle )
			{
				// ATE: THIS IS S DUPLICATE SETTING OF SOLDIER_DEAD. Is set in StrategicHandlePlayerTeamMercDeath()
				// also, but here it's needed to tell tectical to ignore this dude...
				// until StrategicHandlePlayerTeamMercDeath() can get called after death skull interface is done
				pSoldier->uiStatusFlags |= SOLDIER_DEAD;

			}
		}
		else
		{
			// IF this guy has an attacker and he's a good guy, play sound
			SOLDIERTYPE* const attacker = pSoldier->attacker;
			if (attacker != NULL)
			{
				if (attacker->bTeam == gbPlayerNum && gTacticalStatus.ubAttackBusyCount > 0)
				{
					gTacticalStatus.fKilledEnemyOnAttack	= TRUE;
					gTacticalStatus.ubEnemyKilledOnAttack = pSoldier->ubID;
					gTacticalStatus.ubEnemyKilledOnAttackLocation = pSoldier->sGridNo;
					gTacticalStatus.bEnemyKilledOnAttackLevel = pSoldier->bLevel;
					gTacticalStatus.ubEnemyKilledOnAttackKiller = pSoldier->attacker->ubID;

					// also check if we are in mapscreen, if so update soldier's list
					if( guiCurrentScreen == MAP_SCREEN )
					{
						ReBuildCharactersList( );
					}
				}
				else if ( pSoldier->bVisible == TRUE )
				{
					// We were a visible enemy, say laugh!
					if (Random(3) == 0 && !CREATURE_OR_BLOODCAT(attacker))
					{
						DoMercBattleSound(attacker, BATTLE_SOUND_LAUGH1);
					}
				}
			}

			// Handle NPC Dead
			HandleNPCTeamMemberDeath( pSoldier );

			// if a friendly with a profile, increment kills
			// militia also now track kills...
			if (attacker != NULL)
			{
				if (attacker->bTeam == gbPlayerNum)
				{
					// increment kills
					gMercProfiles[attacker->ubProfile].usKills++;
					gStrategicStatus.usPlayerKills++;
				}
				else if (attacker->bTeam == MILITIA_TEAM)
				{
					// get a kill! 2 points!
					attacker->ubMilitiaKills += 2;
				}
			}

			// JA2 Gold: if previous and current attackers are the same, the next-to-previous attacker gets the assist
			SOLDIERTYPE* assister = pSoldier->previous_attacker;
			if (assister == attacker) assister = pSoldier->next_to_previous_attacker;

			if (assister != NULL && assister != attacker)
			{
				if (assister->bTeam == gbPlayerNum )
				{
					gMercProfiles[assister->ubProfile].usAssists++;
				}
				else if (assister->bTeam == MILITIA_TEAM)
				{
					// get an assist - 1 points
					assister->ubMilitiaKills += 1;
				}
			}
		}

		if ( TurnSoldierIntoCorpse( pSoldier, TRUE, TRUE ) )
		{
			*pfMadeCorpse = TRUE;
		}

		// Remove mad as target, one he has died!
		RemoveManAsTarget( pSoldier );

		// Re-evaluate visiblitiy for the team!
		BetweenTurnsVisibilityAdjustments();

		if ( pSoldier->bTeam != gbPlayerNum )
		{
      if ( !pSoldier->fDoingExternalDeath )
      {
			  // Release attacker
			  DebugMsg(TOPIC_JA2, DBG_LEVEL_3, "@@@@@@@ Releasesoldierattacker, code 497 = handle soldier death");
			  ReleaseSoldiersAttacker( pSoldier );
      }
		}

		if ( !( *pfMadeCorpse ) )
		{
			fBuddyJustDead = TRUE;
		}

	}

	if ( pSoldier->bLife > 0 )
	{
		// If we are here - something funny has heppende
		// We either have played a death animation when we are not dead, or we are calling
		// this ani code in an animation which is not a death animation
		DebugMsg( TOPIC_JA2, DBG_LEVEL_3, "Soldier Ani: Death animation called when not dead..." );
	}

	return( fBuddyJustDead );
}


void HandlePlayerTeamMemberDeathAfterSkullAnimation(SOLDIERTYPE* pSoldier)
{
	// Release attacker
  if ( !pSoldier->fDoingExternalDeath )
  {
	  DebugMsg(TOPIC_JA2, DBG_LEVEL_3, "@@@@@@@ Releasesoldierattacker, code 497 = handle soldier death");
	  ReleaseSoldiersAttacker( pSoldier );
  }

	HandlePlayerTeamMemberDeath( pSoldier );

	// now remove character from a squad
	RemoveCharacterFromSquads( pSoldier );
}


void CheckForAndHandleSoldierDeath(SOLDIERTYPE* pSoldier, BOOLEAN* pfMadeCorpse)
{

	if ( HandleSoldierDeath( pSoldier, pfMadeCorpse ) )
	{
		// Select approriate death
		switch( pSoldier->usAnimState )
		{
			case FLYBACK_HIT_DEATH:
				ChangeSoldierState( pSoldier, FLYBACK_HITDEATH_STOP, 0, FALSE );
				break;

			case GENERIC_HIT_DEATH:
				ChangeSoldierState( pSoldier, FALLFORWARD_HITDEATH_STOP, 0, FALSE );
				break;

			case FALLBACK_HIT_DEATH:
				ChangeSoldierState( pSoldier, FALLBACK_HITDEATH_STOP, 0, FALSE );
				break;

			case PRONE_HIT_DEATH:
				ChangeSoldierState( pSoldier, PRONE_HITDEATH_STOP, 0, FALSE );
				break;

			case JFK_HITDEATH:
				ChangeSoldierState( pSoldier, JFK_HITDEATH_STOP, 0, FALSE );
				break;

			case FALLOFF_DEATH:
				ChangeSoldierState( pSoldier, FALLOFF_DEATH_STOP, 0, FALSE );
				break;

			case FALLOFF_FORWARD_DEATH:
				ChangeSoldierState( pSoldier, FALLOFF_FORWARD_DEATH_STOP, 0, FALSE );
				break;

			case WATER_DIE:
				ChangeSoldierState( pSoldier, WATER_DIE_STOP, 0, FALSE );
				break;

			case DEEP_WATER_DIE:
				ChangeSoldierState( pSoldier, DEEP_WATER_DIE_STOPPING, 0, FALSE );
				break;

			case COW_DYING:
				ChangeSoldierState( pSoldier, COW_DYING_STOP, 0, FALSE);
				break;

			case BLOODCAT_DYING:
				ChangeSoldierState( pSoldier, BLOODCAT_DYING_STOP, 0, FALSE);
				break;

			default:

				// IF we are here - something is wrong - we should have an animation stop here
				DebugMsg( TOPIC_JA2, DBG_LEVEL_3, "Soldier Ani: CODE 440 Error, Death STOP not handled" );
		}
	}
}


//#define TESTFALLBACK
//#define TESTFALLFORWARD

static void CheckForAndHandleSoldierIncompacitated(SOLDIERTYPE* pSoldier)
{
	INT16					sNewGridNo;

	if ( pSoldier->bLife < OKLIFE )
	{
		// Cancel services here...
		ReceivingSoldierCancelServices( pSoldier );
		GivingSoldierCancelServices( pSoldier );


		// If we are a monster, set life to zero ( no unconscious )
		switch( pSoldier->ubBodyType )
		{
			case ADULTFEMALEMONSTER:
			case AM_MONSTER:
			case YAF_MONSTER:
			case YAM_MONSTER:
			case LARVAE_MONSTER:
			case INFANT_MONSTER:
			case CRIPPLECIV:
			case ROBOTNOWEAPON:
			case QUEENMONSTER:
			case TANK_NW:
			case TANK_NE:

				pSoldier->bLife = 0;
				break;
		}

		// OK, if we are in a meanwhile and this is elliot...
		if ( AreInMeanwhile( ) )
		{
			SOLDIERTYPE *pQueen;

			pQueen = FindSoldierByProfileID( QUEEN, FALSE );

			if ( pQueen )
			{
				TriggerNPCWithGivenApproach( QUEEN, APPROACH_DONE_SLAPPED, FALSE );
			}
		}

		// We are unconscious now, play randomly, this animation continued, or a new death
		if ( CheckSoldierHitRoof( pSoldier ) )
		{
			return;
		}

		// If guy is now dead, play sound!
		if ( pSoldier->bLife == 0  )
		{
			if ( !AreInMeanwhile() )
			{
				DoMercBattleSound( pSoldier, BATTLE_SOUND_DIE1 );
  			pSoldier->fDeadSoundPlayed = TRUE;
			}
		}

		// Randomly fall back or forward, if we are in the standing hit animation
		if ( pSoldier->usAnimState == GENERIC_HIT_STAND || pSoldier->usAnimState == STANDING_BURST_HIT || pSoldier->usAnimState == RIFLE_STAND_HIT )
		{
			INT8			bTestDirection = pSoldier->bDirection;
			BOOLEAN		fForceDirection = FALSE;
			BOOLEAN		fDoFallback			= FALSE;


			// TRY FALLING BACKWARDS, ( ONLY IF WE ARE A MERC! )
#ifdef TESTFALLBACK
			if ( IS_MERC_BODY_TYPE( pSoldier ) )
#elif defined ( TESTFALLFORWARD )
			if ( 0 )
#else
			if ( Random(100 ) > 40 && IS_MERC_BODY_TYPE( pSoldier ) && !IsProfileATerrorist( pSoldier->ubProfile ) )
#endif
			{
				// CHECK IF WE HAVE AN ATTACKER, TAKE OPPOSITE DIRECTION!
				const SOLDIERTYPE* const attacker = pSoldier->attacker;
				if (attacker != NULL)
				{
					// Find direction!
					bTestDirection = (INT8)GetDirectionFromGridNo(attacker->sGridNo, pSoldier);
					fForceDirection = TRUE;
				}

				sNewGridNo = pSoldier->sGridNo;

				if ( OKFallDirection( pSoldier, sNewGridNo, pSoldier->bLevel, bTestDirection, FALLBACK_HIT_STAND ) )
				{
					// SECOND GRIDNO
					sNewGridNo = NewGridNo(sNewGridNo, DirectionInc(OppositeDirection(bTestDirection)));

					if ( OKFallDirection( pSoldier, sNewGridNo, pSoldier->bLevel, bTestDirection, FALLBACK_HIT_STAND ) )
					{
						// ALL'S OK HERE..... IF WE FORCED DIRECTION, SET!
						if ( fForceDirection )
						{
							EVENT_SetSoldierDesiredDirection( pSoldier, bTestDirection );
							EVENT_SetSoldierDirection( pSoldier, bTestDirection );
						}
						ChangeToFallbackAnimation( pSoldier, pSoldier->bDirection );
						return;
					}
					else
					{
						fDoFallback = TRUE;
					}
				}
				else
				{
					fDoFallback = TRUE;
				}

			}
			else
			{
				fDoFallback = TRUE;
			}

			if ( fDoFallback )
			{
				// 1 )REC DIRECTION
				// 2 ) SET FLAG FOR STARTING TO FALL
        BeginTyingToFall( pSoldier );
				ChangeSoldierState( pSoldier, FALLFORWARD_FROMHIT_STAND, 0, FALSE );
				return;
			}

		}
		else if ( pSoldier->usAnimState == GENERIC_HIT_CROUCH )
		{
			ChangeSoldierState( pSoldier, FALLFORWARD_FROMHIT_CROUCH, 0 , FALSE);
      BeginTyingToFall( pSoldier );
			return;
		}
		else if ( pSoldier->usAnimState == GENERIC_HIT_PRONE )
		{
			ChangeSoldierState( pSoldier, PRONE_LAY_FROMHIT, 0 , FALSE );
			return;
		}
		else if ( pSoldier->usAnimState == ADULTMONSTER_HIT )
		{
			ChangeSoldierState( pSoldier, ADULTMONSTER_DYING, 0 , FALSE );
			return;
		}
		else if ( pSoldier->usAnimState == LARVAE_HIT )
		{
			ChangeSoldierState( pSoldier, LARVAE_DIE, 0 , FALSE );
			return;
		}
		else if ( pSoldier->usAnimState == QUEEN_HIT )
		{
			ChangeSoldierState( pSoldier, QUEEN_DIE, 0 , FALSE );
			return;
		}
		else if ( pSoldier->usAnimState == CRIPPLE_HIT )
		{
			ChangeSoldierState( pSoldier, CRIPPLE_DIE, 0 , FALSE );
			return;
		}
		else if ( pSoldier->usAnimState == ROBOTNW_HIT )
		{
			ChangeSoldierState( pSoldier, ROBOTNW_DIE, 0 , FALSE );
			return;
		}
		else if ( pSoldier->usAnimState == INFANT_HIT )
		{
			ChangeSoldierState( pSoldier, INFANT_DIE, 0 , FALSE );
			return;
		}
		else if ( pSoldier->usAnimState == COW_HIT )
		{
			ChangeSoldierState( pSoldier, COW_DYING, 0 , FALSE );
			return;
		}
		else if ( pSoldier->usAnimState == BLOODCAT_HIT )
		{
			ChangeSoldierState( pSoldier, BLOODCAT_DYING, 0 , FALSE );
			return;
		}
		else if ( pSoldier->usAnimState == WATER_HIT )
		{
			ChangeSoldierState( pSoldier, WATER_DIE, 0 , FALSE );
			return;
		}
		else if ( pSoldier->usAnimState == DEEP_WATER_HIT )
		{
			ChangeSoldierState( pSoldier, DEEP_WATER_DIE, 0 , FALSE );
			return;
		}
		else
		{
			// We have missed something here - send debug msg
			DebugMsg( TOPIC_JA2, DBG_LEVEL_3, "Soldier Ani: Genmeric hit not chained" );
		}
	}

}


BOOLEAN CheckForAndHandleSoldierDyingNotFromHit( SOLDIERTYPE *pSoldier )
{
	if ( pSoldier->bLife == 0 )
	{
		DoMercBattleSound( pSoldier, BATTLE_SOUND_DIE1 );
		pSoldier->fDeadSoundPlayed = TRUE;

	  // Increment  being attacked count
	  pSoldier->bBeingAttackedCount++;

		if ( gGameSettings.fOptions[ TOPTION_BLOOD_N_GORE ] )
		{
			switch( pSoldier->usAnimState )
			{
				case FLYBACKHIT_STOP:
					ChangeSoldierState( pSoldier, FLYBACK_HIT_DEATH, 0, FALSE );
					break;

				case FALLFORWARD_FROMHIT_STAND:
				case FALLFORWARD_FROMHIT_CROUCH:
				case STAND_FALLFORWARD_STOP:
					ChangeSoldierState( pSoldier, GENERIC_HIT_DEATH, 0, FALSE );
					break;

				case FALLBACKHIT_STOP:
					ChangeSoldierState( pSoldier, FALLBACK_HIT_DEATH, 0, FALSE );
					break;

				case PRONE_LAYFROMHIT_STOP:
        case PRONE_LAY_FROMHIT:

					ChangeSoldierState( pSoldier, PRONE_HIT_DEATH, 0, FALSE );
					break;

				case FALLOFF_STOP:
					ChangeSoldierState( pSoldier, FALLOFF_DEATH, 0, FALSE );
					break;

				case FALLOFF_FORWARD_STOP:
					ChangeSoldierState( pSoldier, FALLOFF_FORWARD_DEATH, 0, FALSE);
					break;

        case ADULTMONSTER_HIT:
    			ChangeSoldierState( pSoldier, ADULTMONSTER_DYING, 0 , FALSE );
		    	break;

        case LARVAE_HIT:
	    		ChangeSoldierState( pSoldier, LARVAE_DIE, 0 , FALSE );
			    break;

        case QUEEN_HIT:
    			ChangeSoldierState( pSoldier, QUEEN_DIE, 0 , FALSE );
		    	break;

        case CRIPPLE_HIT:
    			ChangeSoldierState( pSoldier, CRIPPLE_DIE, 0 , FALSE );
		    	break;

        case ROBOTNW_HIT:
    			ChangeSoldierState( pSoldier, ROBOTNW_DIE, 0 , FALSE );
			    break;

        case INFANT_HIT:
    			ChangeSoldierState( pSoldier, INFANT_DIE, 0 , FALSE );
		    	break;

        case COW_HIT:
    			ChangeSoldierState( pSoldier, COW_DYING, 0 , FALSE );
		    	break;

        case BLOODCAT_HIT:
    			ChangeSoldierState( pSoldier, BLOODCAT_DYING, 0 , FALSE );
		    	break;

				default:
					DebugMsg( TOPIC_JA2, DBG_LEVEL_3, String( "Soldier Control: Death state %d has no death hit", pSoldier->usAnimState ) );
					{
						BOOLEAN fMadeCorpse;
						CheckForAndHandleSoldierDeath( pSoldier, &fMadeCorpse );
					}
					break;

			}
		}
		else
		{
			BOOLEAN fMadeCorpse;

			CheckForAndHandleSoldierDeath( pSoldier, &fMadeCorpse );
		}
		return( TRUE );
	}

	return( FALSE );
}


static BOOLEAN CheckForImproperFireGunEnd(SOLDIERTYPE* pSoldier)
{

	if ( AM_A_ROBOT( pSoldier ) )
	{
		return( FALSE );
	}

	// Check single hand for jammed status, ( or ammo is out.. )
	if ( pSoldier->inv[ HANDPOS ].bGunAmmoStatus < 0 || pSoldier->inv[ HANDPOS ].ubGunShotsLeft == 0 )
	{
		// If we have 2 pistols, donot go back!
		if ( Item[ pSoldier->inv[ SECONDHANDPOS ].usItem ].usItemClass != IC_GUN )
		{
			// OK, put gun down....
			InternalSoldierReadyWeapon( pSoldier, pSoldier->bDirection, TRUE );
			return( TRUE );
		}
	}

	return( FALSE );

}


static BOOLEAN HandleUnjamAnimation(SOLDIERTYPE* pSoldier)
{
	// OK, play intermediate animation here..... save in pending animation data, the current
	// code we are at!
	pSoldier->uiPendingActionData1 = pSoldier->usAniCode;
	pSoldier->sPendingActionData2  = pSoldier->usAnimState;
	// Check what animatnion we should do.....
	switch( pSoldier->usAnimState )
	{
		case SHOOT_RIFLE_STAND:
		case STANDING_BURST:
		case FIRE_STAND_BURST_SPREAD:
			// Normal shoot rifle.... play
			ChangeSoldierState( pSoldier, STANDING_SHOOT_UNJAM, 0 , FALSE );
			return( TRUE );

		case PRONE_BURST:
		case SHOOT_RIFLE_PRONE:

			// Normal shoot rifle.... play
			ChangeSoldierState( pSoldier, PRONE_SHOOT_UNJAM, 0 , FALSE );
			return( TRUE );

		case CROUCHED_BURST:
		case SHOOT_RIFLE_CROUCH:

			// Normal shoot rifle.... play
			ChangeSoldierState( pSoldier, CROUCH_SHOOT_UNJAM, 0 , FALSE );
			return( TRUE );

		case SHOOT_DUAL_STAND:

			// Normal shoot rifle.... play
			ChangeSoldierState( pSoldier, STANDING_SHOOT_DWEL_UNJAM, 0 , FALSE );
			return( TRUE );

		case SHOOT_DUAL_PRONE:

			// Normal shoot rifle.... play
			ChangeSoldierState( pSoldier, PRONE_SHOOT_DWEL_UNJAM, 0 , FALSE );
			return( TRUE );

		case SHOOT_DUAL_CROUCH:

			// Normal shoot rifle.... play
			ChangeSoldierState( pSoldier, CROUCH_SHOOT_DWEL_UNJAM, 0 , FALSE );
			return( TRUE );

		case FIRE_LOW_STAND:
		case FIRE_BURST_LOW_STAND:

			// Normal shoot rifle.... play
			ChangeSoldierState( pSoldier, STANDING_SHOOT_LOW_UNJAM, 0 , FALSE );
			return( TRUE );

	}

	return( FALSE );
}


BOOLEAN OKFallDirection( SOLDIERTYPE *pSoldier, INT16 sGridNo, INT8 bLevel, INT8 bTestDirection, UINT16 usAnimState )
{
	STRUCTURE_FILE_REF *	pStructureFileRef;
	UINT16								usAnimSurface;

	// How are the movement costs?
	if ( gubWorldMovementCosts[ sGridNo ][ bTestDirection ][ bLevel ] > TRAVELCOST_SHORE )
	{
		return( FALSE );
	}

  //NOT ok if in water....
	if ( GetTerrainType( sGridNo ) == MED_WATER || GetTerrainType( sGridNo ) == DEEP_WATER || GetTerrainType( sGridNo ) == LOW_WATER )
  {
    return( FALSE );
  }

	// How are we for OK dest?
	if (!NewOKDestination( pSoldier, sGridNo, TRUE, bLevel ) )
	{
		return( FALSE );
	}

	usAnimSurface = DetermineSoldierAnimationSurface( pSoldier, usAnimState );
	pStructureFileRef = GetAnimationStructureRef( pSoldier->ubID, usAnimSurface, usAnimState );

	if ( pStructureFileRef )
	{
		UINT16		usStructureID;
		INT16			sTestGridNo;

		// must make sure that structure data can be added in the direction of the target

		usStructureID = pSoldier->ubID;

		// Okay this is really SCREWY but it's due to the way this function worked before and must
		// work now.  The function is passing in an adjacent gridno but we need to place the structure
		// data in the tile BEFORE.  So we take one step back in the direction opposite to bTestDirection
		// and use that gridno
		sTestGridNo = NewGridNo(sGridNo, DirectionInc(OppositeDirection(bTestDirection)));

		if (!OkayToAddStructureToWorld(sTestGridNo, bLevel, &pStructureFileRef->pDBStructureRef[OneCDirection(bTestDirection)], usStructureID))
		{
			// can't go in that dir!
			return( FALSE );
		}
	}

	return( TRUE );
}

BOOLEAN HandleCheckForDeathCommonCode( SOLDIERTYPE *pSoldier )
{
	// Do we have a primary pending animation?
	if ( pSoldier->usPendingAnimation2 != NO_PENDING_ANIMATION )
	{
		ChangeSoldierState( pSoldier, pSoldier->usPendingAnimation2, 0, FALSE );
		pSoldier->usPendingAnimation2 = NO_PENDING_ANIMATION;
		return( TRUE );
	}

	// CHECK IF WE HAVE A PENDING ANIMATION HERE
	if ( pSoldier->usPendingAnimation != NO_PENDING_ANIMATION )
	{
		ChangeSoldierState( pSoldier, pSoldier->usPendingAnimation, 0, FALSE );
		pSoldier->usPendingAnimation = NO_PENDING_ANIMATION;
		return( TRUE );
	}

	// OTHERWISE, GOTO APPROPRIATE STOPANIMATION!
	pSoldier->bCollapsed = TRUE;

  // CC has requested - handle sight here...
	HandleSight( pSoldier, SIGHT_LOOK );

	// ATE: If it is our turn, make them try to getup...
	if ( gTacticalStatus.ubCurrentTeam == pSoldier->bTeam )
	{
		// Try to getup...
		BeginSoldierGetup( pSoldier );

		// Check this to see if above worked
		if ( !pSoldier->bCollapsed )
		{
			return( TRUE );
		}
	}

	switch( pSoldier->usAnimState )
	{
		case FLYBACK_HIT:
			ChangeSoldierState( pSoldier, FLYBACKHIT_STOP, 0, FALSE );
			break;

		case GENERIC_HIT_DEATHTWITCHNB:
		case FALLFORWARD_FROMHIT_STAND:
		case ENDFALLFORWARD_FROMHIT_CROUCH:

			ChangeSoldierState( pSoldier, STAND_FALLFORWARD_STOP, 0, FALSE );
			break;

		case FALLBACK_HIT_DEATHTWITCHNB:
		case FALLBACK_HIT_STAND:
			ChangeSoldierState( pSoldier, FALLBACKHIT_STOP, 0, FALSE );
			break;

		case PRONE_HIT_DEATHTWITCHNB:
		case PRONE_LAY_FROMHIT:

			ChangeSoldierState( pSoldier, PRONE_LAYFROMHIT_STOP, 0, FALSE );
			break;

		case FALLOFF:
			ChangeSoldierState( pSoldier, FALLOFF_STOP, 0, FALSE );
			break;

		case FALLFORWARD_ROOF:
			ChangeSoldierState( pSoldier, FALLOFF_FORWARD_STOP, 0, FALSE);
			break;

		default:
			// IF we are here - something is wrong - we should have a death animation here
			DebugMsg( TOPIC_JA2, DBG_LEVEL_3, String( "Soldier Ani: unconscious hit sequence needed for animation %d", pSoldier->usAnimState ) );

	}
	// OTHERWISE, GOTO APPROPRIATE STOPANIMATION!
	pSoldier->bCollapsed = TRUE;

	// ATE: If it is our turn, make them try to getup...
	if ( gTacticalStatus.ubCurrentTeam == pSoldier->bTeam )
	{
		// Try to getup...
		BeginSoldierGetup( pSoldier );

		// Check this to see if above worked
		if ( !pSoldier->bCollapsed )
		{
			return( TRUE );
		}
	}

	switch( pSoldier->usAnimState )
	{
		case FLYBACK_HIT:
			ChangeSoldierState( pSoldier, FLYBACKHIT_STOP, 0, FALSE );
			break;

		case GENERIC_HIT_DEATHTWITCHNB:
		case FALLFORWARD_FROMHIT_STAND:
		case ENDFALLFORWARD_FROMHIT_CROUCH:

			ChangeSoldierState( pSoldier, STAND_FALLFORWARD_STOP, 0, FALSE );
			break;

		case FALLBACK_HIT_DEATHTWITCHNB:
		case FALLBACK_HIT_STAND:
			ChangeSoldierState( pSoldier, FALLBACKHIT_STOP, 0, FALSE );
			break;

		case PRONE_HIT_DEATHTWITCHNB:
		case PRONE_LAY_FROMHIT:

			ChangeSoldierState( pSoldier, PRONE_LAYFROMHIT_STOP, 0, FALSE );
			break;

		case FALLOFF:
			ChangeSoldierState( pSoldier, FALLOFF_STOP, 0, FALSE );
			break;

		case FALLFORWARD_ROOF:
			ChangeSoldierState( pSoldier, FALLOFF_FORWARD_STOP, 0, FALSE);
			break;

		default:
			// IF we are here - something is wrong - we should have a death animation here
			DebugMsg( TOPIC_JA2, DBG_LEVEL_3, String( "Soldier Ani: unconscious hit sequence needed for animation %d", pSoldier->usAnimState ) );

	}

	return( TRUE );
}


void KickOutWheelchair( SOLDIERTYPE *pSoldier )
{
	INT16 sNewGridNo;

	// Move forward one gridno....
	sNewGridNo = NewGridNo( (UINT16)pSoldier->sGridNo, (UINT16)( DirectionInc( pSoldier->bDirection ) ) );

	// ATE: Make sure that the gridno is unoccupied!
	if ( !NewOKDestination( pSoldier, sNewGridNo, TRUE, pSoldier->bLevel ) )
	{
		// We should just stay put - will look kind of funny but nothing I can do!
		sNewGridNo = pSoldier->sGridNo;
	}

	EVENT_StopMerc( pSoldier, sNewGridNo, pSoldier->bDirection );
	pSoldier->ubBodyType = REGMALE;
	if ( pSoldier->ubProfile == SLAY && pSoldier->bTeam == CIV_TEAM && !pSoldier->bNeutral )
	{
		HandleNPCDoAction( pSoldier->ubProfile, NPC_ACTION_THREATENINGLY_RAISE_GUN, 0 );
	}
	else
	{
		EVENT_InitNewSoldierAnim( pSoldier, STANDING, 0 , TRUE );
	}

	// If this person has a profile ID, set body type to regmale
	if ( pSoldier->ubProfile != NO_PROFILE )
	{
		gMercProfiles[ pSoldier->ubProfile ].ubBodyType = REGMALE;
	}

}
