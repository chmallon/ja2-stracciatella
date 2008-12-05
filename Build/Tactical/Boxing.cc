#include "Font_Control.h"
#include "Soldier_Control.h"
#include "Overhead.h"
#include "Boxing.h"
#include "Render_Fun.h"
#include "Random.h"
#include "Timer_Control.h"
#include "WorldMan.h"
#include "Soldier_Profile.h"
#include "NPC.h"
#include "OppList.h"
#include "AI.h"
#include "Handle_UI.h"
#include "Points.h"
#include "Interface.h"
#include "Interface_Dialogue.h"
#include "TeamTurns.h"
#include "Music_Control.h"
#include "History.h"
#include "Game_Clock.h"
#include "StrategicMap.h"
#include "Animation_Data.h"
#include "Items.h"

#ifdef JA2TESTVERSION
#	include "Message.h"
#endif


INT16	gsBoxerGridNo[ NUM_BOXERS ] = { 11393, 11233, 11073 };
SOLDIERTYPE* gBoxer[NUM_BOXERS];
BOOLEAN gfBoxerFought[ NUM_BOXERS ] = { FALSE, FALSE, FALSE };
BOOLEAN	gfLastBoxingMatchWonByPlayer = FALSE;
UINT8	gubBoxingMatchesWon = 0;
UINT8	gubBoxersRests = 0;
BOOLEAN gfBoxersResting = FALSE;


void ExitBoxing(void)
{
	// find boxers and turn them neutral again

	// first time through loop, look for AI guy, then for PC guy.... for stupid
	// oppcnt/alert status reasons
	for (UINT8 ubPass = 0; ubPass < 2; ++ubPass)
	{
		// because boxer could die, loop through all soldier ptrs
		FOR_ALL_NON_PLANNING_SOLDIERS(s)
		{
			if (!(s->uiStatusFlags & SOLDIER_BOXER)) continue;
			if (GetRoom(s->sGridNo) != BOXING_RING)  continue;

			if (s->uiStatusFlags & SOLDIER_PC)
			{
				if (ubPass == 0) continue; // pass 0, only handle AI
				// put guy under AI control temporarily
				s->uiStatusFlags |= SOLDIER_PCUNDERAICONTROL;
			}
			else
			{
				if (ubPass == 1) continue; // pass 1, only handle PCs
				// reset AI boxer to neutral
				SetSoldierNeutral(s);
				RecalculateOppCntsDueToBecomingNeutral(s);
			}
			CancelAIAction(s);
			s->bAlertStatus = STATUS_GREEN;
			s->bUnderFire   = 0;

			// if necessary, revive boxer so he can leave ring
			if (s->bLife > 0 && (s->bLife < OKLIFE || s->bBreath < OKBREATH))
			{
				s->bLife = __max(OKLIFE * 2, s->bLife);
				if (s->bBreath < 100)
				{
					// deduct -ve BPs to grant some BPs back (properly)
					DeductPoints(s, 0, -100 * (100 - s->bBreath));
				}
				BeginSoldierGetup(s);
			}
		}
	}

	DeleteTalkingMenu();
	EndAllAITurns();

	if (CheckForEndOfCombatMode(FALSE))
	{
		EndTopMessage();
		SetMusicMode(MUSIC_TACTICAL_NOTHING);
		// Lock UI until we get out of the ring
		guiPendingOverrideEvent = LU_BEGINUILOCK;
	}
}


// in both these cases we're going to want the AI to take over and move the boxers
// out of the ring!
void EndBoxingMatch( SOLDIERTYPE * pLoser )
{
	if (pLoser->bTeam == gbPlayerNum )
	{
		SetBoxingState( LOST_ROUND );
	}
	else
	{
		SetBoxingState( WON_ROUND );
		gfLastBoxingMatchWonByPlayer = TRUE;
		gubBoxingMatchesWon++;
	}
	TriggerNPCRecord( DARREN, 22 );
}

void BoxingPlayerDisqualified( SOLDIERTYPE * pOffender, INT8 bReason )
{
	if (bReason == BOXER_OUT_OF_RING || bReason == NON_BOXER_IN_RING)
	{
		EVENT_StopMerc( pOffender, pOffender->sGridNo, pOffender->bDirection );
	}
	SetBoxingState( DISQUALIFIED );
	TriggerNPCRecord( DARREN, 21 );
	//ExitBoxing();
}

void TriggerEndOfBoxingRecord( SOLDIERTYPE * pSoldier )
{
	// unlock UI
	guiPendingOverrideEvent = LU_ENDUILOCK;

	if ( pSoldier )
	{
		switch( gTacticalStatus.bBoxingState )
		{
			case WON_ROUND:
				AddHistoryToPlayersLog( HISTORY_WON_BOXING, pSoldier->ubProfile, GetWorldTotalMin(), gWorldSectorX, gWorldSectorY );
				TriggerNPCRecord( DARREN, 23 );
				break;
			case LOST_ROUND:
				// log as lost
				AddHistoryToPlayersLog( HISTORY_LOST_BOXING, pSoldier->ubProfile, GetWorldTotalMin(), gWorldSectorX, gWorldSectorY );
				TriggerNPCRecord( DARREN, 24 );
				break;
			case DISQUALIFIED:
				AddHistoryToPlayersLog( HISTORY_DISQUALIFIED_BOXING, pSoldier->ubProfile, GetWorldTotalMin(), gWorldSectorX, gWorldSectorY );
				break;
		}
	}

	SetBoxingState( NOT_BOXING );
}

UINT8 CountPeopleInBoxingRing( void )
{
	UINT8 ubTotalInRing = 0;

	FOR_ALL_MERCS(i)
	{
		if (GetRoom((*i)->sGridNo) == BOXING_RING)
		{
			++ubTotalInRing;
		}
	}

	return( ubTotalInRing );
}


static BOOLEAN PickABoxer(void);


static void CountPeopleInBoxingRingAndDoActions(void)
{
	UINT8						ubTotalInRing = 0;
	UINT8						ubPlayersInRing = 0;
	SOLDIERTYPE *		pInRing[2] = { NULL, NULL };
	SOLDIERTYPE *		pNonBoxingPlayer = NULL;

	FOR_ALL_MERCS(i)
	{
		SOLDIERTYPE* const s = *i;
		if (GetRoom(s->sGridNo) != BOXING_RING) continue;

		if (ubTotalInRing < 2) pInRing[ubTotalInRing] = s;
		++ubTotalInRing;

		if (s->uiStatusFlags & SOLDIER_PC)
		{
			++ubPlayersInRing;
			if (!pNonBoxingPlayer && !(s->uiStatusFlags & SOLDIER_BOXER))
			{
				pNonBoxingPlayer = s;
			}
		}
	}

	if ( ubPlayersInRing > 1 )
	{
		// boxing match just became invalid!
		if ( gTacticalStatus.bBoxingState <= PRE_BOXING )
		{
			BoxingPlayerDisqualified( pNonBoxingPlayer, NON_BOXER_IN_RING );
			// set to not in boxing or it won't be handled otherwise
			SetBoxingState( NOT_BOXING );
		}
		else
		{
			BoxingPlayerDisqualified( pNonBoxingPlayer, NON_BOXER_IN_RING );
		}

		return;
	}


	if ( gTacticalStatus.bBoxingState == BOXING_WAITING_FOR_PLAYER )
	{
		if ( ubTotalInRing == 1 && ubPlayersInRing == 1 )
		{
			// time to go to pre-boxing
			SetBoxingState( PRE_BOXING );
			PickABoxer();
		}
	}
	else
	// if pre-boxing, check for two people (from different teams!) in the ring
	if ( gTacticalStatus.bBoxingState == PRE_BOXING )
	{
		if ( ubTotalInRing == 2 && ubPlayersInRing == 1 )
		{
			// ladieees and gennleman, we have a fight!
			for (UINT32 uiLoop = 0; uiLoop < 2; ++uiLoop)
			{
				if ( ! ( pInRing[ uiLoop ]->uiStatusFlags & SOLDIER_BOXER ) )
				{
					// set as boxer!
					pInRing[ uiLoop ]->uiStatusFlags |= SOLDIER_BOXER;
				}
			}
			// start match!
			SetBoxingState( BOXING );
			gfLastBoxingMatchWonByPlayer = FALSE;

			#ifdef JA2TESTVERSION
				ScreenMsg( FONT_MCOLOR_LTYELLOW, MSG_INTERFACE, L"Boxer APs %d %d", pInRing[0]->bActionPoints, pInRing[1]->bActionPoints );
			#endif
			// give the first turn to a randomly chosen boxer
			EnterCombatMode( pInRing[ Random( 2 ) ]->bTeam );
		}
	}
	/*
	else
	{
		// check to see if the player has more than one person in the ring
		if ( ubPlayersInRing > 1 )
		{
			// boxing match just became invalid!
			BoxingPlayerDisqualified( pNonBoxingPlayer, NON_BOXER_IN_RING );
			return;
		}
	}
	*/

}

BOOLEAN CheckOnBoxers( void )
{
	UINT32					uiLoop;

	// repick boxer IDs every time
	if (gBoxer[0] == NULL)
	{
		// get boxer soldier IDs!
		for( uiLoop = 0; uiLoop < NUM_BOXERS; uiLoop++ )
		{
			SOLDIERTYPE* const s = WhoIsThere2(gsBoxerGridNo[uiLoop], 0);
			if (FindObjClass(s, IC_WEAPON) == NO_SLOT)
			{
				// no weapon so this guy is a boxer
				gBoxer[uiLoop] = s;
			}
		}
	}

	if (gBoxer[0] == NULL && gBoxer[1] == NULL && gBoxer[2] == NULL)
	{
		return( FALSE );
	}

	return( TRUE );
}

BOOLEAN BoxerExists( void )
{
	UINT32	uiLoop;

	for( uiLoop = 0; uiLoop < NUM_BOXERS; uiLoop++ )
	{
		if (WhoIsThere2(gsBoxerGridNo[uiLoop], 0) != NULL)
		{
			return( TRUE );
		}
	}
	return( FALSE );
}


static BOOLEAN PickABoxer(void)
{
	UINT32					uiLoop;

	for( uiLoop = 0; uiLoop < NUM_BOXERS; uiLoop++ )
	{
		SOLDIERTYPE* const pBoxer = gBoxer[uiLoop];
		if (pBoxer != NULL)
		{
			if ( gfBoxerFought[ uiLoop ] )
			{
				// pathetic attempt to prevent multiple AI boxers
				pBoxer->uiStatusFlags &= ~SOLDIER_BOXER;
			}
			else
			{
				// pick this boxer!
				if ( pBoxer->bActive && pBoxer->bInSector && pBoxer->bLife >= OKLIFE )
				{
					pBoxer->uiStatusFlags |= SOLDIER_BOXER;
					SetSoldierNonNeutral( pBoxer );
					RecalculateOppCntsDueToNoLongerNeutral( pBoxer );
					CancelAIAction(pBoxer);
					RESETTIMECOUNTER( pBoxer->AICounter, 0 );
					gfBoxerFought[ uiLoop ] = TRUE;
					// improve stats based on the # of rests these guys have had
					pBoxer->bStrength = __min( 100, pBoxer->bStrength += gubBoxersRests * 5 );
					pBoxer->bDexterity = __min( 100, pBoxer->bDexterity + gubBoxersRests * 5 );
					pBoxer->bAgility = __min( 100, pBoxer->bAgility + gubBoxersRests * 5 );
					pBoxer->bLifeMax = __min( 100, pBoxer->bLifeMax + gubBoxersRests * 5 );
					// give the 3rd boxer martial arts
					if ( (uiLoop == NUM_BOXERS - 1) && pBoxer->ubBodyType == REGMALE )
					{
						pBoxer->ubSkillTrait1 = MARTIALARTS;
					}
					return( TRUE );
				}
			}
		}
	}

	return( FALSE );
}

BOOLEAN BoxerAvailable( void )
{
	UINT8			ubLoop;

	// No way around this, BoxerAvailable will have to go find boxer IDs if they aren't set.
	if (!CheckOnBoxers()) return FALSE;

	for( ubLoop = 0; ubLoop < NUM_BOXERS; ubLoop++ )
	{
		if (gBoxer[ubLoop] != NULL && !gfBoxerFought[ubLoop]) return TRUE;
	}

	return( FALSE );
}


// NOTE THIS IS NOW BROKEN BECAUSE NPC.C ASSUMES THAT BOXERSAVAILABLE < 3 IS A
// SEQUEL FIGHT.   Maybe we could check Kingpin's location instead!
static UINT8 BoxersAvailable(void)
{
	UINT8			ubLoop;
	UINT8			ubCount = 0;

	for( ubLoop = 0; ubLoop < NUM_BOXERS; ubLoop++ )
	{
		if (gBoxer[ubLoop] != NULL && !gfBoxerFought[ubLoop]) ubCount++;
	}

	return( ubCount );
}

BOOLEAN AnotherFightPossible( void )
{
	// Check that and a boxer is still available and
	// a player has at least OKLIFE + 5 life

	// and at least one fight HAS occurred
	UINT8						ubAvailable;

	ubAvailable = BoxersAvailable();

	if ( ubAvailable == NUM_BOXERS || ubAvailable == 0 )
	{
		return( FALSE );
	}

	CFOR_ALL_IN_TEAM(s, gbPlayerNum)
	{
		if (s->bInSector && s->bLife > OKLIFE + 5 && !s->bCollapsed)
		{
			return TRUE;
		}
	}

	return( FALSE );
}


void BoxingMovementCheck( SOLDIERTYPE * pSoldier )
{
	if (GetRoom(pSoldier->sGridNo) == BOXING_RING)
	{
		// someone moving in/into the ring
		CountPeopleInBoxingRingAndDoActions();
	}
	else if ( ( gTacticalStatus.bBoxingState == BOXING ) && ( pSoldier->uiStatusFlags & SOLDIER_BOXER ) )
	{
		// boxer stepped out of the ring!
		BoxingPlayerDisqualified( pSoldier, BOXER_OUT_OF_RING );
		// add the history record here.
		AddHistoryToPlayersLog( HISTORY_DISQUALIFIED_BOXING, pSoldier->ubProfile, GetWorldTotalMin(), gWorldSectorX, gWorldSectorY );
		// make not a boxer any more
		pSoldier->uiStatusFlags &= ~(SOLDIER_BOXER);
		pSoldier->uiStatusFlags &= (~SOLDIER_PCUNDERAICONTROL);
	}
}

void SetBoxingState( INT8 bNewState )
{
	if ( gTacticalStatus.bBoxingState == NOT_BOXING )
	{
		if ( bNewState != NOT_BOXING )
		{
			// pause time
			PauseGame();
		}

	}
	else
	{
		if ( bNewState == NOT_BOXING )
		{
			// unpause time
			UnPauseGame();

			if ( BoxersAvailable() == NUM_BOXERS )
			{
				// set one boxer to be set as boxed so that the game will allow another
				// fight to occur
				gfBoxerFought[ 0 ] = TRUE;
			}

		}
	}
	gTacticalStatus.bBoxingState = bNewState;
	#ifdef JA2TESTVERSION
		ScreenMsg( FONT_MCOLOR_LTYELLOW, MSG_UI_FEEDBACK, L"Boxing state now %d", bNewState );
	#endif
}


void ClearAllBoxerFlags(void)
{
	FOR_ALL_MERCS(i) (*i)->uiStatusFlags &= ~SOLDIER_BOXER;
}
