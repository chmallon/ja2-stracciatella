#include "Laptop.h"
#include "CharProfile.h"
#include "Utilities.h"
#include "WCheck.h"
#include "Debug.h"
#include "Render_Dirty.h"
#include "IMP_MainPage.h"
#include "IMP_Begin_Screen.h"
#include "IMP_Personality_Entrance.h"
#include "IMP_Personality_Quiz.h"
#include "IMP_Personality_Finish.h"
#include "IMP_Attribute_Selection.h"
#include "IMP_Portraits.h"
#include "IMP_Compile_Character.h"
#include "IMP_Voices.h"
#include "IMP_Finish.h"
#include "Soldier_Profile_Type.h"
#include "Soldier_Profile.h"
#include "Animation_Data.h"
#include "Random.h"
#include "LaptopSave.h"


#define ATTITUDE_LIST_SIZE 20


static INT32 AttitudeList[ATTITUDE_LIST_SIZE];
static INT32 iLastElementInAttitudeList = 0;

static INT32 SkillsList[ATTITUDE_LIST_SIZE];
static INT32 iLastElementInSkillsList = 0;

static INT32 PersonalityList[ATTITUDE_LIST_SIZE];
static INT32 iLastElementInPersonalityList = 0;

extern BOOLEAN fLoadingCharacterForPreviousImpProfile;


const char *pPlayerSelectedFaceFileNames[ NUMBER_OF_PLAYER_PORTRAITS ]=
{
	"Faces/200.sti",
	"Faces/201.sti",
	"Faces/202.sti",
	"Faces/203.sti",
	"Faces/204.sti",
	"Faces/205.sti",
	"Faces/206.sti",
	"Faces/207.sti",
	"Faces/208.sti",
	"Faces/209.sti",
	"Faces/210.sti",
	"Faces/211.sti",
	"Faces/212.sti",
	"Faces/213.sti",
	"Faces/214.sti",
	"Faces/215.sti",
};

const char *pPlayerSelectedBigFaceFileNames[ NUMBER_OF_PLAYER_PORTRAITS ]=
{
	"Faces/BigFaces/200.sti",
	"Faces/BigFaces/201.sti",
	"Faces/BigFaces/202.sti",
	"Faces/BigFaces/203.sti",
	"Faces/BigFaces/204.sti",
	"Faces/BigFaces/205.sti",
	"Faces/BigFaces/206.sti",
	"Faces/BigFaces/207.sti",
	"Faces/BigFaces/208.sti",
	"Faces/BigFaces/209.sti",
	"Faces/BigFaces/210.sti",
	"Faces/BigFaces/211.sti",
	"Faces/BigFaces/212.sti",
	"Faces/BigFaces/213.sti",
	"Faces/BigFaces/214.sti",
	"Faces/BigFaces/215.sti",
};


static void SelectMercFace(void);


void CreateACharacterFromPlayerEnteredStats(void)
{
	MERCPROFILESTRUCT* Merc = &gMercProfiles[PLAYER_GENERATED_CHARACTER_ID + LaptopSaveInfo.iVoiceId];

	wcscpy(Merc->zName,     pFullName);
	wcscpy(Merc->zNickname, pNickName);

	Merc->bSex = fCharacterIsMale ? MALE : FEMALE;

	Merc->bLifeMax    = iHealth;
	Merc->bLife       = iHealth;
	Merc->bAgility    = iAgility;
	Merc->bStrength   = iStrength;
	Merc->bDexterity  = iDexterity;
	Merc->bWisdom     = iWisdom;
	Merc->bLeadership = iLeadership;

	Merc->bMarksmanship = iMarksmanship;
	Merc->bMedical      = iMedical;
	Merc->bMechanical   = iMechanical;
	Merc->bExplosive    = iExplosives;

	Merc->bPersonalityTrait = iPersonality;

	Merc->bAttitude = iAttitude;

	Merc->bExpLevel = 1;

	// set time away
	Merc->bMercStatus = 0;

	SelectMercFace();
}


static BOOLEAN DoesCharacterHaveAnAttitude(void)
{
	// simply checks if caracter has an attitude other than normal
	switch (iAttitude)
	{
		case ATT_LONER:
		case ATT_PESSIMIST:
		case ATT_ARROGANT:
		case ATT_BIG_SHOT:
		case ATT_ASSHOLE:
		case ATT_COWARD:
			return TRUE;

		default:
			return FALSE;
	}
}


static BOOLEAN DoesCharacterHaveAPersoanlity(void)
{
	// only one we can get is PSYCHO, and that is not much of a penalty
	return( FALSE );
	/*
	// simply checks if caracter has a personality other than normal
  if( iPersonality != NO_PERSONALITYTRAIT )
	{
		// yep
	  return ( TRUE );
	}
	else
	{
		// nope
		return ( FALSE );
	}
	*/
}


static void CreatePlayerAttitude(void)
{
  // this function will 'roll a die' and decide if any attitude does exists
	INT32	iAttitudeHits[NUM_ATTITUDES] = { 0 };

	iAttitude = ATT_NORMAL;

	if (iLastElementInAttitudeList == 0)
	{
		return;
	}

	// count # of hits for each attitude
	for (INT32 i = 0; i < iLastElementInAttitudeList; i++)
	{
		iAttitudeHits[AttitudeList[i]]++;
	}

	// find highest # of hits for any attitude
	INT32	iHighestHits = 0;
	INT32	iNumAttitudesWithHighestHits = 0;
	for (INT32 i = 0; i < NUM_ATTITUDES; i++)
	{
		if (iAttitudeHits[i])
		{
			if (iAttitudeHits[i] > iHighestHits)
			{
				iHighestHits = iAttitudeHits[i];
				iNumAttitudesWithHighestHits = 1;
			}
			else if (iAttitudeHits[i] == iHighestHits)
			{
				iNumAttitudesWithHighestHits++;
			}
		}
	}

	INT32 iDiceValue = Random(iNumAttitudesWithHighestHits + 1); // XXX TODO0008

	// find attitude
	INT32 iCounter2 = 0;
	for (INT32 i = 0; i < NUM_ATTITUDES; i++)
	{
		if (iAttitudeHits[i] == iHighestHits)
		{
			if (iCounter2 == iDiceValue)
			{
				// this is it!
				iAttitude = iCounter2;
				break;
			}
			else
			{
				// one of the next attitudes...
				iCounter2++;
			}
		}
	}
}


void AddAnAttitudeToAttitudeList( INT8 bAttitude )
{
	// adds an attitude to attitude list
	if (iLastElementInAttitudeList < ATTITUDE_LIST_SIZE)
	{
		AttitudeList[iLastElementInAttitudeList++] = bAttitude;
	}
}


void AddSkillToSkillList( INT8 bSkill )
{
	// adds a skill to skills list
	if (iLastElementInSkillsList < ATTITUDE_LIST_SIZE)
	{
		SkillsList[iLastElementInSkillsList++] = bSkill;
	}
}


static void RemoveSkillFromSkillsList(INT32 Skill)
{
	size_t d = 0;
	for (size_t s = 0; s < iLastElementInSkillsList; s++)
	{
		if (SkillsList[s] == Skill)
		{
			iLastElementInSkillsList--;
		}
		else
		{
			SkillsList[d++] = SkillsList[s];
		}
	}
}


static void ValidateSkillsList(void)
{
	// remove from the generated traits list any traits that don't match
	// the character's skills
	MERCPROFILESTRUCT* pProfile = &gMercProfiles[PLAYER_GENERATED_CHARACTER_ID + LaptopSaveInfo.iVoiceId];

	if (pProfile->bMechanical == 0)
	{
		// without mechanical, electronics is useless
		RemoveSkillFromSkillsList(ELECTRONICS);
	}

	// special check for lockpicking
	INT32 iValue = pProfile->bMechanical;
	iValue = iValue * pProfile->bWisdom    / 100;
	iValue = iValue * pProfile->bDexterity / 100;
	if (iValue + gbSkillTraitBonus[LOCKPICKING] < 50)
	{
		// not good enough for lockpicking!
		RemoveSkillFromSkillsList(LOCKPICKING);
	}

	if (pProfile->bMarksmanship == 0)
	{
		// without marksmanship, the following traits are useless:
		RemoveSkillFromSkillsList(AUTO_WEAPS);
		RemoveSkillFromSkillsList(HEAVY_WEAPS);
	}
}


static void CreatePlayerSkills(void)
{
	ValidateSkillsList();

	iSkillA = SkillsList[Random(iLastElementInSkillsList)];

	// there is no expert level these skills
	if (iSkillA == ELECTRONICS || iSkillA == AMBIDEXT) RemoveSkillFromSkillsList(iSkillA);

	if (iLastElementInSkillsList == 0)
	{
		iSkillB = NO_SKILLTRAIT;
	}
	else
	{
		iSkillB = SkillsList[Random(iLastElementInSkillsList)];
	}
}


void AddAPersonalityToPersonalityList(INT8 bPersonality)
{
	// CJC, Oct 26 98: prevent personality list from being generated
	// because no dialogue was written to support PC personality quotes

	// BUT we can manage this for PSYCHO okay
	if (bPersonality != PSYCHO) return;

  // will add a persoanlity to persoanlity list
  if (iLastElementInPersonalityList < ATTITUDE_LIST_SIZE)
	{
		PersonalityList[iLastElementInPersonalityList++] = bPersonality;
	}
}


static void CreatePlayerPersonality(void)
{
	// only psycho is available since we have no quotes
	// SO if the array is not empty, give them psycho!
	if (iLastElementInPersonalityList == 0)
	{
		iPersonality = NO_PERSONALITYTRAIT;
	}
	else
	{
		iPersonality = PSYCHO;
	}

/*
  // this function will 'roll a die' and decide if any Personality does exists
  INT32 iDiceValue = 0;
  INT32 iCounter = 0;
	INT32 iSecondAttempt = -1;

	// roll dice
	iDiceValue = Random( iLastElementInPersonalityList + 1 );

	iPersonality = NO_PERSONALITYTRAIT;
  if( PersonalityList[ iDiceValue ] !=  NO_PERSONALITYTRAIT )
	{
		for( iCounter = 0; iCounter < iLastElementInPersonalityList; iCounter++ )
		{
			if( iCounter != iDiceValue )
			{
				if( PersonalityList[ iCounter ] == PersonalityList[ iDiceValue ] )
				{
					if( PersonalityList[ iDiceValue ] != PSYCHO )
					{
            iPersonality = PersonalityList[ iDiceValue ];
					}
					else
					{
            iSecondAttempt = iCounter;
					}
					if( iSecondAttempt != iCounter )
					{
						iPersonality = PersonalityList[ iDiceValue ];
					}

				}
			}
		}
	}
*/
}


void CreatePlayersPersonalitySkillsAndAttitude( void )
{
	// creates personality and attitudes from curretly built list
	CreatePlayerPersonality();
	CreatePlayerAttitude();
}


void ResetSkillsAttributesAndPersonality( void )
{
	// reset count of skills attributes and personality
	iLastElementInPersonalityList = 0;
	iLastElementInSkillsList      = 0;
	iLastElementInAttitudeList    = 0;
}


void ResetIncrementCharacterAttributes( void )
{
	// this resets any increments due to character generation
	iAddStrength   = 0;
	iAddDexterity  = 0;
	iAddWisdom     = 0;
	iAddAgility    = 0;
	iAddHealth     = 0;
	iAddLeadership = 0;

	iAddMarksmanship = 0;
	iAddExplosives   = 0;
	iAddMedical      = 0;
	iAddMechanical   = 0;
}


static void SetMercSkinAndHairColors(void);


static void SelectMercFace(void)
{
	// this procedure will select the approriate face for the merc and save offsets
	MERCPROFILESTRUCT* Merc = &gMercProfiles[PLAYER_GENERATED_CHARACTER_ID + LaptopSaveInfo.iVoiceId];

	// now the offsets
  Merc->ubFaceIndex = 200 + iPortraitNumber;

	// eyes
	Merc->usEyesX = 0;
	Merc->usEyesY = 0;

	// mouth
	Merc->usMouthX = 0;
	Merc->usMouthY = 0;

	// set merc skins and hair color
	SetMercSkinAndHairColors();
}


static void SetMercSkinAndHairColors(void)
{
#	define PINKSKIN  "PINKSKIN"
#	define TANSKIN   "TANSKIN"
#	define DARKSKIN  "DARKSKIN"
#	define BLACKSKIN "BLACKSKIN"

#	define BROWNHEAD "BROWNHEAD"
#	define BLACKHEAD "BLACKHEAD" // black skin till here
#	define WHITEHEAD "WHITEHEAD" // dark skin till here
#	define BLONDHEAD "BLONDHEAD"
#	define REDHEAD   "REDHEAD"   // pink/tan skin till here

	static const struct
	{
		const char* Skin;
		const char* Hair;
	} Colors[] =
	{
		{ BLACKSKIN, BROWNHEAD },
		{ TANSKIN,   BROWNHEAD },
		{ TANSKIN,   BROWNHEAD },
		{ DARKSKIN,  BROWNHEAD },
		{ TANSKIN,   BROWNHEAD },
		{ DARKSKIN,  BLACKHEAD },
		{ TANSKIN,   BROWNHEAD },
		{ TANSKIN,   BROWNHEAD },
		{ TANSKIN,   BROWNHEAD },
		{ PINKSKIN,  BROWNHEAD },
		{ TANSKIN,   BLACKHEAD },
		{ TANSKIN,   BLACKHEAD },
		{ PINKSKIN,  BROWNHEAD },
		{ BLACKSKIN, BROWNHEAD },
		{ TANSKIN,   REDHEAD   },
		{ TANSKIN,   BLONDHEAD }
	};

	Assert(iPortraitNumber < lengthof(Colors));
	MERCPROFILESTRUCT* Merc = &gMercProfiles[PLAYER_GENERATED_CHARACTER_ID + LaptopSaveInfo.iVoiceId];
	strcpy(Merc->HAIR, Colors[iPortraitNumber].Hair);
	strcpy(Merc->SKIN, Colors[iPortraitNumber].Skin);
}


static BOOLEAN ShouldThisMercHaveABigBody(void);


void HandleMercStatsForChangesInFace(void)
{
	if (fLoadingCharacterForPreviousImpProfile) return;

	CreatePlayerSkills();

	MERCPROFILESTRUCT* Merc = &gMercProfiles[PLAYER_GENERATED_CHARACTER_ID + LaptopSaveInfo.iVoiceId];

	// body type
	if (fCharacterIsMale)
	{
		if (ShouldThisMercHaveABigBody())
		{
			Merc->ubBodyType = BIGMALE;
			if (iSkillA == MARTIALARTS) iSkillA = HANDTOHAND;
			if (iSkillB == MARTIALARTS) iSkillB = HANDTOHAND;
		}
		else
		{
			Merc->ubBodyType = REGMALE;
    }
	}
	else
	{
		Merc->ubBodyType = REGFEMALE;
		if (iSkillA == MARTIALARTS) iSkillA = HANDTOHAND;
		if (iSkillB == MARTIALARTS) iSkillB = HANDTOHAND;
	}

	// skill trait
	Merc->bSkillTrait  = iSkillA;
	Merc->bSkillTrait2 = iSkillB;
}


static BOOLEAN ShouldThisMercHaveABigBody(void)
{
	// should this merc be a big body typ
	return
		(iPortraitNumber == 0 || iPortraitNumber == 6 || iPortraitNumber == 7) &&
		gMercProfiles[PLAYER_GENERATED_CHARACTER_ID + LaptopSaveInfo.iVoiceId].bStrength >= 75;
}
