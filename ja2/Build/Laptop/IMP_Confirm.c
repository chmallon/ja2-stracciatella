#ifdef PRECOMPILEDHEADERS
	#include "Laptop All.h"
#else
	#include "CharProfile.h"
	#include "IMP_MainPage.h"
	#include "IMP_HomePage.h"
	#include "IMPVideoObjects.h"
	#include "Utilities.h"
	#include "WCheck.h"
	#include "Debug.h"
	#include "WordWrap.h"
	#include "Render_Dirty.h"
	#include "Encrypted_File.h"
	#include "Cursors.h"
	#include "Laptop.h"
	#include "IMP_Compile_Character.h"
	#include "Soldier_Profile_Type.h"
	#include "IMP_Text_System.h"
	#include "IMP_Confirm.h"
	#include "Finances.h"
	#include "Soldier_Profile.h"
	#include "Soldier_Profile_Type.h"
	#include "Soldier_Control.h"
	#include "IMP_Portraits.h"
	#include "IMP_Voices.h"
	#include "Overhead.h"
	#include "Finances.h"
	#include "History.h"
	#include "Game_Clock.h"
	#include "EMail.h"
	#include "Game_Event_Hook.h"
	#include "LaptopSave.h"
	#include "Strategic.h"
#endif

#define IMP_MERC_FILE "IMP.dat"

UINT32 giIMPConfirmButton[ 2 ];
UINT32 giIMPConfirmButtonImage[ 2 ];
BOOLEAN fNoAlreadySelected = FALSE;
UINT16 uiEyeXPositions[ ]={
	8,
	9,
	8,
	6,
	13,
	11,
	8,
	8,
	4,				//208
	5,				//209
	7,
	5,				//211
	7,
	11,
	8,				//214
	5,
};

UINT16 uiEyeYPositions[ ]=
{
	5,
	4,
	5,
	6,
	5,
	5,
	4,
	4,
	4,		//208
	5,
	5,		//210
	7,
	6,		//212
	5,
	5,		//214
	6,
};

UINT16 uiMouthXPositions[]=
{
	8,
	9,
	7,
	7,
	11,
	10,
	8,
	8,
	5,		//208
	6,
	7,		//210
	6,
	7,		//212
	9,
	7,		//214
	5,
};

UINT16 uiMouthYPositions[]={
	21,
	23,
	24,
	25,
	23,
	24,
	24,
	24,
	25,		//208
	24,
	24,		//210
	26,
	24,		//212
	23,
	24,		//214
	26,
};

BOOLEAN fLoadingCharacterForPreviousImpProfile = FALSE;

void CreateConfirmButtons( void );
void DestroyConfirmButtons( void );
void GiveItemsToPC( UINT8 ubProfileId );
void MakeProfileInvItemAnySlot(MERCPROFILESTRUCT *pProfile, UINT16 usItem, UINT8 ubStatus, UINT8 ubHowMany);
void MakeProfileInvItemThisSlot(MERCPROFILESTRUCT *pProfile, UINT32 uiPos, UINT16 usItem, UINT8 ubStatus, UINT8 ubHowMany);
INT32 FirstFreeBigEnoughPocket(MERCPROFILESTRUCT *pProfile, UINT16 usItem);

// callbacks
void BtnIMPConfirmNo( GUI_BUTTON *btn,INT32 reason );
void BtnIMPConfirmYes( GUI_BUTTON *btn,INT32 reason );


void EnterIMPConfirm( void )
{
	// create buttons
	CreateConfirmButtons( );
	return;
}

void RenderIMPConfirm( void )
{

	 // the background
	RenderProfileBackGround( );

		// indent
  RenderAvgMercIndentFrame(90, 40 );

	// highlight answer
  PrintImpText( );

	return;
}

void ExitIMPConfirm( void )
{

	// destroy buttons
  DestroyConfirmButtons( );
	return;
}

void HandleIMPConfirm( void )
{
	return;
}

void CreateConfirmButtons( void )
{
	// create buttons for confirm screen

	giIMPConfirmButtonImage[0]=  LoadButtonImage( "LAPTOP\\button_2.sti" ,-1,0,-1,1,-1 );
	giIMPConfirmButton[0] = CreateIconAndTextButton( giIMPConfirmButtonImage[0], pImpButtonText[ 16 ], FONT12ARIAL,
														 FONT_WHITE, DEFAULT_SHADOW,
														 FONT_WHITE, DEFAULT_SHADOW,
														 TEXT_CJUSTIFIED,
														 LAPTOP_SCREEN_UL_X +  ( 136 ), LAPTOP_SCREEN_WEB_UL_Y + ( 254 ), BUTTON_TOGGLE, MSYS_PRIORITY_HIGH,
														 BtnGenericMouseMoveButtonCallback, (GUI_CALLBACK)BtnIMPConfirmYes);

	giIMPConfirmButtonImage[1]=  LoadButtonImage( "LAPTOP\\button_2.sti" ,-1,0,-1,1,-1 );
	giIMPConfirmButton[1] = CreateIconAndTextButton( giIMPConfirmButtonImage[ 1 ], pImpButtonText[ 17 ], FONT12ARIAL,
														 FONT_WHITE, DEFAULT_SHADOW,
														 FONT_WHITE, DEFAULT_SHADOW,
														 TEXT_CJUSTIFIED,
														 LAPTOP_SCREEN_UL_X +  ( 136 ), LAPTOP_SCREEN_WEB_UL_Y + ( 314 ), BUTTON_TOGGLE, MSYS_PRIORITY_HIGH,
														 BtnGenericMouseMoveButtonCallback, (GUI_CALLBACK)BtnIMPConfirmNo);

 SetButtonCursor(giIMPConfirmButton[ 0 ], CURSOR_WWW);
 SetButtonCursor(giIMPConfirmButton[ 1 ], CURSOR_WWW);

	return;
}


void DestroyConfirmButtons( void )
{
  // destroy buttons for confirm screen

  RemoveButton(giIMPConfirmButton[ 0 ] );
  UnloadButtonImage(giIMPConfirmButtonImage[ 0 ] );


	RemoveButton(giIMPConfirmButton[ 1 ] );
  UnloadButtonImage(giIMPConfirmButtonImage[ 1 ] );
	return;
}



BOOLEAN AddCharacterToPlayersTeam( void )
{

	MERC_HIRE_STRUCT HireMercStruct;


	// last minute chage to make sure merc with right facehas not only the right body but body specific skills...
	// ie..small mercs have martial arts..but big guys and women don't don't

	HandleMercStatsForChangesInFace( );

	memset(&HireMercStruct, 0, sizeof(MERC_HIRE_STRUCT));

	HireMercStruct.ubProfileID = ( UINT8 )( PLAYER_GENERATED_CHARACTER_ID + LaptopSaveInfo.iVoiceId ) ;

	if( fLoadingCharacterForPreviousImpProfile == FALSE )
	{
		// give them items
		GiveItemsToPC( 	HireMercStruct.ubProfileID );
	}


	HireMercStruct.sSectorX									 = gsMercArriveSectorX;
	HireMercStruct.sSectorY									 = gsMercArriveSectorY;
	HireMercStruct.fUseLandingZoneForArrival = TRUE;

	HireMercStruct.fCopyProfileItemsOver = TRUE;

	// indefinite contract length
	HireMercStruct.iTotalContractLength = -1;

	HireMercStruct.ubInsertionCode	= INSERTION_CODE_ARRIVING_GAME;
	HireMercStruct.uiTimeTillMercArrives = GetMercArrivalTimeOfDay( );

	SetProfileFaceData( HireMercStruct.ubProfileID , ( UINT8 ) ( 200 + iPortraitNumber ), uiEyeXPositions[ iPortraitNumber ], uiEyeYPositions[ iPortraitNumber ], uiMouthXPositions[ iPortraitNumber ], uiMouthYPositions[ iPortraitNumber ] );

	//if we succesfully hired the merc
	if( !HireMerc( &HireMercStruct ) )
	{
		return(FALSE);
	}
	else
	{
		return ( TRUE );
	}
}


void  BtnIMPConfirmYes(GUI_BUTTON *btn,INT32 reason)
{

	// btn callback for IMP Homepage About US button
	if (!(btn->uiFlags & BUTTON_ENABLED))
		return;

	if(reason & MSYS_CALLBACK_REASON_LBUTTON_DWN )
	{
		 btn->uiFlags|=(BUTTON_CLICKED_ON);
	}
	else if(reason & MSYS_CALLBACK_REASON_LBUTTON_UP )
	{
		if( btn->uiFlags & BUTTON_CLICKED_ON )
		{

			// reset button
			btn->uiFlags&=~(BUTTON_CLICKED_ON);

			if( LaptopSaveInfo.fIMPCompletedFlag )
			{
				// already here, leave
				return;
			}

			if( LaptopSaveInfo.iCurrentBalance < COST_OF_PROFILE )
			{
				// not enough
		    return;
			}

			// line moved by CJC Nov 28 2002 to AFTER the check for money
			LaptopSaveInfo.fIMPCompletedFlag = TRUE;

			// charge the player
			AddTransactionToPlayersBook(IMP_PROFILE, (UINT8)(PLAYER_GENERATED_CHARACTER_ID + LaptopSaveInfo.iVoiceId), GetWorldTotalMin( ), - ( COST_OF_PROFILE ) );
      AddHistoryToPlayersLog( HISTORY_CHARACTER_GENERATED, 0,GetWorldTotalMin( ), -1, -1 );
			AddCharacterToPlayersTeam( );

			// write the created imp merc
			WriteOutCurrentImpCharacter( ( UINT8 )( PLAYER_GENERATED_CHARACTER_ID + LaptopSaveInfo.iVoiceId ) );

			fButtonPendingFlag = TRUE;
			iCurrentImpPage = IMP_HOME_PAGE;

			// send email notice
			//AddEmail(IMP_EMAIL_PROFILE_RESULTS, IMP_EMAIL_PROFILE_RESULTS_LENGTH, IMP_PROFILE_RESULTS, GetWorldTotalMin( ) );
			AddFutureDayStrategicEvent( EVENT_DAY2_ADD_EMAIL_FROM_IMP, 60 * 7, 0, 2 );
			//RenderCharProfile( );

			ResetCharacterStats();

			//Display a popup msg box telling the user when and where the merc will arrive
			//DisplayPopUpBoxExplainingMercArrivalLocationAndTime( PLAYER_GENERATED_CHARACTER_ID + LaptopSaveInfo.iVoiceId );

			//reset the id of the last merc so we dont get the DisplayPopUpBoxExplainingMercArrivalLocationAndTime() pop up box in another screen by accident
			LaptopSaveInfo.sLastHiredMerc.iIdOfMerc = -1;
		}
	}

}

// fixed? by CJC Nov 28 2002
void BtnIMPConfirmNo( GUI_BUTTON *btn,INT32 reason )
{
  	// btn callback for IMP Homepage About US button
	if (!(btn->uiFlags & BUTTON_ENABLED))
		return;

	if(reason & MSYS_CALLBACK_REASON_LBUTTON_DWN )
	{
		 btn->uiFlags|=(BUTTON_CLICKED_ON);
	}
	else if(reason & MSYS_CALLBACK_REASON_LBUTTON_UP )
	{
		if( btn->uiFlags & BUTTON_CLICKED_ON )
		{

			iCurrentImpPage = IMP_FINISH;

			/*

			LaptopSaveInfo.fIMPCompletedFlag = FALSE;
			ResetCharacterStats();

			fButtonPendingFlag = TRUE;
			iCurrentImpPage = IMP_HOME_PAGE;
			*/
			/*
			if( fNoAlreadySelected == TRUE )
			{
				// already selected no
				fButtonPendingFlag = TRUE;
				iCurrentImpPage = IMP_HOME_PAGE;
			}
      fNoAlreadySelected = TRUE;
			*/
      btn->uiFlags&=~(BUTTON_CLICKED_ON);
		}
	}
}

/*
void BtnIMPConfirmNo( GUI_BUTTON *btn,INT32 reason )
{


  	// btn callback for IMP Homepage About US button
	if (!(btn->uiFlags & BUTTON_ENABLED))
		return;

	if(reason & MSYS_CALLBACK_REASON_LBUTTON_DWN )
	{
		 btn->uiFlags|=(BUTTON_CLICKED_ON);
	}
	else if(reason & MSYS_CALLBACK_REASON_LBUTTON_UP )
	{
		if( btn->uiFlags & BUTTON_CLICKED_ON )
		{
			LaptopSaveInfo.fIMPCompletedFlag = TRUE;
			if( fNoAlreadySelected == TRUE )
			{
				// already selected no
				fButtonPendingFlag = TRUE;
				iCurrentImpPage = IMP_HOME_PAGE;
			}
      fNoAlreadySelected = TRUE;
      btn->uiFlags&=~(BUTTON_CLICKED_ON);
		}
	}
}
*/


#define PROFILE_HAS_SKILL_TRAIT( p, t ) ((p->bSkillTrait == t) || (p->bSkillTrait2 == t))


void GiveItemsToPC( UINT8 ubProfileId )
{
	MERCPROFILESTRUCT *pProfile;


  // gives starting items to merc
	// NOTE: Any guns should probably be from those available in regular gun set

	pProfile = &(gMercProfiles[ubProfileId]);


	// STANDARD EQUIPMENT



	// kevlar vest, leggings, & helmet
	MakeProfileInvItemThisSlot(pProfile, VESTPOS, FLAK_JACKET, 100, 1);
	if ( PreRandom( 100 ) < (UINT32) pProfile->bWisdom )
	{
		MakeProfileInvItemThisSlot(pProfile, HELMETPOS, STEEL_HELMET, 100, 1);
	}

	// canteen
	MakeProfileInvItemThisSlot(pProfile, SMALLPOCK4POS, CANTEEN, 100, 1);


	if (pProfile->bMarksmanship >= 80)
	{
		// good shooters get a better & matching ammo
		MakeProfileInvItemThisSlot( pProfile, HANDPOS, MP5K, 100, 1);
		MakeProfileInvItemThisSlot( pProfile, SMALLPOCK1POS, CLIP9_30, 100, 2);
	}
	else
	{
		// Automatic pistol, with matching ammo
		MakeProfileInvItemThisSlot( pProfile, HANDPOS, BERETTA_93R, 100, 1 );
		MakeProfileInvItemThisSlot( pProfile, SMALLPOCK1POS, CLIP9_15, 100, 3 );
	}


	// OPTIONAL EQUIPMENT: depends on skills & special skills

	if (pProfile->bMedical >= 60)
	{
		// strong medics get full medic kit
		MakeProfileInvItemAnySlot(pProfile, MEDICKIT, 100, 1);
	}
	else
	if (pProfile->bMedical >= 30)
	{
		// passable medics get first aid kit
		MakeProfileInvItemAnySlot(pProfile, FIRSTAIDKIT, 100, 1);
	}

	if (pProfile->bMechanical >= 50)
	{
		// mechanics get toolkit
		MakeProfileInvItemAnySlot(pProfile, TOOLKIT, 100, 1);
	}

	if (pProfile->bExplosive >= 50)
	{
		// loonies get TNT & Detonator
		MakeProfileInvItemAnySlot(pProfile, TNT, 100, 1);
		MakeProfileInvItemAnySlot(pProfile, DETONATOR, 100, 1);
	}


	// check for special skills
	if (PROFILE_HAS_SKILL_TRAIT(pProfile, LOCKPICKING) && ( iMechanical ) )
	{
		MakeProfileInvItemAnySlot(pProfile, LOCKSMITHKIT, 100, 1);
	}

	if (PROFILE_HAS_SKILL_TRAIT(pProfile, HANDTOHAND))
	{
		MakeProfileInvItemAnySlot(pProfile, BRASS_KNUCKLES, 100, 1);
	}

	if (PROFILE_HAS_SKILL_TRAIT(pProfile, ELECTRONICS) && ( iMechanical ) )
	{
		MakeProfileInvItemAnySlot(pProfile, METALDETECTOR, 100, 1);
	}

	if (PROFILE_HAS_SKILL_TRAIT(pProfile, NIGHTOPS))
	{
		MakeProfileInvItemAnySlot(pProfile, BREAK_LIGHT, 100, 2);
	}

	if (PROFILE_HAS_SKILL_TRAIT(pProfile, THROWING))
	{
		MakeProfileInvItemAnySlot(pProfile, THROWING_KNIFE, 100, 1);
	}

	if (PROFILE_HAS_SKILL_TRAIT(pProfile, STEALTHY))
	{
		MakeProfileInvItemAnySlot(pProfile, SILENCER, 100, 1);
	}

	if (PROFILE_HAS_SKILL_TRAIT(pProfile, KNIFING))
	{
		MakeProfileInvItemAnySlot(pProfile, COMBAT_KNIFE, 100, 1);
	}

	if (PROFILE_HAS_SKILL_TRAIT(pProfile, CAMOUFLAGED))
	{
		MakeProfileInvItemAnySlot(pProfile, CAMOUFLAGEKIT, 100, 1);
	}

}


void MakeProfileInvItemAnySlot(MERCPROFILESTRUCT *pProfile, UINT16 usItem, UINT8 ubStatus, UINT8 ubHowMany)
{
	INT32 iSlot;

	iSlot = FirstFreeBigEnoughPocket(pProfile, usItem);

	if (iSlot == -1)
	{
		// no room, item not received
		return;
	}

	// put the item into that slot
	MakeProfileInvItemThisSlot(pProfile, iSlot, usItem, ubStatus, ubHowMany);
}


void MakeProfileInvItemThisSlot(MERCPROFILESTRUCT *pProfile, UINT32 uiPos, UINT16 usItem, UINT8 ubStatus, UINT8 ubHowMany)
{
  pProfile->inv[uiPos]				= usItem;
  pProfile->bInvStatus[uiPos] = ubStatus;
  pProfile->bInvNumber[uiPos] = ubHowMany;
}


INT32 FirstFreeBigEnoughPocket(MERCPROFILESTRUCT *pProfile, UINT16 usItem)
{
	UINT32 uiPos;


	// if it fits into a small pocket
	if (Item[usItem].ubPerPocket != 0)
	{
		// check small pockets first
		for (uiPos = SMALLPOCK1POS; uiPos <= SMALLPOCK8POS; uiPos++)
		{
			if (pProfile->inv[uiPos] == NONE)
			{
				return(uiPos);
			}
		}
	}

	// check large pockets
	for (uiPos = BIGPOCK1POS; uiPos <= BIGPOCK4POS; uiPos++)
	{
		if (pProfile->inv[uiPos] == NONE)
		{
			return(uiPos);
		}
	}


	return(-1);
}


void WriteOutCurrentImpCharacter( INT32 iProfileId )
{
	// grab the profile number and write out what is contained there in
	HWFILE hFile;
	UINT32 uiBytesWritten = 0;

	// open the file for writing
	hFile = FileOpen(IMP_MERC_FILE, FILE_ACCESS_WRITE | FILE_CREATE_ALWAYS, FALSE);

	// write out the profile id
	if (!FileWrite(hFile, &iProfileId, sizeof( INT32 ), &uiBytesWritten))
	{
		return;
	}

	// write out the portrait id
	if (!FileWrite(hFile, &iPortraitNumber, sizeof( INT32 ), &uiBytesWritten))
	{
		return;
	}

	// write out the profile itself
	if (!FileWrite(hFile, &gMercProfiles[ iProfileId ], sizeof( MERCPROFILESTRUCT ), &uiBytesWritten))
	{
		return;
	}

	// close file
	FileClose(hFile);

	return;
}

void LoadInCurrentImpCharacter( void )
{
	INT32 iProfileId = 0;
	HWFILE hFile;
	UINT32 uiBytesRead = 0;

	// open the file for writing
	hFile = FileOpen(IMP_MERC_FILE, FILE_ACCESS_READ, FALSE);

	// valid file?
	if( hFile == -1 )
	{
		return;
	}

	// read in the profile
	if (!FileRead(hFile, &iProfileId,sizeof( INT32 ), &uiBytesRead))
	{
		return;
	}

	// read in the portrait
	if (!FileRead(hFile, &iPortraitNumber ,sizeof( INT32 ), &uiBytesRead))
	{
		return;
	}

	// read in the profile
	if (!FileRead(hFile, &gMercProfiles[ iProfileId ] ,sizeof( MERCPROFILESTRUCT ), &uiBytesRead))
	{
		return;
	}

	// close file
	FileClose(hFile);

	if( LaptopSaveInfo.iCurrentBalance < COST_OF_PROFILE )
	{
		// not enough
		return;
	}


	// charge the player
	// is the character male?
	fCharacterIsMale = ( gMercProfiles[ iProfileId ].bSex == MALE );
	fLoadingCharacterForPreviousImpProfile = TRUE;
	AddTransactionToPlayersBook(IMP_PROFILE,0, GetWorldTotalMin( ), - ( COST_OF_PROFILE ) );
  AddHistoryToPlayersLog( HISTORY_CHARACTER_GENERATED, 0,GetWorldTotalMin( ), -1, -1 );
	LaptopSaveInfo.iVoiceId = iProfileId - PLAYER_GENERATED_CHARACTER_ID;
	AddCharacterToPlayersTeam( );
	AddFutureDayStrategicEvent( EVENT_DAY2_ADD_EMAIL_FROM_IMP, 60 * 7, 0, 2 );
	LaptopSaveInfo.fIMPCompletedFlag = TRUE;
	fPausedReDrawScreenFlag = TRUE;
	fLoadingCharacterForPreviousImpProfile = FALSE;

	return;
}



void ResetIMPCharactersEyesAndMouthOffsets( UINT8 ubMercProfileID )
{
  // ATE: Check boundary conditions!
  if( ( ( gMercProfiles[ ubMercProfileID ].ubFaceIndex - 200 ) > 16 ) || ( ubMercProfileID >= PROF_HUMMER ) )
  {
    return;
  }

	gMercProfiles[ ubMercProfileID ].usEyesX = uiEyeXPositions[ gMercProfiles[ ubMercProfileID ].ubFaceIndex - 200 ];
	gMercProfiles[ ubMercProfileID ].usEyesY = uiEyeYPositions[ gMercProfiles[ ubMercProfileID ].ubFaceIndex - 200  ];


	gMercProfiles[ ubMercProfileID ].usMouthX = uiMouthXPositions[ gMercProfiles[ ubMercProfileID ].ubFaceIndex - 200  ];
	gMercProfiles[ ubMercProfileID ].usMouthY = uiMouthYPositions[ gMercProfiles[ ubMercProfileID ].ubFaceIndex - 200  ];
}
