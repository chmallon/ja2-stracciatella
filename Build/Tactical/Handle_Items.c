#include "Font.h"
#include "Items.h"
#include "Action_Items.h"
#include "Handle_Items.h"
#include "Local.h"
#include "Overhead.h"
#include "Soldier_Find.h"
#include "VObject.h"
#include "Weapons.h"
#include "Points.h"
#include "TileDef.h"
#include "WorldDef.h"
#include "WorldMan.h"
#include "Interface.h"
#include "RenderWorld.h"
#include "Animation_Control.h"
#include "Font_Control.h"
#include "Render_Dirty.h"
#include "World_Items.h"
#include "Text.h"
#include "Timer_Control.h"
#include "WCheck.h"
#include "Interface_Items.h"
#include "Soldier_Profile.h"
#include "Interface_Dialogue.h"
#include "Quests.h"
#include "Message.h"
#include "Isometric_Utils.h"
#include "LOS.h"
#include "Dialogue_Control.h"
#include "AI.h"
#include "Soldier_Macros.h"
#include "Interface_Panels.h"
#include "Strategic_Town_Loyalty.h"
#include "Soldier_Functions.h"
#include "Map_Screen_Helicopter.h"
#include "PathAI.h"
#include "FOV.h"
#include "MessageBoxScreen.h"
#include "Explosion_Control.h"
#include "SkillCheck.h"
#include "Campaign.h"
#include "Random.h"
#include "Structure_Wrap.h"
#include "Interactive_Tiles.h"
#include "SaveLoadMap.h"
#include "ShopKeeper_Interface.h"
#include "Arms_Dealer_Init.h"
#include "Soldier_Add.h"
#include "Sound_Control.h"
#include "Squads.h"
#include "Rotting_Corpses.h"
#include "Soldier_Ani.h"
#include "OppList.h"
#include "QArray.h"
#include "Render_Fun.h"
#include "Environment.h"
#include "Map_Information.h"
#include "GameSettings.h"
#include "StrategicMap.h"
#include "End_Game.h"
#include "Interface_Control.h"
#include "Map_Screen_Interface_Map_Inventory.h"
#include "ScreenIDs.h"
#include "VSurface.h"
#include "MemMan.h"
#include "Debug.h"


#define					NUM_ITEMS_LISTED			8
#define					NUM_ITEM_FLASH_SLOTS	50
#define					MIN_LOB_RANGE					6


typedef void (*ITEM_POOL_LOCATOR_HOOK)(void);


typedef struct
{
	ITEM_POOL*             pItemPool;
	INT8                   bRadioFrame;
	UINT32                 uiLastFrameUpdate;
	ITEM_POOL_LOCATOR_HOOK Callback;
	BOOLEAN                fAllocated;
	UINT8                  ubFlags;
} ITEM_POOL_LOCATOR;


static ITEM_POOL_LOCATOR FlashItemSlots[NUM_ITEM_FLASH_SLOTS];
static UINT32 guiNumFlashItemSlots = 0;


// Disgusting hacks: have to keep track of these values for accesses in callbacks
static SOLDIERTYPE *	gpTempSoldier;
static INT16					gsTempGridno;
static INT8						bTempFrequency;


SOLDIERTYPE *		gpBoobyTrapSoldier;
ITEM_POOL *			gpBoobyTrapItemPool;
INT16						gsBoobyTrapGridNo;
INT8            gbBoobyTrapLevel;
BOOLEAN					gfDisarmingBuriedBomb;
INT8						gbTrapDifficulty;
BOOLEAN					gfJustFoundBoobyTrap = FALSE;


BOOLEAN HandleCheckForBadChangeToGetThrough(SOLDIERTYPE* const pSoldier, const SOLDIERTYPE* const pTargetSoldier, const INT16 sTargetGridNo, const INT8 bLevel)
{
	BOOLEAN						fBadChangeToGetThrough = FALSE;

	if ( pTargetSoldier != NULL )
	{
		if ( SoldierToSoldierBodyPartChanceToGetThrough( pSoldier, pTargetSoldier, pSoldier->bAimShotLocation ) < OK_CHANCE_TO_GET_THROUGH )
		{
			fBadChangeToGetThrough = TRUE;
		}
	}
	else
	{
		if (SoldierToLocationChanceToGetThrough(pSoldier, sTargetGridNo, bLevel, 0, NULL) < OK_CHANCE_TO_GET_THROUGH)
		{
			fBadChangeToGetThrough = TRUE;
		}
	}

	if ( fBadChangeToGetThrough )
	{
		if (gTacticalStatus.sCantGetThroughSoldierGridNo != pSoldier->sGridNo ||
				gTacticalStatus.sCantGetThroughGridNo        != sTargetGridNo     ||
				gTacticalStatus.cant_get_through             != pSoldier)
		{
			gTacticalStatus.fCantGetThrough = FALSE;
		}

		// Have we done this once already?
		if ( !gTacticalStatus.fCantGetThrough )
		{
			gTacticalStatus.fCantGetThrough              = TRUE;
			gTacticalStatus.sCantGetThroughGridNo        = sTargetGridNo;
			gTacticalStatus.cant_get_through             = pSoldier;
			gTacticalStatus.sCantGetThroughSoldierGridNo = pSoldier->sGridNo;

			// PLay quote
			TacticalCharacterDialogue( pSoldier, QUOTE_NO_LINE_OF_FIRE );
			return( FALSE );
		}
		else
		{
			// Is this a different case?
			if (gTacticalStatus.sCantGetThroughGridNo        != sTargetGridNo ||
					gTacticalStatus.cant_get_through             != pSoldier      ||
					gTacticalStatus.sCantGetThroughSoldierGridNo != pSoldier->sGridNo)
			{
				// PLay quote
				gTacticalStatus.sCantGetThroughGridNo	= sTargetGridNo;
				gTacticalStatus.cant_get_through      = pSoldier;

				TacticalCharacterDialogue( pSoldier, QUOTE_NO_LINE_OF_FIRE );
				return( FALSE );
			}
		}
	}
	else
	{
		gTacticalStatus.fCantGetThrough = FALSE;
	}

	return( TRUE );
}


static BOOLEAN HandItemWorks(SOLDIERTYPE* pSoldier, INT8 bSlot);
static void StartBombMessageBox(SOLDIERTYPE* pSoldier, INT16 sGridNo);


INT32 HandleItem(SOLDIERTYPE* const s, INT16 usGridNo, const INT8 bLevel, const UINT16 usHandItem, const BOOLEAN fFromUI)
{
	// Remove any previous actions
	s->ubPendingAction = NO_PENDING_ACTION;

	// here is where we would set a different value if the weapon mode is on
	// "attached weapon"
	s->usAttackingWeapon = usHandItem;

	// Find soldier flags depend on if it's our own merc firing or a NPC
	INT16        sGridNo;
	SOLDIERTYPE* tgt = WhoIsThere2(usGridNo, bLevel);
	if (tgt != NULL && fFromUI)
	{
		// ATE: Check if we are targeting an interactive tile, and adjust gridno accordingly...
		STRUCTURE* pStructure;
		LEVELNODE* const pIntNode = GetCurInteractiveTileGridNoAndStructure(&sGridNo, &pStructure);
		if (pIntNode != NULL && tgt == s)
		{
			// Truncate target soldier
			tgt = NULL;
		}
	}

	// ATE: If in realtime, set attacker count to 0...
	if (!(gTacticalStatus.uiFlags & INCOMBAT))
	{
		DebugMsg(TOPIC_JA2, DBG_LEVEL_3, "Setting attack busy count to 0 due to no combat");
		gTacticalStatus.ubAttackBusyCount = 0;
	}

	if (tgt) tgt->bBeingAttackedCount = 0;

	// Check our soldier's life for unconscious!
	if (s->bLife < OKLIFE)          return ITEM_HANDLE_UNCONSCIOUS;
	if (!HandItemWorks(s, HANDPOS)) return ITEM_HANDLE_BROKEN;

	const INVTYPE* const item = &Item[usHandItem];

	if (fFromUI                                      &&
			s->bTeam == gbPlayerNum                      &&
			tgt                                          &&
			(tgt->bTeam == gbPlayerNum || tgt->bNeutral) &&
			tgt->ubBodyType != CROW                      &&
			item->usItemClass != IC_MEDKIT               &&
			s->ubProfile != NO_PROFILE)
	{
		// nice mercs won't shoot other nice guys or neutral civilians
		if (gMercProfiles[s->ubProfile].ubMiscFlags3 & PROFILE_MISC_FLAG3_GOODGUY &&
				(
					(tgt->ubProfile == NO_PROFILE && tgt->bNeutral) ||
					gMercProfiles[tgt->ubProfile].ubMiscFlags3 & PROFILE_MISC_FLAG3_GOODGUY
				))
		{
			TacticalCharacterDialogue(s, QUOTE_REFUSING_ORDER);
			return ITEM_HANDLE_REFUSAL;
		}
		// buddies won't shoot each other
		if (tgt->ubProfile != NO_PROFILE &&
				WhichBuddy(s->ubProfile, tgt->ubProfile) != -1)
		{
			TacticalCharacterDialogue(s, QUOTE_REFUSING_ORDER);
			return ITEM_HANDLE_REFUSAL;
		}

		// any recruited rebel will refuse to fire on another rebel or neutral nameless civ
		if (s->ubCivilianGroup == REBEL_CIV_GROUP &&
				(
					tgt->ubCivilianGroup == REBEL_CIV_GROUP ||
					(
						tgt->bNeutral                         &&
						tgt->ubProfile       == NO_PROFILE    &&
						tgt->ubCivilianGroup == NON_CIV_GROUP &&
						tgt->ubBodyType      != CROW
					)
				))
		{
			TacticalCharacterDialogue(s, QUOTE_REFUSING_ORDER);
			return ITEM_HANDLE_REFUSAL;
		}
	}

	// Check HAND ITEM
	if (item->usItemClass == IC_GUN || item->usItemClass == IC_THROWING_KNIFE)
	{
		// WEAPONS
		if (usHandItem == ROCKET_RIFLE || usHandItem == AUTO_ROCKET_RIFLE)
		{
			// check imprint ID
			// NB not-imprinted value is NO_PROFILE
			// imprinted value is profile for mercs & NPCs and NO_PROFILE + 1 for generic dudes
			OBJECTTYPE* const wpn = &s->inv[s->ubAttackingHand];
			if (s->ubProfile != NO_PROFILE)
			{
				if (wpn->ubImprintID != s->ubProfile)
				{
					if (wpn->ubImprintID == NO_PROFILE)
					{
						// first shot using "virgin" gun... set imprint ID
						wpn->ubImprintID = s->ubProfile;

						// this could be an NPC (Krott)
						if (s->bTeam == gbPlayerNum)
						{
							PlayJA2Sample(RG_ID_IMPRINTED, HIGHVOLUME, 1, MIDDLE);
							ScreenMsg(FONT_MCOLOR_LTYELLOW, MSG_INTERFACE, L"\"%ls\"", TacticalStr[GUN_GOT_FINGERPRINT]);
							return ITEM_HANDLE_BROKEN;
						}
					}
					else
					{
						// access denied!
						if (s->bTeam == gbPlayerNum)
						{
							PlayJA2Sample(RG_ID_INVALID, HIGHVOLUME, 1, MIDDLE);
						}
						return ITEM_HANDLE_BROKEN;
					}
				}
			}
			else
			{
				// guaranteed not to be controlled by the player, so no feedback required
				if (wpn->ubImprintID != NO_PROFILE + 1)
				{
					if (wpn->ubImprintID != NO_PROFILE) return ITEM_HANDLE_BROKEN;
					wpn->ubImprintID = NO_PROFILE + 1;
				}
			}
		}

		// IF we are not a throwing knife, check for ammo, reloading...
		if (item->usItemClass != IC_THROWING_KNIFE)
		{
			// CHECK FOR AMMO!
			if (!EnoughAmmo(s, fFromUI, HANDPOS))
			{
				//ATE: Reflect that we need to reset for bursting
				s->fDoSpread = FALSE;
				return ITEM_HANDLE_NOAMMO;
			}

			// Check if we are reloading
			if ((gTacticalStatus.uiFlags & REALTIME || !(gTacticalStatus.uiFlags & INCOMBAT)) &&
					s->fReloading)
			{
				return ITEM_HANDLE_RELOADING;
			}
		}

		// Get gridno - either soldier's position or the gridno
		const INT16 sTargetGridNo = (tgt != NULL ? tgt->sGridNo : usGridNo);

		// If it's a player guy, check ChanceToGetThrough to play quote
		if (fFromUI                             &&
				gTacticalStatus.uiFlags & TURNBASED &&
				gTacticalStatus.uiFlags & INCOMBAT  &&
				!s->fDoSpread                       &&
				!HandleCheckForBadChangeToGetThrough(s, tgt, sTargetGridNo, bLevel))
		{
			return ITEM_HANDLE_OK;
		}

		// Get AP COSTS
		// ATE: OK something funny going on here - AI seems to NEED FALSE here,
		// Our guys NEED TRUE. We shoulkd at some time make sure the AI and
		// our guys are deducting/checking in the same manner to avoid
		// these differences.
		const INT16 sAPCost = CalcTotalAPsToAttack(s, sTargetGridNo, TRUE, s->bAimTime);

		BOOLEAN fAddingTurningCost  = FALSE;
		BOOLEAN fAddingRaiseGunCost = FALSE;
		GetAPChargeForShootOrStabWRTGunRaises(s, sTargetGridNo, TRUE, &fAddingTurningCost, &fAddingRaiseGunCost);

		// If we are standing and are asked to turn AND raise gun, ignore raise gun...
		if (gAnimControl[s->usAnimState].ubHeight == ANIM_STAND)
		{
			if (fAddingRaiseGunCost) s->fDontChargeTurningAPs = TRUE;
		}
		else
		{
			// If raising gun, don't charge turning!
			if (fAddingTurningCost) s->fDontChargeReadyAPs = TRUE;
		}

		// If this is a player guy, show message about no APS
		if (!EnoughPoints(s, sAPCost, 0, fFromUI)) return ITEM_HANDLE_NOAPS;

		// Psychos might possibly switch to burst if they can
		if (s->ubProfile != NO_PROFILE                              &&
				gMercProfiles[s->ubProfile].bPersonalityTrait == PSYCHO &&
				!s->bDoBurst                                            &&
				IsGunBurstCapable(s, HANDPOS))
		{
			// chance of firing burst if we have points... chance decreasing when ordered to do aimed shot

			// temporarily set burst to true to calculate action points
			s->bDoBurst = TRUE;
			const INT16 sAPCost = CalcTotalAPsToAttack(s, sTargetGridNo, TRUE, 0);
			// reset burst mode to false (which is what it was at originally)
			s->bDoBurst = FALSE;

			// we have enough points to do this burst, roll the dice and see if we want to change
			if (EnoughPoints(s, sAPCost, 0, FALSE) && Random(3 + s->bAimTime) == 0)
			{
				DoMercBattleSound(s, BATTLE_SOUND_LAUGH1);
				s->bDoBurst    = TRUE;
				s->bWeaponMode = WM_BURST;
				ScreenMsg(FONT_MCOLOR_LTYELLOW, MSG_INTERFACE, gzLateLocalizedString[26], s->name);
			}
		}

		// Deduct points if our target is different!
		// if attacking a new target (or if the specific target is uncertain)

		if (fFromUI)
		{
			// set the target level; if the AI calls this it will have set the level already...
			s->bTargetLevel = gsInterfaceLevel;
		}

		if (item->usItemClass != IC_THROWING_KNIFE)
		{
			// If doing spread, set down the first gridno.....
			if (!s->fDoSpread || s->sSpreadLocations[0] == 0)
			{
				SendBeginFireWeaponEvent(s, sTargetGridNo);
			}
			else
			{
				SendBeginFireWeaponEvent(s, s->sSpreadLocations[0]);
			}

			// ATE: Here to make cursor go back to move after LAW shot...
			if (fFromUI && usHandItem == ROCKET_LAUNCHER)
			{
				guiPendingOverrideEvent = A_CHANGE_TO_MOVE;
			}
		}
		else
		{
			// Start knife throw attack
			const UINT8 ubDirection = GetDirectionFromGridNo(sTargetGridNo, s);
			EVENT_SoldierBeginKnifeThrowAttack(s, sTargetGridNo, ubDirection);
		}

		// If in turn based - refresh aim to first level
		if (fFromUI                             &&
				gTacticalStatus.uiFlags & TURNBASED &&
				gTacticalStatus.uiFlags & INCOMBAT)
		{
			s->bShownAimTime = REFINE_AIM_1;

			// Locate to soldier if he's about to shoot!
			if (s->bTeam != gbPlayerNum) ShowRadioLocator(s, SHOW_LOCATOR_NORMAL);
		}

		SetUIBusy(s);
		return ITEM_HANDLE_OK;
	}

	//TRY PUNCHING
	if (item->usItemClass == IC_PUNCH)
	{
		INT16 sGotLocation = NOWHERE;
		UINT8 ubDirection;
		INT16 sAdjustedGridNo;
		for (INT16 i = 0; i < NUM_WORLD_DIRECTIONS; ++i)
		{
			const INT16 sSpot = NewGridNo(s->sGridNo, DirectionInc(i));

			// Make sure movement costs are OK....
			if (gubWorldMovementCosts[sSpot][i][bLevel] >= TRAVELCOST_BLOCKED)
			{
				continue;
			}

			// Check for who is there...
			if (tgt != NULL && tgt == WhoIsThere2(sSpot, s->bLevel))
			{
				// We've got a guy here....
				// Who is the one we want......
				sGotLocation    = sSpot;
				sAdjustedGridNo = tgt->sGridNo;
				ubDirection     = i;
				break;
			}
		}

		BOOLEAN	fGotAdjacent = FALSE;
		if (sGotLocation == NOWHERE)
		{
			// See if we can get there to punch
			const INT16 sActionGridNo = FindAdjacentGridEx(s, usGridNo, &ubDirection, &sAdjustedGridNo, TRUE, FALSE);
			if (sActionGridNo != -1)
			{
				// OK, we've got somebody...
				sGotLocation = sActionGridNo;
				fGotAdjacent = TRUE;
			}
		}

		// Did we get a loaction?
		if (sGotLocation == NOWHERE) return ITEM_HANDLE_CANNOT_GETTO_LOCATION;

		s->sTargetGridNo = usGridNo;
		s->usActionData  = usGridNo;
		// CHECK IF WE ARE AT THIS GRIDNO NOW
		if (s->sGridNo != sGotLocation && fGotAdjacent)
		{
			// SEND PENDING ACTION
			s->ubPendingAction          = MERC_PUNCH;
			s->sPendingActionData2      = sAdjustedGridNo;
			s->bPendingActionData3      = ubDirection;
			s->ubPendingActionAnimCount = 0;

			// WALK UP TO DEST FIRST
			EVENT_InternalGetNewSoldierPath(s, sGotLocation, s->usUIMovementMode, FALSE, TRUE);
		}
		else
		{
			s->bAction = AI_ACTION_KNIFE_STAB;
			EVENT_SoldierBeginPunchAttack(s, sAdjustedGridNo, ubDirection);
		}

		SetUIBusy(s);
		gfResetUIMovementOptimization = TRUE;
		return ITEM_HANDLE_OK;
	}

	//USING THE MEDKIT
	if (item->usItemClass == IC_MEDKIT)
	{
		// ATE: AI CANNOT GO THROUGH HERE!
		const INT16 usMapPos = (gTacticalStatus.fAutoBandageMode ? usGridNo : GetMouseMapPos());

		// See if we can get there to stab
		BOOLEAN	fHadToUseCursorPos = FALSE;
		UINT8   ubDirection;
		INT16   sAdjustedGridNo;
		INT16   sActionGridNo      = FindAdjacentGridEx(s, usGridNo, &ubDirection, &sAdjustedGridNo, TRUE, FALSE);
		if (sActionGridNo == -1)
		{
			// Try another location...
			sActionGridNo = FindAdjacentGridEx(s, usMapPos, &ubDirection, &sAdjustedGridNo, TRUE, FALSE);
			if (sActionGridNo == -1) return ITEM_HANDLE_CANNOT_GETTO_LOCATION;

			if (!gTacticalStatus.fAutoBandageMode) fHadToUseCursorPos = TRUE;
		}

		// Calculate AP costs...
		INT16 sAPCost = GetAPsToBeginFirstAid(s);
		sAPCost += PlotPath(s, sActionGridNo, NO_COPYROUTE, FALSE, s->usUIMovementMode, s->bActionPoints);
		if (!EnoughPoints(s, sAPCost, 0, fFromUI)) return ITEM_HANDLE_NOAPS;

		SetUIBusy(s);

		// CHECK IF WE ARE AT THIS GRIDNO NOW
		if (s->sGridNo != sActionGridNo)
		{
			// SEND PENDING ACTION
			s->ubPendingAction = MERC_GIVEAID;
			if      (fHadToUseCursorPos) s->sPendingActionData2 = usMapPos;
			else if (tgt != NULL)        s->sPendingActionData2 = tgt->sGridNo;
			else                         s->sPendingActionData2 = usGridNo;
			s->bPendingActionData3      = ubDirection;
			s->ubPendingActionAnimCount = 0;

			// WALK UP TO DEST FIRST
			EVENT_InternalGetNewSoldierPath(s, sActionGridNo, s->usUIMovementMode, FALSE, TRUE);
		}
		else
		{
			EVENT_SoldierBeginFirstAid(s, sAdjustedGridNo, ubDirection);
		}

		if (fFromUI) guiPendingOverrideEvent = A_CHANGE_TO_MOVE;
		return ITEM_HANDLE_OK;
	}

	if (usHandItem == WIRECUTTERS)
	{
		// See if we can get there to stab
		UINT8 ubDirection;
		INT16 sAdjustedGridNo;
		const INT16 sActionGridNo = FindAdjacentGridEx(s, usGridNo, &ubDirection, &sAdjustedGridNo, TRUE, FALSE);
		if (sActionGridNo == -1) return ITEM_HANDLE_CANNOT_GETTO_LOCATION;

		// Calculate AP costs...
		INT16 sAPCost = GetAPsToCutFence(s);
		sAPCost += PlotPath(s, sActionGridNo, NO_COPYROUTE, FALSE, s->usUIMovementMode, s->bActionPoints);
		if (!EnoughPoints(s, sAPCost, 0, fFromUI)) return ITEM_HANDLE_NOAPS;

		// CHECK IF WE ARE AT THIS GRIDNO NOW
		if (s->sGridNo != sActionGridNo)
		{
			// SEND PENDING ACTION
			s->ubPendingAction          = MERC_CUTFFENCE;
			s->sPendingActionData2      = sAdjustedGridNo;
			s->bPendingActionData3      = ubDirection;
			s->ubPendingActionAnimCount = 0;

			// WALK UP TO DEST FIRST
			EVENT_InternalGetNewSoldierPath(s, sActionGridNo, s->usUIMovementMode, FALSE, TRUE);
		}
		else
		{
			EVENT_SoldierBeginCutFence(s, sAdjustedGridNo, ubDirection);
		}

		SetUIBusy(s);
		if (fFromUI) guiPendingOverrideEvent = A_CHANGE_TO_MOVE;
		return ITEM_HANDLE_OK;
	}

	if (usHandItem == TOOLKIT)
	{
		// For repair, check if we are over a vehicle, then get gridnot to edge of that vehicle!
		BOOLEAN      fVehicle       = FALSE;
		INT16        sVehicleGridNo = -1;
		SOLDIERTYPE* t;
		if (IsRepairableStructAtGridNo(usGridNo, &t) == 2)
		{
			const INT16 sNewGridNo = FindGridNoFromSweetSpotWithStructDataFromSoldier(s, s->usUIMovementMode, 5, 0, t);
			if (sNewGridNo != NOWHERE)
			{
				usGridNo       = sNewGridNo;
				sVehicleGridNo = t->sGridNo;
				fVehicle       = TRUE;
			}
		}

		// See if we can get there to stab
		UINT8 ubDirection;
		INT16 sAdjustedGridNo;
		const INT16 sActionGridNo = FindAdjacentGridEx(s, usGridNo, &ubDirection, &sAdjustedGridNo, TRUE, FALSE);
		if (sActionGridNo == -1) return ITEM_HANDLE_CANNOT_GETTO_LOCATION;

		// Calculate AP costs...
		INT16 sAPCost = GetAPsToBeginRepair(s);
		sAPCost += PlotPath(s, sActionGridNo, NO_COPYROUTE, FALSE, s->usUIMovementMode, s->bActionPoints);
		if (!EnoughPoints(s, sAPCost, 0, fFromUI)) return ITEM_HANDLE_NOAPS;

		// CHECK IF WE ARE AT THIS GRIDNO NOW
		if (s->sGridNo != sActionGridNo)
		{
			// SEND PENDING ACTION
			s->ubPendingAction          = MERC_REPAIR;
			s->sPendingActionData2      = fVehicle ? sVehicleGridNo : sAdjustedGridNo;
			s->bPendingActionData3      = ubDirection;
			s->ubPendingActionAnimCount = 0;

			// WALK UP TO DEST FIRST
			EVENT_InternalGetNewSoldierPath(s, sActionGridNo, s->usUIMovementMode, FALSE, TRUE);
		}
		else
		{
			EVENT_SoldierBeginRepair(s, sAdjustedGridNo, ubDirection);
		}

		SetUIBusy(s);
		if (fFromUI) guiPendingOverrideEvent = A_CHANGE_TO_MOVE;
		return ITEM_HANDLE_OK;
	}

	if (usHandItem == GAS_CAN)
	{
		// For refueling, check if we are over a vehicle, then get gridno to edge of that vehicle!
		INT16                    sVehicleGridNo = -1;
		const SOLDIERTYPE* const t              = GetRefuelableStructAtGridNo(usGridNo);
		if (t != NULL)
		{
			const INT16 sNewGridNo = FindGridNoFromSweetSpotWithStructDataFromSoldier(s, s->usUIMovementMode, 5, 0, t);
			if (sNewGridNo != NOWHERE)
			{
				usGridNo       = sNewGridNo;
				sVehicleGridNo = t->sGridNo;
			}
		}

		// See if we can get there to stab
		UINT8 ubDirection;
		INT16 sAdjustedGridNo;
		const INT16 sActionGridNo = FindAdjacentGridEx(s, usGridNo, &ubDirection, &sAdjustedGridNo, TRUE, FALSE);
		if (sActionGridNo == -1) return ITEM_HANDLE_CANNOT_GETTO_LOCATION;

		// Calculate AP costs...
		INT16 sAPCost = GetAPsToRefuelVehicle(s);
		sAPCost += PlotPath(s, sActionGridNo, NO_COPYROUTE, FALSE, s->usUIMovementMode, s->bActionPoints);
		if (!EnoughPoints(s, sAPCost, 0, fFromUI)) return ITEM_HANDLE_NOAPS;

		// CHECK IF WE ARE AT THIS GRIDNO NOW
		if (s->sGridNo != sActionGridNo)
		{
			// SEND PENDING ACTION
			s->ubPendingAction      = MERC_FUEL_VEHICLE;
			s->sPendingActionData2  = sAdjustedGridNo;
			s->sPendingActionData2  = sVehicleGridNo;
			s->bPendingActionData3  = ubDirection;
			s->ubPendingActionAnimCount = 0;

			// WALK UP TO DEST FIRST
			EVENT_InternalGetNewSoldierPath(s, sActionGridNo, s->usUIMovementMode, FALSE, TRUE);
		}
		else
		{
			EVENT_SoldierBeginRefuel(s, sAdjustedGridNo, ubDirection);
		}

		SetUIBusy(s);
		if (fFromUI) guiPendingOverrideEvent = A_CHANGE_TO_MOVE;
		return ITEM_HANDLE_OK;
	}

	if (usHandItem == JAR)
	{
		UINT8 ubDirection;
		INT16 sAdjustedGridNo;
		const INT16 sActionGridNo = FindAdjacentGridEx(s, usGridNo, &ubDirection, &sAdjustedGridNo, TRUE, FALSE);
		if (sActionGridNo == -1) return ITEM_HANDLE_CANNOT_GETTO_LOCATION;

		// Calculate AP costs...
		INT16 sAPCost = GetAPsToUseJar(s, sActionGridNo);
		sAPCost += PlotPath(s, sActionGridNo, NO_COPYROUTE, FALSE, s->usUIMovementMode, s->bActionPoints);
		if (!EnoughPoints(s, sAPCost, 0, fFromUI)) return ITEM_HANDLE_NOAPS;

		// CHECK IF WE ARE AT THIS GRIDNO NOW
		if (s->sGridNo != sActionGridNo)
		{
			// SEND PENDING ACTION
			s->ubPendingAction          = MERC_TAKEBLOOD;
			s->sPendingActionData2      = sAdjustedGridNo;
			s->bPendingActionData3      = ubDirection;
			s->ubPendingActionAnimCount = 0;

			// WALK UP TO DEST FIRST
			EVENT_InternalGetNewSoldierPath(s, sActionGridNo, s->usUIMovementMode, FALSE, TRUE);
		}
		else
		{
			EVENT_SoldierBeginTakeBlood(s, sAdjustedGridNo, ubDirection);
		}

		SetUIBusy(s);
		if (fFromUI) guiPendingOverrideEvent = A_CHANGE_TO_MOVE;
		return ITEM_HANDLE_OK;
	}

	if (usHandItem == STRING_TIED_TO_TIN_CAN)
	{
		// Get structure info for in tile!
		STRUCTURE* pStructure;
		const LEVELNODE* const pIntTile = GetCurInteractiveTileGridNoAndStructure(&usGridNo, &pStructure);
		// We should not have null here if we are given this flag...
		if (pIntTile == NULL) return ITEM_HANDLE_CANNOT_GETTO_LOCATION;

		UINT8 ubDirection;
		INT16 sAdjustedGridNo;
		const INT16 sActionGridNo = FindAdjacentGridEx(s, usGridNo, &ubDirection, &sAdjustedGridNo, FALSE, TRUE);
		if (sActionGridNo == -1) return ITEM_HANDLE_CANNOT_GETTO_LOCATION;

		// Calculate AP costs...
		INT16 sAPCost = AP_ATTACH_CAN;
		sAPCost += PlotPath(s, sActionGridNo, NO_COPYROUTE, FALSE, s->usUIMovementMode, s->bActionPoints);
		if (!EnoughPoints(s, sAPCost, 0, fFromUI)) return ITEM_HANDLE_NOAPS;

		// CHECK IF WE ARE AT THIS GRIDNO NOW
		if (s->sGridNo != sActionGridNo)
		{
			// SEND PENDING ACTION
			s->ubPendingAction          = MERC_ATTACH_CAN;
			s->sPendingActionData2      = usGridNo;
			s->bPendingActionData3      = ubDirection;
			s->ubPendingActionAnimCount = 0;

			// WALK UP TO DEST FIRST
			EVENT_InternalGetNewSoldierPath(s, sActionGridNo, s->usUIMovementMode, FALSE, TRUE);
		}
		else
		{
			EVENT_SoldierBeginTakeBlood(s, usGridNo, ubDirection);
		}

		SetUIBusy(s);
		if (fFromUI) guiPendingOverrideEvent = A_CHANGE_TO_MOVE;
		return ITEM_HANDLE_OK;
	}

	// Check for remote detonator cursor....
	if (item->ubCursor == REMOTECURS)
	{
		const INT16 sAPCost = AP_USE_REMOTE;
		if (!EnoughPoints(s, sAPCost, 0, fFromUI)) return ITEM_HANDLE_NOAPS;

		DeductPoints(s, sAPCost, 0);
		if ( usHandItem == XRAY_DEVICE )
		{
			PlayLocationJA2Sample(s->sGridNo, USE_X_RAY_MACHINE, HIGHVOLUME, 1);
			ActivateXRayDevice(s);
		}
		else // detonator
		{
			// Save gridno....
			s->sPendingActionData2 = usGridNo;
			EVENT_SoldierBeginUseDetonator(s);
			if (fFromUI) guiPendingOverrideEvent = A_CHANGE_TO_MOVE;
		}
		return ITEM_HANDLE_OK;
	}

	BOOLEAN fDropBomb = FALSE;
	// Check for mine.. anything without a detonator.....
	if (item->ubCursor == BOMBCURS) fDropBomb = TRUE;

	// Check for a bomb like a mine, that uses a pressure detonator
	if (item->ubCursor == INVALIDCURS)
	{
		// Found detonator...
		const OBJECTTYPE* const obj = &s->inv[s->ubAttackingHand];
		if (FindAttachment(obj, DETONATOR)    != ITEM_NOT_FOUND ||
				FindAttachment(obj, REMDETONATOR) != ITEM_NOT_FOUND)
		{
			fDropBomb = TRUE;
		}
	}

	if (fDropBomb)
	{
		// Save gridno....
		s->sPendingActionData2 = usGridNo;

		if (s->sGridNo != usGridNo)
		{
			// SEND PENDING ACTION
			s->ubPendingAction          = MERC_DROPBOMB;
			s->ubPendingActionAnimCount = 0;

			// WALK UP TO DEST FIRST
			EVENT_InternalGetNewSoldierPath(s, usGridNo, s->usUIMovementMode, FALSE, TRUE);
		}
		else
		{
			EVENT_SoldierBeginDropBomb(s);
		}

		SetUIBusy(s);
		if (fFromUI) guiPendingOverrideEvent = A_CHANGE_TO_MOVE;
		return ITEM_HANDLE_OK;
	}

	//USING THE BLADE
	if (item->usItemClass == IC_BLADE)
	{
		UINT8 ubDirection;
		INT16 sAdjustedGridNo;
		INT16 sActionGridNo;
		// See if we can get there to stab
		if (s->ubBodyType == BLOODCAT)
		{
			sActionGridNo = FindNextToAdjacentGridEx(s, usGridNo, &ubDirection, &sAdjustedGridNo, TRUE, FALSE);
		}
		else if (CREATURE_OR_BLOODCAT(s) && PythSpacesAway(s->sGridNo, usGridNo) > 1)
		{
			sActionGridNo = FindNextToAdjacentGridEx(s, usGridNo, &ubDirection, &sAdjustedGridNo, TRUE, FALSE);
			if (sActionGridNo == -1)
			{
				sActionGridNo = FindAdjacentGridEx(s, usGridNo, &ubDirection, &sAdjustedGridNo, TRUE, FALSE);
			}
		}
		else
		{
			sActionGridNo = FindAdjacentGridEx(s, usGridNo, &ubDirection, &sAdjustedGridNo, TRUE, FALSE);
		}
		if (sActionGridNo == -1) return ITEM_HANDLE_CANNOT_GETTO_LOCATION;

		s->usActionData = sActionGridNo;

		// CHECK IF WE ARE AT THIS GRIDNO NOW
		if (s->sGridNo != sActionGridNo)
		{
			// SEND PENDING ACTION
			s->ubPendingAction          = MERC_KNIFEATTACK;
			s->sPendingActionData2      = sAdjustedGridNo;
			s->bPendingActionData3      = ubDirection;
			s->ubPendingActionAnimCount = 0;

			// WALK UP TO DEST FIRST
			EVENT_InternalGetNewSoldierPath(s, sActionGridNo, s->usUIMovementMode, FALSE, TRUE);
		}
		else
		{
			// for the benefit of the AI
			s->bAction = AI_ACTION_KNIFE_STAB;
			EVENT_SoldierBeginBladeAttack(s, sAdjustedGridNo, ubDirection);
		}

		SetUIBusy(s);

		if (fFromUI)
		{
			guiPendingOverrideEvent       = A_CHANGE_TO_MOVE;
			gfResetUIMovementOptimization = TRUE;
		}

		return ITEM_HANDLE_OK;
	}

	if (item->usItemClass == IC_TENTACLES)
	{
		gTacticalStatus.ubAttackBusyCount++;
		DebugMsg(TOPIC_JA2, DBG_LEVEL_3, String("!!!!!!! Starting swipe attack, incrementing a.b.c in HandleItems to %d", gTacticalStatus.ubAttackBusyCount));
		const INT16 sAPCost = CalcTotalAPsToAttack(s, sGridNo, FALSE, s->bAimTime);
		DeductPoints(s, sAPCost, 0);
		EVENT_InitNewSoldierAnim(s, QUEEN_SWIPE, 0, FALSE);
		s->bAction = AI_ACTION_KNIFE_STAB;
		return ITEM_HANDLE_OK;
	}

	// THIS IS IF WE WERE FROM THE UI
	if (item->usItemClass == IC_GRENADE || item->usItemClass == IC_LAUNCHER || item->usItemClass == IC_THROWN)
	{
		// Get gridno - either soldier's position or the gridno
		const INT16 sTargetGridNo = (tgt != NULL ? tgt->sGridNo : usGridNo);
		const INT16 sAPCost       = MinAPsToAttack(s, sTargetGridNo, TRUE);

		// Check if these is room to place mortar!
		if (usHandItem == MORTAR)
		{
			const UINT8 ubDirection  = (UINT8)GetDirectionFromGridNo(sTargetGridNo, s);
			const INT16 sCheckGridNo = NewGridNo((UINT16)s->sGridNo, (UINT16)DirectionInc(ubDirection));
			if (!OKFallDirection(s, sCheckGridNo, s->bLevel, ubDirection, s->usAnimState))
			{
				return ITEM_HANDLE_NOROOM;
			}

			s->fDontChargeAPsForStanceChange = TRUE;
		}
		else if (usHandItem == GLAUNCHER || usHandItem == UNDER_GLAUNCHER)
		{
			BOOLEAN fAddingTurningCost  = FALSE;
			BOOLEAN fAddingRaiseGunCost = FALSE;
			GetAPChargeForShootOrStabWRTGunRaises(s, sTargetGridNo, TRUE, &fAddingTurningCost, &fAddingRaiseGunCost);

			// If we are standing and are asked to turn AND raise gun, ignore raise gun...
			if (gAnimControl[s->usAnimState].ubHeight == ANIM_STAND)
			{
				if (fAddingRaiseGunCost) s->fDontChargeTurningAPs = TRUE;
			}
			else
			{
				// If raising gun, don't charge turning!
				if (fAddingTurningCost) s->fDontChargeReadyAPs = TRUE;
			}
		}

		// If this is a player guy, show message about no APS
		if (!EnoughPoints(s, sAPCost, 0, fFromUI)) return ITEM_HANDLE_NOAPS;

		s->ubAttackingHand   = HANDPOS;
		s->usAttackingWeapon = usHandItem;
		s->bTargetLevel      = bLevel;

		// Look at the cursor, if toss cursor...
		if (item->ubCursor == TOSSCURS)
		{
			s->sTargetGridNo = sTargetGridNo;
			s->target        = WhoIsThere2(sTargetGridNo, s->bTargetLevel);

			// Increment attack counter...
			gTacticalStatus.ubAttackBusyCount++;

			// ATE: Don't charge turning...
			s->fDontChargeTurningAPs = TRUE;

			FireWeapon(s, sTargetGridNo);
		}
		else
		{
			SendBeginFireWeaponEvent(s, sTargetGridNo);
		}

		SetUIBusy(s);
		return ITEM_HANDLE_OK;
	}

	// CHECK FOR BOMB....
	if (item->ubCursor == INVALIDCURS)
	{
		// Found detonator...
		OBJECTTYPE* const obj =  &s->inv[usHandItem];
		if ( FindAttachment(obj, DETONATOR) != ITEM_NOT_FOUND || FindAttachment(obj, REMDETONATOR))
		{
			StartBombMessageBox(s, usGridNo);
			if (fFromUI) guiPendingOverrideEvent = A_CHANGE_TO_MOVE;
			return ITEM_HANDLE_OK;
		}
	}

	return ITEM_HANDLE_OK;
}


void HandleSoldierDropBomb( SOLDIERTYPE *pSoldier, INT16 sGridNo )
{
	// Does this have detonator that needs info?
	if ( FindAttachment( &(pSoldier->inv[ HANDPOS ] ), DETONATOR) != ITEM_NOT_FOUND || FindAttachment( &(pSoldier->inv[ HANDPOS ] ), REMDETONATOR ) != ITEM_NOT_FOUND )
	{
		StartBombMessageBox( pSoldier, sGridNo );
	}
	else
	{
		// We have something... all we do is place...
		if ( ArmBomb( &(pSoldier->inv[ HANDPOS ]), 0 ) )
		{
			// EXPLOSIVES GAIN (25):  Place a bomb, or buried and armed a mine
			StatChange( pSoldier, EXPLODEAMT, 25, FALSE );

			pSoldier->inv[ HANDPOS ].bTrap = __min( 10, ( EffectiveExplosive( pSoldier ) / 20) + (EffectiveExpLevel( pSoldier ) / 3) );
			pSoldier->inv[ HANDPOS ].ubBombOwner = pSoldier->ubID + 2;

			// we now know there is something nasty here
			gpWorldLevelData[ sGridNo ].uiFlags |= MAPELEMENT_PLAYER_MINE_PRESENT;

			AddItemToPool( sGridNo, &(pSoldier->inv[ HANDPOS ] ), BURIED, pSoldier->bLevel, WORLD_ITEM_ARMED_BOMB, 0 );
			DeleteObj( &(pSoldier->inv[ HANDPOS ]) );
		}
	}
}

void HandleSoldierUseRemote( SOLDIERTYPE *pSoldier, INT16 sGridNo )
{
	StartBombMessageBox( pSoldier, sGridNo );
}

void SoldierHandleDropItem( SOLDIERTYPE *pSoldier )
{
	// LOOK IN PANDING DATA FOR ITEM TO DROP, AND LOCATION
	if ( pSoldier->pTempObject != NULL )
	{
		if ( pSoldier->bVisible != -1 )
		{
			PlayLocationJA2Sample(pSoldier->sGridNo, THROW_IMPACT_2, MIDVOLUME, 1);
		}

		AddItemToPool( pSoldier->sGridNo, pSoldier->pTempObject, 1, pSoldier->bLevel, 0 , -1 );
		NotifySoldiersToLookforItems( );

		MemFree( pSoldier->pTempObject );
		pSoldier->pTempObject = NULL;
	}
}


void HandleSoldierThrowItem( SOLDIERTYPE *pSoldier, INT16 sGridNo )
{
	// Determine what to do
	UINT8 ubDirection;

	// Set attacker to NOBODY, since it's not a combat attack
	pSoldier->target = NULL;

	// Alrighty, switch based on stance!
	switch( gAnimControl[ pSoldier->usAnimState ].ubHeight )
	{
		case ANIM_STAND:

			// CHECK IF WE ARE NOT ON THE SAME GRIDNO
			if ( sGridNo == pSoldier->sGridNo )
			{
				PickDropItemAnimation( pSoldier );
			}
			else
			{
				// CHANGE DIRECTION AT LEAST
				ubDirection = (UINT8)GetDirectionFromGridNo( sGridNo, pSoldier );

				SoldierGotoStationaryStance( pSoldier );

				EVENT_SetSoldierDesiredDirection( pSoldier, ubDirection );
				pSoldier->fTurningUntilDone = TRUE;

				// Draw item depending on distance from buddy
				if ( GetRangeFromGridNoDiff( sGridNo, pSoldier->sGridNo ) < MIN_LOB_RANGE )
				{
					pSoldier->usPendingAnimation = LOB_ITEM;
				}
				else
				{
					pSoldier->usPendingAnimation = THROW_ITEM;
				}

			}
			break;

		case ANIM_CROUCH:
		case ANIM_PRONE:

			// CHECK IF WE ARE NOT ON THE SAME GRIDNO
			if ( sGridNo == pSoldier->sGridNo )
			{
				// OK, JUST DROP ITEM!
				if ( pSoldier->pTempObject != NULL )
				{
					AddItemToPool( sGridNo, pSoldier->pTempObject, 1, pSoldier->bLevel, 0, -1 );
					NotifySoldiersToLookforItems( );

					MemFree( pSoldier->pTempObject );
					pSoldier->pTempObject = NULL;
				}
			}
			else
			{
				// OK, go from prone/crouch to stand first!
				ubDirection = (UINT8)GetDirectionFromGridNo( sGridNo, pSoldier );
				EVENT_SetSoldierDesiredDirectionForward(pSoldier, ubDirection);

				ChangeSoldierState( pSoldier, THROW_ITEM, 0 , FALSE );
			}
	}

}


void SoldierGiveItem( SOLDIERTYPE *pSoldier, SOLDIERTYPE *pTargetSoldier, OBJECTTYPE *pObject, INT8 bInvPos )
{
	INT16 sActionGridNo, sAdjustedGridNo;
	UINT8	ubDirection;

	 // Remove any previous actions
	 pSoldier->ubPendingAction		 = NO_PENDING_ACTION;

	 // See if we can get there to stab
	 sActionGridNo =  FindAdjacentGridEx( pSoldier, pTargetSoldier->sGridNo, &ubDirection, &sAdjustedGridNo, TRUE, FALSE );
	 if ( sActionGridNo != -1 )
	 {
			// SEND PENDING ACTION
			pSoldier->ubPendingAction = MERC_GIVEITEM;

			pSoldier->bPendingActionData5 = bInvPos;
			// Copy temp object
			pSoldier->pTempObject	= MALLOC(OBJECTTYPE);
			*pSoldier->pTempObject = *pObject;


			pSoldier->sPendingActionData2  = pTargetSoldier->sGridNo;
			pSoldier->bPendingActionData3  = ubDirection;
			pSoldier->uiPendingActionData4 = pTargetSoldier->ubID;
			pSoldier->ubPendingActionAnimCount = 0;

			// Set soldier as engaged!
			pSoldier->uiStatusFlags |= SOLDIER_ENGAGEDINACTION;

			// CHECK IF WE ARE AT THIS GRIDNO NOW
			if ( pSoldier->sGridNo != sActionGridNo )
			{
				// WALK UP TO DEST FIRST
				EVENT_InternalGetNewSoldierPath( pSoldier, sActionGridNo, pSoldier->usUIMovementMode, FALSE, TRUE );
			}
			else
			{
				EVENT_SoldierBeginGiveItem( pSoldier );
			  // CHANGE DIRECTION OF TARGET TO OPPOSIDE DIRECTION!
			  EVENT_SetSoldierDesiredDirection( pSoldier, ubDirection );
			}

			// Set target as engaged!
			pTargetSoldier->uiStatusFlags |= SOLDIER_ENGAGEDINACTION;
	 }
}

BOOLEAN SoldierDropItem( SOLDIERTYPE * pSoldier, OBJECTTYPE * pObj )
{
	pSoldier->pTempObject = MALLOC(OBJECTTYPE);
	if (pSoldier->pTempObject == NULL)
	{
		// OUT OF MEMORY! YIKES!
		return( FALSE );
	}
	*pSoldier->pTempObject = *pObj;
	PickDropItemAnimation( pSoldier );
	return( TRUE );
}

void SoldierPickupItem( SOLDIERTYPE *pSoldier, INT32 iItemIndex, INT16 sGridNo, INT8 bZLevel )
{
	INT16							sActionGridNo;

	// Remove any previous actions
	pSoldier->ubPendingAction		 = NO_PENDING_ACTION;

	sActionGridNo = AdjustGridNoForItemPlacement( pSoldier, sGridNo );

	// SET PENDING ACTIONS!
	pSoldier->ubPendingAction = MERC_PICKUPITEM;
	pSoldier->uiPendingActionData1 = iItemIndex;
	pSoldier->sPendingActionData2  = sActionGridNo;
	pSoldier->uiPendingActionData4 = sGridNo;
	pSoldier->bPendingActionData3  = bZLevel;
	pSoldier->ubPendingActionAnimCount = 0;

	// Deduct points!
	//sAPCost = GetAPsToPickupItem( pSoldier, sGridNo );
	//DeductPoints( pSoldier, sAPCost, 0 );
	SetUIBusy(pSoldier);

	// CHECK IF NOT AT SAME GRIDNO
	if ( pSoldier->sGridNo != sActionGridNo )
	{
		if ( pSoldier->bTeam == gbPlayerNum )
		{
			EVENT_InternalGetNewSoldierPath( pSoldier, sActionGridNo, pSoldier->usUIMovementMode, TRUE, TRUE );

			// Say it only if we don;t have to go too far!
			if ( pSoldier->usPathDataSize > 5 )
			{
				DoMercBattleSound(  pSoldier, BATTLE_SOUND_OK1 );
			}
		}
		else
		{
			EVENT_InternalGetNewSoldierPath( pSoldier, sActionGridNo, pSoldier->usUIMovementMode, FALSE, TRUE );
		}
	}
	else
	{
		// DO ANIMATION OF PICKUP NOW!
		PickPickupAnimation( pSoldier, pSoldier->uiPendingActionData1, (INT16)( pSoldier->uiPendingActionData4 ), pSoldier->bPendingActionData3 );
	}
}


static void HandleAutoPlaceFail(SOLDIERTYPE* const pSoldier, OBJECTTYPE* const o, const INT16 sGridNo)
{
	if (pSoldier->bTeam == gbPlayerNum)
	{
		// Place it in buddy's hand!
		if ( gpItemPointer == NULL )
		{
			InternalBeginItemPointer(pSoldier, o, NO_SLOT);
		}
		else
		{
			// Add back to world...
			AddItemToPool(sGridNo, o, 1 , pSoldier->bLevel, 0, -1);

			// If we are a merc, say DAMN quote....
			if ( pSoldier->bTeam == gbPlayerNum )
			{
				DoMercBattleSound( pSoldier, BATTLE_SOUND_CURSE1 );
			}
		}
	}
}


static void CheckForPickedOwnership(void);
static BOOLEAN ContinuePastBoobyTrap(SOLDIERTYPE* pSoldier, INT16 sGridNo, INT32 iItemIndex, BOOLEAN fInStrategic, BOOLEAN* pfSaidQuote);
static BOOLEAN ItemExistsAtLocation(INT16 sGridNo, INT32 iItemIndex, UINT8 ubLevel);
static BOOLEAN ItemPoolOKForPickup(SOLDIERTYPE* pSoldier, const ITEM_POOL* pItemPool, INT8 bZLevel);
static BOOLEAN LookForHiddenItems(INT16 sGridNo, INT8 ubLevel, BOOLEAN fSetLocator);
static void SwitchMessageBoxCallBack(UINT8 ubExitValue);


void SoldierGetItemFromWorld(SOLDIERTYPE* const s, const INT32 iItemIndex, const INT16 sGridNo, const INT8 bZLevel, const BOOLEAN* const pfSelectionList)
{
	BOOLEAN fShouldSayCoolQuote = FALSE;
  BOOLEAN fSaidBoobyTrapQuote = FALSE;
	// OK. CHECK IF WE ARE DOING ALL IN THIS POOL....
	if (iItemIndex == ITEM_PICKUP_ACTION_ALL || iItemIndex == ITEM_PICKUP_SELECTION)
	{
		const ITEM_POOL* pItemPoolToDelete = NULL;
		INT32            cnt               = 0;
		const ITEM_POOL* next;
		for (const ITEM_POOL* i = GetItemPool(sGridNo, s->bLevel); i != NULL; i = next)
		{
			next = i->pNext;

			if (!ItemPoolOKForPickup(s, i, bZLevel)) continue;

			if (iItemIndex == ITEM_PICKUP_SELECTION && !pfSelectionList[cnt++]) continue;

			if (!ContinuePastBoobyTrap(s, sGridNo, i->iItemIndex, FALSE, &fSaidBoobyTrapQuote))
			{
				break; // boobytrap found... stop picking up things!
			}

			WORLDITEM*  const wi = GetWorldItem(i->iItemIndex);
			OBJECTTYPE* const o  = &wi->o;

			if (ItemIsCool(o)) fShouldSayCoolQuote = TRUE;

			if (o->usItem == SWITCH)
			{
				// ask about activating the switch!
				bTempFrequency = o->bFrequency;
				gpTempSoldier = s;
				DoMessageBox(MSG_BOX_BASIC_STYLE, TacticalStr[ACTIVATE_SWITCH_PROMPT], GAME_SCREEN, MSG_BOX_FLAG_YESNO, SwitchMessageBoxCallBack, NULL);
				continue;
			}

			// Make copy of item
			OBJECTTYPE Object = *o;
			if (!AutoPlaceObject(s, &Object, TRUE))
			{
				// check to see if the object has been swapped with one in inventory
				if (Object.usItem != o->usItem || Object.ubNumberOfObjects != o->ubNumberOfObjects)
				{
					// copy back because item changed, and we must make sure the item pool reflects this.
					*o = Object;
				}

				pItemPoolToDelete = i;
				continue; // try to place any others
			}

			RemoveItemFromPool(wi);
		}

		// ATE; If here, and we failed to add any more stuff, put failed one in our cursor...
		if (pItemPoolToDelete != NULL)
		{
			gfDontChargeAPsToPickup = TRUE;
			WORLDITEM* const wi = GetWorldItem(pItemPoolToDelete->iItemIndex);
			HandleAutoPlaceFail(s, &wi->o, sGridNo);
			RemoveItemFromPool(wi);
		}
	}
	else
	{
		// REMOVE ITEM FROM POOL
		if (ItemExistsAtLocation(sGridNo, iItemIndex, s->bLevel) &&
				ContinuePastBoobyTrap(s, sGridNo, iItemIndex, FALSE, &fSaidBoobyTrapQuote))
		{
			WORLDITEM*  const wi = GetWorldItem(iItemIndex);
			OBJECTTYPE* const o  = &wi->o;

			if (ItemIsCool(o)) fShouldSayCoolQuote = TRUE;

			if (o->usItem == SWITCH)
			{
				// handle switch
				bTempFrequency = o->bFrequency;
				gpTempSoldier  = s;
				DoMessageBox(MSG_BOX_BASIC_STYLE, TacticalStr[ACTIVATE_SWITCH_PROMPT], GAME_SCREEN, MSG_BOX_FLAG_YESNO, SwitchMessageBoxCallBack, NULL);
			}
			else
			{
				RemoveItemFromPool(wi);

				if (!AutoPlaceObject(s, o, TRUE))
				{
					gfDontChargeAPsToPickup = TRUE;
					HandleAutoPlaceFail(s, o, sGridNo);
				}
			}
		}
	}

	// OK, check if potentially a good candidate for cool quote
	if (s->bTeam == OUR_TEAM)
	{
		if (fShouldSayCoolQuote                                                     &&
				QuoteExp_GotGunOrUsedGun[s->ubProfile] == QUOTE_FOUND_SOMETHING_SPECIAL && // Do we have this quote..?
				!(s->usQuoteSaidFlags & SOLDIER_QUOTE_SAID_FOUND_SOMETHING_NICE))          // Have we not said it today?
		{
			s->usQuoteSaidFlags |= SOLDIER_QUOTE_SAID_FOUND_SOMETHING_NICE;
			TacticalCharacterDialogue(s, QUOTE_FOUND_SOMETHING_SPECIAL);
		}
		else if (!fSaidBoobyTrapQuote)
		{
			DoMercBattleSound(s, BATTLE_SOUND_GOTIT);
		}

		// OK partner......look for any hidden items!
		if (LookForHiddenItems(sGridNo, s->bLevel, TRUE))
		{
			// WISDOM GAIN (5):  Found a hidden object
			StatChange(s, WISDOMAMT, 5, FALSE);

			// We've found something!
			TacticalCharacterDialogue(s, QUOTE_SPOTTED_SOMETHING_ONE + Random(2));
		}
	}

	gpTempSoldier = s;
	gsTempGridno  = sGridNo;
	SetCustomizableTimerCallbackAndDelay(1000, CheckForPickedOwnership, TRUE);
}


static void BoobyTrapMessageBoxCallBack(UINT8 ubExitValue);
static BOOLEAN DoesItemPoolContainAllHiddenItems(const ITEM_POOL* pItemPool);
static ITEM_POOL* GetItemPoolForIndex(INT16 sGridNo, INT32 iItemIndex, UINT8 ubLevel);
static INT16 GetNumOkForDisplayItemsInPool(const ITEM_POOL* pItemPool, INT8 bZLevel);


void HandleSoldierPickupItem( SOLDIERTYPE *pSoldier, INT32 iItemIndex, INT16 sGridNo, INT8 bZLevel )
{
	UINT16				usNum;

	// Draw menu if more than one item!
	ITEM_POOL* pItemPool = GetItemPool(sGridNo, pSoldier->bLevel);
	if (pItemPool != NULL)
	{
		// OK, if an enemy, go directly ( skip menu )
		if ( pSoldier->bTeam != gbPlayerNum )
		{
			SoldierGetItemFromWorld( pSoldier, iItemIndex, sGridNo, bZLevel, NULL );
		}
		else
		{
			if (gpWorldLevelData[ sGridNo ].uiFlags & MAPELEMENT_PLAYER_MINE_PRESENT)
			{
				// have the computer ask us if we want to proceed

				// override the item index passed in with the one for the bomb in this
				// tile
				iItemIndex = FindWorldItemForBombInGridNo( sGridNo, pSoldier->bLevel );
#ifdef JA2TESTVERSION
				if (iItemIndex == -1)
				{
					// WTF????
					ScreenMsg( FONT_MCOLOR_LTYELLOW, MSG_ERROR, L"Cannot find bomb item in gridno %d", sGridNo );
					return;
				}
#endif

				gpBoobyTrapItemPool = GetItemPoolForIndex( sGridNo, iItemIndex, pSoldier->bLevel );
				gpBoobyTrapSoldier = pSoldier;
				gsBoobyTrapGridNo = sGridNo;
				gbBoobyTrapLevel  = pSoldier->bLevel;
				gfDisarmingBuriedBomb = TRUE;
				gbTrapDifficulty = GetWorldItem(iItemIndex)->o.bTrap;

				DoMessageBox( MSG_BOX_BASIC_STYLE, TacticalStr[ DISARM_TRAP_PROMPT ], GAME_SCREEN, ( UINT8 )MSG_BOX_FLAG_YESNO, BoobyTrapMessageBoxCallBack, NULL );
			}
			else
			{
				// OK, only hidden items exist...
				if ( pSoldier->bTeam == gbPlayerNum && DoesItemPoolContainAllHiddenItems( pItemPool ) )
				{
					// He's touched them....
					if (LookForHiddenItems(sGridNo, pSoldier->bLevel, TRUE))
					{
						// WISDOM GAIN (5):  Found a hidden object
						StatChange( pSoldier, WISDOMAMT, 5, FALSE );

						// We've found something!
						TacticalCharacterDialogue( pSoldier, (UINT16)( QUOTE_SPOTTED_SOMETHING_ONE + Random( 2 ) ) );
					}
					else
					{
						// Say NOTHING quote...
						DoMercBattleSound( pSoldier, BATTLE_SOUND_NOTHING );
					}
				}
				else
				{
					// If only one good item exists....
					if ( ( usNum = GetNumOkForDisplayItemsInPool( pItemPool, bZLevel ) ) == 1 )
					{
						// Find first OK item....
						while( !ItemPoolOKForDisplay( pItemPool, bZLevel  ) )
						{
							pItemPool = pItemPool->pNext;
						}
						SoldierGetItemFromWorld( pSoldier, pItemPool->iItemIndex, sGridNo, bZLevel, NULL );
					}
					else
					{
						if ( usNum != 0 )
						{
							// Freeze guy!
							pSoldier->fPauseAllAnimation = TRUE;

							InitializeItemPickupMenu( pSoldier, sGridNo, pItemPool, 0, 0, bZLevel );

							guiPendingOverrideEvent = G_GETTINGITEM;
						}
						else
						{
							DoMercBattleSound( pSoldier, BATTLE_SOUND_NOTHING );
						}
					}
				}
			}
		}
	}
	else
	{
		// Say NOTHING quote...
		DoMercBattleSound( pSoldier, BATTLE_SOUND_NOTHING );
	}

}


static LEVELNODE* AddItemGraphicToWorld(const INVTYPE* const pItem, const INT16 sGridNo, const UINT8 ubLevel)
{
	UINT16			usTileIndex;
	LEVELNODE		*pNode;

	usTileIndex = GetTileGraphicForItem( pItem );

	// OK, Do stuff differently base on level!
	if ( ubLevel == 0 )
	{
		pNode = AddStructToTail(sGridNo, usTileIndex);
		//SET FLAG FOR AN ITEM
	}
	else
	{
		pNode = AddOnRoofToHead(sGridNo, usTileIndex);
		//SET FLAG FOR AN ITEM
	}
	pNode->uiFlags |= LEVELNODE_ITEM;

	// DIRTY INTERFACE
	fInterfacePanelDirty = DIRTYLEVEL2;

	// DIRTY TILE
	gpWorldLevelData[ sGridNo ].uiFlags |= MAPELEMENT_REDRAW;
	SetRenderFlags(RENDER_FLAG_MARKED);

	return( pNode );
}


static void RemoveItemGraphicFromWorld(INT16 sGridNo, UINT8 ubLevel, LEVELNODE* pLevelNode)
{
	LEVELNODE *pNode;

	// OK, Do stuff differently base on level!
	// Loop through and find pointer....
	if ( ubLevel == 0 )
	{
		pNode = gpWorldLevelData[ sGridNo ].pStructHead;
	}
	else
	{
		pNode = gpWorldLevelData[ sGridNo ].pOnRoofHead;
	}

	while( pNode != NULL )
	{
		if ( pNode == pLevelNode )
		{
			// Found one!
			if ( ubLevel == 0 )
			{
				RemoveStructFromLevelNode( sGridNo, pNode );
			}
			else
			{
				RemoveOnRoofFromLevelNode( sGridNo, pNode );
			}

			break;
		}

		pNode = pNode->pNext;
	}

	// DIRTY INTERFACE
	fInterfacePanelDirty = DIRTYLEVEL2;

	// DIRTY TILE
	gpWorldLevelData[ sGridNo ].uiFlags |= MAPELEMENT_REDRAW;
	SetRenderFlags(RENDER_FLAG_MARKED);

	//TEMP RENDER FULL!!!
	SetRenderFlags(RENDER_FLAG_FULL);
}


INT32 AddItemToPool(INT16 sGridNo, OBJECTTYPE* const pObject, const INT8 bVisible, const UINT8 ubLevel, const UINT16 usFlags, const INT8 bRenderZHeightAboveLevel)
{
	return InternalAddItemToPool(&sGridNo, pObject, bVisible, ubLevel, usFlags, bRenderZHeightAboveLevel);
}


static void AdjustItemPoolVisibility(ITEM_POOL* pItemPool);


INT32 InternalAddItemToPool(INT16* const psGridNo, OBJECTTYPE* const pObject, INT8 bVisible, UINT8 ubLevel, UINT16 usFlags, INT8 bRenderZHeightAboveLevel)
{
	Assert(pObject->ubNumberOfObjects <= MAX_OBJECTS_PER_SLOT);

	// ATE: Check if the gridno is OK
	if (*psGridNo == NOWHERE)
	{
		// Display warning.....
		ScreenMsg(FONT_MCOLOR_LTYELLOW, MSG_BETAVERSION, L"Error: Item %d was given invalid grid location %d for item pool. Please Report.", pObject->usItem, *psGridNo);
    *psGridNo = gMapInformation.sCenterGridNo;
		//return -1;
	}
	INT16 sNewGridNo = *psGridNo;

	/* if location is in water and item sinks, do not add */
	switch (GetTerrainType(sNewGridNo))
	{
		case DEEP_WATER:
		case LOW_WATER:
		case MED_WATER:
			if (Item[pObject->usItem].fFlags & ITEM_SINKS) return -1;
			break;
	}

	// First things first - look at where we are to place the items, and
	// set some flags appropriately

	// On a structure?
	//Locations on roofs without a roof is not possible, so
	//we convert the onroof intention to ground.
	if (ubLevel && !FlatRoofAboveGridNo(sNewGridNo)) ubLevel = 0;

	BOOLEAN fForceOnGround;
	if (bRenderZHeightAboveLevel == -1)
	{
		fForceOnGround = TRUE;
		bRenderZHeightAboveLevel = 0;
	}
	else
	{
		fForceOnGround = FALSE;
	}

	// Check structure database
	BOOLEAN fObjectInOpenable = FALSE;
	if (gpWorldLevelData[sNewGridNo].pStructureHead && pObject->usItem != OWNERSHIP && pObject->usItem != ACTION_ITEM)
	{
		// Something is here, check obstruction in future
		const INT16 sDesiredLevel = ubLevel ? STRUCTURE_ON_ROOF : STRUCTURE_ON_GROUND;
		for (STRUCTURE* pStructure = FindStructure(sNewGridNo, STRUCTURE_BLOCKSMOVES); pStructure; pStructure = FindNextStructure(pStructure, STRUCTURE_BLOCKSMOVES))
		{
			if (pStructure->fFlags & (STRUCTURE_PERSON | STRUCTURE_CORPSE)) continue;
			if (pStructure->sCubeOffset != sDesiredLevel) continue;

			// If we are going into a raised struct AND we have above level set to -1
			if (StructureBottomLevel(pStructure) != 1 && fForceOnGround) break;

			// Adjust the item's gridno to the base of struct.....
			const STRUCTURE* const pBase = FindBaseStructure(pStructure);

			// Get LEVELNODE for struct and remove!
			sNewGridNo = pBase->sGridNo;

			// Check for openable flag....
			if (pStructure->fFlags & STRUCTURE_OPENABLE)
			{
				// ATE: Set a flag here - we need to know later that we're in an openable...
				fObjectInOpenable = TRUE;

				// Something of note is here....
				// SOME sort of structure is here.... set render flag to off
				usFlags |= WORLD_ITEM_DONTRENDER;

				// Openable.. check if it's closed, if so, set visiblity...
				if (!(pStructure->fFlags & STRUCTURE_OPEN))
				{
					// -2 means - don't reveal!
					bVisible = -2;
				}

				bRenderZHeightAboveLevel = CONVERT_INDEX_TO_PIXELS(StructureHeight(pStructure));
				break;
			}
			else if (pStructure->fFlags & STRUCTURE_GENERIC) // can we place an item on top?
			{
				// If we are going into a raised struct AND we have above level set to -1
				if (StructureBottomLevel(pStructure) != 1 && fForceOnGround) break;

				// Find most dense area...
				UINT8 ubLevel0;
				UINT8 ubLevel1;
				UINT8 ubLevel2;
				UINT8 ubLevel3;
				if (StructureDensity(pStructure, &ubLevel0, &ubLevel1, &ubLevel2, &ubLevel3))
				{
					bRenderZHeightAboveLevel = 0;
					UINT8 max = 0;
					if (ubLevel3 > max) { max = ubLevel3; bRenderZHeightAboveLevel = CONVERT_INDEX_TO_PIXELS(4); }
					if (ubLevel2 > max) { max = ubLevel2; bRenderZHeightAboveLevel = CONVERT_INDEX_TO_PIXELS(3); }
					if (ubLevel1 > max) { max = ubLevel1; bRenderZHeightAboveLevel = CONVERT_INDEX_TO_PIXELS(2); }
					if (ubLevel0 > max) { max = ubLevel0; bRenderZHeightAboveLevel = CONVERT_INDEX_TO_PIXELS(1); }
				}

				// Set flag indicating it has an item on top!
				pStructure->fFlags |= STRUCTURE_HASITEMONTOP;
				break;
			}
		}
	}

	if (pObject->usItem == SWITCH && !fObjectInOpenable)
	{
		if (bVisible != -2)
		{
			// switch items which are not hidden inside objects should be considered buried
			bVisible = BURIED;
			// and they are pressure-triggered unless there is a switch structure there
			if (FindStructure(*psGridNo, STRUCTURE_SWITCH) != NULL)
			{
				pObject->bDetonatorType = BOMB_SWITCH;
			}
			else
			{
				pObject->bDetonatorType = BOMB_PRESSURE;
			}
		}
		else
		{
			// else they are manually controlled
			pObject->bDetonatorType = BOMB_SWITCH;
		}
	}
	else if (pObject->usItem == ACTION_ITEM)
	{
		switch (pObject->bActionValue)
		{
			case ACTION_ITEM_SMALL_PIT:
			case ACTION_ITEM_LARGE_PIT:
				// mark as known about by civs and creatures
				gpWorldLevelData[sNewGridNo].uiFlags |= MAPELEMENT_ENEMY_MINE_PRESENT;
				break;

			default: break;
		}
	}

	*psGridNo = sNewGridNo;

	//First add the item to the global list.  This is so the game can keep track
	//of where the items are, for file i/o, etc.
	const INT32 iWorldItem = AddItemToWorld(sNewGridNo, pObject, ubLevel, usFlags, bRenderZHeightAboveLevel, bVisible);

	// Check for and existing pool on the object layer

	ITEM_POOL* const new_item = MALLOC(ITEM_POOL);
	new_item->pNext      = NULL;
	new_item->iItemIndex = iWorldItem;

	ITEM_POOL* item_pool = GetItemPool(sNewGridNo, ubLevel);

	LEVELNODE* const pNode = AddItemGraphicToWorld(&Item[pObject->usItem], sNewGridNo, ubLevel);
	new_item->pLevelNode = pNode;

	if (item_pool != NULL)
	{
		// Add to exitsing pool

		// Set pool head value in levelnode
		pNode->pItemPool = item_pool;

		// Get last item in list
		while (item_pool->pNext != NULL) item_pool = item_pool->pNext;

		// Set Next of previous
		item_pool->pNext = new_item;
	}
	else
	{
		pNode->pItemPool = new_item;

		// Set flag to indicate item pool presence
		gpWorldLevelData[sNewGridNo].uiFlags |= MAPELEMENT_ITEMPOOL_PRESENT;
	}

	// Set visible!
	new_item->bVisible = bVisible;

	// If bbisible is true, render makered world
	if (bVisible == 1 && GridNoOnScreen(sNewGridNo))
	{
		//gpWorldLevelData[sNewGridNo].uiFlags |= MAPELEMENT_REDRAW;
		//SetRenderFlags(RENDER_FLAG_MARKED);
		SetRenderFlags(RENDER_FLAG_FULL);
	}

	// Set flahs timer
	new_item->bFlashColor              = FALSE;
	new_item->sGridNo                  = sNewGridNo;
	new_item->bVisible                 = bVisible;

	// ATE: Get head of pool again....
	ITEM_POOL* const pool_head = GetItemPool(sNewGridNo, ubLevel);
	if (pool_head != NULL) AdjustItemPoolVisibility(pool_head);

	return iWorldItem;
}


static BOOLEAN ItemExistsAtLocation(INT16 sGridNo, INT32 iItemIndex, UINT8 ubLevel)
{
	// Check for an existing pool on the object layer
	const ITEM_POOL* pItemPool = GetItemPool(sGridNo, ubLevel);
	if (pItemPool != NULL)
	{
	// LOOP THROUGH LIST TO FIND NODE WE WANT
		const ITEM_POOL* pItemPoolTemp = pItemPool;
		while( pItemPoolTemp != NULL )
		{
			if ( pItemPoolTemp->iItemIndex == iItemIndex )
			{
				return( TRUE );
			}
			pItemPoolTemp = pItemPoolTemp->pNext;
		}

	}

	return( FALSE );
}

BOOLEAN ItemTypeExistsAtLocation( INT16 sGridNo, UINT16 usItem, UINT8 ubLevel, INT32 * piItemIndex )
{
	// Check for an existing pool on the object layer
	const ITEM_POOL* pItemPool = GetItemPool(sGridNo, ubLevel);
	if (pItemPool != NULL)
	{
	// LOOP THROUGH LIST TO FIND ITEM WE WANT
		const ITEM_POOL* pItemPoolTemp = pItemPool;
		while( pItemPoolTemp != NULL )
		{
			if (GetWorldItem(pItemPoolTemp->iItemIndex)->o.usItem == usItem)
			{
				if ( piItemIndex )
				{
					*piItemIndex = pItemPoolTemp->iItemIndex;
				}
				return( TRUE );
			}
			pItemPoolTemp = pItemPoolTemp->pNext;
		}

	}

	return( FALSE );
}


static INT32 GetItemOfClassTypeInPool(INT16 sGridNo, UINT32 uiItemClass, UINT8 ubLevel)
{

	// Check for an existing pool on the object layer
	const ITEM_POOL* pItemPool = GetItemPool(sGridNo, ubLevel);
	if (pItemPool != NULL)
	{
	// LOOP THROUGH LIST TO FIND NODE WE WANT
		const ITEM_POOL* pItemPoolTemp = pItemPool;
		while( pItemPoolTemp != NULL )
		{
			if (Item[GetWorldItem(pItemPoolTemp->iItemIndex)->o.usItem].usItemClass & uiItemClass)
			{
				return( pItemPoolTemp->iItemIndex );
			}
			pItemPoolTemp = pItemPoolTemp->pNext;
		}

	}

	return( -1 );
}


static ITEM_POOL* GetItemPoolForIndex(INT16 sGridNo, INT32 iItemIndex, UINT8 ubLevel)
{
	// Check for an existing pool on the object layer
	ITEM_POOL* pItemPool = GetItemPool(sGridNo, ubLevel);
	if (pItemPool != NULL)
	{
	// LOOP THROUGH LIST TO FIND NODE WE WANT
		ITEM_POOL* pItemPoolTemp = pItemPool;
		while( pItemPoolTemp != NULL )
		{
			if (pItemPoolTemp->iItemIndex == iItemIndex )
			{
				return( pItemPoolTemp );
			}
			pItemPoolTemp = pItemPoolTemp->pNext;
		}

	}

	return( NULL );
}


BOOLEAN DoesItemPoolContainAnyHiddenItems(const ITEM_POOL* pItemPool)
{
	// LOOP THROUGH LIST TO FIND NODE WE WANT
	while( pItemPool != NULL )
	{
		if (GetWorldItem(pItemPool->iItemIndex)->bVisible == HIDDEN_ITEM)
		{
			return( TRUE );
		}

		pItemPool = pItemPool->pNext;
	}

	return( FALSE );
}


static BOOLEAN DoesItemPoolContainAllHiddenItems(const ITEM_POOL* pItemPool)
{
	// LOOP THROUGH LIST TO FIND NODE WE WANT
	while( pItemPool != NULL )
	{
		if (GetWorldItem(pItemPool->iItemIndex)->bVisible != HIDDEN_ITEM)
		{
			return( FALSE );
		}

		pItemPool = pItemPool->pNext;
	}

	return( TRUE );
}


static BOOLEAN LookForHiddenItems(INT16 sGridNo, INT8 ubLevel, BOOLEAN fSetLocator)
{
	ITEM_POOL *pHeadItemPool = NULL;
	BOOLEAN		fFound = FALSE;

	ITEM_POOL* pItemPool = GetItemPool(sGridNo, ubLevel);
	if (pItemPool != NULL)
	{
		pHeadItemPool = pItemPool;

		// LOOP THROUGH LIST TO FIND NODE WE WANT
		while( pItemPool != NULL )
		{
			WORLDITEM* const wi = GetWorldItem(pItemPool->iItemIndex);
			if (wi->bVisible == HIDDEN_ITEM && wi->o.usItem != OWNERSHIP)
			{
				fFound = TRUE;
				wi->bVisible = INVISIBLE;
			}

			pItemPool = pItemPool->pNext;
		}
	}

	// If found, set item pool visibility...
	if ( fFound )
	{
		SetItemPoolVisibilityOn( pHeadItemPool, INVISIBLE, fSetLocator );
	}

	return( fFound );
}


INT8 GetZLevelOfItemPoolGivenStructure( INT16 sGridNo, UINT8 ubLevel, STRUCTURE *pStructure )
{
	if ( pStructure == NULL )
	{
		return( 0 );
	}

	// OK, check if this struct contains items....
	const ITEM_POOL* pItemPool = GetItemPool(sGridNo, ubLevel);
	if (pItemPool != NULL)
	{
		return( GetLargestZLevelOfItemPool( pItemPool ) );
	}
	return( 0 );
}


INT8 GetLargestZLevelOfItemPool(const ITEM_POOL* pItemPool)
{
	// OK, loop through pools and get any height != 0........
	while( pItemPool != NULL )
	{
		const WORLDITEM* const wi = GetWorldItem(pItemPool->iItemIndex);
		if (wi->bRenderZHeightAboveLevel > 0) return wi->bRenderZHeightAboveLevel;

		pItemPool = pItemPool->pNext;
	}

	return( 0 );
}


static void RemoveItemPool(INT16 sGridNo, UINT8 ubLevel)
{
	const ITEM_POOL* pItemPool;

	// Check for and existing pool on the object layer
	while ((pItemPool = GetItemPool(sGridNo, ubLevel)) != NULL)
	{
		RemoveItemFromPool(GetWorldItem(pItemPool->iItemIndex));
	}
}

void RemoveAllUnburiedItems( INT16 sGridNo, UINT8 ubLevel )
{
	// Check for and existing pool on the object layer
	const ITEM_POOL* pItemPool = GetItemPool(sGridNo, ubLevel);
	while( pItemPool )
	{
		WORLDITEM* const wi = GetWorldItem(pItemPool->iItemIndex);
		if (wi->bVisible == BURIED)
		{
			pItemPool = pItemPool->pNext;
		}
		else
		{
			RemoveItemFromPool(wi);
			// get new start pointer
			pItemPool = GetItemPool(sGridNo, ubLevel);
		}
	}
}


static void LoopLevelNodeForShowThroughFlag(LEVELNODE* pNode)
{
	for (; pNode != NULL; pNode = pNode->pNext)
	{
		if (!(pNode->uiFlags & LEVELNODE_ITEM)) continue;

		pNode->uiFlags |= LEVELNODE_SHOW_THROUGH;

		if (gGameSettings.fOptions[TOPTION_GLOW_ITEMS])
		{
			pNode->uiFlags |= LEVELNODE_DYNAMIC;
		}
	}
}


static void HandleItemObscuredFlag(INT16 sGridNo, UINT8 ubLevel)
{
	MAP_ELEMENT* const e = &gpWorldLevelData[sGridNo];
	LEVELNODE* const   n = (ubLevel == 0 ? e->pStructHead : e->pOnRoofHead);
	LoopLevelNodeForShowThroughFlag(n);
}


static void SetItemPoolLocator(ITEM_POOL* pItemPool, ITEM_POOL_LOCATOR_HOOK Callback);


BOOLEAN SetItemPoolVisibilityOn( ITEM_POOL *pItemPool, INT8 bAllGreaterThan, BOOLEAN fSetLocator )
{
	ITEM_POOL		*pItemPoolTemp;
	BOOLEAN			fAtLeastModified = FALSE, fDeleted = FALSE;

	pItemPoolTemp = pItemPool;
	while( pItemPoolTemp != NULL )
	{
		WORLDITEM* const wi            = GetWorldItem(pItemPoolTemp->iItemIndex);
		INT8       const bVisibleValue = wi->bVisible;

		// Update each item...
		if ( bVisibleValue != VISIBLE )
		{
			if (wi->o.usItem == ACTION_ITEM)
			{
				// NEVER MAKE VISIBLE!
				pItemPoolTemp = pItemPoolTemp->pNext;
				continue;
			}

			// If we have reached a visible value we should not modify, ignore...
			if (bVisibleValue >= bAllGreaterThan && wi->o.usItem != OWNERSHIP)
			{
				// Update the world value
				wi->bVisible = VISIBLE;

				fAtLeastModified = TRUE;
			}

			/*
			if (wi->o.usItem == ACTION_ITEM)
			{
				OBJECTTYPE* const pObj = &wi->o;
				switch( pObj->bActionValue )
				{
					case ACTION_ITEM_SMALL_PIT:
					case ACTION_ITEM_LARGE_PIT:
						if (pObj->bDetonatorType == 0)
						{
							// randomly set to active or destroy the item!
							if (Random( 100 ) < 65)
							{
								ArmBomb( pObj, 0 ); // will be set to pressure type so freq is irrelevant
								wi->usFlags |= WORLD_ITEM_ARMED_BOMB;
								AddBombToWorld( pItemPoolTemp->iItemIndex );
							}
							else
							{
								// get pointer to the next element NOW
								pItemPoolTemp	= pItemPoolTemp->pNext;
								// set flag so we don't traverse an additional time
								fDeleted = TRUE;
								// remove item from pool
								RemoveItemFromPool(wi);
							}
						}
						break;
					default:
						break;
				}
			}
			*/

			if (fDeleted)
			{
				// don't get the 'next' pointer because we did so above

				// reset fDeleted to false so we don't skip moving through the list more than once
				fDeleted = FALSE;
			}
			else
			{
				pItemPoolTemp						= pItemPoolTemp->pNext;
			}

		}
		else
		{
			pItemPoolTemp						= pItemPoolTemp->pNext;
		}
	}

	// If we didn;t find any that should be modified..
	if ( !fAtLeastModified )
	{
		return( FALSE );
	}


	// Update global pool bVisible to true ( if at least one is visible... )
	pItemPoolTemp = pItemPool;
	while( pItemPoolTemp != NULL )
	{
		pItemPoolTemp->bVisible = VISIBLE;

		pItemPoolTemp						= pItemPoolTemp->pNext;
	}

	// Handle obscured flag...
	const WORLDITEM* const wi = GetWorldItem(pItemPool->iItemIndex);
	HandleItemObscuredFlag(pItemPool->sGridNo, wi->ubLevel);

	if ( fSetLocator )
	{
		SetItemPoolLocator(pItemPool, NULL);
	}

	return( TRUE );
}


void SetItemPoolVisibilityHidden(ITEM_POOL* pItemPool)
{
	ITEM_POOL* pItemPoolTemp;

	pItemPoolTemp = pItemPool;
	while( pItemPoolTemp != NULL )
	{
		// Update the world value
		GetWorldItem(pItemPoolTemp->iItemIndex)->bVisible = HIDDEN_IN_OBJECT;
		pItemPoolTemp->bVisible = HIDDEN_IN_OBJECT;

		pItemPoolTemp						= pItemPoolTemp->pNext;
	}
}


// This determines the overall initial visibility of the pool...
// IF ANY are set to VISIBLE, MODIFY
static void AdjustItemPoolVisibility(ITEM_POOL* pItemPool)
{
	ITEM_POOL		*pItemPoolTemp;
	BOOLEAN			fAtLeastModified = FALSE;

	pItemPoolTemp = pItemPool;
	while( pItemPoolTemp != NULL )
	{
		// DEFAULT ITEM POOL TO INVISIBLE....
		pItemPoolTemp->bVisible = INVISIBLE;

		// Update each item...
		// If we have reached a visible value we should not modify, ignore...
		if (GetWorldItem(pItemPoolTemp->iItemIndex)->bVisible == VISIBLE)
		{
			fAtLeastModified = TRUE;
		}

		pItemPoolTemp						= pItemPoolTemp->pNext;
	}

	// Handle obscured flag...
	const WORLDITEM* const wi = GetWorldItem(pItemPool->iItemIndex);
	HandleItemObscuredFlag(pItemPool->sGridNo, wi->ubLevel);

	// If we didn;t find any that should be modified..
	if ( !fAtLeastModified )
	{
		return;
	}

	// Update global pool bVisible to true ( if at least one is visible... )
	pItemPoolTemp = pItemPool;
	while( pItemPoolTemp != NULL )
	{
		pItemPoolTemp->bVisible = VISIBLE;

		pItemPoolTemp						= pItemPoolTemp->pNext;
	}

	// Handle obscured flag...
	HandleItemObscuredFlag(pItemPool->sGridNo, wi->ubLevel);
}


void RemoveItemFromPool(WORLDITEM* const wi)
{
	ITEM_POOL* prev = NULL;
	ITEM_POOL* item = GetItemPool(wi->sGridNo, wi->ubLevel);
	for (;; prev = item, item = item->pNext)
	{
		// Could not find item? Maybe somebody got it before we got there!
		if (item == NULL) return;
		if (GetWorldItem(item->iItemIndex) == wi) break;
	}

	RemoveItemGraphicFromWorld(wi->sGridNo, wi->ubLevel, item->pLevelNode);

	if (item->bFlashColor != 0) RemoveFlashItemSlot(item);

	ITEM_POOL* const next = item->pNext;

	if (prev != NULL)
	{
		prev->pNext = next;
	}
	else if (next != NULL)
	{
		// This node was the head, set next as head at this gridno
		const MAP_ELEMENT* const m = &gpWorldLevelData[wi->sGridNo];
		for (LEVELNODE* l = (wi->ubLevel == 0 ? m->pStructHead : m->pOnRoofHead); l != NULL; l = l->pNext)
		{
			if (!(l->uiFlags & LEVELNODE_ITEM)) continue;
			l->pItemPool = next;
		}
	}
	else
	{
		// This was the last item in the pool
		gpWorldLevelData[wi->sGridNo].uiFlags &= ~MAPELEMENT_ITEMPOOL_PRESENT;

		/* If there is a structure with the has item on top flag set, reset it,
		 * because there are no more items here */
		if (wi->bRenderZHeightAboveLevel > 0)
		{
			STRUCTURE* const s = FindStructure(item->sGridNo, STRUCTURE_HASITEMONTOP);
			if (s != NULL)
			{
				s->fFlags &= ~STRUCTURE_HASITEMONTOP;
				// Re-adjust interactive tile...
				BeginCurInteractiveTileCheck();
			}
		}
	}

	AdjustItemPoolVisibility(item);
	RemoveItemFromWorld(item->iItemIndex);
	MemFree(item);
}


BOOLEAN MoveItemPools( INT16 sStartPos, INT16 sEndPos )
{
	// note, only works between locations on the ground

	// While there is an existing pool
	const ITEM_POOL* pItemPool;
	while ((pItemPool = GetItemPool(sStartPos, 0)) != NULL)
	{
		WORLDITEM* const wi            = GetWorldItem(pItemPool->iItemIndex);
		WORLDITEM        TempWorldItem = *wi;
		RemoveItemFromPool(wi);
		AddItemToPool( sEndPos, &(TempWorldItem.o), -1, TempWorldItem.ubLevel, TempWorldItem.usFlags, TempWorldItem.bRenderZHeightAboveLevel );
	}
	return( TRUE );
}

ITEM_POOL* GetItemPool(UINT16 usMapPos, UINT8 ubLevel)
{
	LEVELNODE *pObject;

	if ( ubLevel == 0 )
	{
		pObject = gpWorldLevelData[ usMapPos ].pStructHead;
	}
	else
	{
		pObject = gpWorldLevelData[ usMapPos ].pOnRoofHead;
	}

	// LOOP THORUGH OBJECT LAYER
	while( pObject != NULL )
	{
		if ( pObject->uiFlags & LEVELNODE_ITEM )
		{
			return pObject->pItemPool;
		}

		pObject = pObject->pNext;
	}

	return NULL;
}


void NotifySoldiersToLookforItems(void)
{
	FOR_ALL_MERCS(i) (*i)->uiStatusFlags |= SOLDIER_LOOKFOR_ITEMS;
}


void AllSoldiersLookforItems(void)
{
	FOR_ALL_MERCS(i) RevealRoofsAndItems(*i, TRUE);
}


static INT16 GetNumOkForDisplayItemsInPool(const ITEM_POOL* pItemPool, INT8 bZLevel)
{
	INT32						cnt;

	//Determine total #
	cnt = 0;
	while( pItemPool != NULL )
	{
		if ( ItemPoolOKForDisplay( pItemPool, bZLevel ) )
		{
			cnt++;
		}

		pItemPool = pItemPool->pNext;
	}

	return( (UINT16) cnt );
}


BOOLEAN AnyItemsVisibleOnLevel(const ITEM_POOL* pItemPool, INT8 bZLevel)
{
	if ( ( gTacticalStatus.uiFlags & SHOW_ALL_ITEMS ) )
	{
		return( TRUE );
	}

	//Determine total #
	while( pItemPool != NULL )
	{
		const WORLDITEM* const wi = GetWorldItem(pItemPool->iItemIndex);
		if (wi->bRenderZHeightAboveLevel == bZLevel && wi->bVisible == VISIBLE)
		{
			return( TRUE );
		}

		pItemPool = pItemPool->pNext;

	}

	return( FALSE );
}


BOOLEAN ItemPoolOKForDisplay(const ITEM_POOL* pItemPool, INT8 bZLevel)
{
	if (gTacticalStatus.uiFlags&SHOW_ALL_ITEMS)
	{
		return( TRUE );
	}

	const WORLDITEM* const wi = GetWorldItem(pItemPool->iItemIndex);
	// Setup some conditions!
	if (wi->bVisible != VISIBLE)
	{
		return( FALSE );
	}

	// If -1, it means find all
	if (wi->bRenderZHeightAboveLevel != bZLevel && bZLevel != -1)
		{
		return( FALSE );
	}

	return( TRUE );
}


static BOOLEAN ItemPoolOKForPickup(SOLDIERTYPE* pSoldier, const ITEM_POOL* pItemPool, INT8 bZLevel)
{
	if (gTacticalStatus.uiFlags&SHOW_ALL_ITEMS)
	{
		return( TRUE );
	}

	const WORLDITEM* const wi = GetWorldItem(pItemPool->iItemIndex);
	if ( pSoldier->bTeam == gbPlayerNum )
	{
		// Setup some conditions!
		if (wi->bVisible != VISIBLE)
		{
			return( FALSE );
		}
	}

	// If -1, it means find all
	if (wi->bRenderZHeightAboveLevel != bZLevel && bZLevel != -1)
		{
		return( FALSE );
	}

	return( TRUE );
}


void DrawItemPoolList(const ITEM_POOL* const pItemPool, const INT8 bZLevel, const INT16 sXPos, const INT16 sYPos)
{
	for (const ITEM_POOL* i = pItemPool; i != NULL; i = i->pNext)
	{
		if (!ItemPoolOKForDisplay(i, bZLevel)) continue;

		const WORLDITEM* const wi = GetWorldItem(i->iItemIndex);
		HandleAnyMercInSquadHasCompatibleStuff(&wi->o);
	}

	// Calculate maximum with of the item names and count the items to display
	INT16 max_w      = 0;
	INT   item_count = 0;
	for (const ITEM_POOL* i = pItemPool; i != NULL; i = i->pNext)
	{
		if (!ItemPoolOKForDisplay(i, bZLevel)) continue;

		if (item_count++ == NUM_ITEMS_LISTED)
		{
			const INT16 w = StringPixLength(TacticalStr[ITEMPOOL_POPUP_MORE_STR], SMALLFONT1);
			if (max_w < w) max_w = w;
			break;
		}

		const WORLDITEM* const wi  = GetWorldItem(i->iItemIndex);
		const wchar_t*         txt = ShortItemNames[wi->o.usItem];
		wchar_t                buf[100];
		if (wi->o.ubNumberOfObjects > 1)
		{
			swprintf(buf, lengthof(buf), L"%ls (%d)", txt, wi->o.ubNumberOfObjects);
			txt = buf;
		}

		const INT16 w = StringPixLength(txt, SMALLFONT1);
		if (max_w < w) max_w = w;
	}

	/* Put list to the right of the given coordinate, if there is space,
	 * otherwise to the left */
	INT16 const x = (sXPos + 15 + max_w <= SCREEN_WIDTH ? sXPos + 15 : sXPos - max_w);

	/* Try to center the list vertically relative to the given coordinate, but
	 * clamp to the view area */
	INT16 const dy = GetFontHeight(SMALLFONT1) - 2;
	INT16 const h  = dy * item_count;
	INT16       y;
	if (sYPos < h / 2)
	{
		y = 0;
	}
	else if (sYPos + h / 2 >= gsVIEWPORT_WINDOW_END_Y)
	{
		y = gsVIEWPORT_WINDOW_END_Y - h;
	}
	else
	{
		y = sYPos - h / 2;
	}

	SetFont(SMALLFONT1);
	SetFontBackground(FONT_MCOLOR_BLACK);
	SetFontForeground(FONT_MCOLOR_DKGRAY);

	// Draw the item names
	UINT display_count = 0;
	for (const ITEM_POOL* i = pItemPool; i != NULL; i = i->pNext)
	{
		if (!ItemPoolOKForDisplay(i, bZLevel)) continue;

		if (display_count++ == NUM_ITEMS_LISTED)
		{
			const wchar_t* const more = TacticalStr[ITEMPOOL_POPUP_MORE_STR];
			gprintfdirty(x, y, more);
			mprintf(     x, y, more);
			break;
		}

		const WORLDITEM* const wi  = GetWorldItem(i->iItemIndex);
		const wchar_t*         txt = ShortItemNames[wi->o.usItem];
		wchar_t                buf[100];
		if (wi->o.ubNumberOfObjects > 1)
		{
			swprintf(buf, lengthof(buf), L"%ls (%d)", txt, wi->o.ubNumberOfObjects);
			txt = buf;
		}

		gprintfdirty(x, y, txt);
		mprintf(     x, y, txt);
		y += dy;
	}
}


static void AddFlashItemSlot(ITEM_POOL* pItemPool, ITEM_POOL_LOCATOR_HOOK Callback);


static void SetItemPoolLocator(ITEM_POOL* pItemPool, ITEM_POOL_LOCATOR_HOOK Callback)
{
	pItemPool->bFlashColor = 59;
	AddFlashItemSlot(pItemPool, Callback);
}


/// ITEM POOL INDICATOR FUNCTIONS


static ITEM_POOL_LOCATOR* GetFreeFlashItemSlot(void)
{
	for (ITEM_POOL_LOCATOR* l = FlashItemSlots; l != FlashItemSlots + guiNumFlashItemSlots; ++l)
	{
		if (!l->fAllocated) return l;
	}
	if (guiNumFlashItemSlots < NUM_ITEM_FLASH_SLOTS)
	{
		return &FlashItemSlots[guiNumFlashItemSlots++];
	}
	return NULL;
}


static void RecountFlashItemSlots(void)
{
INT32 uiCount;

	for(uiCount=guiNumFlashItemSlots-1; (uiCount >=0) ; uiCount--)
	{
		if( ( FlashItemSlots[uiCount].fAllocated ) )
		{
			guiNumFlashItemSlots=(UINT32)(uiCount+1);
			break;
		}
	}
}


static void AddFlashItemSlot(ITEM_POOL* pItemPool, ITEM_POOL_LOCATOR_HOOK Callback)
{
	ITEM_POOL_LOCATOR* const l = GetFreeFlashItemSlot();
	if (l == NULL) return;

	l->pItemPool         = pItemPool;
	l->bRadioFrame       = 0;
	l->uiLastFrameUpdate = GetJA2Clock();
	l->Callback          = Callback;
	l->fAllocated        = TRUE;
	l->ubFlags           = ITEM_LOCATOR_LOCKED;
}


BOOLEAN RemoveFlashItemSlot(const ITEM_POOL* pItemPool)
{
	UINT32 uiCount;

	CHECKF( pItemPool != NULL );

	for( uiCount=0; uiCount < guiNumFlashItemSlots; uiCount++)
	{
		if ( FlashItemSlots[ uiCount ].fAllocated )
		{
			if ( FlashItemSlots[ uiCount ].pItemPool == pItemPool )
			{
				FlashItemSlots[ uiCount ].fAllocated = FALSE;

				// Check if we have a callback and call it if so!
				if ( FlashItemSlots[ uiCount ].Callback != NULL )
				{
					FlashItemSlots[ uiCount ].Callback( );
				}

				return( TRUE );
			}
		}
	}

	return( TRUE );
}


void HandleFlashingItems( )
{
	UINT32 cnt;
	ITEM_POOL		*pItemPool;
	LEVELNODE		*pObject;
	ITEM_POOL_LOCATOR	*pLocator;
	BOOLEAN			fDoLocator = FALSE;

	if ( COUNTERDONE( CYCLERENDERITEMCOLOR ) )
	{
			RESETCOUNTER( CYCLERENDERITEMCOLOR );

			for ( cnt = 0; cnt < guiNumFlashItemSlots; cnt++ )
			{
					pLocator  = &( FlashItemSlots[ cnt ] );

					if ( pLocator->fAllocated )
					{
						fDoLocator = TRUE;

						if ( ( pLocator->ubFlags & ITEM_LOCATOR_LOCKED ) )
						{
							if ( gTacticalStatus.fLockItemLocators == FALSE )
							{
							  // Turn off!
								pLocator->ubFlags &= (~ITEM_LOCATOR_LOCKED);
							}
							else
							{
								fDoLocator = FALSE;
							}
						}

						if ( fDoLocator )
						{
							pItemPool = pLocator->pItemPool;

							// Update radio locator
							{
								UINT32			uiClock;

								uiClock = GetJA2Clock( );

								// Update frame values!
								if ( ( uiClock - pLocator->uiLastFrameUpdate ) > 80 )
								{
									pLocator->uiLastFrameUpdate = uiClock;

									// Update frame
									pLocator->bRadioFrame++;

									if ( pLocator->bRadioFrame == 5 )
									{
										pLocator->bRadioFrame = 0;
									}
								}
							}

							// UPDATE FLASH COLOR VALUE
							pItemPool->bFlashColor--;

							const WORLDITEM* const wi = GetWorldItem(pItemPool->iItemIndex);
							if (wi->ubLevel == 0)
							{
								pObject = gpWorldLevelData[ pItemPool->sGridNo ].pStructHead;
							}
							else
							{
								pObject = gpWorldLevelData[ pItemPool->sGridNo ].pOnRoofHead;
							}

							// LOOP THORUGH OBJECT LAYER
							while( pObject != NULL )
							{
								if ( pObject->uiFlags & LEVELNODE_ITEM )
								{
									if ( pItemPool->bFlashColor == 1 )
									{
										//pObject->uiFlags &= (~LEVELNODE_DYNAMIC);
										//pObject->uiFlags |= ( LEVELNODE_LASTDYNAMIC  );
									}
									else
									{
										//pObject->uiFlags |= LEVELNODE_DYNAMIC;
									}

								}

								pObject = pObject->pNext;
							}

							if ( pItemPool->bFlashColor == 1 )
							{
								pItemPool->bFlashColor = 0;

								// REMOVE TIMER!
								RemoveFlashItemSlot( pItemPool );

								SetRenderFlags( RENDER_FLAG_FULL );
							}
						}
					}
			}

			RecountFlashItemSlots( );

	}

}


void RenderTopmostFlashingItems(void)
{
	for (UINT32 cnt = 0; cnt < guiNumFlashItemSlots; ++cnt)
	{
		const ITEM_POOL_LOCATOR* const l = &FlashItemSlots[cnt];
		if (!l->fAllocated) continue;

		if (l->ubFlags & ITEM_LOCATOR_LOCKED) continue;

		const ITEM_POOL* const ip = l->pItemPool;

		// Update radio locator
		INT16 sX;
		INT16 sY;
		ConvertGridNoToCenterCellXY(ip->sGridNo, &sX, &sY);

		const FLOAT dOffsetX = sX - gsRenderCenterX;
		const FLOAT dOffsetY = sY - gsRenderCenterY;

		// Calculate guy's position
		FLOAT dTempX_S;
		FLOAT dTempY_S;
		FloatFromCellToScreenCoordinates(dOffsetX, dOffsetY, &dTempX_S, &dTempY_S);

		INT16 sXPos = (gsVIEWPORT_END_X - gsVIEWPORT_START_X) / 2 + (INT16)dTempX_S;
		INT16 sYPos = (gsVIEWPORT_END_Y - gsVIEWPORT_START_Y) / 2 + (INT16)dTempY_S - gpWorldLevelData[ip->sGridNo].sHeight;

		const WORLDITEM* const wi = GetWorldItem(ip->iItemIndex);

		// Adjust for offset position on screen
		sXPos -= gsRenderWorldOffsetX;
		sYPos -= gsRenderWorldOffsetY;
		sYPos -= wi->bRenderZHeightAboveLevel;

		// Adjust for render height
		sYPos += gsRenderHeight;

		// Adjust for level height
		if (wi->ubLevel) sYPos -= ROOF_LEVEL_HEIGHT;

		// Center circle!
		sXPos -= 20;
		sYPos -= 20;

		const INT32 iBack = RegisterBackgroundRect(BGND_FLAG_SINGLE, sXPos, sYPos, sXPos + 40, sYPos + 40);
		if (iBack != NO_BGND_RECT) SetBackgroundRectFilled(iBack);

		BltVideoObject(FRAME_BUFFER, guiRADIO, l->bRadioFrame, sXPos, sYPos);

		DrawItemPoolList(ip, wi->bRenderZHeightAboveLevel, sXPos, sYPos);
	}
}


SOLDIERTYPE* VerifyGiveItem(SOLDIERTYPE* const pSoldier)
{
	INT16				sGridNo;
	UINT8				ubTargetMercID;

	// DO SOME CHECKS IF WE CAN DO ANIMATION.....

	sGridNo		= pSoldier->sPendingActionData2;
	ubTargetMercID = (UINT8)pSoldier->uiPendingActionData4;

	// See if our target is still available
	SOLDIERTYPE* const tgt = WhoIsThere2(sGridNo, pSoldier->bLevel);
	if (tgt != NULL)
	{
		// Check if it's the same merc!
		if (tgt->ubID != ubTargetMercID) return NULL;

		// Look for item in hand....

		return tgt;
	}
	else
	{
		if ( pSoldier->pTempObject != NULL )
		{
			AddItemToPool( pSoldier->sGridNo, pSoldier->pTempObject, 1, pSoldier->bLevel, 0 , -1 );

			// Place it on the ground!
			ScreenMsg( FONT_MCOLOR_LTYELLOW, MSG_INTERFACE, TacticalStr[ ITEM_HAS_BEEN_PLACED_ON_GROUND_STR ], ShortItemNames[ pSoldier->pTempObject->usItem ] );

			// OK, disengage buddy
			pSoldier->uiStatusFlags &= (~SOLDIER_ENGAGEDINACTION );

			if ( ubTargetMercID != NOBODY )
			{
				GetMan(ubTargetMercID)->uiStatusFlags &= ~SOLDIER_ENGAGEDINACTION;
			}

			MemFree( pSoldier->pTempObject );
			pSoldier->pTempObject = NULL;

		}
	}

	return NULL;
}


void SoldierGiveItemFromAnimation( SOLDIERTYPE *pSoldier )
{
	INT8				bInvPos;
	OBJECTTYPE	TempObject;
	UINT8				ubProfile;

	UINT16			usItemNum;
	BOOLEAN			fToTargetPlayer = FALSE;

	// Get items from pending data

	// Get objectype and delete
	TempObject = *pSoldier->pTempObject;
	MemFree( pSoldier->pTempObject );
	pSoldier->pTempObject = NULL;


	bInvPos = pSoldier->bPendingActionData5;
	usItemNum = TempObject.usItem;

  // ATE: OK, check if we have an item in the cursor from
  // this soldier and from this inv slot, if so, delete!!!!!!!
	if (gpItemPointer != NULL                 &&
			pSoldier      == gpItemPointerSoldier &&
			bInvPos       == gbItemPointerSrcSlot &&
			usItemNum     == gpItemPointer->usItem)
	{
		// Remove!!!
		EndItemPointer( );
	}

	// ATE: Deduct APs!
	DeductPoints( pSoldier, AP_PICKUP_ITEM, 0 );

	SOLDIERTYPE* const pTSoldier = VerifyGiveItem(pSoldier);
	if (pTSoldier != NULL)
	{
		// DAVE! - put stuff here to bring up shopkeeper.......

		//if the user just clicked on an arms dealer
		if( IsMercADealer( pTSoldier->ubProfile ) )
		{
			UnSetEngagedInConvFromPCAction( pSoldier );

			//if the dealer is Micky,
			/*
			if( pTSoldier->ubProfile == MICKY )
			{
				//and the items are alcohol, dont enter the shopkeeper
				if( GetArmsDealerItemTypeFromItemNumber( TempObject.usItem ) == ARMS_DEALER_ALCOHOL )
					return;
			}
			*/

			if ( NPCHasUnusedRecordWithGivenApproach( pTSoldier->ubProfile, APPROACH_BUYSELL ) )
			{
				TriggerNPCWithGivenApproach( pTSoldier->ubProfile, APPROACH_BUYSELL, TRUE );
				return;
			}
			// now also check for buy/sell lines (Oct 13)
			/*
			else if ( NPCWillingToAcceptItem( pTSoldier->ubProfile, pSoldier->ubProfile, &TempObject ) )
			{
				TriggerNPCWithGivenApproach( pTSoldier->ubProfile, APPROACH_GIVINGITEM, TRUE );
				return;
			}*/
			else if ( !NPCWillingToAcceptItem( pTSoldier->ubProfile, pSoldier->ubProfile, &TempObject ) )
			{

				//Enter the shopkeeper interface
				EnterShopKeeperInterfaceScreen( pTSoldier->ubProfile );

// removed the if, because if the player picked up an item straight from the ground or money strait from the money
// interface, the item would NOT have a bInvPos, therefore it would not get added to the dealer, and would get deleted
//				if ( bInvPos != NO_SLOT )
				{
					// MUST send in NO_SLOT, as the SKI wille expect it to exist in inv if not....
					AddItemToPlayersOfferAreaAfterShopKeeperOpen( &TempObject, NO_SLOT );

/*
Changed because if the player gave 1 item from a pile, the rest of the items in the piule would disappear
					// OK, r	emove the item, as the SKI will give it back once done
					DeleteObj( &( pSoldier->inv[ bInvPos ] ) );
*/


					if ( bInvPos != NO_SLOT )
					{
						RemoveObjFrom( &pSoldier->inv[ bInvPos ], TempObject.ubNumberOfObjects );
					}

					DirtyMercPanelInterface( pSoldier, DIRTYLEVEL2 );
				}

				return;
			}
		}

		// OK< FOR NOW HANDLE NPC's DIFFERENT!
		ubProfile = pTSoldier->ubProfile;

		// 1 ) PLayer to NPC = NPC
		// 2 ) Player to player = player;
		// 3 ) NPC to player = player;
		// 4 ) NPC TO NPC = NPC

		// Switch on target...
		// Are we a player dude.. ( target? )
		if ( ubProfile < FIRST_RPC || RPC_RECRUITED( pTSoldier ) )
		{
			fToTargetPlayer = TRUE;
		}


		if ( fToTargetPlayer )
		{
			// begin giving
			DirtyMercPanelInterface( pSoldier, DIRTYLEVEL2 );

			// We are a merc, add!
			if ( !AutoPlaceObject( pTSoldier, &TempObject, TRUE ) )
			{
				// Erase!
				if ( bInvPos != NO_SLOT )
				{
					DeleteObj( &( pSoldier->inv[ bInvPos ] ) );
					DirtyMercPanelInterface( pSoldier, DIRTYLEVEL2 );
				}

				AddItemToPool( pSoldier->sGridNo, &TempObject, 1, pSoldier->bLevel, 0 , -1 );

				// We could not place it!
				// Drop it on the ground?
				ScreenMsg( FONT_MCOLOR_LTYELLOW, MSG_INTERFACE, TacticalStr[ ITEM_HAS_BEEN_PLACED_ON_GROUND_STR ], ShortItemNames[ usItemNum ] );

				// OK, disengage buddy
				pSoldier->uiStatusFlags &= (~SOLDIER_ENGAGEDINACTION );
				pTSoldier->uiStatusFlags &= (~SOLDIER_ENGAGEDINACTION );
			}
			else
			{
				// Erase!
				if ( bInvPos != NO_SLOT )
				{
					DeleteObj( &( pSoldier->inv[ bInvPos ] ) );
					DirtyMercPanelInterface( pSoldier, DIRTYLEVEL2 );
				}

				// OK, it's given, display message!
				ScreenMsg( FONT_MCOLOR_LTYELLOW, MSG_INTERFACE, TacticalStr[ ITEM_HAS_BEEN_GIVEN_TO_STR ], ShortItemNames[ usItemNum ], pTSoldier->name );
				if (usItemNum == MONEY)
				{
					// are we giving money to an NPC, to whom we owe money?
					if (pTSoldier->ubProfile != NO_PROFILE && gMercProfiles[pTSoldier->ubProfile].iBalance < 0)
					{
						gMercProfiles[pTSoldier->ubProfile].iBalance += TempObject.uiMoneyAmount;
						if (gMercProfiles[pTSoldier->ubProfile].iBalance >= 0)
						{
							// don't let the player accumulate credit (?)
							gMercProfiles[pTSoldier->ubProfile].iBalance = 0;

							// report the payment and set facts to indicate people not being owed money
							ScreenMsg( FONT_MCOLOR_LTYELLOW, MSG_INTERFACE, TacticalStr[ GUY_HAS_BEEN_PAID_IN_FULL_STR ], pTSoldier->name );
						}
						else
						{
							// report the payment
							ScreenMsg( FONT_MCOLOR_LTYELLOW, MSG_INTERFACE, TacticalStr[ GUY_STILL_OWED_STR ], pTSoldier->name, -gMercProfiles[pTSoldier->ubProfile].iBalance );
						}
					}
				}
			}
		}
		else
		{
			// Erase!
			if ( bInvPos != NO_SLOT )
			{
				RemoveObjs( &(pSoldier->inv[ bInvPos ]), TempObject.ubNumberOfObjects );
				DirtyMercPanelInterface( pSoldier, DIRTYLEVEL2 );
			}

			// Now intiate conv
			InitiateConversation( pTSoldier, pSoldier, APPROACH_GIVINGITEM, (INT32)&TempObject );
		}
	}

	// OK, disengage buddy
	pSoldier->uiStatusFlags &= (~SOLDIER_ENGAGEDINACTION );
	pTSoldier->uiStatusFlags &= (~SOLDIER_ENGAGEDINACTION );

}



INT16 AdjustGridNoForItemPlacement( SOLDIERTYPE *pSoldier, INT16 sGridNo )
{
	STRUCTURE		*pStructure;
	INT16				sDesiredLevel;
	INT16				sActionGridNo;
	BOOLEAN			fStructFound = FALSE;
	INT16				sAdjustedGridNo;

	sActionGridNo = sGridNo;

	// Check structure database
	if( gpWorldLevelData[ sGridNo ].pStructureHead )
	{
		// Something is here, check obstruction in future
		sDesiredLevel = pSoldier->bLevel ? STRUCTURE_ON_ROOF : STRUCTURE_ON_GROUND;
		pStructure = FindStructure( (INT16)sGridNo, STRUCTURE_BLOCKSMOVES );
		while( pStructure )
		{
			if( !(pStructure->fFlags & STRUCTURE_PASSABLE) && pStructure->sCubeOffset == sDesiredLevel )
			{
				// Check for openable flag....
				//if ( pStructure->fFlags & ( STRUCTURE_OPENABLE | STRUCTURE_HASITEMONTOP ) )
				{
					fStructFound = TRUE;
					break;
				}
			}
			pStructure = FindNextStructure( pStructure, STRUCTURE_BLOCKSMOVES );
		}
	}

	// ATE: IF a person is found, use adjacent gridno for it!
	const SOLDIERTYPE* const tgt = WhoIsThere2(sGridNo, pSoldier->bLevel);
	if (fStructFound || (tgt != NULL && tgt != pSoldier))
	{
		// GET ADJACENT GRIDNO
		sActionGridNo = FindAdjacentGridEx(pSoldier, sGridNo, NULL, &sAdjustedGridNo, FALSE, FALSE);

		if ( sActionGridNo == -1 )
		{
			sActionGridNo = sAdjustedGridNo;
		}
	}

	return( sActionGridNo );
}


static void BombMessageBoxCallBack(UINT8 ubExitValue);


static void StartBombMessageBox(SOLDIERTYPE* pSoldier, INT16 sGridNo)
{
	UINT8 ubRoom;

	gpTempSoldier = pSoldier;
	gsTempGridno = sGridNo;
	if (pSoldier->inv[HANDPOS].usItem == REMOTEBOMBTRIGGER)
	{
		DoMessageBox( MSG_BOX_BASIC_SMALL_BUTTONS, TacticalStr[ CHOOSE_BOMB_FREQUENCY_STR ], GAME_SCREEN, ( UINT8 )MSG_BOX_FLAG_FOUR_NUMBERED_BUTTONS, BombMessageBoxCallBack, NULL );
	}
	else if (pSoldier->inv[HANDPOS].usItem == REMOTETRIGGER)
	{
		#ifdef JA2DEMO
			{
				UINT8 ubRoom;

				if ( InARoom( pSoldier->sGridNo, &ubRoom ) && ubRoom == 31 )
				{
					SetOffBombsByFrequency(pSoldier, FIRST_MAP_PLACED_FREQUENCY + 4);
					DoMercBattleSound( pSoldier, BATTLE_SOUND_OK1 );
				}
				else
				{
					DoMercBattleSound( pSoldier, BATTLE_SOUND_CURSE1 );
				}
			}
		#else
      // PLay sound....
      PlayJA2Sample(USE_STATUE_REMOTE, HIGHVOLUME, 1, MIDDLEPAN);


			// Check what sector we are in....
			if ( gWorldSectorX == 3 && gWorldSectorY == MAP_ROW_O && gbWorldSectorZ == 0 )
			{
				if ( InARoom( pSoldier->sGridNo, &ubRoom ) && ubRoom == 4 )
				{
					DoMercBattleSound( pSoldier, BATTLE_SOUND_OK1 );

					// Open statue
					ChangeO3SectorStatue( FALSE );

				}
				else
				{
					DoMercBattleSound( pSoldier, BATTLE_SOUND_CURSE1 );
				}
			}
			else
			{
				DoMercBattleSound( pSoldier, BATTLE_SOUND_CURSE1 );
			}

		#endif
	}
	else if ( FindAttachment( &(pSoldier->inv[HANDPOS]), DETONATOR) != ITEM_NOT_FOUND )
	{
		DoMessageBox( MSG_BOX_BASIC_SMALL_BUTTONS, TacticalStr[ CHOOSE_TIMER_STR ], GAME_SCREEN, ( UINT8 )MSG_BOX_FLAG_FOUR_NUMBERED_BUTTONS, BombMessageBoxCallBack, NULL );
	}
	else if ( FindAttachment( &(pSoldier->inv[HANDPOS]), REMDETONATOR) != ITEM_NOT_FOUND )
	{
		DoMessageBox( MSG_BOX_BASIC_SMALL_BUTTONS, TacticalStr[ CHOOSE_REMOTE_FREQUENCY_STR ], GAME_SCREEN, ( UINT8 )MSG_BOX_FLAG_FOUR_NUMBERED_BUTTONS, BombMessageBoxCallBack, NULL );
	}
}


static void BombMessageBoxCallBack(UINT8 ubExitValue)
{
	if (gpTempSoldier)
	{
		if (gpTempSoldier->inv[HANDPOS].usItem == REMOTEBOMBTRIGGER)
		{
			SetOffBombsByFrequency(gpTempSoldier, ubExitValue);
		}
		else
		{
			INT32 iResult;

			if (FindAttachment( &(gpTempSoldier->inv[HANDPOS]), REMDETONATOR ) != ITEM_NOT_FOUND )
			{
				iResult = SkillCheck( gpTempSoldier, PLANTING_REMOTE_BOMB_CHECK, 0 );
			}
			else
			{
				iResult = SkillCheck( gpTempSoldier, PLANTING_BOMB_CHECK, 0 );
			}

			if ( iResult >= 0 )
			{
				// EXPLOSIVES GAIN (25):  Place a bomb, or buried and armed a mine
				StatChange( gpTempSoldier, EXPLODEAMT, 25, FALSE );
			}
			else
			{
				// EXPLOSIVES GAIN (10):  Failed to place a bomb, or bury and arm a mine
				StatChange( gpTempSoldier, EXPLODEAMT, 10, FROM_FAILURE );

				// oops!  How badly did we screw up?
				if ( iResult >= -20 )
				{
					// messed up the setting
					if ( ubExitValue == 0 )
					{
						ubExitValue = 1;
					}
					else
					{
						// change up/down by 1
						ubExitValue = (UINT8) (ubExitValue + Random( 3 ) - 1);
					}
					// and continue
				}
				else
				{
					// OOPS! ... BOOM!
					IgniteExplosionXY(NULL, gpTempSoldier->sX, gpTempSoldier->sY, gpWorldLevelData[gpTempSoldier->sGridNo].sHeight, gpTempSoldier->sGridNo, gpTempSoldier->inv[HANDPOS].usItem, gpTempSoldier->bLevel);
					return;
				}
			}

			if ( ArmBomb( &(gpTempSoldier->inv[HANDPOS]), ubExitValue ) )
			{
				gpTempSoldier->inv[ HANDPOS ].bTrap = __min( 10, ( EffectiveExplosive( gpTempSoldier ) / 20) + (EffectiveExpLevel( gpTempSoldier ) / 3) );
				// HACK IMMINENT!
				// value of 1 is stored in maps for SIDE of bomb owner... when we want to use IDs!
				// so we add 2 to all owner IDs passed through here and subtract 2 later
				gpTempSoldier->inv[HANDPOS].ubBombOwner = gpTempSoldier->ubID + 2;
				AddItemToPool( gsTempGridno, &(gpTempSoldier->inv[HANDPOS]), 1, gpTempSoldier->bLevel, WORLD_ITEM_ARMED_BOMB, 0 );
				DeleteObj( &(gpTempSoldier->inv[HANDPOS]) );
			}
		}
	}

}


static BOOLEAN HandItemWorks(SOLDIERTYPE* pSoldier, INT8 bSlot)
{
	BOOLEAN							fItemJustBroke = FALSE, fItemWorks = TRUE;
	OBJECTTYPE *				pObj;

	pObj = &( pSoldier->inv[ bSlot ] );

	// if the item can be damaged, than we must check that it's in good enough
	// shape to be usable, and doesn't break during use.
	// Exception: land mines.  You can bury them broken, they just won't blow!
	if ( (Item[ pObj->usItem ].fFlags & ITEM_DAMAGEABLE) && (pObj->usItem != MINE) && (Item[ pObj->usItem ].usItemClass != IC_MEDKIT) && pObj->usItem != GAS_CAN )
	{
		// if it's still usable, check whether it breaks
		if ( pObj->bStatus[0] >= USABLE)
		{
			// if a dice roll is greater than the item's status
			if ( (Random(80) + 20) >= (UINT32) (pObj->bStatus[0] + 50) )
			{
				fItemJustBroke = TRUE;
				fItemWorks = FALSE;

				// item breaks, and becomes unusable...  so its status is reduced
				// to somewhere between 1 and the 1 less than USABLE
				pObj->bStatus[0] = (INT8) ( 1 + Random( USABLE - 1 ) );
			}
		}
		else	// it's already unusable
		{
			fItemWorks = FALSE;
		}

		if (!fItemWorks && pSoldier->bTeam == gbPlayerNum)
		{
			// merc says "This thing doesn't work!"
			TacticalCharacterDialogue( pSoldier, QUOTE_USELESS_ITEM );
			if (fItemJustBroke)
			{
				DirtyMercPanelInterface( pSoldier, DIRTYLEVEL2 );
			}
		}
	}

	if ( fItemWorks && bSlot == HANDPOS && Item[ pObj->usItem ].usItemClass == IC_GUN )
	{
		// are we using two guns at once?
		if ( Item[ pSoldier->inv[SECONDHANDPOS].usItem ].usItemClass == IC_GUN &&
			pSoldier->inv[SECONDHANDPOS].bGunStatus >= USABLE &&
			pSoldier->inv[SECONDHANDPOS].ubGunShotsLeft > 0)
		{
			// check the second gun for breakage, and if IT breaks, return false
			return( HandItemWorks( pSoldier, SECONDHANDPOS ) );
		}
	}

	return( fItemWorks );
}


// set off the booby trap in mapscreen
static void SetOffBoobyTrapInMapScreen(SOLDIERTYPE* pSoldier, OBJECTTYPE* pObject)
{
	UINT8 ubPtsDmg = 0;

	// check if trapped item is an explosive, if so then up the amount of dmg
	if( ( pObject -> usItem == TNT )|| (  pObject -> usItem == RDX ) )
	{
		// for explosive
		ubPtsDmg = 0;
	}
	else
	{
		// normal mini grenade dmg
		ubPtsDmg = 0;
	}

	// injure the inventory character
	SoldierTakeDamage(pSoldier, ubPtsDmg, ubPtsDmg, TAKE_DAMAGE_EXPLOSION, NULL);

	// play the sound
	PlayJA2Sample(EXPLOSION_1, BTNVOLUME, 1, MIDDLEPAN);
}


static void SetOffBoobyTrap(ITEM_POOL* pItemPool)
{
	if ( pItemPool )
	{
		WORLDITEM* const wi = GetWorldItem(pItemPool->iItemIndex);
		IgniteExplosion(NULL, gpWorldLevelData[pItemPool->sGridNo].sHeight + wi->bRenderZHeightAboveLevel, pItemPool->sGridNo, MINI_GRENADE, 0);
		RemoveItemFromPool(wi);
	}
}


static void BoobyTrapDialogueCallBack(void);
static void BoobyTrapInMapScreenMessageBoxCallBack(UINT8 ubExitValue);


static BOOLEAN ContinuePastBoobyTrap(SOLDIERTYPE* const pSoldier, const INT16 sGridNo, const INT32 iItemIndex, const BOOLEAN fInStrategic, BOOLEAN* const pfSaidQuote)
{
	BOOLEAN					fBoobyTrapKnowledge;
	INT8						bTrapDifficulty, bTrapDetectLevel;

	OBJECTTYPE* const pObj = &GetWorldItem(iItemIndex)->o;

  (*pfSaidQuote) = FALSE;

	if (pObj->bTrap > 0)
	{
		if (pSoldier->bTeam == gbPlayerNum)
		{
			// does the player know about this item?
			fBoobyTrapKnowledge = ((pObj->fFlags & OBJECT_KNOWN_TO_BE_TRAPPED) > 0);

			// blue flag stuff?

			if (!fBoobyTrapKnowledge)
			{
				bTrapDifficulty = pObj->bTrap;
				bTrapDetectLevel = CalcTrapDetectLevel( pSoldier, FALSE );
				if (bTrapDetectLevel >= bTrapDifficulty)
				{
					// spotted the trap!
					pObj->fFlags |= OBJECT_KNOWN_TO_BE_TRAPPED;
					fBoobyTrapKnowledge = TRUE;

					// Make him warn us:

					// Set things up..
					gpBoobyTrapSoldier = pSoldier;
					gpBoobyTrapItemPool = GetItemPoolForIndex( sGridNo, iItemIndex, pSoldier->bLevel );
					gsBoobyTrapGridNo = sGridNo;
				  gbBoobyTrapLevel  = pSoldier->bLevel;
					gfDisarmingBuriedBomb = FALSE;
					gbTrapDifficulty = bTrapDifficulty;

					// And make the call for the dialogue
					SetStopTimeQuoteCallback( BoobyTrapDialogueCallBack );
					TacticalCharacterDialogue( pSoldier, QUOTE_BOOBYTRAP_ITEM );

          (*pfSaidQuote) = TRUE;

					return( FALSE );
				}
			}

			gpBoobyTrapItemPool = GetItemPoolForIndex( sGridNo, iItemIndex, pSoldier->bLevel );
			if (fBoobyTrapKnowledge)
			{
				// have the computer ask us if we want to proceed
				gpBoobyTrapSoldier = pSoldier;
				gsBoobyTrapGridNo = sGridNo;
				gbBoobyTrapLevel  = pSoldier->bLevel;
				gfDisarmingBuriedBomb = FALSE;
				gbTrapDifficulty = pObj->bTrap;

				if( fInStrategic )
				{
					DoMessageBox( MSG_BOX_BASIC_STYLE, TacticalStr[ DISARM_BOOBYTRAP_PROMPT ], MAP_SCREEN, ( UINT8 )MSG_BOX_FLAG_YESNO, BoobyTrapInMapScreenMessageBoxCallBack, NULL );
				}
				else
				{
					DoMessageBox( MSG_BOX_BASIC_STYLE, TacticalStr[ DISARM_BOOBYTRAP_PROMPT ], GAME_SCREEN, ( UINT8 )MSG_BOX_FLAG_YESNO, BoobyTrapMessageBoxCallBack, NULL );
				}
			}
			else
			{
				// oops!
				SetOffBoobyTrap( gpBoobyTrapItemPool );
			}

			return( FALSE );

		}
		// else, enemies etc always know about boobytraps and are not affected by them
	}

	return( TRUE );
}


static void BoobyTrapDialogueCallBack(void)
{
	gfJustFoundBoobyTrap = TRUE;

	// now prompt the user...
	if( guiTacticalInterfaceFlags & INTERFACE_MAPSCREEN )
	{
		DoScreenIndependantMessageBox( TacticalStr[ DISARM_BOOBYTRAP_PROMPT ],  ( UINT8 )MSG_BOX_FLAG_YESNO, BoobyTrapInMapScreenMessageBoxCallBack );
	}
	else
	{
		DoScreenIndependantMessageBox( TacticalStr[ DISARM_BOOBYTRAP_PROMPT ],  ( UINT8 )MSG_BOX_FLAG_YESNO, BoobyTrapMessageBoxCallBack );
	}
}


static void RemoveBlueFlagDialogueCallBack(UINT8 ubExitValue);


static void BoobyTrapMessageBoxCallBack(UINT8 ubExitValue)
{
	if ( gfJustFoundBoobyTrap )
	{
		// NOW award for finding boobytrap
		// WISDOM GAIN:  Detected a booby-trap
		StatChange( gpBoobyTrapSoldier, WISDOMAMT, (UINT16) (3 * gbTrapDifficulty), FALSE );
		// EXPLOSIVES GAIN:  Detected a booby-trap
		StatChange( gpBoobyTrapSoldier, EXPLODEAMT, (UINT16) (3 * gbTrapDifficulty), FALSE );
		gfJustFoundBoobyTrap = FALSE;
	}

	if (ubExitValue == MSG_BOX_RETURN_YES)
	{
		INT32						iCheckResult;

		iCheckResult = SkillCheck( gpBoobyTrapSoldier, DISARM_TRAP_CHECK, 0 );

		if (iCheckResult >= 0)
		{
			WORLDITEM* const wi = GetWorldItem(gpBoobyTrapItemPool->iItemIndex);

			// get the item
			OBJECTTYPE Object = wi->o;

			// NB owner grossness... bombs 'owned' by the enemy are stored with side value 1 in
			// the map. So if we want to detect a bomb placed by the player, owner is > 1, and
			// owner - 2 gives the ID of the character who planted it
			if ( Object.ubBombOwner > 1 && ( (INT32)Object.ubBombOwner - 2 >= gTacticalStatus.Team[ OUR_TEAM ].bFirstID && Object.ubBombOwner - 2 <= gTacticalStatus.Team[ OUR_TEAM ].bLastID ) )
			{
				// our own bomb! no exp
			}
			else
			{
				// disarmed a boobytrap!
				StatChange( gpBoobyTrapSoldier, EXPLODEAMT, (UINT16) (6 * gbTrapDifficulty), FALSE );
				// have merc say this is good
				DoMercBattleSound( gpBoobyTrapSoldier, BATTLE_SOUND_COOL1 );
			}

			if (gfDisarmingBuriedBomb)
			{
				if (Object.usItem == SWITCH)
				{
					// give the player a remote trigger instead
					CreateItem( REMOTEBOMBTRIGGER, (INT8) (1 + Random( 9 )), &Object );
				}
				else if (Object.usItem == ACTION_ITEM && Object.bActionValue != ACTION_ITEM_BLOW_UP )
				{
					// give the player a detonator instead
					CreateItem( DETONATOR, (INT8) (1 + Random( 9 )), &Object );
				}
				else
				{
					// switch action item to the real item type
					CreateItem( Object.usBombItem, Object.bBombStatus, &Object );
				}

				// remove any blue flag graphic
				RemoveBlueFlag( gsBoobyTrapGridNo, gbBoobyTrapLevel );
			}
			else
			{
				Object.bTrap = 0;
				Object.fFlags &= ~( OBJECT_KNOWN_TO_BE_TRAPPED );
			}

			// place it in the guy's inventory/cursor
			if ( AutoPlaceObject( gpBoobyTrapSoldier, &Object, TRUE ) )
			{
				// remove it from the ground
				RemoveItemFromPool(wi);
			}
			else
			{
				// make sure the item in the world is untrapped
				OBJECTTYPE* const o = &wi->o;
				o->bTrap   = 0;
				o->fFlags &= ~OBJECT_KNOWN_TO_BE_TRAPPED;

				// ATE; If we failed to add to inventory, put failed one in our cursor...
				gfDontChargeAPsToPickup = TRUE;
				HandleAutoPlaceFail(gpBoobyTrapSoldier, o, gsBoobyTrapGridNo);
				RemoveItemFromPool(wi);
			}
		}
		else
		{
			// oops! trap goes off
      StatChange( gpBoobyTrapSoldier, EXPLODEAMT, (INT8) (3 * gbTrapDifficulty ), FROM_FAILURE );

			DoMercBattleSound( gpBoobyTrapSoldier, BATTLE_SOUND_CURSE1 );

			if (gfDisarmingBuriedBomb)
			{
				SetOffBombsInGridNo(gpBoobyTrapSoldier, gsBoobyTrapGridNo, TRUE, gbBoobyTrapLevel);
			}
			else
			{
				SetOffBoobyTrap( gpBoobyTrapItemPool );
			}
		}
	}
	else
	{
		if (gfDisarmingBuriedBomb)
		{
			DoMessageBox( MSG_BOX_BASIC_STYLE, TacticalStr[ REMOVE_BLUE_FLAG_PROMPT ], GAME_SCREEN, ( UINT8 )MSG_BOX_FLAG_YESNO, RemoveBlueFlagDialogueCallBack, NULL );
		}
		// otherwise do nothing
	}
}


static void BoobyTrapInMapScreenMessageBoxCallBack(UINT8 ubExitValue)
{
	if ( gfJustFoundBoobyTrap )
	{
		// NOW award for finding boobytrap

		// WISDOM GAIN:  Detected a booby-trap
		StatChange( gpBoobyTrapSoldier, WISDOMAMT, (UINT16) (3 * gbTrapDifficulty), FALSE );
		// EXPLOSIVES GAIN:  Detected a booby-trap
		StatChange( gpBoobyTrapSoldier, EXPLODEAMT, (UINT16) (3 * gbTrapDifficulty), FALSE );
		gfJustFoundBoobyTrap = FALSE;
	}

	if (ubExitValue == MSG_BOX_RETURN_YES)
	{
		INT32						iCheckResult;
		OBJECTTYPE 			Object;

		iCheckResult = SkillCheck( gpBoobyTrapSoldier, DISARM_TRAP_CHECK, 0 );

		if (iCheckResult >= 0)
		{
			// disarmed a boobytrap!
      StatChange( gpBoobyTrapSoldier, EXPLODEAMT, (UINT16) (6 * gbTrapDifficulty), FALSE );


			// have merc say this is good
			DoMercBattleSound( gpBoobyTrapSoldier, BATTLE_SOUND_COOL1 );

			// get the item
			Object = *gpItemPointer;

			if (gfDisarmingBuriedBomb)
			{
				if (Object.usItem == SWITCH)
				{
					// give the player a remote trigger instead
					CreateItem( REMOTEBOMBTRIGGER, (INT8) (1 + Random( 9 )), &Object );
				}
				else if (Object.usItem == ACTION_ITEM && Object.bActionValue != ACTION_ITEM_BLOW_UP )
				{
					// give the player a detonator instead
					CreateItem( DETONATOR, (INT8) (1 + Random( 9 )), &Object );
				}
				else
				{
					// switch action item to the real item type
					CreateItem( Object.usBombItem, Object.bBombStatus, &Object );
				}
			}
			else
			{
				Object.bTrap = 0;
				Object.fFlags &= ~( OBJECT_KNOWN_TO_BE_TRAPPED );
			}

			MAPEndItemPointer( );

			// place it in the guy's inventory/cursor
			if ( !AutoPlaceObject( gpBoobyTrapSoldier, &Object, TRUE ) )
			{
				AutoPlaceObjectInInventoryStash( &Object );
			}

			HandleButtonStatesWhileMapInventoryActive( );
		}
		else
		{
			// oops! trap goes off
      StatChange( gpBoobyTrapSoldier, EXPLODEAMT, (INT8) (3 * gbTrapDifficulty ), FROM_FAILURE );

			DoMercBattleSound( gpBoobyTrapSoldier, BATTLE_SOUND_CURSE1 );

			if (gfDisarmingBuriedBomb)
			{
				SetOffBombsInGridNo(gpBoobyTrapSoldier, gsBoobyTrapGridNo, TRUE, gbBoobyTrapLevel);
			}
			else
			{
				SetOffBoobyTrap( gpBoobyTrapItemPool );
			}
		}
	}
	else
	{
		if (gfDisarmingBuriedBomb)
		{
			DoMessageBox( MSG_BOX_BASIC_STYLE, TacticalStr[ REMOVE_BLUE_FLAG_PROMPT ], GAME_SCREEN, ( UINT8 )MSG_BOX_FLAG_YESNO, RemoveBlueFlagDialogueCallBack, NULL );
		}
		// otherwise do nothing
	}
}


static void SwitchMessageBoxCallBack(UINT8 ubExitValue)
{
	if ( ubExitValue == MSG_BOX_RETURN_YES )
	{
    // Message that switch is activated...
		ScreenMsg( FONT_MCOLOR_LTYELLOW, MSG_INTERFACE, gzLateLocalizedString[ 60 ] );
		SetOffBombsByFrequency(gpTempSoldier, bTempFrequency);
	}
}


static void AddBlueFlag(INT16 sGridNo, INT8 bLevel);


BOOLEAN NearbyGroundSeemsWrong(SOLDIERTYPE* const s, const INT16 sGridNo, const BOOLEAN fCheckAroundGridno, INT16* const psProblemGridNo)
{
	BOOLEAN fMining;
	UINT8   ubDetectLevel;
	if (FindObj(s, METALDETECTOR) != NO_SLOT)
	{
		fMining       = TRUE;
		ubDetectLevel = 0;
	}
	else
	{
		fMining       = FALSE;
		ubDetectLevel = CalcTrapDetectLevel(s, FALSE);
	}

	const UINT32 fCheckFlag = (s->bSide == 0 ? MAPELEMENT_PLAYER_MINE_PRESENT : MAPELEMENT_ENEMY_MINE_PRESENT);

	// check every tile around gridno for the presence of "nasty stuff"
	for (UINT8 ubDirection = 0; ubDirection < 8; ++ubDirection)
	{
		INT16 sNextGridNo;
		if (fCheckAroundGridno)
		{
			// get the gridno of the next spot adjacent to lastGridno in that direction
			sNextGridNo = NewGridNo(sGridNo, (INT16)DirectionInc((UINT8)ubDirection));

			// don't check directions that are impassable!
			UINT8 ubMovementCost = gubWorldMovementCosts[sNextGridNo][ubDirection][s->bLevel];
			if (IS_TRAVELCOST_DOOR(ubMovementCost))
			{
				ubMovementCost = DoorTravelCost(NULL, sNextGridNo, ubMovementCost, FALSE, NULL);
			}
			if (ubMovementCost >= TRAVELCOST_BLOCKED) continue;
		}
		else
		{
			// we should just be checking the gridno
			sNextGridNo = sGridNo;
			ubDirection = 8; // don't loop
		}

		// if this sNextGridNo isn't out of bounds... but it never can be
		const MAP_ELEMENT* const pMapElement = &gpWorldLevelData[sNextGridNo];
		// already know there's a mine there?
		if (pMapElement->uiFlags & fCheckFlag) continue;

		// check for boobytraps
		CFOR_ALL_WORLD_BOMBS(wb)
		{
			WORLDITEM* const wi = GetWorldItem(wb->iItemIndex);
			if (wi->sGridNo != sNextGridNo) continue;

			OBJECTTYPE* const o = &wi->o;
			if (o->bDetonatorType != BOMB_PRESSURE)     continue;
			if (o->fFlags & OBJECT_KNOWN_TO_BE_TRAPPED) continue;
			if (o->fFlags & OBJECT_DISABLED_BOMB)       continue;

			if (fMining && o->bTrap <= 10)
			{
				// add blue flag
				AddBlueFlag(sNextGridNo, s->bLevel);
				*psProblemGridNo = NOWHERE;
				return TRUE;
			}
			else if (ubDetectLevel >= o->bTrap)
			{
				if (s->uiStatusFlags & SOLDIER_PC)
				{
					// detected explosives buried nearby...
					StatChange(s, EXPLODEAMT, (UINT16)o->bTrap, FALSE);

					// set item as known
					o->fFlags |= OBJECT_KNOWN_TO_BE_TRAPPED;
				}

				*psProblemGridNo = sNextGridNo;
				return TRUE;
			}
		}
	}

	*psProblemGridNo = NOWHERE;
	return FALSE;
}


static void MineSpottedLocatorCallback(void);


void MineSpottedDialogueCallBack( void )
{
  // ATE: REALLY IMPORTANT - ALL CALLBACK ITEMS SHOULD UNLOCK
  gTacticalStatus.fLockItemLocators = FALSE;

	ITEM_POOL* pItemPool = GetItemPool(gsBoobyTrapGridNo, gbBoobyTrapLevel);

	guiPendingOverrideEvent = LU_BEGINUILOCK;

	// play a locator at the location of the mine
	SetItemPoolLocator(pItemPool, MineSpottedLocatorCallback);
}


static void MineSpottedMessageBoxCallBack(UINT8 ubExitValue);


static void MineSpottedLocatorCallback(void)
{
	guiPendingOverrideEvent = LU_ENDUILOCK;

	// now ask the player if he wants to place a blue flag.
	DoMessageBox( MSG_BOX_BASIC_STYLE, TacticalStr[ PLACE_BLUE_FLAG_PROMPT ], GAME_SCREEN, ( UINT8 )MSG_BOX_FLAG_YESNO, MineSpottedMessageBoxCallBack, NULL );
}


static void MineSpottedMessageBoxCallBack(UINT8 ubExitValue)
{
	if (ubExitValue == MSG_BOX_RETURN_YES)
	{
		// place a blue flag where the mine was found
		AddBlueFlag( gsBoobyTrapGridNo, gbBoobyTrapLevel );
	}
}


static void RemoveBlueFlagDialogueCallBack(UINT8 ubExitValue)
{
	if (ubExitValue == MSG_BOX_RETURN_YES)
	{
		RemoveBlueFlag( gsBoobyTrapGridNo, gbBoobyTrapLevel );
	}
}


static void AddBlueFlag(INT16 sGridNo, INT8 bLevel)
{
	LEVELNODE *pNode;

	ApplyMapChangesToMapTempFile( TRUE );
	gpWorldLevelData[ sGridNo ].uiFlags |= MAPELEMENT_PLAYER_MINE_PRESENT;

  pNode = AddStructToTail( sGridNo, BLUEFLAG_GRAPHIC );

	if ( pNode )
	{
		pNode->uiFlags |= LEVELNODE_SHOW_THROUGH;
	}

	ApplyMapChangesToMapTempFile( FALSE );
	RecompileLocalMovementCostsFromRadius( sGridNo, bLevel );
	SetRenderFlags(RENDER_FLAG_FULL);
}

void RemoveBlueFlag( INT16 sGridNo, INT8 bLevel )
{
	ApplyMapChangesToMapTempFile( TRUE );
	gpWorldLevelData[sGridNo].uiFlags &= ~(MAPELEMENT_PLAYER_MINE_PRESENT);

  if ( bLevel == 0 )
  {
	  RemoveStruct( sGridNo, BLUEFLAG_GRAPHIC );
  }
  else
  {
    RemoveOnRoof( sGridNo, BLUEFLAG_GRAPHIC );
  }

	ApplyMapChangesToMapTempFile( FALSE );
	RecompileLocalMovementCostsFromRadius( sGridNo, bLevel );
	SetRenderFlags(RENDER_FLAG_FULL);
}


void MakeNPCGrumpyForMinorOffense(SOLDIERTYPE* const pSoldier, const SOLDIERTYPE* const pOffendingSoldier)
{
	CancelAIAction(pSoldier);

	switch( pSoldier->ubProfile )
	{
		case FREDO:
		case FRANZ:
		case HERVE:
		case PETER:
		case ALBERTO:
		case CARLO:
		case MANNY:
		case GABBY:
		case ARNIE:
		case HOWARD:
		case SAM:
		case FATHER:
		case TINA:
		case ARMAND:
		case WALTER:
			gMercProfiles[ pSoldier->ubProfile ].ubMiscFlags3 |= PROFILE_MISC_FLAG3_NPC_PISSED_OFF;
			TriggerNPCWithIHateYouQuote( pSoldier->ubProfile );
			break;
		default:
			// trigger NPCs with quote if available
			AddToShouldBecomeHostileOrSayQuoteList(pSoldier);
			break;
	}

  if ( pOffendingSoldier )
  {
	  pSoldier->bNextAction = AI_ACTION_CHANGE_FACING;
	  pSoldier->usNextActionData = atan8( pSoldier->sX, pSoldier->sY, pOffendingSoldier->sX, pOffendingSoldier->sY );
  }
}


static void TestPotentialOwner(SOLDIERTYPE* pSoldier)
{
	if (pSoldier->bInSector && pSoldier->bLife >= OKLIFE)
	{
		if ( SoldierToSoldierLineOfSightTest( pSoldier, gpTempSoldier, (UINT8) DistanceVisible( pSoldier, DIRECTION_IRRELEVANT, 0, gpTempSoldier->sGridNo, gpTempSoldier->bLevel ), TRUE ) )
		{
			MakeNPCGrumpyForMinorOffense( pSoldier, gpTempSoldier );
		}
	}
}


static void CheckForPickedOwnership(void)
{
	// LOOP THROUGH LIST TO FIND NODE WE WANT
	const ITEM_POOL* pItemPool = GetItemPool(gsTempGridno, gpTempSoldier->bLevel);

	while( pItemPool )
	{
		const OBJECTTYPE* const o = &GetWorldItem(pItemPool->iItemIndex)->o;
		if (o->usItem == OWNERSHIP)
		{
			if (o->ubOwnerProfile != NO_PROFILE)
			{
				SOLDIERTYPE* const pSoldier = FindSoldierByProfileID(o->ubOwnerProfile);
				if ( pSoldier )
				{
					TestPotentialOwner( pSoldier );
				}
			}
			const UINT8 ubCivGroup = o->ubOwnerCivGroup;
			if (ubCivGroup != NON_CIV_GROUP)
			{
				if ( ubCivGroup == HICKS_CIV_GROUP && CheckFact( FACT_HICKS_MARRIED_PLAYER_MERC, 0 ) )
				{
					// skip because hicks appeased
					pItemPool = pItemPool->pNext;
					continue;
				}
				FOR_ALL_IN_TEAM(s, CIV_TEAM)
				{
					if (s->ubCivilianGroup == ubCivGroup)
					{
						TestPotentialOwner(s);
					}
				}
			}
		}
		pItemPool = pItemPool->pNext;
	}

}


static void LoopLevelNodeForItemGlowFlag(LEVELNODE* pNode, BOOLEAN fOn)
{
	for (; pNode != NULL; pNode = pNode->pNext)
	{
		if (!(pNode->uiFlags & LEVELNODE_ITEM)) continue;

		pNode->uiFlags &= ~LEVELNODE_DYNAMIC;
		pNode->uiFlags |= (fOn ? LEVELNODE_DYNAMIC : 0);
	}
}


void ToggleItemGlow(BOOLEAN fOn)
{
	for (UINT32 cnt = 0; cnt < WORLD_MAX; ++cnt)
	{
		MAP_ELEMENT* const e = &gpWorldLevelData[cnt];
		LoopLevelNodeForItemGlowFlag(e->pStructHead, fOn);
		LoopLevelNodeForItemGlowFlag(e->pOnRoofHead, fOn);
	}

	gGameSettings.fOptions[TOPTION_GLOW_ITEMS] = fOn;

	SetRenderFlags(RENDER_FLAG_FULL);
}


BOOLEAN ContinuePastBoobyTrapInMapScreen( OBJECTTYPE *pObject, SOLDIERTYPE *pSoldier )
{
	BOOLEAN					fBoobyTrapKnowledge;
	INT8						bTrapDifficulty, bTrapDetectLevel;

	if (pObject->bTrap > 0)
	{
		if (pSoldier->bTeam == gbPlayerNum)
		{
			// does the player know about this item?
			fBoobyTrapKnowledge = ((pObject->fFlags & OBJECT_KNOWN_TO_BE_TRAPPED) > 0);

			// blue flag stuff?

			if (!fBoobyTrapKnowledge)
			{
				bTrapDifficulty = pObject->bTrap;
				bTrapDetectLevel = CalcTrapDetectLevel( pSoldier, FALSE );
				if (bTrapDetectLevel >= bTrapDifficulty)
				{
					// spotted the trap!
					pObject->fFlags |= OBJECT_KNOWN_TO_BE_TRAPPED;
					fBoobyTrapKnowledge = TRUE;

					// Make him warn us:
					gpBoobyTrapSoldier = pSoldier;

					// And make the call for the dialogue
					SetStopTimeQuoteCallback( BoobyTrapDialogueCallBack );
					TacticalCharacterDialogue( pSoldier, QUOTE_BOOBYTRAP_ITEM );

					return( FALSE );
				}
			}

			if (fBoobyTrapKnowledge)
			{
				// have the computer ask us if we want to proceed
				gpBoobyTrapSoldier = pSoldier;
				gbTrapDifficulty = pObject->bTrap;
				DoMessageBox( MSG_BOX_BASIC_STYLE, TacticalStr[ DISARM_BOOBYTRAP_PROMPT ], MAP_SCREEN, ( UINT8 )MSG_BOX_FLAG_YESNO, BoobyTrapInMapScreenMessageBoxCallBack, NULL );
			}
			else
			{
				// oops!
				SetOffBoobyTrapInMapScreen( pSoldier, pObject );
			}

			return( FALSE );

		}
		// else, enemies etc always know about boobytraps and are not affected by them
	}

	return( TRUE );
}


// Well, clears all item pools
static void ClearAllItemPools(void)
{
	UINT32 cnt;

	for ( cnt = 0; cnt < WORLD_MAX; cnt++ )
	{
		RemoveItemPool( (INT16)cnt, 0 );
		RemoveItemPool( (INT16)cnt, 1 );
	}
}


// Refresh item pools
void RefreshItemPools(const WORLDITEM* const pItemList, const INT32 iNumberOfItems)
{
	ClearAllItemPools( );

	RefreshWorldItemsIntoItemPools(  pItemList, iNumberOfItems );
}


INT16 FindNearestAvailableGridNoForItem( INT16 sSweetGridNo, INT8 ubRadius )
{
	INT16  sTop, sBottom;
	INT16  sLeft, sRight;
	INT16  cnt1, cnt2;
	INT16		sGridNo;
	INT32		uiRange, uiLowestRange = 999999;
	INT16		sLowestGridNo=0;
	INT32					leftmost;
	BOOLEAN	fFound = FALSE;
	SOLDIERTYPE soldier;
	UINT8 ubSaveNPCAPBudget;
	UINT8 ubSaveNPCDistLimit;

	//Save AI pathing vars.  changing the distlimit restricts how
	//far away the pathing will consider.
	ubSaveNPCAPBudget = gubNPCAPBudget;
	ubSaveNPCDistLimit = gubNPCDistLimit;
	gubNPCAPBudget = 0;
	gubNPCDistLimit = ubRadius;

	//create dummy soldier, and use the pathing to determine which nearby slots are
	//reachable.
	memset( &soldier, 0, sizeof( SOLDIERTYPE ) );
	soldier.bTeam = 1;
	soldier.sGridNo = sSweetGridNo;

	sTop		= ubRadius;
	sBottom = -ubRadius;
	sLeft   = - ubRadius;
	sRight  = ubRadius;

	//clear the mapelements of potential residue MAPELEMENT_REACHABLE flags
	//in the square region.
	for( cnt1 = sBottom; cnt1 <= sTop; cnt1++ )
	{
		for( cnt2 = sLeft; cnt2 <= sRight; cnt2++ )
		{
			sGridNo = sSweetGridNo + (WORLD_COLS * cnt1) + cnt2;
			if ( sGridNo >=0 && sGridNo < WORLD_MAX )
			{
				gpWorldLevelData[ sGridNo ].uiFlags &= (~MAPELEMENT_REACHABLE);
			}
		}
	}

	//Now, find out which of these gridnos are reachable
	//(use the fake soldier and the pathing settings)
	FindBestPath( &soldier, NOWHERE, 0, WALKING, COPYREACHABLE, 0 );

	uiLowestRange = 999999;

	for( cnt1 = sBottom; cnt1 <= sTop; cnt1++ )
	{
		leftmost = ( ( sSweetGridNo + ( WORLD_COLS * cnt1 ) )/ WORLD_COLS ) * WORLD_COLS;

		for( cnt2 = sLeft; cnt2 <= sRight; cnt2++ )
		{
			sGridNo = sSweetGridNo + ( WORLD_COLS * cnt1 ) + cnt2;
			if ( sGridNo >=0 && sGridNo < WORLD_MAX && sGridNo >= leftmost && sGridNo < ( leftmost + WORLD_COLS ) &&
				gpWorldLevelData[ sGridNo ].uiFlags & MAPELEMENT_REACHABLE )
			{
				// Go on sweet stop
				if ( NewOKDestination( &soldier, sGridNo, TRUE, soldier.bLevel ) )
				{
					uiRange = GetRangeInCellCoordsFromGridNoDiff( sSweetGridNo, sGridNo );

					if ( uiRange < uiLowestRange )
					{
						sLowestGridNo = sGridNo;
						uiLowestRange = uiRange;
						fFound = TRUE;
					}
				}
			}
		}
	}
	gubNPCAPBudget = ubSaveNPCAPBudget;
	gubNPCDistLimit = ubSaveNPCDistLimit;
	if ( fFound )
	{
		return sLowestGridNo;
	}
	return NOWHERE;
}


BOOLEAN CanPlayerUseRocketRifle(SOLDIERTYPE* const s, const BOOLEAN fDisplay)
{
	const OBJECTTYPE* const wpn = &s->inv[s->ubAttackingHand];
	if (wpn->usItem == ROCKET_RIFLE || wpn->usItem == AUTO_ROCKET_RIFLE)
  {
    // check imprint ID
    // NB not-imprinted value is NO_PROFILE
    // imprinted value is profile for mercs & NPCs and NO_PROFILE + 1 for generic dudes
    if (s->ubProfile     != NO_PROFILE   &&
				wpn->ubImprintID != s->ubProfile &&
				wpn->ubImprintID != NO_PROFILE) // NOT a virgin gun...
		{
			// access denied!
			if (s->bTeam == gbPlayerNum)
			{
				PlayJA2Sample(RG_ID_INVALID, HIGHVOLUME, 1, MIDDLE);
				if (fDisplay)
				{
					ScreenMsg(FONT_MCOLOR_LTYELLOW, MSG_UI_FEEDBACK, L"\"%ls\"", TacticalStr[GUN_NOGOOD_FINGERPRINT]);
				}
			}
			return FALSE;
		}
  }
  return TRUE;
}
