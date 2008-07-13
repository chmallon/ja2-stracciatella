#include "Font_Control.h"
#include "Types.h"
#include "Soldier_Control.h"
#include "Strategic_Merc_Handler.h"
#include "History.h"
#include "Game_Clock.h"
#include "Insurance_Contract.h"
#include "Soldier_Profile.h"
#include "Game_Event_Hook.h"
#include "Assignments.h"
#include "Overhead.h"
#include "Merc_Contract.h"
#include "Merc_Hiring.h"
#include "Dialogue_Control.h"
#include "Random.h"
#include "Morale.h"
#include "Mercs.h"
#include "MapScreen.h"
#include "Weapons.h"
#include "Personnel.h"
#include "Strategic_Movement.h"
#include "LaptopSave.h"
#include "Message.h"
#include "Text.h"
#include "Interface.h"
#include "Strategic.h"
#include "StrategicMap.h"
#include "Strategic_Status.h"
#include "AIM.h"
#include "EMail.h"
#include "Map_Screen_Interface.h"
#include "Campaign.h"
#include "Soldier_Add.h"
#include "ScreenIDs.h"
#include "JAScreens.h"
#include "Soldier_Macros.h"
#include "Finances.h"
#include "Quests.h"


#define		NUM_DAYS_TILL_UNPAID_RPC_QUITS				3


void StrategicHandlePlayerTeamMercDeath( SOLDIERTYPE *pSoldier )
{
	INT16 sSectorX, sSectorY;

	//if the soldier HAS a profile
	if( pSoldier->ubProfile != NO_PROFILE )
	{
		//add to the history log the fact that the merc died and the circumstances

		// CJC Nov 11, 2002
		// Use the soldier's sector location unless impossible
		if (pSoldier->sSectorX != 0 && pSoldier->sSectorY != 0)
		{
			sSectorX = pSoldier->sSectorX;
			sSectorY = pSoldier->sSectorY;
		}
		else
		{
			sSectorX = gWorldSectorX;
			sSectorY = gWorldSectorY;
		}

		const SOLDIERTYPE* const killer = pSoldier->attacker;
		const UINT8              code   = (killer && killer->bTeam == OUR_TEAM ? HISTORY_MERC_KILLED_CHARACTER : HISTORY_MERC_KILLED);
		AddHistoryToPlayersLog(code, pSoldier->ubProfile, GetWorldTotalMin(), sSectorX, sSectorY);
	}

	if ( guiCurrentScreen != GAME_SCREEN )
	{
		ScreenMsg(FONT_RED, MSG_INTERFACE, pMercDeadString, pSoldier->name);
	}

	// robot and EPCs don't count against death rate - the mercs back home don't particularly give a damn about locals & machines!
	if ( !AM_AN_EPC( pSoldier ) && !AM_A_ROBOT( pSoldier ) )
	{
		// keep track of how many mercs have died under player's command (for death rate, can't wait until removed from team)
		gStrategicStatus.ubMercDeaths++;
	}


	pSoldier->uiStatusFlags |= SOLDIER_DEAD;

	// Set breath to 0!
	pSoldier->bBreathMax = pSoldier->bBreath = 0;

	// not asleep, DEAD!
	pSoldier->fMercAsleep = FALSE;


	//if the merc had life insurance
	if( pSoldier->usLifeInsurance )
	{
		// if he didn't die during auto-resolve
		if( guiCurrentScreen != AUTORESOLVE_SCREEN )
		{
			// check whether this was obviously a suspicious death
			// if killed within an hour of being insured
			if ( pSoldier->uiStartTimeOfInsuranceContract <= GetWorldTotalMin() && GetWorldTotalMin() - pSoldier->uiStartTimeOfInsuranceContract < 60 )
			{
				gMercProfiles[ pSoldier->ubProfile ].ubSuspiciousDeath = VERY_SUSPICIOUS_DEATH;
			}
			// if killed by someone on our team, or while there weren't any opponents around
			else if (pSoldier->attacker->bTeam == OUR_TEAM || !gTacticalStatus.fEnemyInSector)
			{
				// cause insurance company to suspect fraud and investigate this claim
				gMercProfiles[ pSoldier->ubProfile ].ubSuspiciousDeath = SUSPICIOUS_DEATH;
			}
		}

		AddLifeInsurancePayout( pSoldier );
	}


	// robot and EPCs don't penalize morale - merc don't care about fighting machines and the lives of locals much
	if ( !AM_AN_EPC( pSoldier ) && !AM_A_ROBOT( pSoldier ) )
	{
		// Change morale of others based on this
		HandleMoraleEvent( pSoldier, MORALE_TEAMMATE_DIED, pSoldier->sSectorX, pSoldier->sSectorY, pSoldier->bSectorZ );
	}

	//if its a MERC merc, record the time of his death
	if( pSoldier->ubWhatKindOfMercAmI == MERC_TYPE__MERC )
	{
		pSoldier->iEndofContractTime = GetWorldTotalMin();

		//set is so Speck can say that a merc is dead
		LaptopSaveInfo.ubSpeckCanSayPlayersLostQuote = 1;
	}

	//Set the fact that the merc is DEAD!!
	gMercProfiles[ pSoldier->ubProfile ].bMercStatus = MERC_IS_DEAD;

	// handle strategic level death
	HandleStrategicDeath( pSoldier );
}


/* MercDailyUpdate() gets called every day at midnight.  If something is to
 * happen to a merc that day, add an event for it. */
void MercDailyUpdate()
{
	// if its the first day, leave
	if (GetWorldDay() == 1) return;

	ScreenMsg(MSG_FONT_RED, MSG_DEBUG, L"%ls - Doing MercDailyUpdate", WORLDTIMESTR);

	/* if the death rate is very low (this is independent of mercs' personal
	 * deathrate tolerances) */
	if (CalcDeathRate() < 5)
	{
		// everyone gets a morale bonus, which also gets player a reputation bonus.
		HandleMoraleEvent(NULL, MORALE_LOW_DEATHRATE, -1, -1, -1);
	}

	/* add an event so the merc will say the departing warning (2 hours prior to
	 * leaving).  Do so for all time slots they will depart from */
	AddSameDayStrategicEvent(EVENT_MERC_ABOUT_TO_LEAVE, MERC_ARRIVE_TIME_SLOT_1 - 2 * 60,	0);
	AddSameDayStrategicEvent(EVENT_MERC_ABOUT_TO_LEAVE, MERC_ARRIVE_TIME_SLOT_2 - 2 * 60,	0);
	AddSameDayStrategicEvent(EVENT_MERC_ABOUT_TO_LEAVE, MERC_ARRIVE_TIME_SLOT_3 - 2 * 60,	0);

	AddSameDayStrategicEvent(EVENT_BEGIN_CONTRACT_RENEWAL_SEQUENCE, MERC_ARRIVE_TIME_SLOT_1, 0);
	AddSameDayStrategicEvent(EVENT_BEGIN_CONTRACT_RENEWAL_SEQUENCE, MERC_ARRIVE_TIME_SLOT_2, 0);
	AddSameDayStrategicEvent(EVENT_BEGIN_CONTRACT_RENEWAL_SEQUENCE, MERC_ARRIVE_TIME_SLOT_3, 0);

	FOR_ALL_IN_TEAM(s, gbPlayerNum)
	{
		if (s->bAssignment == ASSIGNMENT_POW)
		{
			s->iEndofContractTime += 1440;
		}
		else if (s->bAssignment != IN_TRANSIT)
		{
			//CJC: Reset dialogue flags for quotes that can be said once/day
			s->usQuoteSaidFlags &= ~SOLDIER_QUOTE_SAID_ANNOYING_MERC;
			// ATE: Reset likes gun flag
			s->usQuoteSaidFlags &= ~SOLDIER_QUOTE_SAID_LIKESGUN;
			// ATE; Reset found something nice flag...
			s->usQuoteSaidFlags &= ~SOLDIER_QUOTE_SAID_FOUND_SOMETHING_NICE;

      // ATE: Decrement tolerance value...
      if (--s->bCorpseQuoteTolerance < 0) s->bCorpseQuoteTolerance = 0;

			MERCPROFILESTRUCT* const p = GetProfile(s->ubProfile);

			// CJC: For some personalities, reset personality quote said flag
			switch (p->bPersonalityTrait)
			{
				case HEAT_INTOLERANT:
				case CLAUSTROPHOBIC:
				case NONSWIMMER:
				case FEAR_OF_INSECTS:
					// repeatable once per day
					s->usQuoteSaidFlags &= ~SOLDIER_QUOTE_SAID_PERSONALITY;
					break;

				default: break;
			}

			//ATE: Try to see if our equipment sucks!
			if (SoldierHasWorseEquipmentThanUsedTo(s))
			{
				// Randomly anytime between 6:00, and 10:00
				AddSameDayStrategicEvent(EVENT_MERC_COMPLAIN_EQUIPMENT, 360 + Random(1080), s->ubProfile);
			}

			// increment days served by this grunt
			++p->usTotalDaysServed;

			// player has hired him, so he'll eligible to get killed off on another job
			p->ubMiscFlags3 |= PROFILE_MISC_FLAG3_PLAYER_HAD_CHANCE_TO_HIRE;

			//if the character is an RPC
			if (FIRST_RPC <= s->ubProfile && s->ubProfile < FIRST_NPC)
			{
				//increment the number of days the mercs has been on the team
				++s->iTotalContractLength;

				// The player owes the salary
				INT16	const sSalary          = p->sSalary;
				INT32	      iMoneyOwedToMerc = sSalary;

				//if the player owes the npc money, the balance field will be negative
				if (p->iBalance < 0) iMoneyOwedToMerc += -p->iBalance;

				//if the player owes money
				if (iMoneyOwedToMerc != 0)
				{
					//if the player can afford to pay them
					if (LaptopSaveInfo.iCurrentBalance >= iMoneyOwedToMerc)
					{
						//add the transaction to the player
						AddTransactionToPlayersBook(PAYMENT_TO_NPC, s->ubProfile, GetWorldTotalMin(), -iMoneyOwedToMerc);

						// reset the amount, if the player owed money to the npc
						if (p->iBalance < 0) p->iBalance = 0;
					}
					else
					{
						// Display a screen msg indicating that the npc was NOT paid
						wchar_t zMoney[128];
						SPrintMoney(zMoney, sSalary);
						ScreenMsg(FONT_MCOLOR_WHITE, MSG_INTERFACE, pMessageStrings[MSG_CANT_AFFORD_TO_PAY_NPC_DAILY_SALARY_MSG], p->zNickname, zMoney);

						/* if the merc hasnt been paid for NUM_DAYS_TILL_UNPAID_RPC_QUITS
						 * days, the merc will quit */
						if (p->iBalance - sSalary <= -(sSalary * NUM_DAYS_TILL_UNPAID_RPC_QUITS))
						{
							// Set it up so the merc quits
							MercsContractIsFinished(s);
						}
						else
						{
							//set how much money the player owes the merc
							p->iBalance -= sSalary;

							// Add even for displaying a dialogue telling the player this....
							AddSameDayStrategicEvent(EVENT_RPC_WHINE_ABOUT_PAY, MERC_ARRIVE_TIME_SLOT_1, s->ubID);
						}
					}
				}
			}

			DailyMoraleUpdate(s);
			CheckIfMercGetsAnotherContract(s);
		}

		// if active, here, & alive (POW is ok, don't care)
		if (s->bAssignment != ASSIGNMENT_DEAD &&
				s->bAssignment != IN_TRANSIT)
		{
			// increment the "man days" played counter for each such merc in the player's employment
			++gStrategicStatus.uiManDaysPlayed;
		}
	}

	/* Determine for all MERC mercs, whether they should levae, because the
	 * player refused to pay */
	if (LaptopSaveInfo.gubPlayersMercAccountStatus == MERC_ACCOUNT_INVALID)
	{
		FOR_ALL_IN_TEAM(s, gbPlayerNum)
		{
			if (s->bAssignment == ASSIGNMENT_POW) continue;
			if (s->bAssignment == IN_TRANSIT)     continue;

			if (s->ubWhatKindOfMercAmI != MERC_TYPE__MERC) continue;

			if (!IsTheSoldierAliveAndConcious(s)) continue;

			MercsContractIsFinished(s);
		}
	}

	for (INT32 cnt = 0; cnt < NUM_PROFILES; ++cnt)
	{
		MERCPROFILESTRUCT* const p = GetProfile(cnt);

		// dead guys don't do nuthin' !
		if (p->bMercStatus == MERC_IS_DEAD) continue;

		//Every day reset this variable
		p->uiPrecedentQuoteSaid = 0;

		// skip anyone currently on the player's team
		if (IsMercOnTeam(cnt)) continue;

		if (cnt < AIM_AND_MERC_MERCS && p->bMercStatus != MERC_RETURNING_HOME)
		{
			// check if any of his stats improve through working or training
			HandleUnhiredMercImprovement(p);

			// if he's working on another job
			if (p->bMercStatus == MERC_WORKING_ELSEWHERE)
			{
				// check if he's killed
				HandleUnhiredMercDeaths(cnt);
			}
		}

		// if merc is currently unavailable
		if (p->uiDayBecomesAvailable > 0)
		{
			if (--p->uiDayBecomesAvailable == 0 &&    // Check to see if the merc has become available
					p->bMercStatus != MERC_FIRED_AS_A_POW) // if the merc CAN become ready
			{
				p->bMercStatus = MERC_OK;

				// if the player has left a message for this merc
				if (p->ubMiscFlags3 & PROFILE_MISC_FLAG3_PLAYER_LEFT_MSG_FOR_MERC_AT_AIM)
				{
					//remove the Flag, so if the merc goes on another assignment, the player can leave an email.
					p->ubMiscFlags3 &= ~PROFILE_MISC_FLAG3_PLAYER_LEFT_MSG_FOR_MERC_AT_AIM;

					// TO DO: send E-mail to player telling him the merc has returned from an assignment
					AddEmail(AIM_REPLY_BARRY + cnt * AIM_REPLY_LENGTH_BARRY, AIM_REPLY_LENGTH_BARRY, 6 + cnt, GetWorldTotalMin());
				}
			}
		}
		else	// was already available today
		{
			if (cnt < AIM_AND_MERC_MERCS)
			{
				// check to see if he goes on another assignment
				UINT32 uiChance;
				if (cnt < MAX_NUMBER_MERCS)
				{ // A.I.M. merc
					uiChance = 2 * p->bExpLevel;

					// player has now had a chance to hire him, so he'll eligible to get killed off on another job
					p->ubMiscFlags3 |= PROFILE_MISC_FLAG3_PLAYER_HAD_CHANCE_TO_HIRE;
				}
				else
				{ // M.E.R.C. merc - very rarely get other work
					uiChance = 1 * p->bExpLevel;

					// player doesn't have a chance to hire any M.E.R.C's until after Speck's E-mail is sent
					if (GetWorldDay() > DAYS_TIL_M_E_R_C_AVAIL)
					{
						// player has now had a chance to hire him, so he'll eligible to get killed off on another job
						p->ubMiscFlags3 |= PROFILE_MISC_FLAG3_PLAYER_HAD_CHANCE_TO_HIRE;
					}
				}

				if (Random(100) < uiChance)
				{
					p->bMercStatus = MERC_WORKING_ELSEWHERE;
					p->uiDayBecomesAvailable = 1 + Random(6 + (p->bExpLevel / 2)); // 1-(6 to 11) days
				}
			}
		}

		// Decrement morale hangover (merc appears hirable, he just gives lame refusals during this time, though)
		if (p->ubDaysOfMoraleHangover > 0) --p->ubDaysOfMoraleHangover;
	}

	HandleSlayDailyEvent();
	ReBuildCharactersList();
}


// ATE: This function deals with MERC MERC and NPC's leaving because of not getting paid...
// NOT AIM renewals....
void MercsContractIsFinished(SOLDIERTYPE* const pSoldier)
{
	#ifndef JA2DEMO

	//if the soldier was removed before getting into this function, return
	if( !pSoldier->bActive )
		return;

	if( fShowContractMenu )
	{
		fShowContractMenu = FALSE;
	}

	// go to mapscreen
	SpecialCharacterDialogueEvent( DIALOGUE_SPECIAL_EVENT_ENTER_MAPSCREEN,0,0,0,0,0 );


	if( pSoldier->ubWhatKindOfMercAmI == MERC_TYPE__MERC )
	{
		//if the players account status is invalid
		if( LaptopSaveInfo.gubPlayersMercAccountStatus == MERC_ACCOUNT_INVALID )
		{
			//Send the merc home

			InterruptTime( );
			PauseGame();
			LockPauseState( 9 );

			// Say quote for wishing to leave
			TacticalCharacterDialogue( pSoldier, QUOTE_NOT_GETTING_PAID );

			TacticalCharacterDialogueWithSpecialEvent( pSoldier, 0, DIALOGUE_SPECIAL_EVENT_CONTRACT_ENDING_NO_ASK_EQUIP, 0, 0 );

			pSoldier->ubLeaveHistoryCode = HISTORY_MERC_QUIT;
		}
	}
	else if( pSoldier->ubWhatKindOfMercAmI == MERC_TYPE__NPC )
	{
		InterruptTime( );
		PauseGame();
		LockPauseState( 10 );

		TacticalCharacterDialogue( pSoldier, QUOTE_AIM_SEEN_MIKE );

		TacticalCharacterDialogueWithSpecialEvent( pSoldier, 0, DIALOGUE_SPECIAL_EVENT_CONTRACT_ENDING_NO_ASK_EQUIP, 0, 0 );

		pSoldier->ubLeaveHistoryCode = HISTORY_MERC_QUIT;

	}

	#endif
}


// ATE: Called for RPCs who should now complain about no pay...
void RPCWhineAboutNoPay(SOLDIERTYPE* const pSoldier)
{
	#ifndef JA2DEMO
	//if the soldier was removed before getting into this function, return
	if( !pSoldier->bActive )
		return;

	if( pSoldier->ubWhatKindOfMercAmI == MERC_TYPE__NPC )
	{
		// Say quote for needing pay!
		TacticalCharacterDialogue( pSoldier, QUOTE_NOT_GETTING_PAID );
	}

	#endif
}


// OK loop through and check!
BOOLEAN SoldierHasWorseEquipmentThanUsedTo( SOLDIERTYPE *pSoldier )
{
	INT32		cnt;
	UINT16	usItem;
	INT8		bBestArmour = -1;
	INT8		bBestGun = -1;

	for ( cnt = 0; cnt < NUM_INV_SLOTS; cnt++ )
	{
		usItem = pSoldier->inv[ cnt ].usItem;

		// Look for best gun/armour
		if ( usItem != NOTHING )
		{
			// Check if it's a gun
			if ( Item[ usItem ].usItemClass & IC_GUN )
			{
				if ( Weapon[ usItem ].ubDeadliness > bBestGun )
				{
					bBestGun = Weapon[ usItem ].ubDeadliness;
				}
			}

			// If it's armour
			if ( Item[ usItem ].usItemClass & IC_ARMOUR )
			{
				if ( Armour[ Item[ usItem ].ubClassIndex ].ubProtection > bBestArmour )
				{
					bBestArmour = Armour[ Item[ usItem ].ubClassIndex ].ubProtection;
				}
			}
		}
	}

	// Modify these values based on morale - lower opinion of equipment if morale low, increase if high
	// this of course assumes default morale is 50
	if ( bBestGun != -1 )
	{
		bBestGun		= (bBestGun		 * (50 + pSoldier->bMorale)) / 100;
	}
	if ( bBestArmour != -1 )
	{
		bBestArmour = (bBestArmour * (50 + pSoldier->bMorale)) / 100;
	}

	// OK, check values!
	if ( 	(bBestGun != -1 && bBestGun < ( gMercProfiles[ pSoldier->ubProfile ].bMainGunAttractiveness / 2 )) ||
				(bBestArmour != -1 && bBestArmour < ( gMercProfiles[ pSoldier->ubProfile ].bArmourAttractiveness / 2 )) )
	{
		// Pipe up!
		return( TRUE );
	}

	return( FALSE );
}


void MercComplainAboutEquipment( UINT8 ubProfile )
{
	if ( ubProfile == LARRY_NORMAL  )
	{
		if ( CheckFact( FACT_LARRY_CHANGED, 0 ) )
		{
			ubProfile = LARRY_DRUNK;
		}
	}
	else if ( ubProfile == LARRY_DRUNK )
	{
		if (!CheckFact(FACT_LARRY_CHANGED, 0))
		{
			ubProfile = LARRY_NORMAL;
		}
	}
	// Are we dead/ does merc still exist?
	SOLDIERTYPE* const pSoldier = FindSoldierByProfileID(ubProfile);
	if ( pSoldier != NULL )
	{
		if (!pSoldier->fMercAsleep          &&
				pSoldier->bLife       >= OKLIFE &&
				pSoldier->bAssignment <  ON_DUTY)
		{
			//ATE: Double check that this problem still exists!
			if ( SoldierHasWorseEquipmentThanUsedTo( pSoldier ) )
			{
				// Say quote!
				TacticalCharacterDialogue( pSoldier, QUOTE_WHINE_EQUIPMENT );
			}
		}
	}
}



void UpdateBuddyAndHatedCounters( void )
{
	INT32									iLoop;
	UINT8									ubOtherProfileID;
	MERCPROFILESTRUCT			*pProfile;
	BOOLEAN								fSameGroupOnly;

	BOOLEAN								fUpdatedTimeTillNextHatedComplaint = FALSE;

	FOR_ALL_IN_TEAM(pSoldier, gbPlayerNum)
	{
		fSameGroupOnly = FALSE;

		//if the merc is active and on a combat assignment
		if (pSoldier->bAssignment < ON_DUTY)
		{
			pProfile = &(gMercProfiles[ pSoldier->ubProfile ]);

			// if we're moving, we only check vs other people in our squad
			if (pSoldier->ubGroupID != 0 && PlayerIDGroupInMotion( pSoldier->ubGroupID ))
			{
				fSameGroupOnly = TRUE;
			}

			fUpdatedTimeTillNextHatedComplaint = FALSE;

			CFOR_ALL_IN_TEAM(pOtherSoldier, gbPlayerNum)
			{
				// is this guy in the same sector and on active duty (or in the same moving group)
				if (pOtherSoldier != pSoldier && pOtherSoldier->bAssignment < ON_DUTY)
				{
					if (fSameGroupOnly)
					{
						// all we have to check is the group ID
						if (pSoldier->ubGroupID != pOtherSoldier->ubGroupID)
						{
							continue;
						}
					}
					else
					{
						// check to see if the location is the same
						if (pOtherSoldier->sSectorX != pSoldier->sSectorX ||
							  pOtherSoldier->sSectorY != pSoldier->sSectorY ||
								pOtherSoldier->bSectorZ != pSoldier->bSectorZ)
						{
							continue;
						}

						// if the OTHER soldier is in motion then we don't do anything!
						if (pOtherSoldier->ubGroupID != 0 && PlayerIDGroupInMotion( pOtherSoldier->ubGroupID ))
						{
							continue;
						}
					}

					ubOtherProfileID = pOtherSoldier->ubProfile;

					for ( iLoop = 0; iLoop < 4; iLoop++ )
					{
						switch( iLoop )
						{
							case 0:
							case 1:
								if (pProfile->bHated[iLoop] == ubOtherProfileID)
								{
									// arrgs, we're on assignment with the person we loathe!
									if ( pProfile->bHatedCount[iLoop] > 0 )
									{
										pProfile->bHatedCount[iLoop]--;
										if ( pProfile->bHatedCount[iLoop] == 0 && pSoldier->bInSector && gTacticalStatus.fEnemyInSector )
										{
											// just reduced count to 0 but we have enemy in sector...
											pProfile->bHatedCount[iLoop] = 1;
										}
										else if (pProfile->bHatedCount[iLoop] > 0 && (pProfile->bHatedCount[iLoop] == pProfile->bHatedTime[iLoop] / 2 || ( pProfile->bHatedCount[iLoop] < pProfile->bHatedTime[iLoop] / 2 && pProfile->bHatedCount[iLoop] % TIME_BETWEEN_HATED_COMPLAINTS == 0 ) ) )
										{
											// complain!
											if (iLoop == 0)
											{
												TacticalCharacterDialogue( pSoldier, QUOTE_HATED_MERC_ONE );
											}
											else
											{
												TacticalCharacterDialogue( pSoldier, QUOTE_HATED_MERC_TWO );
											}
											StopTimeCompression();
										}
										else if ( pProfile->bHatedCount[iLoop] == 0 )
										{
											// zero count!
											if (pSoldier->ubWhatKindOfMercAmI == MERC_TYPE__MERC || pSoldier->ubWhatKindOfMercAmI == MERC_TYPE__NPC )
											{
												// MERC mercs leave now!
												if (iLoop == 0)
												{
													TacticalCharacterDialogue( pSoldier, QUOTE_MERC_QUIT_HATED1 );
												}
												else
												{
													TacticalCharacterDialogue( pSoldier, QUOTE_MERC_QUIT_HATED2 );
												}

												// Leave now! ( handle equipment too )....
												TacticalCharacterDialogueWithSpecialEvent( pSoldier, 0, DIALOGUE_SPECIAL_EVENT_CONTRACT_ENDING, 0,0 );

												pSoldier->ubLeaveHistoryCode = HISTORY_MERC_QUIT;
											}
											else
											{
												// complain!
												if (iLoop == 0)
												{
													TacticalCharacterDialogue( pSoldier, QUOTE_HATED_MERC_ONE );
												}
												else
												{
													TacticalCharacterDialogue( pSoldier, QUOTE_HATED_MERC_TWO );
												}
												pProfile->ubTimeTillNextHatedComplaint = TIME_BETWEEN_HATED_COMPLAINTS - 1;
											}
										}
									}
									else
									{
										// if we haven't updated the time till our next complaint, do so
										// if it's 0, gripe.
										if ( !fUpdatedTimeTillNextHatedComplaint )
										{
											if ( pProfile->ubTimeTillNextHatedComplaint == 0 )
											{
												pProfile->ubTimeTillNextHatedComplaint = TIME_BETWEEN_HATED_COMPLAINTS - 1;
											}
											else
											{
												pProfile->ubTimeTillNextHatedComplaint--;
											}
											fUpdatedTimeTillNextHatedComplaint = TRUE;
										}

										if ( pProfile->ubTimeTillNextHatedComplaint == 0 )
										{
											// complain!
											if (iLoop == 0)
											{
												TacticalCharacterDialogue( pSoldier, QUOTE_HATED_MERC_ONE );
											}
											else
											{
												TacticalCharacterDialogue( pSoldier, QUOTE_HATED_MERC_TWO );
											}
										}
									}
								}
								break;
							case 2:
								if (pProfile->bLearnToHate == ubOtherProfileID)
								{
									if ( pProfile->bLearnToHateCount > 0 )
									{
										pProfile->bLearnToHateCount--;
										if ( pProfile->bLearnToHateCount == 0 && pSoldier->bInSector && gTacticalStatus.fEnemyInSector )
										{
											// just reduced count to 0 but we have enemy in sector...
											pProfile->bLearnToHateCount = 1;
										}
										else if (pProfile->bLearnToHateCount > 0 && (pProfile->bLearnToHateCount == pProfile->bLearnToHateTime / 2 || pProfile->bLearnToHateCount < pProfile->bLearnToHateTime / 2 && pProfile->bLearnToHateCount % TIME_BETWEEN_HATED_COMPLAINTS == 0 ) )
										{
											// complain!
											TacticalCharacterDialogue( pSoldier, QUOTE_LEARNED_TO_HATE_MERC );
											StopTimeCompression();
										}
										else if (pProfile->bLearnToHateCount == 0)
										{
											// set as bHated[2];
											pProfile->bHated[2] = pProfile->bLearnToHate;
											pProfile->bMercOpinion[ubOtherProfileID] = HATED_OPINION;

											if (pSoldier->ubWhatKindOfMercAmI == MERC_TYPE__MERC || (pSoldier->ubWhatKindOfMercAmI == MERC_TYPE__NPC && (pSoldier->ubProfile == DEVIN || pSoldier->ubProfile == SLAY || pSoldier->ubProfile == IGGY || pSoldier->ubProfile == CONRAD ) ) )
											{
												// Leave now! ( handle equipment too )....
												TacticalCharacterDialogue( pSoldier, QUOTE_MERC_QUIT_LEARN_TO_HATE );
												TacticalCharacterDialogueWithSpecialEvent( pSoldier, 0, DIALOGUE_SPECIAL_EVENT_CONTRACT_ENDING, 0,0 );
												pSoldier->ubLeaveHistoryCode = HISTORY_MERC_QUIT;

											}
											else if (pSoldier->ubWhatKindOfMercAmI == MERC_TYPE__NPC)
											{
												// whine again
												TacticalCharacterDialogue( pSoldier, QUOTE_LEARNED_TO_HATE_MERC );
											}

										}
										if (pProfile->bLearnToHateCount < pProfile->bLearnToHateTime / 2)
										{
											// gradual opinion drop
											pProfile->bMercOpinion[ubOtherProfileID] += (HATED_OPINION - pProfile->bMercOpinion[ubOtherProfileID]) / (pProfile->bLearnToHateCount + 1);
										}
									}
									else
									{
										if ( !fUpdatedTimeTillNextHatedComplaint )
										{
											if ( pProfile->ubTimeTillNextHatedComplaint == 0 )
											{
												pProfile->ubTimeTillNextHatedComplaint = TIME_BETWEEN_HATED_COMPLAINTS - 1;
											}
											else
											{
												pProfile->ubTimeTillNextHatedComplaint--;
											}
											fUpdatedTimeTillNextHatedComplaint = TRUE;
										}

										if ( pProfile->ubTimeTillNextHatedComplaint == 0 )
										{
											// complain!
											TacticalCharacterDialogue( pSoldier, QUOTE_LEARNED_TO_HATE_MERC );
										}
									}
								}
								break;
							case 3:
								if (pProfile->bLearnToLikeCount > 0	&& pProfile->bLearnToLike == ubOtherProfileID)
								{
									pProfile->bLearnToLikeCount--;
									if (pProfile->bLearnToLikeCount == 0)
									{
										// add to liked!
										pProfile->bBuddy[2] = pProfile->bLearnToLike;
										pProfile->bMercOpinion[ubOtherProfileID] = BUDDY_OPINION;
									}
									else if (pProfile->bLearnToLikeCount < pProfile->bLearnToLikeTime / 2)
									{
										// increase opinion of them!
										pProfile->bMercOpinion[ubOtherProfileID] += (BUDDY_OPINION - pProfile->bMercOpinion[ubOtherProfileID]) / (pProfile->bLearnToLikeCount + 1);
										break;
									}
								}
								break;
						}
					}
				}
			}
		}
	}
}


void HourlyCamouflageUpdate( void )
{
	FOR_ALL_IN_TEAM(pSoldier, gbPlayerNum)
	{
		// if the merc has non-zero camo, degrade it by 1%
		if( ( pSoldier->bCamo > 0) && ( !( HAS_SKILL_TRAIT( pSoldier, CAMOUFLAGED) ) ) )
		{
			pSoldier->bCamo -= 2;
			if (pSoldier->bCamo <= 0)
			{
				pSoldier->bCamo = 0;
				// Reload palettes....
				if ( pSoldier->bInSector )
				{
					CreateSoldierPalettes( pSoldier );
				}

				ScreenMsg(FONT_MCOLOR_LTYELLOW, MSG_INTERFACE, Message[STR_CAMO_WORN_OFF], pSoldier->name);
				DirtyMercPanelInterface( pSoldier, DIRTYLEVEL2 );
			}
		}
		// if the merc has non-zero monster smell, degrade it by 1
		if ( pSoldier->bMonsterSmell > 0 )
		{
			pSoldier->bMonsterSmell--;

			/*
			if (pSoldier->bMonsterSmell == 0)
			{
				// Reload palettes....

				if ( pSoldier->bInSector )
				{
					CreateSoldierPalettes( pSoldier );
				}

				ScreenMsg(FONT_MCOLOR_LTYELLOW, MSG_INTERFACE, Message[STR_CAMO_WORN_OFF], pSoldier->name);
				DirtyMercPanelInterface( pSoldier, DIRTYLEVEL2 );
			}
			*/
		}
	}
}
