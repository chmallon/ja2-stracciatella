#ifdef PRECOMPILEDHEADERS
	#include "Tactical All.h"
#else
	#include <wchar.h>
	#include <stdio.h>
	#include <string.h>
	#include "WCheck.h"
	#include "stdlib.h"
	#include "Debug.h"
	#include "MemMan.h"
	#include "Overhead_Types.h"
	#include "Soldier_Control.h"
	#include "Random.h"
	#include "Campaign.h"
	#include "Dialogue_Control.h"
	#include "Map_Screen_Interface.h"
	#include "Message.h"
	#include "Game_Clock.h"
	#include "Strategic_Mines.h"
	#include "Strategic_Status.h"
	#include "Sys_Globals.h"
	#include "Text.h"
	#include "GameSettings.h"
	#include "Assignments.h"
	#include "MapScreen.h"
	#include "Interface.h"
	#include "Game_Event_Hook.h"
	#include "Overhead.h"
	#include "Meanwhile.h"
	#include "Quests.h"
	#include "Soldier_Macros.h"
	#include "Squads.h"
	#include "Strategic.h"
	#include "StrategicMap.h"
	#include "Town_Militia.h"
#endif

#include "EMail.h"


extern	UINT8	gbPlayerNum;


#ifdef JA2TESTVERSION
// comment out to get rid of stat change msgs
//#define STAT_CHANGE_DEBUG
#endif

#ifdef STAT_CHANGE_DEBUG
const wchar_t* const  wDebugStatStrings[] = {
	L"",
	L"Life (Max)",
  L"Agility",
  L"Dexterity",
  L"Wisdom",
  L"Medical",
  L"Explosives",
  L"Mechanical",
	L"Marksmanship",
  L"Experience Level",
  L"Strength",
  L"Leadership",
};
#endif


static void UpdateStats(SOLDIERTYPE* pSoldier);
static void ProcessStatChange(MERCPROFILESTRUCT* pProfile, UINT8 ubStat, UINT16 usNumChances, UINT8 ubReason);


// give pSoldier usNumChances to improve ubStat.  If it's from training, it doesn't count towards experience level gain
void StatChange(SOLDIERTYPE *pSoldier, UINT8 ubStat, UINT16 usNumChances, UINT8 ubReason)
{
	Assert(pSoldier != NULL);
	Assert(pSoldier->bActive);

	// ignore non-player soldiers
	if (!PTR_OURTEAM)
		return;

	// ignore anything without a profile
	if (pSoldier->ubProfile == NO_PROFILE)
		return;

	// ignore vehicles and robots
	if( ( pSoldier->uiStatusFlags & SOLDIER_VEHICLE ) || ( pSoldier->uiStatusFlags & SOLDIER_ROBOT ) )
		return;

	if( pSoldier->bAssignment == ASSIGNMENT_POW )
	{
		ScreenMsg( FONT_ORANGE, MSG_BETAVERSION, L"ERROR: StatChange: %S improving stats while POW! ubStat %d", pSoldier->name, ubStat );
		return;
	}

	// no points earned while somebody is unconscious (for assist XPs, and such)
	if ( pSoldier->bLife < CONSCIOUSNESS )
		return;


#ifdef TESTVERSION
	if ( gTacticalStatus.fStatChangeCheatOn )
	{
		usNumChances = 100;
	}
#endif

	ProcessStatChange( &( gMercProfiles[ pSoldier->ubProfile ] ), ubStat, usNumChances, ubReason );

	// Update stats....right away... ATE
	UpdateStats( pSoldier );
}


static void ProfileUpdateStats(MERCPROFILESTRUCT* pProfile);


// this is the equivalent of StatChange(), but for use with mercs not currently on player's team
// give pProfile usNumChances to improve ubStat.  If it's from training, it doesn't count towards experience level gain
static void ProfileStatChange(MERCPROFILESTRUCT* pProfile, UINT8 ubStat, UINT16 usNumChances, UINT8 ubReason)
{
	// dead guys don't do nuthin' !
	if ( pProfile->bMercStatus == MERC_IS_DEAD )
		return;

	if ( pProfile->bLife < OKLIFE )
		return;

	ProcessStatChange( pProfile, ubStat, usNumChances, ubReason );

	// Update stats....right away... ATE
	ProfileUpdateStats( pProfile );
}


static UINT16 SubpointsPerPoint(UINT8 ubStat, INT8 bExpLevel);


static void ProcessStatChange(MERCPROFILESTRUCT* pProfile, UINT8 ubStat, UINT16 usNumChances, UINT8 ubReason)
{
  UINT32 uiCnt,uiEffLevel;
  INT16 sSubPointChange = 0;
	UINT16 usChance=0;
	UINT16 usSubpointsPerPoint;
	UINT16 usSubpointsPerLevel;
	INT8 bCurrentRating;
  UINT16 *psStatGainPtr;
	BOOLEAN fAffectedByWisdom = TRUE;

	Assert(pProfile != NULL);

  if ( pProfile->bEvolution == NO_EVOLUTION )
    return;     // No change possible, quit right away

  // if this is a Reverse-Evolving merc who attempting to train
  if ( ( ubReason == FROM_TRAINING ) && ( pProfile->bEvolution == DEVOLVE ) )
    return;	// he doesn't get any benefit, but isn't penalized either

	if (usNumChances == 0)
		return;


	usSubpointsPerPoint = SubpointsPerPoint(ubStat, pProfile->bExpLevel);
	usSubpointsPerLevel = SubpointsPerPoint(EXPERAMT, pProfile->bExpLevel);

  switch (ubStat)
  {
    case HEALTHAMT:
      bCurrentRating = pProfile->bLifeMax;
      psStatGainPtr = &(pProfile->sLifeGain);
			// NB physical stat checks not affected by wisdom, unless training is going on
			fAffectedByWisdom = FALSE;
      break;

    case AGILAMT:
      bCurrentRating = pProfile->bAgility;
      psStatGainPtr = &(pProfile->sAgilityGain);
			fAffectedByWisdom = FALSE;
      break;

    case DEXTAMT:
      bCurrentRating = pProfile->bDexterity;
      psStatGainPtr = &(pProfile->sDexterityGain);
			fAffectedByWisdom = FALSE;
      break;

    case WISDOMAMT:
      bCurrentRating = pProfile->bWisdom;
      psStatGainPtr = &(pProfile->sWisdomGain);
      break;

    case MEDICALAMT:
      bCurrentRating = pProfile->bMedical;
      psStatGainPtr = &(pProfile->sMedicalGain);
      break;

    case EXPLODEAMT:
      bCurrentRating = pProfile->bExplosive;
      psStatGainPtr = &(pProfile->sExplosivesGain);
      break;

    case MECHANAMT:
      bCurrentRating = pProfile->bMechanical;
      psStatGainPtr = &(pProfile->sMechanicGain);
      break;

    case MARKAMT:
      bCurrentRating = pProfile->bMarksmanship;
      psStatGainPtr = &(pProfile->sMarksmanshipGain);
      break;

    case EXPERAMT:
      bCurrentRating = pProfile->bExpLevel;
      psStatGainPtr = &(pProfile->sExpLevelGain);
      break;

		case STRAMT:
      bCurrentRating = pProfile->bStrength;
      psStatGainPtr = &(pProfile->sStrengthGain);
			fAffectedByWisdom = FALSE;
      break;

		case LDRAMT:
      bCurrentRating = pProfile->bLeadership;
      psStatGainPtr = &(pProfile->sLeadershipGain);
      break;

    default:
			// BETA message
      ScreenMsg( FONT_ORANGE, MSG_BETAVERSION, L"ERROR: ProcessStatChange: Rcvd unknown ubStat %d", ubStat);
      return;
  }


	if (ubReason == FROM_TRAINING)
	{
		// training always affected by wisdom
		fAffectedByWisdom = TRUE;
	}


	// stats/skills of 0 can NEVER be improved!
	if ( bCurrentRating <= 0 )
	{
		return;
	}


  // loop once for each chance to improve
  for (uiCnt = 0; uiCnt < usNumChances; uiCnt++)
  {
    if (pProfile->bEvolution == NORMAL_EVOLUTION)               // Evolves!
    {
			// if this is improving from a failure, and a successful roll would give us enough to go up a point
			if ((ubReason == FROM_FAILURE) && ((*psStatGainPtr + 1) >= usSubpointsPerPoint))
			{
				// can't improve any more from this statchange, because Ian don't want failures causin increases!
				break;
			}

      if (ubStat != EXPERAMT)
			{
				// NON-experience level changes, actual usChance depends on bCurrentRating
				// Base usChance is '100 - bCurrentRating'
				usChance = 100 - (bCurrentRating + (*psStatGainPtr / usSubpointsPerPoint));

				// prevent training beyond the training cap
				if ((ubReason == FROM_TRAINING) && (bCurrentRating + (*psStatGainPtr / usSubpointsPerPoint) >= TRAINING_RATING_CAP))
				{
					usChance = 0;
				}
			}
      else
      {
			  // Experience level changes, actual usChance depends on level
			  // Base usChance is '100 - (10 * current level)'
				usChance = 100 - 10 * (bCurrentRating + (*psStatGainPtr / usSubpointsPerPoint));
      }

      // if there IS a usChance, adjust it for high or low wisdom (50 is avg)
      if (usChance > 0 && fAffectedByWisdom)
			{
				usChance += (usChance * (pProfile->bWisdom + (pProfile->sWisdomGain / SubpointsPerPoint(WISDOMAMT, pProfile->bExpLevel)) - 50)) / 100;
			}

/*
      // if the stat is Marksmanship, and the guy is a hopeless shot
      if ((ubStat == MARKAMT) && (pProfile->bSpecialTrait == HOPELESS_SHOT))
			{
				usChance /= 5;		// MUCH slower to improve, divide usChance by 5
			}
*/

      // maximum possible usChance is 99%
      if (usChance > 99)
			{
				usChance = 99;
			}

      if (PreRandom(100) < usChance)
      {
        (*psStatGainPtr)++;
				sSubPointChange++;

        // as long as we're not dealing with exp_level changes (already added above!)
        // and it's not from training, and the exp level isn't max'ed out already
        if ((ubStat != EXPERAMT) && (ubReason != FROM_TRAINING))
        {
          uiEffLevel = pProfile->bExpLevel + (pProfile->sExpLevelGain / usSubpointsPerLevel);

					// if level is not at maximum
	        if (uiEffLevel < MAXEXPLEVEL)
					{
						// if this is NOT improving from a failure, OR it would NOT give us enough to go up a level
						if ((ubReason != FROM_FAILURE) || ((pProfile->sExpLevelGain + 1) < usSubpointsPerLevel))
						{
							// all other stat changes count towards experience level changes (1 for 1 basis)
							pProfile->sExpLevelGain++;
						}
					}
				}
	    }
    }
    else                          // Regresses!
    {
			// regression can happen from both failures and successes (but not training, checked above)

      if (ubStat != EXPERAMT)
      {
				// NON-experience level changes, actual usChance depends on bCurrentRating
				switch (ubStat)
				{
					case HEALTHAMT:
					case AGILAMT:
					case DEXTAMT:
					case WISDOMAMT:
					case STRAMT:
						// Base usChance is 'bCurrentRating - 1', since these must remain at 1-100
						usChance = bCurrentRating + (*psStatGainPtr / usSubpointsPerPoint) - 1;
						break;

					case MEDICALAMT:
					case EXPLODEAMT:
					case MECHANAMT:
					case MARKAMT:
					case LDRAMT:
						// Base usChance is 'bCurrentRating', these can drop to 0
						usChance = bCurrentRating + (*psStatGainPtr / usSubpointsPerPoint);
						break;
				}
      }
      else
			{
				// Experience level changes, actual usChance depends on level
				// Base usChance is '10 * (current level - 1)'
				usChance = 10 * (bCurrentRating + (*psStatGainPtr / usSubpointsPerPoint) - 1);

				// if there IS a usChance, adjust it for high or low wisdom (50 is avg)
				if (usChance > 0 && fAffectedByWisdom)
				{
					usChance -= (usChance * (pProfile->bWisdom + (pProfile->sWisdomGain / SubpointsPerPoint(WISDOMAMT, pProfile->bExpLevel)) - 50)) / 100;
				}

				// if there's ANY usChance, minimum usChance is 1% regardless of wisdom
				if (usChance < 1)
				{
					usChance = 1;
				}
      }

      if (PreRandom(100) < usChance)
      {
				(*psStatGainPtr)--;
				sSubPointChange--;

        // as long as we're not dealing with exp_level changes (already added above!)
        // and it's not from training, and the exp level isn't max'ed out already
        if ((ubStat != EXPERAMT) && (ubReason != FROM_TRAINING))
        {
          uiEffLevel = pProfile->bExpLevel + (pProfile->sExpLevelGain / usSubpointsPerLevel );

					// if level is not at minimum
          if (uiEffLevel > 1)
					{
            // all other stat changes count towards experience level changes (1 for 1 basis)
            pProfile->sExpLevelGain--;
					}
				}
      }
    }
  }

#ifdef STAT_CHANGE_DEBUG
	if (sSubPointChange != 0)
	{
		// debug message
		ScreenMsg( MSG_FONT_RED, MSG_DEBUG, L"%S's %S changed by %d", pProfile->zNickname, wDebugStatStrings[ubStat], sSubPointChange );
	}
#endif


	// exclude training, that's not under our control
	if (ubReason != FROM_TRAINING)
	{
		// increment counters that track how often stat changes are being awarded
		pProfile->usStatChangeChances[ ubStat ] += usNumChances;
		pProfile->usStatChangeSuccesses[ ubStat ] += abs( sSubPointChange );
	}
}


static void ProcessUpdateStats(MERCPROFILESTRUCT* pProfile, SOLDIERTYPE* pSoldier);


// convert hired mercs' stats subpoint changes into actual point changes where warranted
static void UpdateStats(SOLDIERTYPE* pSoldier)
{
	ProcessUpdateStats( &( gMercProfiles[ pSoldier->ubProfile ] ), pSoldier );
}


// UpdateStats version for mercs not currently on player's team
static void ProfileUpdateStats(MERCPROFILESTRUCT* pProfile)
{
	ProcessUpdateStats( pProfile, NULL );
}


static UINT32 CalcNewSalary(UINT32 uiOldSalary, BOOLEAN fIncrease, UINT32 uiMaxLimit);


void ChangeStat( MERCPROFILESTRUCT *pProfile, SOLDIERTYPE *pSoldier, UINT8 ubStat, INT16 sPtsChanged )
{
	// this function changes the stat a given amount...
	INT16 *psStatGainPtr = NULL;
	INT8 *pbStatPtr = NULL;
	INT8 *pbSoldierStatPtr = NULL;
	INT8 *pbStatDeltaPtr = NULL;
	UINT32 *puiStatTimerPtr = NULL;
	BOOLEAN fChangeTypeIncrease;
	BOOLEAN fChangeSalary;
	UINT32 uiLevelCnt;
	UINT8 ubMercMercIdValue = 0;
	UINT16 usIncreaseValue = 0;
	UINT16 usSubpointsPerPoint;

	usSubpointsPerPoint = SubpointsPerPoint(ubStat, pProfile->bExpLevel );

	// build ptrs to appropriate profiletype stat fields
	switch( ubStat )
	{
	case HEALTHAMT:
    psStatGainPtr = &(pProfile->sLifeGain);
		pbStatDeltaPtr = &(pProfile->bLifeDelta);
		pbStatPtr = &(pProfile->bLifeMax);
    break;

  case AGILAMT:
    psStatGainPtr = &(pProfile->sAgilityGain);
		pbStatDeltaPtr = &(pProfile->bAgilityDelta);
		pbStatPtr = &(pProfile->bAgility);
    break;

  case DEXTAMT:
    psStatGainPtr = &(pProfile->sDexterityGain);
		pbStatDeltaPtr = &(pProfile->bDexterityDelta);
		pbStatPtr = &(pProfile->bDexterity);
    break;

  case WISDOMAMT:
    psStatGainPtr = &(pProfile->sWisdomGain);
		pbStatDeltaPtr = &(pProfile->bWisdomDelta);
    pbStatPtr = &(pProfile->bWisdom);
		break;

  case MEDICALAMT:
    psStatGainPtr = &(pProfile->sMedicalGain);
		pbStatDeltaPtr = &(pProfile->bMedicalDelta);
    pbStatPtr = &(pProfile->bMedical);
		break;

  case EXPLODEAMT:
    psStatGainPtr = &(pProfile->sExplosivesGain);
		pbStatDeltaPtr = &(pProfile->bExplosivesDelta);
    pbStatPtr = &(pProfile->bExplosive);
		break;

  case MECHANAMT:
    psStatGainPtr = &(pProfile->sMechanicGain);
		pbStatDeltaPtr = &(pProfile->bMechanicDelta);
    pbStatPtr = &(pProfile->bMechanical);
		break;

  case MARKAMT:
    psStatGainPtr = &(pProfile->sMarksmanshipGain);
		pbStatDeltaPtr = &(pProfile->bMarksmanshipDelta);
    pbStatPtr = &(pProfile->bMarksmanship);
		break;

  case EXPERAMT:
    psStatGainPtr = &(pProfile->sExpLevelGain);
		pbStatDeltaPtr = &(pProfile->bExpLevelDelta);
    pbStatPtr = &(pProfile->bExpLevel);
		break;

	case STRAMT:
    psStatGainPtr = &(pProfile->sStrengthGain);
		pbStatDeltaPtr = &(pProfile->bStrengthDelta);
    pbStatPtr = &(pProfile->bStrength);
		break;

	case LDRAMT:
    psStatGainPtr = &(pProfile->sLeadershipGain);
		pbStatDeltaPtr = &(pProfile->bLeadershipDelta);
    pbStatPtr = &(pProfile->bLeadership);
		break;
	}


	// if this merc is currently on the player's team
	if (pSoldier != NULL)
	{
		// build ptrs to appropriate soldiertype stat fields
		switch( ubStat )
		{
		case HEALTHAMT:
			pbSoldierStatPtr = &( pSoldier->bLifeMax );
			puiStatTimerPtr = &( pSoldier->uiChangeHealthTime);
			usIncreaseValue = HEALTH_INCREASE;
			break;

		case AGILAMT:
			pbSoldierStatPtr = &( pSoldier->bAgility );
			puiStatTimerPtr = &( pSoldier->uiChangeAgilityTime);
			usIncreaseValue = AGIL_INCREASE;
			break;

		case DEXTAMT:
			pbSoldierStatPtr = &( pSoldier->bDexterity );
			puiStatTimerPtr = &( pSoldier->uiChangeDexterityTime);
			usIncreaseValue = DEX_INCREASE;
			break;

		case WISDOMAMT:
			pbSoldierStatPtr = &( pSoldier->bWisdom );
			puiStatTimerPtr = &( pSoldier->uiChangeWisdomTime);
			usIncreaseValue = WIS_INCREASE;
			break;

		case MEDICALAMT:
			pbSoldierStatPtr = &( pSoldier->bMedical );
			puiStatTimerPtr = &( pSoldier->uiChangeMedicalTime);
			usIncreaseValue = MED_INCREASE;
			break;

		case EXPLODEAMT:
			pbSoldierStatPtr = &( pSoldier->bExplosive );
			puiStatTimerPtr = &( pSoldier->uiChangeExplosivesTime);
			usIncreaseValue = EXP_INCREASE;
			break;

		case MECHANAMT:
			pbSoldierStatPtr = &( pSoldier->bMechanical );
			puiStatTimerPtr = &( pSoldier->uiChangeMechanicalTime);
			usIncreaseValue = MECH_INCREASE;
			break;

		case MARKAMT:
			pbSoldierStatPtr = &( pSoldier->bMarksmanship );
			puiStatTimerPtr = &( pSoldier->uiChangeMarksmanshipTime);
			usIncreaseValue = MRK_INCREASE;
			break;

		case EXPERAMT:
			pbSoldierStatPtr = &(pSoldier->bExpLevel);
			puiStatTimerPtr = &( pSoldier->uiChangeLevelTime );
			usIncreaseValue = LVL_INCREASE;
			break;

		case STRAMT:
			pbSoldierStatPtr = &(pSoldier->bStrength);
			puiStatTimerPtr = &( pSoldier->uiChangeStrengthTime);
			usIncreaseValue = STRENGTH_INCREASE;
			break;

		case LDRAMT:
			pbSoldierStatPtr = &( pSoldier->bLeadership);
			puiStatTimerPtr = &( pSoldier->uiChangeLeadershipTime);
			usIncreaseValue = LDR_INCREASE;
			break;
		}
	}

	// ptrs set up, now handle
	// if the stat needs to change
	if ( sPtsChanged != 0 )
	{
		// if a stat improved
		if ( sPtsChanged > 0 )
		{
			fChangeTypeIncrease = TRUE;
		}
		else
		{
			fChangeTypeIncrease = FALSE;
		}

		// update merc profile stat
		*pbStatPtr += sPtsChanged;

		// if this merc is currently on the player's team (DON'T count increases earned outside the player's employ)
		if (pSoldier != NULL)
		{
			// also note the delta (how much this stat has changed since start of game)
			*pbStatDeltaPtr += sPtsChanged;
		}

		// reduce gain to the unused subpts only
		*psStatGainPtr = ( *psStatGainPtr ) % usSubpointsPerPoint;


		// if the guy is employed by player
		if (pSoldier != NULL)
		{
			// transfer over change to soldiertype structure
			*pbSoldierStatPtr = *pbStatPtr;

			// if it's a level gain, or sometimes for other stats
			// ( except health; not only will it sound silly, but
			// also we give points for health on sector traversal and this would
			// probaby mess up battle handling too )
			if ( (ubStat != HEALTHAMT) && ( (ubStat == EXPERAMT) || Random( 100 ) < 25 ) )
			//if ( (ubStat != EXPERAMT) && (ubStat != HEALTHAMT) && ( Random( 100 ) < 25 ) )
			{
				// Pipe up with "I'm getting better at this!"
				TacticalCharacterDialogueWithSpecialEventEx( pSoldier, 0, DIALOGUE_SPECIAL_EVENT_DISPLAY_STAT_CHANGE, fChangeTypeIncrease, sPtsChanged, ubStat );
				TacticalCharacterDialogue( pSoldier, QUOTE_EXPERIENCE_GAIN );
			}
			else
			{
				CHAR16 wTempString[ 128 ];

				// tell player about it
				BuildStatChangeString( wTempString, lengthof(wTempString), pSoldier->name, fChangeTypeIncrease, sPtsChanged, ubStat );
				ScreenMsg( FONT_MCOLOR_LTYELLOW, MSG_INTERFACE, wTempString );
			}

			// update mapscreen soldier info panel
			fCharacterInfoPanelDirty = TRUE;

			// remember what time it changed at, it's displayed in a different color for a while afterwards
			*puiStatTimerPtr = GetJA2Clock();

			if( fChangeTypeIncrease )
			{
				pSoldier->usValueGoneUp |= usIncreaseValue;
			}
			else
			{
				pSoldier->usValueGoneUp &= ~( usIncreaseValue );
			}

			fInterfacePanelDirty = DIRTYLEVEL2;
		}


		// special handling for LIFEMAX
		if( ubStat == HEALTHAMT )
		{
			// adjust current health by the same amount as max health
			pProfile->bLife += sPtsChanged;

			// don't let this kill a guy or knock him out!!!
			if (pProfile->bLife < OKLIFE)
			{
				pProfile->bLife = OKLIFE;
			}

			// if the guy is employed by player
			if (pSoldier != NULL)
			{
				// adjust current health by the same amount as max health
				pSoldier->bLife += sPtsChanged;

				// don't let this kill a guy or knock him out!!!
				if (pSoldier->bLife < OKLIFE)
				{
					pSoldier->bLife = OKLIFE;
				}
			}
		}

		// special handling for EXPERIENCE LEVEL
		// merc salaries increase if level goes up (but they don't go down if level drops!)
		if( (ubStat == EXPERAMT) && fChangeTypeIncrease)
		{
			// if the guy is employed by player
			if (pSoldier != NULL)
			{
				switch (pSoldier->ubWhatKindOfMercAmI)
				{
					case MERC_TYPE__AIM_MERC:
						// A.I.M.
						pSoldier->fContractPriceHasIncreased = TRUE;
						fChangeSalary = TRUE;
						break;

					case MERC_TYPE__MERC:
						// M.E.R.C.
						ubMercMercIdValue = pSoldier->ubProfile;

						// Biff's profile id ( 40 ) is the base
						ubMercMercIdValue -= BIFF;

						// offset for the 2 profiles of Larry (we only have one email for Larry..but 2 profile entries
						if( ubMercMercIdValue >= ( LARRY_DRUNK - BIFF ) )
						{
							ubMercMercIdValue--;
						}

						//
						// Send special E-mail
						//

//	DEF: 03/06/99 Now sets an event that will be processed later in the day
//						ubEmailOffset = MERC_UP_LEVEL_BIFF + MERC_UP_LEVEL_LENGTH_BIFF * ( ubMercMercIdValue );
//						AddEmail( ubEmailOffset, MERC_UP_LEVEL_LENGTH_BIFF, SPECK_FROM_MERC, GetWorldTotalMin() );
						AddStrategicEvent( EVENT_MERC_MERC_WENT_UP_LEVEL_EMAIL_DELAY, GetWorldTotalMin( ) + 60 + Random( 60 ), ubMercMercIdValue );

						fChangeSalary = TRUE;
						break;

					default:
						// others don't increase salary
						fChangeSalary = FALSE;
						break;
				}
			}
			else	// not employed by player
			{
				// only AIM and M.E.R.C.s update stats when not on player's team, and both of them DO change salary
				fChangeSalary = TRUE;
			}

			if (fChangeSalary)
			{
				// increase all salaries and medical deposits, once for each level gained
				for (uiLevelCnt = 0; uiLevelCnt < (UINT32) sPtsChanged; uiLevelCnt++)
				{
					pProfile->sSalary								= (INT16) CalcNewSalary(pProfile->sSalary,								fChangeTypeIncrease, MAX_DAILY_SALARY);
					pProfile->uiWeeklySalary				=					CalcNewSalary(pProfile->uiWeeklySalary,					fChangeTypeIncrease, MAX_LARGE_SALARY);
					pProfile->uiBiWeeklySalary			=					CalcNewSalary(pProfile->uiBiWeeklySalary,				fChangeTypeIncrease, MAX_LARGE_SALARY);
					pProfile->sTrueSalary						= (INT16) CalcNewSalary(pProfile->sTrueSalary,						fChangeTypeIncrease, MAX_DAILY_SALARY);
					pProfile->sMedicalDepositAmount = (INT16) CalcNewSalary(pProfile->sMedicalDepositAmount,	fChangeTypeIncrease, MAX_DAILY_SALARY);

					//if (pSoldier != NULL)
						// DON'T increase the *effective* medical deposit, it's already been paid out
						// pSoldier->usMedicalDeposit = pProfile->sMedicalDepositAmount;
				}
			}
		}
	}

	return;
}


// pSoldier may be NULL!
static void ProcessUpdateStats(MERCPROFILESTRUCT* pProfile, SOLDIERTYPE* pSoldier)
{
	// this function will run through the soldier's profile and update their stats based on any accumulated gain pts.
	UINT8 ubStat = 0;
	INT16 *psStatGainPtr = NULL;
	INT8 *pbStatPtr = NULL;
	INT8 *pbSoldierStatPtr = NULL;
	INT8 bMinStatValue;
	INT8 bMaxStatValue;
	UINT16 usSubpointsPerPoint;
	INT16 sPtsChanged;


	// if hired, not back at AIM
	if ( pSoldier != NULL )
	{
		// ATE: if in the midst of an attack, if in the field, delay all stat changes until the check made after the 'attack'...
		if ( ( gTacticalStatus.ubAttackBusyCount > 0 ) && pSoldier->bInSector && ( gTacticalStatus.uiFlags & INCOMBAT ) )
			return;

		// ignore non-player soldiers
		if (!PTR_OURTEAM)
			return;

		// ignore anything without a profile
		if (pSoldier->ubProfile == NO_PROFILE)
			return;

		// ignore vehicles and robots
		if( ( pSoldier->uiStatusFlags & SOLDIER_VEHICLE ) || ( pSoldier->uiStatusFlags & SOLDIER_ROBOT ) )
			return;

		// delay increases while merc is dying
		if (pSoldier->bLife < OKLIFE)
			return;

		// ignore POWs - shouldn't ever be getting this far
		if( pSoldier->bAssignment == ASSIGNMENT_POW )
		{
			return;
		}
	}
	else
	{
		// dead guys don't do nuthin' !
		if ( pProfile->bMercStatus == MERC_IS_DEAD )
			return;

		if ( pProfile->bLife < OKLIFE )
			return;
	}


	// check every attribute, skill, and exp.level, too
	for( ubStat = FIRST_CHANGEABLE_STAT; ubStat <= LAST_CHANGEABLE_STAT; ubStat++ )
	{
		// set default min & max, subpoints/pt.
		bMinStatValue = 1;
		bMaxStatValue = MAX_STAT_VALUE;
		usSubpointsPerPoint = SubpointsPerPoint(ubStat, pProfile->bExpLevel);

		// build ptrs to appropriate profiletype stat fields
		switch( ubStat )
		{
			case HEALTHAMT:
      psStatGainPtr = &(pProfile->sLifeGain);
			pbStatPtr = &(pProfile->bLifeMax);

			bMinStatValue = OKLIFE;
      break;

    case AGILAMT:
      psStatGainPtr = &(pProfile->sAgilityGain);
			pbStatPtr = &(pProfile->bAgility);
      break;

    case DEXTAMT:
      psStatGainPtr = &(pProfile->sDexterityGain);
			pbStatPtr = &(pProfile->bDexterity);
      break;

    case WISDOMAMT:
      psStatGainPtr = &(pProfile->sWisdomGain);
      pbStatPtr = &(pProfile->bWisdom);
			break;

    case MEDICALAMT:
      psStatGainPtr = &(pProfile->sMedicalGain);
      pbStatPtr = &(pProfile->bMedical);

			bMinStatValue = 0;
			break;

    case EXPLODEAMT:
      psStatGainPtr = &(pProfile->sExplosivesGain);
      pbStatPtr = &(pProfile->bExplosive);

			bMinStatValue = 0;
			break;

    case MECHANAMT:
      psStatGainPtr = &(pProfile->sMechanicGain);
      pbStatPtr = &(pProfile->bMechanical);

			bMinStatValue = 0;
			break;

    case MARKAMT:
      psStatGainPtr = &(pProfile->sMarksmanshipGain);
      pbStatPtr = &(pProfile->bMarksmanship);

			bMinStatValue = 0;
			break;

    case EXPERAMT:
      psStatGainPtr = &(pProfile->sExpLevelGain);
      pbStatPtr = &(pProfile->bExpLevel);

			bMaxStatValue = MAXEXPLEVEL;
			break;

		case STRAMT:
      psStatGainPtr = &(pProfile->sStrengthGain);
      pbStatPtr = &(pProfile->bStrength);
			break;

		case LDRAMT:
      psStatGainPtr = &(pProfile->sLeadershipGain);
      pbStatPtr = &(pProfile->bLeadership);
			break;
		}


		// if this merc is currently on the player's team
		if (pSoldier != NULL)
		{
			// build ptrs to appropriate soldiertype stat fields
			switch( ubStat )
			{
				case HEALTHAMT:
				pbSoldierStatPtr = &( pSoldier->bLifeMax );
				break;

			case AGILAMT:
				pbSoldierStatPtr = &( pSoldier->bAgility );
				break;

			case DEXTAMT:
				pbSoldierStatPtr = &( pSoldier->bDexterity );
				break;

			case WISDOMAMT:
				pbSoldierStatPtr = &( pSoldier->bWisdom );
				break;

			case MEDICALAMT:
				pbSoldierStatPtr = &( pSoldier->bMedical );
				break;

			case EXPLODEAMT:
				pbSoldierStatPtr = &( pSoldier->bExplosive );
				break;

			case MECHANAMT:
				pbSoldierStatPtr = &( pSoldier->bMechanical );
				break;

			case MARKAMT:
				pbSoldierStatPtr = &( pSoldier->bMarksmanship );
				break;

			case EXPERAMT:
				pbSoldierStatPtr = &(pSoldier->bExpLevel);
				break;

			case STRAMT:
				pbSoldierStatPtr = &(pSoldier->bStrength);
				break;

			case LDRAMT:
				pbSoldierStatPtr = &( pSoldier->bLeadership);
				break;
			}
		}


		// ptrs set up, now handle

		// Calc how many full points worth of stat changes we have accumulated in this stat (positive OR negative!)
		// NOTE: for simplicity, this hopes nobody will go up more than one level at once, which would change the subpoints/pt
		sPtsChanged = ( *psStatGainPtr ) / usSubpointsPerPoint;

		// gone too high or too low?..handle the fact
		if( (*pbStatPtr + sPtsChanged) > bMaxStatValue )
		{
			// reduce change to reach max value and reset stat gain ptr
			sPtsChanged = bMaxStatValue - *pbStatPtr;
			*psStatGainPtr = 0;
		}
		else
		if( (*pbStatPtr + sPtsChanged) < bMinStatValue )
		{
			// reduce change to reach min value and reset stat gain ptr
			sPtsChanged = bMinStatValue - *pbStatPtr;
			*psStatGainPtr = 0;
		}


		// if the stat needs to change
		if ( sPtsChanged != 0 )
		{
			// Otherwise, use normal stat increase stuff...
			ChangeStat( pProfile, pSoldier, ubStat, sPtsChanged );
		}
	}

	return;
}


void HandleAnyStatChangesAfterAttack( void )
{
  INT32 cnt;
	SOLDIERTYPE *pSoldier;

	// must check everyone on player's team, not just the shooter
	for ( cnt = 0, pSoldier = MercPtrs[ 0 ]; cnt <= gTacticalStatus.Team[ MercPtrs[ 0 ]->bTeam ].bLastID; cnt++,pSoldier++)
	{
		if (pSoldier -> bActive)
		{
			ProcessUpdateStats( &( gMercProfiles[ pSoldier->ubProfile ] ), pSoldier );
		}
	}
}


static UINT32 RoundOffSalary(UINT32 uiSalary);


static UINT32 CalcNewSalary(UINT32 uiOldSalary, BOOLEAN fIncrease, UINT32 uiMaxLimit)
{
  UINT32 uiNewSalary;

	// if he was working for free, it's still free!
	if (uiOldSalary == 0)
	{
		return(0);
	}

  if (fIncrease)
	{
    uiNewSalary = (UINT32) (uiOldSalary * SALARY_CHANGE_PER_LEVEL);
	}
  else
	{
    uiNewSalary = (UINT32) (uiOldSalary / SALARY_CHANGE_PER_LEVEL);
	}

  // round it off to a reasonable multiple
  uiNewSalary = RoundOffSalary(uiNewSalary);

	// let's set some reasonable limits here, lest it get silly one day
	if (uiNewSalary > uiMaxLimit)
		uiNewSalary = uiMaxLimit;

	if (uiNewSalary < 5)
		uiNewSalary = 5;


  return(uiNewSalary);
}


static UINT32 RoundOffSalary(UINT32 uiSalary)
{
	UINT32 uiMultiple;


	// determine what multiple value the salary should be rounded off to
	if      (uiSalary <=   250) uiMultiple =    5;
	else if (uiSalary <=   500) uiMultiple =   10;
	else if (uiSalary <=  1000) uiMultiple =   25;
	else if (uiSalary <=  2000) uiMultiple =   50;
	else if (uiSalary <=  5000) uiMultiple =  100;
	else if (uiSalary <= 10000) uiMultiple =  500;
	else if (uiSalary <= 25000) uiMultiple = 1000;
	else if (uiSalary <= 50000) uiMultiple = 2000;
	else												uiMultiple = 5000;


	// if the salary doesn't divide evenly by the multiple
	if (uiSalary % uiMultiple)
	{
		// then we have to make it so, as Picard would say <- We have to wonder how much Alex gets out
		// and while we're at it, we round up to next higher multiple if halfway
		uiSalary = ((uiSalary + (uiMultiple / 2)) / uiMultiple) * uiMultiple;
	}

	return(uiSalary);
}


static UINT16 SubpointsPerPoint(UINT8 ubStat, INT8 bExpLevel)
{
	UINT16 usSubpointsPerPoint;

	// figure out how many subpoints this type of stat needs to change
  switch (ubStat)
  {
    case HEALTHAMT:
    case AGILAMT:
    case DEXTAMT:
    case WISDOMAMT:
		case STRAMT:
			// attributes
			usSubpointsPerPoint = ATTRIBS_SUBPOINTS_TO_IMPROVE;
      break;

    case MEDICALAMT:
    case EXPLODEAMT:
    case MECHANAMT:
    case MARKAMT:
		case LDRAMT:
			// skills
			usSubpointsPerPoint = SKILLS_SUBPOINTS_TO_IMPROVE;
      break;

    case EXPERAMT:
			usSubpointsPerPoint = LEVEL_SUBPOINTS_TO_IMPROVE * bExpLevel;
      break;

    default:
			// BETA message
      ScreenMsg( FONT_ORANGE, MSG_BETAVERSION, L"SubpointsPerPoint: ERROR - Unknown ubStat %d", ubStat);
      return(100);
  }

	return(usSubpointsPerPoint);
}


// handles stat changes for mercs not currently working for the player
void HandleUnhiredMercImprovement( MERCPROFILESTRUCT *pProfile )
{
	UINT8 ubNumStats;
	UINT8 ubStat;
	UINT16 usNumChances;

	ubNumStats = LAST_CHANGEABLE_STAT - FIRST_CHANGEABLE_STAT + 1;

	// if he's working on another job
	if (pProfile->bMercStatus == MERC_WORKING_ELSEWHERE)
	{
		// if he did't do anything interesting today
		if (Random(100) < 20)
		{
			// no chance to change today
			return;
		}

		// it's real on the job experience, counts towards experience

		// all stats (including experience itself) get an equal chance to improve
		// 80 wisdom gives 8 rolls per stat per day, 10 stats, avg success rate 40% = 32pts per day,
		// so about 10 working days to hit lvl 2.  This seems high, but mercs don't actually "work" that often, and it's twice
		// as long to hit level 3.  If we go lower, attribs & skills will barely move.
		usNumChances = ( pProfile->bWisdom / 10 );
		for (ubStat = FIRST_CHANGEABLE_STAT; ubStat <= LAST_CHANGEABLE_STAT; ubStat++)
		{
			ProfileStatChange( pProfile, ubStat, usNumChances, FALSE );
		}
	}
	else
	{
		// if the merc just takes it easy (high level or stupid mercs are more likely to)
		if (((INT8) Random(10) < pProfile->bExpLevel) || ((INT8) Random(100) > pProfile->bWisdom))
		{
			// no chance to change today
			return;
		}

		// it's just practise/training back home
		do
		{
			// pick ONE stat at random to focus on (it may be beyond training cap, but so what, too hard to weed those out)
			ubStat = (UINT8) (FIRST_CHANGEABLE_STAT + Random(ubNumStats));
			// except experience - can't practise that!
		} while (ubStat == EXPERAMT);

		// try to improve that one stat
		ProfileStatChange( pProfile, ubStat, ( UINT16 ) ( pProfile->bWisdom / 2 ), FROM_TRAINING );
	}

	ProfileUpdateStats( pProfile );
}


// handles possible death of mercs not currently working for the player
void HandleUnhiredMercDeaths( INT32 iProfileID )
{
	UINT8 ubMaxDeaths;
	INT16 sChance;
	MERCPROFILESTRUCT *pProfile = &(gMercProfiles[ iProfileID ]);


	// if the player has never yet had the chance to hire this merc
	if ( !( pProfile->ubMiscFlags3 & PROFILE_MISC_FLAG3_PLAYER_HAD_CHANCE_TO_HIRE) )
	{
		// then we're not allowed to kill him (to avoid really pissing off player by killing his very favorite merc)
		return;
	}

	// how many in total can be killed like this depends on player's difficulty setting
	switch( gGameOptions.ubDifficultyLevel )
	{
		case DIF_LEVEL_EASY:
			ubMaxDeaths = 1;
			break;
		case DIF_LEVEL_MEDIUM:
			ubMaxDeaths = 2;
			break;
		case DIF_LEVEL_HARD:
			ubMaxDeaths = 3;
			break;
		default:
			Assert(FALSE);
			ubMaxDeaths = 0;
			break;
	}

	// if we've already hit the limit in this game, skip these checks
	if (gStrategicStatus.ubUnhiredMercDeaths >= ubMaxDeaths)
	{
		return;
	}


	// calculate this merc's (small) chance to get killed today (out of 1000)
	sChance = 10 - pProfile->bExpLevel;

	switch (pProfile->bPersonalityTrait)
	{
		case FORGETFUL:
		case NERVOUS:
		case PSYCHO:
			// these guys are somewhat more likely to get killed (they have "problems")
			sChance += 2;
			break;
	}

	// stealthy guys are slightly less likely to get killed (they're careful)
	if (pProfile->bSkillTrait == STEALTHY)
	{
		sChance -= 1;
	}
	if (pProfile->bSkillTrait2 == STEALTHY)
	{
		sChance -= 1;
	}


	if ((INT16) PreRandom(1000) < sChance)
	{
		// this merc gets Killed In Action!!!
		pProfile->bMercStatus = MERC_IS_DEAD;
		pProfile->uiDayBecomesAvailable = 0;

		// keep count of how many there have been
		gStrategicStatus.ubUnhiredMercDeaths++;

		//send an email as long as the merc is from aim
		if( iProfileID < BIFF )
		{
			//send an email to the player telling the player that a merc died
			AddEmailWithSpecialData(MERC_DIED_ON_OTHER_ASSIGNMENT, MERC_DIED_ON_OTHER_ASSIGNMENT_LENGTH, AIM_SITE, GetWorldTotalMin(), 0, iProfileID );
		}
	}
}


static UINT8 CalcImportantSectorControl(void);


// These HAVE to total 100% at all times!!!
#define PROGRESS_PORTION_KILLS		25
#define PROGRESS_PORTION_CONTROL	25
#define PROGRESS_PORTION_INCOME		50


// returns a number between 0-100, this is an estimate of how far a player has progressed through the game
UINT8 CurrentPlayerProgressPercentage(void)
{
	UINT32 uiCurrentIncome;
	UINT32 uiPossibleIncome;
	UINT8 ubCurrentProgress;
	UINT8 ubKillsPerPoint;
	UINT16 usKillsProgress;
	UINT16 usControlProgress;


	if( gfEditMode )
		return 0;

	// figure out the player's current mine income
	uiCurrentIncome = PredictIncomeFromPlayerMines();

	// figure out the player's potential mine income
	uiPossibleIncome = CalcMaxPlayerIncomeFromMines();

	// either of these indicates a critical failure of some sort
	Assert(uiPossibleIncome > 0);
	Assert(uiCurrentIncome <= uiPossibleIncome);

	// for a rough guess as to how well the player is doing,
	// we'll take the current mine income / potential mine income as a percentage

	//Kris:  Make sure you don't divide by zero!!!
	if( uiPossibleIncome > 0)
	{
		ubCurrentProgress = (UINT8) ((uiCurrentIncome * PROGRESS_PORTION_INCOME) / uiPossibleIncome);
	}
	else
	{
		ubCurrentProgress = 0;
	}

	// kills per point depends on difficulty, and should match the ratios of starting enemy populations (730/1050/1500)
	switch( gGameOptions.ubDifficultyLevel )
	{
		case DIF_LEVEL_EASY:
			ubKillsPerPoint = 7;
			break;
		case DIF_LEVEL_MEDIUM:
			ubKillsPerPoint = 10;
			break;
		case DIF_LEVEL_HARD:
			ubKillsPerPoint = 15;
			break;
		default:
			Assert(FALSE);
			ubKillsPerPoint = 10;
			break;
	}

	usKillsProgress = gStrategicStatus.usPlayerKills / ubKillsPerPoint;
	if (usKillsProgress > PROGRESS_PORTION_KILLS)
	{
		usKillsProgress = PROGRESS_PORTION_KILLS;
	}

	// add kills progress to income progress
	ubCurrentProgress += usKillsProgress;


	// 19 sectors in mining towns + 3 wilderness SAMs each count double.  Balime & Meduna are extra and not required
	usControlProgress = CalcImportantSectorControl();
	if (usControlProgress > PROGRESS_PORTION_CONTROL)
	{
		usControlProgress = PROGRESS_PORTION_CONTROL;
	}

	// add control progress
	ubCurrentProgress += usControlProgress;

	return(ubCurrentProgress);
}


UINT8 HighestPlayerProgressPercentage(void)
{
	if( gfEditMode )
		return 0;

	return(gStrategicStatus.ubHighestProgress);
}


// monitors the highest level of progress that player has achieved so far (checking hourly),
// as opposed to his immediate situation (which may be worse if he's suffered a setback).
void HourlyProgressUpdate(void)
{
	UINT8 ubCurrentProgress;

	ubCurrentProgress = CurrentPlayerProgressPercentage();

	// if this is new high, remember it as that
	if (ubCurrentProgress > gStrategicStatus.ubHighestProgress)
	{
		// CJC:  note when progress goes above certain values for the first time

		// at 35% start the Madlab quest
		if ( ubCurrentProgress >= 35 && gStrategicStatus.ubHighestProgress < 35 )
		{
			HandleScientistAWOLMeanwhileScene();
		}

		// at 50% make Mike available to the strategic AI
		if ( ubCurrentProgress >= 50 && gStrategicStatus.ubHighestProgress < 50 )
		{
			SetFactTrue( FACT_MIKE_AVAILABLE_TO_ARMY );
		}

		// at 70% add Iggy to the world
		if ( ubCurrentProgress >= 70 && gStrategicStatus.ubHighestProgress < 70 )
		{
			gMercProfiles[ IGGY ].sSectorX = 5;
			gMercProfiles[ IGGY ].sSectorY = MAP_ROW_C;
		}

		gStrategicStatus.ubHighestProgress = ubCurrentProgress;

		// debug message
		ScreenMsg( MSG_FONT_RED, MSG_DEBUG, L"New player progress record: %d%%", gStrategicStatus.ubHighestProgress );
	}
}



#ifdef JA2TESTVERSION
void TestDumpStatChanges(void)
{
  UINT32 uiProfileId;
	UINT8 ubStat;
  CHAR8 zPrintFileName[60];
  FILE *FDump;
  MERCPROFILESTRUCT *pProfile;
	BOOLEAN fMercUsed;
	CHAR8 cEvolutionChars[3] = { '+', '=', '-' };
	UINT32 uiTotalSuccesses[ 12 ];
	UINT32 uiTotalChances[ 12 ];


	// clear totals
	memset( uiTotalSuccesses, 0, sizeof( uiTotalSuccesses ) );
	memset( uiTotalChances,   0, sizeof( uiTotalChances ) );

  // open output file
 	strcpy(zPrintFileName, "C:\\Temp\\StatChanges.TXT");
  FDump = fopen(zPrintFileName, "wt");

  if (FDump == NULL)
    return;


	// print headings
	fprintf(FDump, "   NAME   SRV EVL ");
	fprintf(FDump, "---HEALTH-- --AGILITY-- -DEXTERITY- ---WISDOM-- --MEDICAL-- --EXPLOSIV- --MECHANIC- --MARKSMAN- -EXP.LEVEL- --STRENGTH- -LEADERSHIP");
	fprintf(FDump, "\n");


  // loop through profiles
	for (uiProfileId = 0; uiProfileId < NUM_PROFILES; uiProfileId++)
  {
    pProfile = &(gMercProfiles[uiProfileId]);

		fMercUsed = FALSE;

		// see if this guy should be printed at all (only mercs actually used are dumped)
		for( ubStat = FIRST_CHANGEABLE_STAT; ubStat <= LAST_CHANGEABLE_STAT; ubStat++ )
		{
			if ( pProfile->usStatChangeChances[ ubStat ] > 0 )
			{
				fMercUsed = TRUE;
				break;
			}
    }

		if (fMercUsed)
		{
			// print nickname
			fprintf(FDump, "%-10ls ", pProfile->zNickname);
			// print days served
			fprintf(FDump, "%3d ", pProfile->usTotalDaysServed);
			// print evolution type
			fprintf(FDump, "%c ", cEvolutionChars[ pProfile->bEvolution ]);

			// now print all non-zero stats
			for( ubStat = FIRST_CHANGEABLE_STAT; ubStat <= LAST_CHANGEABLE_STAT; ubStat++ )
			{
				if ( pProfile->usStatChangeChances[ ubStat ] > 0 )
				{
					// print successes/chances
					fprintf(FDump, " %5d/%-5d", pProfile->usStatChangeSuccesses[ ubStat ], pProfile->usStatChangeChances[ ubStat ]);
				}
				else
				{
					//
					fprintf(FDump, "            ");
				}

				uiTotalSuccesses[ ubStat ] += pProfile->usStatChangeSuccesses[ ubStat ];
				uiTotalChances[ ubStat ]   += pProfile->usStatChangeChances[ ubStat ];
			}

			// newline
			fprintf(FDump, "\n");
		}
  }


	// print totals:
	fprintf(FDump, "TOTAL        ");

	for( ubStat = FIRST_CHANGEABLE_STAT; ubStat <= LAST_CHANGEABLE_STAT; ubStat++ )
	{
		fprintf(FDump, " %5d/%-5d", uiTotalSuccesses[ ubStat ], uiTotalChances[ ubStat ]);
	}

	fprintf(FDump, "\n");


  fclose(FDump);
}
#endif



void AwardExperienceBonusToActiveSquad( UINT8 ubExpBonusType )
{
	UINT16 usXPs = 0;
	UINT8 ubGuynum;
	SOLDIERTYPE *pSoldier;


	Assert ( ubExpBonusType < NUM_EXP_BONUS_TYPES );

	switch ( ubExpBonusType )
	{
		case EXP_BONUS_MINIMUM:		usXPs =   25;			break;
		case EXP_BONUS_SMALL:			usXPs =   50;			break;
		case EXP_BONUS_AVERAGE:		usXPs =  100;			break;
		case EXP_BONUS_LARGE:			usXPs =  200;			break;
		case EXP_BONUS_MAXIMUM:		usXPs =  400;			break;
	}

	// to do: find guys in sector on the currently active squad, those that are conscious get this amount in XPs
	for ( ubGuynum = gTacticalStatus.Team[ gbPlayerNum ].bFirstID, pSoldier = MercPtrs[ ubGuynum ];
				ubGuynum <= gTacticalStatus.Team[ gbPlayerNum ].bLastID;
				ubGuynum++, pSoldier++ )
	{
		if ( pSoldier->bActive && pSoldier->bInSector && IsMercOnCurrentSquad( pSoldier ) && ( pSoldier->bLife >= CONSCIOUSNESS ) &&
				 !( pSoldier->uiStatusFlags & SOLDIER_VEHICLE ) && !AM_A_ROBOT( pSoldier ) )
		{
			StatChange( pSoldier, EXPERAMT, usXPs, FALSE );
		}
	}
}



void BuildStatChangeString( STR16 wString, size_t Length, STR16 wName, BOOLEAN fIncrease, INT16 sPtsChanged, UINT8 ubStat )
{
	UINT8 ubStringIndex;


	Assert( sPtsChanged != 0 );
	Assert( ubStat >= FIRST_CHANGEABLE_STAT );
	Assert( ubStat <= LAST_CHANGEABLE_STAT );

	// if just a 1 point change
	if ( abs( sPtsChanged ) == 1 )
	{
		// use singular
		ubStringIndex = 2;
	}
	else
	{
		ubStringIndex = 3;
		// use plural
	}

	if ( ubStat == EXPERAMT )
	{
		// use "level/levels instead of point/points
		ubStringIndex += 2;
	}

	swprintf( wString, Length, L"%S %S %d %S %S", wName, sPreStatBuildString[ fIncrease ? 1 : 0 ], abs( sPtsChanged ),
					sPreStatBuildString[ ubStringIndex ], sStatGainStrings[ ubStat - FIRST_CHANGEABLE_STAT ] );
}


static UINT8 CalcImportantSectorControl(void)
{
	UINT8 ubMapX, ubMapY;
	UINT8	ubSectorControlPts = 0;


	for ( ubMapX = 1; ubMapX < MAP_WORLD_X - 1; ubMapX++ )
	{
		for ( ubMapY = 1; ubMapY < MAP_WORLD_Y - 1; ubMapY++ )
		{
			// if player controlled
			if ( StrategicMap[ CALCULATE_STRATEGIC_INDEX( ubMapX, ubMapY ) ].fEnemyControlled == FALSE )
			{
				// towns where militia can be trained and SAM sites are important sectors
				if ( MilitiaTrainingAllowedInSector( ubMapX, ubMapY, 0 ) )
				{
					ubSectorControlPts++;

					// SAM sites count double - they have no income, but have significant air control value
					if ( IsThisSectorASAMSector( ubMapX, ubMapY, 0 ) )
					{
						ubSectorControlPts++;
					}
				}
			}
		}
	}

	return( ubSectorControlPts );
}


void MERCMercWentUpALevelSendEmail( UINT8 ubMercMercIdValue )
{
	UINT8 ubEmailOffset = 0;

	ubEmailOffset = MERC_UP_LEVEL_BIFF + MERC_UP_LEVEL_LENGTH_BIFF * ( ubMercMercIdValue );
	AddEmail( ubEmailOffset, MERC_UP_LEVEL_LENGTH_BIFF, SPECK_FROM_MERC, GetWorldTotalMin() );
}
