#include <stdexcept>

#include "Buffer.h"
#include "Font.h"
#include "Font_Control.h"
#include "GameLoop.h"
#include "LoadSaveData.h"
#include "LoadSaveEMail.h"
#include "LoadSaveMercProfile.h"
#include "LoadSaveSoldierType.h"
#include "LoadSaveTacticalStatusType.h"
#include "Local.h"
#include "Timer_Control.h"
#include "Types.h"
#include "Soldier_Profile.h"
#include "FileMan.h"
#include "Debug.h"
#include "Overhead.h"
#include "Keys.h"
#include "Finances.h"
#include "History.h"
#include "Files.h"
#include "Laptop.h"
#include "StrategicMap.h"
#include "Game_Events.h"
#include "Game_Clock.h"
#include "Soldier_Create.h"
#include "WorldDef.h"
#include "LaptopSave.h"
#include "Queen_Command.h"
#include "SaveLoadGame.h"
#include "Tactical_Save.h"
#include "Squads.h"
#include "Environment.h"
#include "Lighting.h"
#include "Strategic_Movement.h"
#include "Quests.h"
#include "OppList.h"
#include "Message.h"
#include "NPC.h"
#include "Merc_Hiring.h"
#include "SaveLoadScreen.h"
#include "GameVersion.h"
#include "GameSettings.h"
#include "Music_Control.h"
#include "Options_Screen.h"
#include "AI.h"
#include "RenderWorld.h"
#include "SmokeEffects.h"
#include "Random.h"
#include "Map_Screen_Interface.h"
#include "Map_Screen_Interface_Border.h"
#include "Map_Screen_Interface_Bottom.h"
#include "Interface.h"
#include "Map_Screen_Helicopter.h"
#include "Arms_Dealer_Init.h"
#include "Tactical_Placement_GUI.h"
#include "Strategic_Mines.h"
#include "Strategic_Town_Loyalty.h"
#include "Vehicles.h"
#include "Merc_Contract.h"
#include "Bullets.h"
#include "Air_Raid.h"
#include "Physics.h"
#include "Strategic_Pathing.h"
#include "TeamTurns.h"
#include "Explosion_Control.h"
#include "Creature_Spreading.h"
#include "Strategic_Status.h"
#include "PreBattle_Interface.h"
#include "Boxing.h"
#include "Strategic_AI.h"
#include "Map_Screen_Interface_Map.h"
#include "Meanwhile.h"
#include "Dialogue_Control.h"
#include "Text.h"
#include "LightEffects.h"
#include "HelpScreen.h"
#include "Animated_ProgressBar.h"
#include "MercTextBox.h"
#include "Map_Information.h"
#include "Interface_Items.h"
#include "Civ_Quotes.h"
#include "Scheduling.h"
#include "Animation_Data.h"
#include "Cheats.h"
#include "Render_Dirty.h"
#include "Strategic_Event_Handler.h"
#include "Interface_Panels.h"
#include "Interface_Dialogue.h"
#include "Assignments.h"
#include "Video.h"
#include "VSurface.h"
#include "MemMan.h"
#include "JAScreens.h"
#include "BobbyR.h"
#include "IMP_Portraits.h"
#include "Loading_Screen.h"
#include "Interface_Utils.h"
#include "IMP_Confirm.h"
#include "Enemy_Soldier_Save.h"
#include "BobbyRMailOrder.h"
#include "Mercs.h"


static const char g_quicksave_name[] = "QuickSave";
static const char g_savegame_name[]  = "SaveGame";
static const char g_savegame_ext[]   = "sav";
static const char g_savegame_dir[]   = "../SavedGames";


//Global variable used
#ifdef JA2BETAVERSION
static UINT32 guiNumberOfMapTempFiles; // Test purposes
static UINT32 guiSizeOfTempFiles;
static char   gzNameOfMapTempFile[128];
#endif

extern		INT32					giSortStateForMapScreenList;
extern		INT16					sDeadMercs[ NUMBER_OF_SQUADS ][ NUMBER_OF_SOLDIERS_PER_SQUAD ];
extern		INT32					giRTAILastUpdateTime;
extern		BOOLEAN				gfRedrawSaveLoadScreen;
extern		UINT8					gubScreenCount;
extern		INT16					sWorldSectorLocationOfFirstBattle;
extern		BOOLEAN				gfLoadedGame;
extern		UINT8					gubDesertTemperature;
extern		UINT8					gubGlobalTemperature;
extern		BOOLEAN				gfCreatureMeanwhileScenePlayed;
#ifdef JA2BETAVERSION
	extern		UINT8					gubReportMapscreenLock;
#endif

static BOOLEAN gMusicModeToPlay = FALSE;

BOOLEAN	gfUseConsecutiveQuickSaveSlots = FALSE;
#ifdef JA2BETAVERSION
static UINT32 guiCurrentQuickSaveNumber = 0;
#endif
UINT32	guiLastSaveGameNum;

UINT32	guiJA2EncryptionSet = 0;

typedef struct
{
	//The screen that the gaem was saved from
	UINT32	uiCurrentScreen;

	UINT32	uiCurrentUniqueSoldierId;

	//The music that was playing when the game was saved
	UINT8		ubMusicMode;

	//Flag indicating that we have purchased something from Tony
	BOOLEAN	fHavePurchasedItemsFromTony_UNUSED; // XXX HACK000B

	//The selected soldier in tactical
	UINT16	usSelectedSoldier;

	// The x and y scroll position
	INT16		sRenderCenterX;
	INT16		sRenderCenterY;

	BOOLEAN	fAtLeastOneMercWasHired;

	//General Map screen state flags
	BOOLEAN	fShowItemsFlag;
	BOOLEAN	fShowTownFlag;
	BOOLEAN	fShowTeamFlag;
	BOOLEAN	fShowMineFlag;
	BOOLEAN	fShowAircraftFlag;

	// is the helicopter available to player?
	BOOLEAN fHelicopterAvailable;

	// helicopter vehicle id
	INT32 iHelicopterVehicleId;

	// total distance travelled
	INT32 UNUSEDiTotalHeliDistanceSinceRefuel; // XXX HACK000B

	// total owed to player
	INT32 iTotalAccumulatedCostByPlayer;

	// whether or not skyrider is alive and well? and on our side yet?
	BOOLEAN fSkyRiderAvailable;

	// skyrider engaging in a monologue
	BOOLEAN UNUSEDfSkyriderMonologue;

	// list of sector locations
	INT16 UNUSED[ 2 ][ 2 ];

	// is the heli in the air?
	BOOLEAN fHelicopterIsAirBorne;

	// is the pilot returning straight to base?
	BOOLEAN fHeliReturnStraightToBase;

	// heli hovering
	BOOLEAN fHoveringHelicopter;

	// time started hovering
	UINT32 uiStartHoverTime;

	// what state is skyrider's dialogue in in?
	UINT32 uiHelicopterSkyriderTalkState;

	// the flags for skyrider events
	BOOLEAN fShowEstoniRefuelHighLight;
	BOOLEAN fShowOtherSAMHighLight;
	BOOLEAN fShowDrassenSAMHighLight;

	UINT32	uiEnvWeather;

	UINT8		ubDefaultButton;




	BOOLEAN	fSkyriderEmptyHelpGiven;
	BOOLEAN	fEnterMapDueToContract;
	UINT8		ubHelicopterHitsTaken;
	UINT8		ubQuitType; // XXX HACK000B
	BOOLEAN fSkyriderSaidCongratsOnTakingSAM;
	INT16		sContractRehireSoldierID;


	GAME_OPTIONS	GameOptions;

	UINT32	uiSeedNumber;

	//The GetJA2Clock() value
	UINT32	uiBaseJA2Clock;

	INT16		sCurInterfacePanel;

	UINT8		ubSMCurrentMercID;

	BOOLEAN	fFirstTimeInMapScreen; // XXX HACK000B

	BOOLEAN	fDisableDueToBattleRoster;

	BOOLEAN fDisableMapInterfaceDueToBattle;

	INT16		sBoxerGridNo[ NUM_BOXERS ];
	UINT8		ubBoxerID[ NUM_BOXERS ];
	BOOLEAN	fBoxerFought[ NUM_BOXERS ];

	BOOLEAN	fHelicopterDestroyed;								//if the chopper is destroyed
	BOOLEAN	fShowMapScreenHelpText; // If true, displays help in mapscreen // XXX HACK000B

	INT32		iSortStateForMapScreenList;
	BOOLEAN	fFoundTixa;

	UINT32	uiTimeOfLastSkyriderMonologue;
	BOOLEAN fShowCambriaHospitalHighLight;
	BOOLEAN fSkyRiderSetUp;
	BOOLEAN fRefuelingSiteAvailable[ NUMBER_OF_REFUEL_SITES ];


	//Meanwhile stuff
	MEANWHILE_DEFINITION	gCurrentMeanwhileDef;

	BOOLEAN ubPlayerProgressSkyriderLastCommentedOn;

	BOOLEAN								gfMeanwhileTryingToStart;
	BOOLEAN								gfInMeanwhile;

	// list of dead guys for squads...in id values -> -1 means no one home
	INT16 sDeadMercs[ NUMBER_OF_SQUADS ][ NUMBER_OF_SOLDIERS_PER_SQUAD ];

	// levels of publicly known noises
	INT8	gbPublicNoiseLevel[MAXTEAMS];

	UINT8		gubScreenCount;

	UINT16	usOldMeanWhileFlags;

	INT32		iPortraitNumber;

	INT16		sWorldSectorLocationOfFirstBattle;

	BOOLEAN	fUnReadMailFlag;
	BOOLEAN fNewMailFlag;
	BOOLEAN	fOldUnReadFlag; // XXX HACK000B
	BOOLEAN	fOldNewMailFlag;

	BOOLEAN	fShowMilitia;

	BOOLEAN	fNewFilesInFileViewer;

	BOOLEAN	fLastBoxingMatchWonByPlayer;

	UINT32	uiUNUSED;

	BOOLEAN fSamSiteFound[ NUMBER_OF_SAMS ];

	UINT8		ubNumTerrorists;
	UINT8		ubCambriaMedicalObjects;

	BOOLEAN	fDisableTacticalPanelButtons;

	INT16		sSelMapX;
	INT16		sSelMapY;
	INT32		iCurrentMapSectorZ;

	UINT16	usHasPlayerSeenHelpScreenInCurrentScreen;
	BOOLEAN	fHideHelpInAllScreens;
	UINT8		ubBoxingMatchesWon;

	UINT8		ubBoxersRests;
	BOOLEAN	fBoxersResting;
	UINT8		ubDesertTemperature;
	UINT8		ubGlobalTemperature;

	INT16		sMercArriveSectorX;
	INT16		sMercArriveSectorY;

	BOOLEAN fCreatureMeanwhileScenePlayed;
	UINT8		ubPlayerNum;
	//New stuff for the Prebattle interface / autoresolve
	BOOLEAN fPersistantPBI;
	UINT8 ubEnemyEncounterCode;

	BOOLEAN ubExplicitEnemyEncounterCode;
	BOOLEAN fBlitBattleSectorLocator;
	UINT8 ubPBSectorX;
	UINT8 ubPBSectorY;

	UINT8 ubPBSectorZ;
	BOOLEAN fCantRetreatInPBI;
	BOOLEAN fExplosionQueueActive;
	UINT8	ubUnused[1];

	UINT32	uiMeanWhileFlags;

	INT8 bSelectedInfoChar;
	INT8 bHospitalPriceModifier;
	INT8 bUnused2[ 2 ];

	INT32 iHospitalTempBalance;
	INT32 iHospitalRefund;

  INT8  fPlayerTeamSawJoey;
	INT8	fMikeShouldSayHi;

	UINT8		ubFiller[550];		//This structure should be 1024 bytes

} GENERAL_SAVE_INFO;
CASSERT(sizeof(GENERAL_SAVE_INFO) == 1024)


UINT32	guiSaveGameVersion=0;


static UINT8 gubSaveGameLoc = 0;

ScreenID guiScreenToGotoAfterLoadingSavedGame = ERROR_SCREEN; // XXX TODO001A was not properly initialised (0)

extern		UINT32		guiCurrentUniqueSoldierId;


static UINT32 CalcJA2EncryptionSet(SAVED_GAME_HEADER* pSaveGameHeader);
static void PauseBeforeSaveGame(void);
static void UnPauseAfterSaveGame(void);
static void SaveGeneralInfo(HWFILE);
static void SaveMeanwhileDefsFromSaveGameFile(HWFILE);
static void SaveMercProfiles(HWFILE);
static void SaveOppListInfoToSavedGame(HWFILE);
static void SavePreRandomNumbersToSaveGameFile(HWFILE);
static void SaveSoldierStructure(HWFILE hFile);
static void SaveTacticalStatusToSavedGame(HWFILE);
static void SaveWatchedLocsToSavedGame(HWFILE);

#ifdef JA2BETAVERSION
static void InitSaveGameFilePosition(void);
static void InitShutDownMapTempFileTest(BOOLEAN fInit, const char* pNameOfFile, UINT8 ubSaveGameID);
static void SaveGameFilePosition(const HWFILE save, const char* pMsg);
#else
#	define InitSaveGameFilePosition()                  ((void)0)
#	define InitShutDownMapTempFileTest(init, name, id) ((void)0)
#	define SaveGameFilePosition(file, msg)             ((void)0)
#endif


BOOLEAN SaveGame( UINT8 ubSaveGameID, const wchar_t *GameDesc)
{
	CHAR8		zSaveGameName[ 512 ];
	BOOLEAN	fPausedStateBeforeSaving = gfGamePaused;
	BOOLEAN	fLockPauseStateBeforeSaving = gfLockPauseState;
	UINT16	usPosX, usActualWidth, usActualHeight;
	BOOLEAN fWePausedIt = FALSE;

	if( ubSaveGameID >= NUM_SAVE_GAMES && ubSaveGameID != SAVE__ERROR_NUM && ubSaveGameID != SAVE__END_TURN_NUM )
		return( FALSE );

	if ( !GamePaused() )
	{
		PauseBeforeSaveGame();
		fWePausedIt = TRUE;
	}

	InitShutDownMapTempFileTest(TRUE, "SaveMapTempFile", ubSaveGameID);

	//Place a message on the screen telling the user that we are saving the game
	{ AutoMercPopUpBox const save_load_game_message_box(PrepareMercPopupBox(0, BASIC_MERC_POPUP_BACKGROUND, BASIC_MERC_POPUP_BORDER, zSaveLoadText[SLG_SAVING_GAME_MESSAGE], 300, 0, 0, 0, &usActualWidth, &usActualHeight));
		usPosX = (SCREEN_WIDTH - usActualWidth) / 2;
		RenderMercPopUpBox(save_load_game_message_box, usPosX, 160, FRAME_BUFFER);
	}

	InvalidateScreen();

	ExecuteBaseDirtyRectQueue( );
	EndFrameBufferRender( );
	RefreshScreen();

	//
	// make sure we redraw the screen when we are done
	//

	//if we are in the game screen
	if( guiCurrentScreen == GAME_SCREEN )
	{
		SetRenderFlags( RENDER_FLAG_FULL );
	}

	else if( guiCurrentScreen == MAP_SCREEN )
	{
		fMapPanelDirty = TRUE;
		fTeamPanelDirty = TRUE;
		fCharacterInfoPanelDirty = TRUE;
	}

	else if( guiCurrentScreen == SAVE_LOAD_SCREEN )
	{
		gfRedrawSaveLoadScreen = TRUE;
	}




	gubSaveGameLoc = ubSaveGameID;

	InitSaveGameFilePosition();

	//Set the fact that we are saving a game
	gTacticalStatus.uiFlags |= LOADING_SAVED_GAME;


	try
	{
		//Save the current sectors open temp files to the disk
		SaveCurrentSectorsInformationToTempItemFile();

		SAVED_GAME_HEADER SaveGameHeader;
		memset(&SaveGameHeader, 0, sizeof(SaveGameHeader));

		//if we are saving the quick save,
		if (ubSaveGameID == 0)
		{
#ifdef JA2BETAVERSION
			//Increment the quicksave counter
			guiCurrentQuickSaveNumber++;

			if (gfUseConsecutiveQuickSaveSlots)
				swprintf(SaveGameHeader.sSavedGameDesc, lengthof(SaveGameHeader.sSavedGameDesc), L"%hs%03d", g_quicksave_name, guiCurrentQuickSaveNumber);
			else
#endif
				swprintf(SaveGameHeader.sSavedGameDesc, lengthof(SaveGameHeader.sSavedGameDesc), L"%hs", g_quicksave_name);
		}
		else
		{
			if (GameDesc[0] == L'\0') GameDesc = pMessageStrings[MSG_NODESC];
			wcslcpy(SaveGameHeader.sSavedGameDesc, GameDesc, lengthof(SaveGameHeader.sSavedGameDesc));
		}

		MakeFileManDirectory(g_savegame_dir);

		//Create the name of the file
		CreateSavedGameFileNameFromNumber(ubSaveGameID, zSaveGameName);

		// create the save game file
		AutoSGPFile f(FileOpen(zSaveGameName, FILE_ACCESS_WRITE | FILE_CREATE_ALWAYS));
		SaveGameFilePosition(f, "Just Opened File");

		//
		// If there are no enemy or civilians to save, we have to check BEFORE savinf the sector info struct because
		// the NewWayOfSavingEnemyAndCivliansToTempFile will RESET the civ or enemy flag AFTER they have been saved.
		//
		NewWayOfSavingEnemyAndCivliansToTempFile( gWorldSectorX, gWorldSectorY, gbWorldSectorZ, TRUE, TRUE );
		NewWayOfSavingEnemyAndCivliansToTempFile( gWorldSectorX, gWorldSectorY, gbWorldSectorZ, FALSE, TRUE );







		//
		// Setup the save game header
		//

		SaveGameHeader.uiSavedGameVersion = guiSavedGameVersion;
		strcpy(SaveGameHeader.zGameVersionNumber, g_version_number);

		SaveGameHeader.uiFlags;

		//The following will be used to quickly access info to display in the save/load screen
		SaveGameHeader.uiDay = GetWorldDay();
		SaveGameHeader.ubHour = (UINT8)GetWorldHour();
		SaveGameHeader.ubMin = (UINT8)guiMin;

		//copy over the initial game options
		SaveGameHeader.sInitialGameOptions = gGameOptions;

		//Get the sector value to save.
		GetBestPossibleSectorXYZValues(&SaveGameHeader.sSectorX, &SaveGameHeader.sSectorY, &SaveGameHeader.bSectorZ);

		SaveGameHeader.ubNumOfMercsOnPlayersTeam = NumberOfMercsOnPlayerTeam();
		SaveGameHeader.iCurrentBalance = LaptopSaveInfo.iCurrentBalance;


		SaveGameHeader.uiCurrentScreen = guiPreviousOptionScreen;

		SaveGameHeader.fAlternateSector = GetSectorFlagStatus( gWorldSectorX, gWorldSectorY, gbWorldSectorZ, SF_USE_ALTERNATE_MAP );

		if( gfWorldLoaded )
		{
			SaveGameHeader.fWorldLoaded = TRUE;
			SaveGameHeader.ubLoadScreenID = GetLoadScreenID( gWorldSectorX, gWorldSectorY, gbWorldSectorZ );
		}
		else
		{
			SaveGameHeader.fWorldLoaded = FALSE;
			SaveGameHeader.ubLoadScreenID = 0;
		}

		SaveGameHeader.uiRandom = Random( RAND_MAX );

		//
		// Save the Save Game header file
		//
		FileWrite(f, &SaveGameHeader, sizeof(SAVED_GAME_HEADER));
		SaveGameFilePosition(f, "Save Game Header");

		guiJA2EncryptionSet = CalcJA2EncryptionSet( &SaveGameHeader );

		//
		//Save the gTactical Status array, plus the curent secotr location
		//
		SaveTacticalStatusToSavedGame(f);
		SaveGameFilePosition(f, "Tactical Status");

		// save the game clock info
		SaveGameClock(f, fPausedStateBeforeSaving, fLockPauseStateBeforeSaving);
		SaveGameFilePosition(f, "Game Clock");

		// save the strategic events
		SaveStrategicEventsToSavedGame(f);
		SaveGameFilePosition(f, "Strategic Events");

		SaveLaptopInfoToSavedGame(f);
		SaveGameFilePosition(f, "Laptop Info");

		//
		// Save the merc profiles
		//
		SaveMercProfiles(f);
		SaveGameFilePosition(f, "Merc Profiles");

		//
		// Save the soldier structure
		//
		SaveSoldierStructure(f);
		SaveGameFilePosition(f, "Soldier Structure");

		//Save the Finaces Data file
		SaveFilesToSavedGame(FINANCES_DATA_FILE, f);
		SaveGameFilePosition(f, "Finances Data File");

		//Save the history file
		SaveFilesToSavedGame(HISTORY_DATA_FILE, f);
		SaveGameFilePosition(f, "History file");

		//Save the Laptop File file
		SaveFilesToSavedGame(FILES_DAT_FILE, f);
		SaveGameFilePosition(f, "The Laptop FILES file");

		//Save email stuff to save file
		SaveEmailToSavedGame(f);
		SaveGameFilePosition(f, "Email");

		//Save the strategic information
		SaveStrategicInfoToSavedFile(f);
		SaveGameFilePosition(f, "Strategic Information");

		//save the underground information
		SaveUnderGroundSectorInfoToSaveGame(f);
		SaveGameFilePosition(f, "Underground Information");

		//save the squad info
		SaveSquadInfoToSavedGameFile(f);
		SaveGameFilePosition(f, "Squad Info");

		SaveStrategicMovementGroupsToSaveGameFile(f);
		SaveGameFilePosition(f, "Strategic Movement Groups");

		//Save all the map temp files from the maps\temp directory into the saved game file
		SaveMapTempFilesToSavedGameFile(f);
		SaveGameFilePosition(f, "All the Map Temp files");

		SaveQuestInfoToSavedGameFile(f);
		SaveGameFilePosition(f, "Quest Info");

		SaveOppListInfoToSavedGame(f);
		SaveGameFilePosition(f, "OppList info");

		SaveMapScreenMessagesToSaveGameFile(f);
		SaveGameFilePosition(f, "MapScreen Messages");

		SaveNPCInfoToSaveGameFile(f);
		SaveGameFilePosition(f, "NPC Info");

		SaveKeyTableToSaveGameFile(f);
		SaveGameFilePosition(f, "KeyTable");

		SaveTempNpcQuoteArrayToSaveGameFile(f);
		SaveGameFilePosition(f, "NPC Temp Quote File");

		SavePreRandomNumbersToSaveGameFile(f);
		SaveGameFilePosition(f, "PreGenerated Random Files");

		SaveArmsDealerInventoryToSaveGameFile(f);
		SaveGameFilePosition(f, "Arms Dealers Inventory");

		SaveGeneralInfo(f);
		SaveGameFilePosition(f, "Misc. Info");

		SaveMineStatusToSaveGameFile(f);
		SaveGameFilePosition(f, "Mine Status");

		SaveStrategicTownLoyaltyToSaveGameFile(f);
		SaveGameFilePosition(f, "Town Loyalty");

		SaveVehicleInformationToSaveGameFile(f);
		SaveGameFilePosition(f, "Vehicle Information");

		SaveBulletStructureToSaveGameFile(f);
		SaveGameFilePosition(f, "Bullet Information");

		SavePhysicsTableToSaveGameFile(f);
		SaveGameFilePosition(f, "Physics Table");

		SaveAirRaidInfoToSaveGameFile(f);
		SaveGameFilePosition(f, "Air Raid Info");

		SaveTeamTurnsToTheSaveGameFile(f);
		SaveGameFilePosition(f, "Team Turn Info");

		SaveExplosionTableToSaveGameFile(f);
		SaveGameFilePosition(f, "Explosion Table");

		SaveCreatureDirectives(f);
		SaveGameFilePosition(f, "Creature Spreading");

		SaveStrategicStatusToSaveGameFile(f);
		SaveGameFilePosition(f, "Strategic Status");

		SaveStrategicAI(f);
		SaveGameFilePosition(f, "Strategic AI");

		SaveWatchedLocsToSavedGame(f);
		SaveGameFilePosition(f, "Watched Locs Info");

		SaveItemCursorToSavedGame(f);
		SaveGameFilePosition(f, "ItemCursor Info");

		SaveCivQuotesToSaveGameFile(f);
		SaveGameFilePosition(f, "Civ Quote System");

		SaveBackupNPCInfoToSaveGameFile(f);
		SaveGameFilePosition(f, "Backed up NPC Info");

		SaveMeanwhileDefsFromSaveGameFile(f);
		SaveGameFilePosition(f, "Meanwhile Definitions");

		// save meanwhiledefs

		SaveSchedules(f);
		SaveGameFilePosition(f, "Schedules");

		// Save extra vehicle info
		NewSaveVehicleMovementInfoToSavedGameFile(f);
		SaveGameFilePosition(f, "Vehicle Movement Stuff");

		// Save contract renewal sequence stuff
		SaveContractRenewalDataToSaveGameFile(f);
		SaveGameFilePosition(f, "Contract Renewal Data");

		// Save leave list stuff
		SaveLeaveItemList(f);
		SaveGameFilePosition(f, "leave list");

		//do the new way of saving bobbyr mail order items
		NewWayOfSavingBobbyRMailOrdersToSaveGameFile(f);
		SaveGameFilePosition(f, "New way of saving Bobby R mailorders");
	}
	catch (...)
	{
		if (fWePausedIt) UnPauseAfterSaveGame();

		//Delete the failed attempt at saving
		DeleteSaveGameNumber(ubSaveGameID);

		//Put out an error message
		ScreenMsg(FONT_MCOLOR_WHITE, MSG_INTERFACE, zSaveLoadText[SLG_SAVE_GAME_ERROR]);

		InitShutDownMapTempFileTest(FALSE, "SaveMapTempFile", ubSaveGameID);

		//Check for enough free hard drive space
		NextLoopCheckForEnoughFreeHardDriveSpace();

#ifdef JA2BETAVERSION
		if (fDisableDueToBattleRoster || fDisableMapInterfaceDueToBattle)
		{
			gubReportMapscreenLock = 2;
		}
#endif
		return FALSE;
	}

	//if we succesfully saved the game, mark this entry as the last saved game file
	if( ubSaveGameID != SAVE__ERROR_NUM && ubSaveGameID != SAVE__END_TURN_NUM )
	{
		gGameSettings.bLastSavedGameSlot = ubSaveGameID;
	}

	//Save the save game settings
	SaveGameSettings();

	//
	// Display a screen message that the save was succesful
	//

	//if its the quick save slot
	if( ubSaveGameID == 0 )
	{
		ScreenMsg( FONT_MCOLOR_WHITE, MSG_INTERFACE, pMessageStrings[ MSG_SAVESUCCESS ] );
	}
//#ifdef JA2BETAVERSION
	else if( ubSaveGameID == SAVE__END_TURN_NUM )
	{
//		ScreenMsg( FONT_MCOLOR_WHITE, MSG_INTERFACE, pMessageStrings[ MSG_END_TURN_AUTO_SAVE ] );
	}
//#endif
	else
	{
		ScreenMsg( FONT_MCOLOR_WHITE, MSG_INTERFACE, pMessageStrings[ MSG_SAVESLOTSUCCESS ] );
	}

	//restore the music mode
	SetMusicMode( gubMusicMode );

	//Unset the fact that we are saving a game
	gTacticalStatus.uiFlags &= ~LOADING_SAVED_GAME;

	UnPauseAfterSaveGame();

	InitShutDownMapTempFileTest(FALSE, "SaveMapTempFile", ubSaveGameID);

	#ifdef JA2BETAVERSION
		ValidateSoldierInitLinks( 2 );
	#endif

	//Check for enough free hard drive space
	NextLoopCheckForEnoughFreeHardDriveSpace();

	return( TRUE );
}


UINT32 guiBrokenSaveGameVersion = 0;


static void HandleOldBobbyRMailOrders(void);
static void LoadGeneralInfo(HWFILE);
static void LoadMeanwhileDefsFromSaveGameFile(HWFILE);
static void LoadOppListInfoFromSavedGame(HWFILE);
static void LoadPreRandomNumbersFromSaveGameFile(HWFILE);
static void LoadSavedMercProfiles(HWFILE);
static void LoadSoldierStructure(HWFILE);
static void LoadTacticalStatusFromSavedGame(HWFILE);
static void LoadWatchedLocsFromSavedGame(HWFILE);
static void TruncateStrategicGroupSizes(void);
static void UpdateMercMercContractInfo(void);

#ifdef JA2BETAVERSION
static void InitLoadGameFilePosition(void);
static void LoadGameFilePosition(HWFILE load, const char* pMsg);
#else
#	define InitLoadGameFilePosition()      ((void)0)
#	define LoadGameFilePosition(file, msg) ((void)0)
#endif


BOOLEAN LoadSavedGame( UINT8 ubSavedGameID )
{
	SAVED_GAME_HEADER SaveGameHeader;

	INT16 sLoadSectorX;
	INT16 sLoadSectorY;
	INT8 bLoadSectorZ;
	CHAR8		zSaveGameName[ 512 ];

	UINT32 uiRelStartPerc;
	UINT32 uiRelEndPerc;

	uiRelStartPerc = uiRelEndPerc =0;

  TrashAllSoldiers( );
  RemoveAllGroups();

	//Empty the dialogue Queue cause someone could still have a quote in waiting
	EmptyDialogueQueue( );

	//If there is someone talking, stop them
	StopAnyCurrentlyTalkingSpeech( );

  ZeroAnimSurfaceCounts( );

	ShutdownNPCQuotes();

	//is it a valid save number
	if( ubSavedGameID >= NUM_SAVE_GAMES )
	{
		if( ubSavedGameID != SAVE__END_TURN_NUM )
			return( FALSE );
	}
	else if( !gbSaveGameArray[ ubSavedGameID ] )
		return( FALSE );

	InitShutDownMapTempFileTest(TRUE, "LoadMapTempFile", ubSavedGameID);

	//Used in mapescreen to disable the annoying 'swoosh' transitions
	gfDontStartTransitionFromLaptop = TRUE;

	// Reset timer callbacks
	gpCustomizableTimerCallback = NULL;

	gubSaveGameLoc = ubSavedGameID;
	InitLoadGameFilePosition();

	//Set the fact that we are loading a saved game
	gTacticalStatus.uiFlags |= LOADING_SAVED_GAME;

	//Trash the existing world.  This is done to ensure that if we are loading a game that doesn't have
	//a world loaded, that we trash it beforehand -- else the player could theoretically enter that sector
	//where it would be in a pre-load state.
	TrashWorld();


	//Deletes all the Temp files in the Maps\Temp directory
	InitTacticalSave( TRUE );

	// ATE; Added to empry dialogue q
	EmptyDialogueQueue( );

	//Create the name of the file
	CreateSavedGameFileNameFromNumber( ubSavedGameID, zSaveGameName );

	try
	{
		// open the save game file
		AutoSGPFile f(FileOpen(zSaveGameName, FILE_ACCESS_READ));
		LoadGameFilePosition(f, "Just Opened File");

		//Load the Save Game header file
		FileRead(f, &SaveGameHeader, sizeof(SAVED_GAME_HEADER));
		LoadGameFilePosition(f, "Save Game Header");

		guiJA2EncryptionSet = CalcJA2EncryptionSet( &SaveGameHeader );

		guiBrokenSaveGameVersion = SaveGameHeader.uiSavedGameVersion;

		//if the player is loading up an older version of the game, and the person DOESNT have the cheats on,
		if (SaveGameHeader.uiSavedGameVersion < 65 && !CHEATER_CHEAT_LEVEL()) goto load_failed;

		//Store the loading screenID that was saved
		gubLastLoadingScreenID = SaveGameHeader.ubLoadScreenID;

		//HACK
		guiSaveGameVersion = SaveGameHeader.uiSavedGameVersion;

#if 0 // XXX was commented out
		LoadGeneralInfo(f);
		LoadGameFilePosition(f, "Misc info");
#endif

		//Load the gtactical status structure plus the current sector x,y,z
		LoadTacticalStatusFromSavedGame(f);
		LoadGameFilePosition(f, "Tactical Status");

		//This gets reset by the above function
		gTacticalStatus.uiFlags |= LOADING_SAVED_GAME;


		//Load the game clock ingo
		LoadGameClock(f);
		LoadGameFilePosition(f, "Game Clock");

		//if we are suppose to use the alternate sector
		if( SaveGameHeader.fAlternateSector )
		{
			SetSectorFlag( gWorldSectorX, gWorldSectorY, gbWorldSectorZ, SF_USE_ALTERNATE_MAP );
			gfUseAlternateMap = TRUE;
		}


		//if the world was loaded when saved, reload it, otherwise dont
		if( SaveGameHeader.fWorldLoaded || SaveGameHeader.uiSavedGameVersion < 50 )
		{
			//Get the current world sector coordinates
			sLoadSectorX = gWorldSectorX;
			sLoadSectorY = gWorldSectorY;
			bLoadSectorZ = gbWorldSectorZ;

			// This will guarantee that the sector will be loaded
			gbWorldSectorZ = -1;


			//if we should load a sector ( if the person didnt just start the game game )
			if( ( gWorldSectorX != 0 ) && ( gWorldSectorY != 0 ) )
			{
				//Load the sector
				SetCurrentWorldSector( sLoadSectorX, sLoadSectorY, bLoadSectorZ );
			}
		}
		else
		{ //By clearing these values, we can avoid "in sector" checks -- at least, that's the theory.
			gWorldSectorX = gWorldSectorY = 0;

			//Since there is no
			if( SaveGameHeader.sSectorX == -1 || SaveGameHeader.sSectorY == -1 || SaveGameHeader.bSectorZ == -1 )
				gubLastLoadingScreenID = LOADINGSCREEN_HELI;
			else
				gubLastLoadingScreenID = GetLoadScreenID( SaveGameHeader.sSectorX, SaveGameHeader.sSectorY, SaveGameHeader.bSectorZ );

			BeginLoadScreen();
		}

		CreateLoadingScreenProgressBar();
		SetProgressBarColor( 0, 0, 0, 150 );

#ifdef JA2BETAVERSION
		//set the font
		SetProgressBarMsgAttributes( 0, FONT12ARIAL, FONT_MCOLOR_WHITE, 0 );

		//
		// Set the tile so we don see the text come up
		//

		//if the world is unloaded, we must use the save buffer for the text
		if( SaveGameHeader.fWorldLoaded )
			SetProgressBarTextDisplayFlag( 0, TRUE, TRUE, FALSE );
		else
			SetProgressBarTextDisplayFlag( 0, TRUE, TRUE, TRUE );
#endif

		uiRelStartPerc = 0;

		uiRelEndPerc += 1;
		SetRelativeStartAndEndPercentage( 0, uiRelStartPerc, uiRelEndPerc, L"Strategic Events..." );
		RenderProgressBar( 0, 100 );
		uiRelStartPerc = uiRelEndPerc;

		//load the game events
		LoadStrategicEventsFromSavedGame(f);
		LoadGameFilePosition(f, "Strategic Events");

		uiRelEndPerc += 0;
		SetRelativeStartAndEndPercentage( 0, uiRelStartPerc, uiRelEndPerc, L"Laptop Info" );
		RenderProgressBar( 0, 100 );
		uiRelStartPerc = uiRelEndPerc;


		LoadLaptopInfoFromSavedGame(f);
		LoadGameFilePosition(f, "Laptop Info");

		uiRelEndPerc += 0;
		SetRelativeStartAndEndPercentage( 0, uiRelStartPerc, uiRelEndPerc, L"Merc Profiles..." );
		RenderProgressBar( 0, 100 );
		uiRelStartPerc = uiRelEndPerc;

		//
		// Load all the saved Merc profiles
		//
		LoadSavedMercProfiles(f);
		LoadGameFilePosition(f, "Merc Profiles");

		uiRelEndPerc += 30;
		SetRelativeStartAndEndPercentage( 0, uiRelStartPerc, uiRelEndPerc, L"Soldier Structure..." );
		uiRelStartPerc = uiRelEndPerc;


		//
		// Load the soldier structure info
		//
		LoadSoldierStructure(f);
		LoadGameFilePosition(f, "Soldier Structure");

		uiRelEndPerc += 1;
		SetRelativeStartAndEndPercentage( 0, uiRelStartPerc, uiRelEndPerc, L"Finances Data File..." );
		RenderProgressBar( 0, 100 );
		uiRelStartPerc = uiRelEndPerc;


		//
		// Load the Finances Data and write it to a new file
		//
		LoadFilesFromSavedGame(FINANCES_DATA_FILE, f);
		LoadGameFilePosition(f, "Finances Data File");

		uiRelEndPerc += 1;
		SetRelativeStartAndEndPercentage( 0, uiRelStartPerc, uiRelEndPerc, L"History File..." );
		RenderProgressBar( 0, 100 );
		uiRelStartPerc = uiRelEndPerc;


		//
		// Load the History Data and write it to a new file
		//
		LoadFilesFromSavedGame(HISTORY_DATA_FILE, f);
		LoadGameFilePosition(f, "History File");

		uiRelEndPerc += 1;
		SetRelativeStartAndEndPercentage( 0, uiRelStartPerc, uiRelEndPerc, L"The Laptop FILES file..." );
		RenderProgressBar( 0, 100 );
		uiRelStartPerc = uiRelEndPerc;


		//
		// Load the Files Data and write it to a new file
		//
		LoadFilesFromSavedGame(FILES_DAT_FILE, f);
		LoadGameFilePosition(f, "The Laptop FILES file");

		uiRelEndPerc += 1;
		SetRelativeStartAndEndPercentage( 0, uiRelStartPerc, uiRelEndPerc, L"Email..." );
		RenderProgressBar( 0, 100 );
		uiRelStartPerc = uiRelEndPerc;


		// Load the data for the emails
		LoadEmailFromSavedGame(f);
		LoadGameFilePosition(f, "Email");

		uiRelEndPerc += 1;
		SetRelativeStartAndEndPercentage( 0, uiRelStartPerc, uiRelEndPerc, L"Strategic Information..." );
		RenderProgressBar( 0, 100 );
		uiRelStartPerc = uiRelEndPerc;


		//Load the strategic Information
		LoadStrategicInfoFromSavedFile(f);
		LoadGameFilePosition(f, "Strategic Information");

		uiRelEndPerc += 1;
		SetRelativeStartAndEndPercentage( 0, uiRelStartPerc, uiRelEndPerc, L"UnderGround Information..." );
		RenderProgressBar( 0, 100 );
		uiRelStartPerc = uiRelEndPerc;


		//Load the underground information
		LoadUnderGroundSectorInfoFromSavedGame(f);
		LoadGameFilePosition(f, "UnderGround Information");

		uiRelEndPerc += 1;
		SetRelativeStartAndEndPercentage( 0, uiRelStartPerc, uiRelEndPerc, L"Squad Info..." );
		RenderProgressBar( 0, 100 );
		uiRelStartPerc = uiRelEndPerc;


		// Load all the squad info from the saved game file
		LoadSquadInfoFromSavedGameFile(f);
		LoadGameFilePosition(f, "Squad Info");

		uiRelEndPerc += 1;
		SetRelativeStartAndEndPercentage( 0, uiRelStartPerc, uiRelEndPerc, L"Strategic Movement Groups..." );
		RenderProgressBar( 0, 100 );
		uiRelStartPerc = uiRelEndPerc;


		//Load the group linked list
		LoadStrategicMovementGroupsFromSavedGameFile(f);
		LoadGameFilePosition(f, "Strategic Movement Groups");

		uiRelEndPerc += 30;
		SetRelativeStartAndEndPercentage( 0, uiRelStartPerc, uiRelEndPerc, L"All the Map Temp files..." );
		uiRelStartPerc = uiRelEndPerc;


		// Load all the map temp files from the saved game file into the maps\temp directory
		LoadMapTempFilesFromSavedGameFile(f);
		LoadGameFilePosition(f, "All the Map Temp files");

		uiRelEndPerc += 1;
		SetRelativeStartAndEndPercentage( 0, uiRelStartPerc, uiRelEndPerc, L"Quest Info..." );
		RenderProgressBar( 0, 100 );
		uiRelStartPerc = uiRelEndPerc;


		LoadQuestInfoFromSavedGameFile(f);
		LoadGameFilePosition(f, "Quest Info");

		uiRelEndPerc += 1;
		SetRelativeStartAndEndPercentage( 0, uiRelStartPerc, uiRelEndPerc, L"OppList Info..." );
		RenderProgressBar( 0, 100 );
		uiRelStartPerc = uiRelEndPerc;


		LoadOppListInfoFromSavedGame(f);
		LoadGameFilePosition(f, "OppList Info");

		uiRelEndPerc += 1;
		SetRelativeStartAndEndPercentage( 0, uiRelStartPerc, uiRelEndPerc, L"MapScreen Messages..." );
		RenderProgressBar( 0, 100 );
		uiRelStartPerc = uiRelEndPerc;


		LoadMapScreenMessagesFromSaveGameFile(f);
		LoadGameFilePosition(f, "MapScreen Messages");

		uiRelEndPerc += 1;
		SetRelativeStartAndEndPercentage( 0, uiRelStartPerc, uiRelEndPerc, L"NPC Info..." );
		RenderProgressBar( 0, 100 );
		uiRelStartPerc = uiRelEndPerc;


		LoadNPCInfoFromSavedGameFile(f, SaveGameHeader.uiSavedGameVersion);
		LoadGameFilePosition(f, "NPC Info");

		uiRelEndPerc += 1;
		SetRelativeStartAndEndPercentage( 0, uiRelStartPerc, uiRelEndPerc, L"KeyTable..." );
		RenderProgressBar( 0, 100 );
		uiRelStartPerc = uiRelEndPerc;


		LoadKeyTableFromSaveedGameFile(f);
		LoadGameFilePosition(f, "KeyTable");

		uiRelEndPerc += 1;
		SetRelativeStartAndEndPercentage( 0, uiRelStartPerc, uiRelEndPerc, L"Npc Temp Quote File..." );
		RenderProgressBar( 0, 100 );
		uiRelStartPerc = uiRelEndPerc;


		LoadTempNpcQuoteArrayToSaveGameFile(f);
		LoadGameFilePosition(f, "Npc Temp Quote File");

		uiRelEndPerc += 0;
		SetRelativeStartAndEndPercentage( 0, uiRelStartPerc, uiRelEndPerc, L"PreGenerated Random Files..." );
		RenderProgressBar( 0, 100 );
		uiRelStartPerc = uiRelEndPerc;


		LoadPreRandomNumbersFromSaveGameFile(f);
		LoadGameFilePosition(f, "PreGenerated Random Files");

		uiRelEndPerc += 0;
		SetRelativeStartAndEndPercentage( 0, uiRelStartPerc, uiRelEndPerc, L"Smoke Effect Structures..." );
		RenderProgressBar( 0, 100 );
		uiRelStartPerc = uiRelEndPerc;


		LoadSmokeEffectsFromLoadGameFile(f);
		LoadGameFilePosition(f, "Smoke Effect Structures");

		uiRelEndPerc += 1;
		SetRelativeStartAndEndPercentage( 0, uiRelStartPerc, uiRelEndPerc, L"Arms Dealers Inventory..." );
		RenderProgressBar( 0, 100 );
		uiRelStartPerc = uiRelEndPerc;


		LoadArmsDealerInventoryFromSavedGameFile(f, SaveGameHeader.uiSavedGameVersion >= 54, SaveGameHeader.uiSavedGameVersion >= 55);
		LoadGameFilePosition(f, "Arms Dealers Inventory");

		uiRelEndPerc += 0;
		SetRelativeStartAndEndPercentage( 0, uiRelStartPerc, uiRelEndPerc, L"Misc info..." );
		RenderProgressBar( 0, 100 );
		uiRelStartPerc = uiRelEndPerc;


		LoadGeneralInfo(f);
		LoadGameFilePosition(f, "Misc info");

		uiRelEndPerc += 1;
		SetRelativeStartAndEndPercentage( 0, uiRelStartPerc, uiRelEndPerc, L"Mine Status..." );
		RenderProgressBar( 0, 100 );
		uiRelStartPerc = uiRelEndPerc;


		LoadMineStatusFromSavedGameFile(f);
		LoadGameFilePosition(f, "Mine Status");

		uiRelEndPerc += 0;
		SetRelativeStartAndEndPercentage( 0, uiRelStartPerc, uiRelEndPerc, L"Town Loyalty..." );
		RenderProgressBar( 0, 100 );
		uiRelStartPerc = uiRelEndPerc;


		if( SaveGameHeader.uiSavedGameVersion	>= 21 )
		{
			LoadStrategicTownLoyaltyFromSavedGameFile(f);
			LoadGameFilePosition(f, "Town Loyalty");
		}

		uiRelEndPerc += 1;
		SetRelativeStartAndEndPercentage( 0, uiRelStartPerc, uiRelEndPerc, L"Vehicle Information..." );
		RenderProgressBar( 0, 100 );
		uiRelStartPerc = uiRelEndPerc;


		if( SaveGameHeader.uiSavedGameVersion	>= 22 )
		{
			LoadVehicleInformationFromSavedGameFile(f, SaveGameHeader.uiSavedGameVersion);
			LoadGameFilePosition(f, "Vehicle Information");
		}

		uiRelEndPerc += 1;
		SetRelativeStartAndEndPercentage( 0, uiRelStartPerc, uiRelEndPerc, L"Bullet Information..." );
		RenderProgressBar( 0, 100 );
		uiRelStartPerc = uiRelEndPerc;


		if( SaveGameHeader.uiSavedGameVersion	>= 24 )
		{
			LoadBulletStructureFromSavedGameFile(f);
			LoadGameFilePosition(f, "Bullet Information");
		}

		uiRelEndPerc += 1;
		SetRelativeStartAndEndPercentage( 0, uiRelStartPerc, uiRelEndPerc, L"Physics table..." );
		RenderProgressBar( 0, 100 );
		uiRelStartPerc = uiRelEndPerc;


		if( SaveGameHeader.uiSavedGameVersion	>= 24 )
		{
			LoadPhysicsTableFromSavedGameFile(f);
			LoadGameFilePosition(f, "Physics table");
		}

		uiRelEndPerc += 1;
		SetRelativeStartAndEndPercentage( 0, uiRelStartPerc, uiRelEndPerc, L"Air Raid Info..." );
		RenderProgressBar( 0, 100 );
		uiRelStartPerc = uiRelEndPerc;


		if( SaveGameHeader.uiSavedGameVersion	>= 24 )
		{
			LoadAirRaidInfoFromSaveGameFile(f);
			LoadGameFilePosition(f, "Air Raid Info");
		}

		uiRelEndPerc += 0;
		SetRelativeStartAndEndPercentage( 0, uiRelStartPerc, uiRelEndPerc, L"Team Turn Info..." );
		RenderProgressBar( 0, 100 );
		uiRelStartPerc = uiRelEndPerc;


		if( SaveGameHeader.uiSavedGameVersion	>= 24 )
		{
			LoadTeamTurnsFromTheSavedGameFile(f);
			LoadGameFilePosition(f, "Team Turn Info");
		}

		uiRelEndPerc += 1;
		SetRelativeStartAndEndPercentage( 0, uiRelStartPerc, uiRelEndPerc, L"Explosion Table..." );
		RenderProgressBar( 0, 100 );
		uiRelStartPerc = uiRelEndPerc;


		if( SaveGameHeader.uiSavedGameVersion	>= 25 )
		{
			LoadExplosionTableFromSavedGameFile(f);
			LoadGameFilePosition(f, "Explosion Table");
		}

		uiRelEndPerc += 1;
		SetRelativeStartAndEndPercentage( 0, uiRelStartPerc, uiRelEndPerc, L"Creature Spreading..." );
		RenderProgressBar( 0, 100 );
		uiRelStartPerc = uiRelEndPerc;


		if( SaveGameHeader.uiSavedGameVersion	>= 27 )
		{
			LoadCreatureDirectives(f, SaveGameHeader.uiSavedGameVersion);
			LoadGameFilePosition(f, "Creature Spreading");
		}

		uiRelEndPerc += 1;
		SetRelativeStartAndEndPercentage( 0, uiRelStartPerc, uiRelEndPerc, L"Strategic Status..." );
		RenderProgressBar( 0, 100 );
		uiRelStartPerc = uiRelEndPerc;


		if( SaveGameHeader.uiSavedGameVersion	>= 28 )
		{
			LoadStrategicStatusFromSaveGameFile(f);
			LoadGameFilePosition(f, "Strategic Status");
		}

		uiRelEndPerc += 1;
		SetRelativeStartAndEndPercentage( 0, uiRelStartPerc, uiRelEndPerc, L"Strategic AI..." );
		RenderProgressBar( 0, 100 );
		uiRelStartPerc = uiRelEndPerc;


		if( SaveGameHeader.uiSavedGameVersion	>= 31 )
		{
			LoadStrategicAI(f);
			LoadGameFilePosition(f, "Strategic AI");
		}

		uiRelEndPerc += 1;
		SetRelativeStartAndEndPercentage( 0, uiRelStartPerc, uiRelEndPerc, L"Lighting Effects..." );
		RenderProgressBar( 0, 100 );
		uiRelStartPerc = uiRelEndPerc;


		if( SaveGameHeader.uiSavedGameVersion	>= 37 )
		{
			LoadLightEffectsFromLoadGameFile(f);
			LoadGameFilePosition(f, "Lighting Effects");
		}

		uiRelEndPerc += 1;
		SetRelativeStartAndEndPercentage( 0, uiRelStartPerc, uiRelEndPerc, L"Watched Locs Info..." );
		RenderProgressBar( 0, 100 );
		uiRelStartPerc = uiRelEndPerc;


		if( SaveGameHeader.uiSavedGameVersion	>= 38 )
		{
			LoadWatchedLocsFromSavedGame(f);
		}
		LoadGameFilePosition(f, "Watched Locs Info");

		uiRelEndPerc += 1;
		SetRelativeStartAndEndPercentage( 0, uiRelStartPerc, uiRelEndPerc, L"Item cursor Info..." );
		RenderProgressBar( 0, 100 );
		uiRelStartPerc = uiRelEndPerc;


		if( SaveGameHeader.uiSavedGameVersion	>= 39 )
		{
			LoadItemCursorFromSavedGame(f);
		}
		LoadGameFilePosition(f, "Item cursor Info");

		uiRelEndPerc += 1;
		SetRelativeStartAndEndPercentage( 0, uiRelStartPerc, uiRelEndPerc, L"Civ Quote System..." );
		RenderProgressBar( 0, 100 );
		uiRelStartPerc = uiRelEndPerc;


		if( SaveGameHeader.uiSavedGameVersion >= 51 )
		{
			LoadCivQuotesFromLoadGameFile(f);
		}
		LoadGameFilePosition(f, "Civ Quote System");

		uiRelEndPerc += 1;
		SetRelativeStartAndEndPercentage( 0, uiRelStartPerc, uiRelEndPerc, L"Backed up NPC Info..." );
		RenderProgressBar( 0, 100 );
		uiRelStartPerc = uiRelEndPerc;


		if( SaveGameHeader.uiSavedGameVersion >= 53 )
		{
			LoadBackupNPCInfoFromSavedGameFile(f, SaveGameHeader.uiSavedGameVersion);
		}
		LoadGameFilePosition(f, "Backed up NPC Info");

		uiRelEndPerc += 1;
		SetRelativeStartAndEndPercentage( 0, uiRelStartPerc, uiRelEndPerc, L"Meanwhile definitions..." );
		RenderProgressBar( 0, 100 );
		uiRelStartPerc = uiRelEndPerc;


		if( SaveGameHeader.uiSavedGameVersion >= 58 )
		{
			LoadMeanwhileDefsFromSaveGameFile(f);
			LoadGameFilePosition(f, "Meanwhile definitions");
		}
		else
		{
			gMeanwhileDef[gCurrentMeanwhileDef.ubMeanwhileID] = gCurrentMeanwhileDef;
		}


		uiRelEndPerc += 1;
		SetRelativeStartAndEndPercentage( 0, uiRelStartPerc, uiRelEndPerc, L"Schedules..." );
		RenderProgressBar( 0, 100 );
		uiRelStartPerc = uiRelEndPerc;


		if( SaveGameHeader.uiSavedGameVersion >= 59 )
		{
			// trash schedules loaded from map
			DestroyAllSchedulesWithoutDestroyingEvents();
			LoadSchedulesFromSave(f);
			LoadGameFilePosition(f, "Schedules");
		}

		uiRelEndPerc += 1;
		SetRelativeStartAndEndPercentage( 0, uiRelStartPerc, uiRelEndPerc, L"Extra Vehicle Info..." );
		RenderProgressBar( 0, 100 );
		uiRelStartPerc = uiRelEndPerc;


		if( SaveGameHeader.uiSavedGameVersion >= 61 )
		{
			if( SaveGameHeader.uiSavedGameVersion < 84 )
			{
				LoadVehicleMovementInfoFromSavedGameFile(f);
				LoadGameFilePosition(f, "Extra Vehicle Info");
			}
			else
			{
				NewLoadVehicleMovementInfoFromSavedGameFile(f);
				LoadGameFilePosition(f, "Extra Vehicle Info");
			}
		}

		uiRelEndPerc += 1;
		SetRelativeStartAndEndPercentage( 0, uiRelStartPerc, uiRelEndPerc, L"Contract renweal sequence stuff..." );
		RenderProgressBar( 0, 100 );
		uiRelStartPerc = uiRelEndPerc;



		if( SaveGameHeader.uiSavedGameVersion < 62 )
		{
			// the older games had a bug where this flag was never being set
			gfResetAllPlayerKnowsEnemiesFlags = TRUE;
		}

		if( SaveGameHeader.uiSavedGameVersion >= 67 )
		{
			LoadContractRenewalDataFromSaveGameFile(f);
			LoadGameFilePosition(f, "Contract renweal sequence stuff");
		}

		if( SaveGameHeader.uiSavedGameVersion >= 70 )
		{
			LoadLeaveItemList(f);
			LoadGameFilePosition(f, "Leave List");
		}

		if( SaveGameHeader.uiSavedGameVersion <= 73 )
		{
			// Patch vehicle fuel
			AddVehicleFuelToSave( );
		}


		if( SaveGameHeader.uiSavedGameVersion >= 85 )
		{
			NewWayOfLoadingBobbyRMailOrdersToSaveGameFile(f);
			LoadGameFilePosition(f, "New way of loading Bobby R mailorders");
		}

		//If there are any old Bobby R Mail orders, tranfer them to the new system
		if( SaveGameHeader.uiSavedGameVersion < 85 )
		{
			HandleOldBobbyRMailOrders();
		}


		uiRelEndPerc += 1;
		SetRelativeStartAndEndPercentage( 0, uiRelStartPerc, uiRelEndPerc, L"Final Checks..." );
		RenderProgressBar( 0, 100 );
		uiRelStartPerc = uiRelEndPerc;
	}
	catch (...) { goto load_failed; }

	// ATE: Patch? Patch up groups.....( will only do for old saves.. )
	UpdatePersistantGroupsFromOldSave( SaveGameHeader.uiSavedGameVersion );


	if( SaveGameHeader.uiSavedGameVersion	<= 40 )
	{
		// Cancel all pending purchase orders for BobbyRay's.  Starting with version 41, the BR orders events are
		// posted with the usItemIndex itself as the parameter, rather than the inventory slot index.  This was
		// done to make it easier to modify BR's traded inventory lists later on without breaking saves.
		CancelAllPendingBRPurchaseOrders();
	}


	//if the world is loaded, apply the temp files to the loaded map
	if( SaveGameHeader.fWorldLoaded || SaveGameHeader.uiSavedGameVersion < 50 )
	{
		// Load the current sectors Information From the temporary files
		if( !LoadCurrentSectorsInformationFromTempItemsFile() )
		{
			InitExitGameDialogBecauseFileHackDetected();
			guiSaveGameVersion=0;
			return( TRUE );
		}
	}

	uiRelEndPerc += 1;
	SetRelativeStartAndEndPercentage( 0, uiRelStartPerc, uiRelEndPerc, L"Final Checks..." );
	RenderProgressBar( 0, 100 );
	uiRelStartPerc = uiRelEndPerc;

	InitAI();

	//Update the mercs in the sector with the new soldier info
	UpdateMercsInSector( gWorldSectorX, gWorldSectorY, gbWorldSectorZ );

	PostSchedules();


	uiRelEndPerc += 1;
	SetRelativeStartAndEndPercentage( 0, uiRelStartPerc, uiRelEndPerc, L"Final Checks..." );
	RenderProgressBar( 0, 100 );
	uiRelStartPerc = uiRelEndPerc;


	//Reset the lighting level if we are outside
	if( gbWorldSectorZ == 0 )
		LightSetBaseLevel( GetTimeOfDayAmbientLightLevel() );

	//if we have been to this sector before
//	if( SectorInfo[ SECTOR( gWorldSectorX,gWorldSectorY) ].uiFlags & SF_ALREADY_VISITED )
	{
		//Reset the fact that we are loading a saved game
		gTacticalStatus.uiFlags &= ~LOADING_SAVED_GAME;
	}

	// CJC January 13: we can't do this because (a) it resets militia IN THE MIDDLE OF
	// COMBAT, and (b) if we add militia to the teams while LOADING_SAVED_GAME is set,
	// the team counters will not be updated properly!!!
//	ResetMilitia();


	uiRelEndPerc += 1;
	SetRelativeStartAndEndPercentage( 0, uiRelStartPerc, uiRelEndPerc, L"Final Checks..." );
	RenderProgressBar( 0, 100 );
	uiRelStartPerc = uiRelEndPerc;

	//if the UI was locked in the saved game file
	if( gTacticalStatus.ubAttackBusyCount > 1 )
	{
		//Lock the ui
		SetUIBusy(GetSelectedMan());
	}

	//Reset the shadow
  SetFontShadow( DEFAULT_SHADOW );

	//if we succesfully LOADED! the game, mark this entry as the last saved game file
	gGameSettings.bLastSavedGameSlot		= ubSavedGameID;

	//Save the save game settings
	SaveGameSettings();


	uiRelEndPerc += 1;
	SetRelativeStartAndEndPercentage( 0, uiRelStartPerc, uiRelEndPerc, L"Final Checks..." );
	RenderProgressBar( 0, 100 );
	uiRelStartPerc = uiRelEndPerc;


	//Reset the Ai Timer clock
	giRTAILastUpdateTime = 0;

	//if we are in tactical
	if( guiScreenToGotoAfterLoadingSavedGame == GAME_SCREEN )
	{
		//Initialize the current panel
		InitializeCurrentPanel( );

		SelectSoldier(GetSelectedMan(), SELSOLDIER_FORCE_RESELECT);
	}

	uiRelEndPerc += 1;
	SetRelativeStartAndEndPercentage( 0, uiRelStartPerc, uiRelEndPerc, L"Final Checks..." );
	RenderProgressBar( 0, 100 );
	uiRelStartPerc = uiRelEndPerc;


	// init extern faces
	InitalizeStaticExternalNPCFaces( );

	// load portraits
	LoadCarPortraitValues( );

	// OK, turn OFF show all enemies....
	gTacticalStatus.uiFlags&= (~SHOW_ALL_MERCS );
	gTacticalStatus.uiFlags &= ~SHOW_ALL_ITEMS ;

	InitShutDownMapTempFileTest(FALSE, "LoadMapTempFile", ubSavedGameID);

	if ( (gTacticalStatus.uiFlags & INCOMBAT) )
	{
		DebugMsg(TOPIC_JA2, DBG_LEVEL_3, "Setting attack busy count to 0 from load");
		gTacticalStatus.ubAttackBusyCount = 0;
	}

	if( SaveGameHeader.uiSavedGameVersion	< 64 )
	{ //Militia/enemies/creature team sizes have been changed from 32 to 20.  This function
		//will simply kill off the excess.  This will allow the ability to load previous saves, though
		//there will still be problems, though a LOT less than there would be without this call.
		TruncateStrategicGroupSizes();
	}

	// ATE: if we are within this window where skyridder was foobared, fix!
	if ( SaveGameHeader.uiSavedGameVersion >= 61 && SaveGameHeader.uiSavedGameVersion <= 65 )
	{
		MERCPROFILESTRUCT *pProfile;

		if ( !fSkyRiderSetUp )
		{
			// see if we can find him and remove him if so....
			SOLDIERTYPE* const pSoldier = FindSoldierByProfileID(SKYRIDER);
			if (pSoldier != NULL) TacticalRemoveSoldier(pSoldier);

			// add the pilot at a random location!
			pProfile = &(gMercProfiles[ SKYRIDER ]);
			switch( Random( 4 ) )
			{
				case 0:
					pProfile->sSectorX = 15;
					pProfile->sSectorY = MAP_ROW_B;
					pProfile->bSectorZ = 0;
					break;
				case 1:
					pProfile->sSectorX = 14;
					pProfile->sSectorY = MAP_ROW_E;
					pProfile->bSectorZ = 0;
					break;
				case 2:
					pProfile->sSectorX = 12;
					pProfile->sSectorY = MAP_ROW_D;
					pProfile->bSectorZ = 0;
					break;
				case 3:
					pProfile->sSectorX = 16;
					pProfile->sSectorY = MAP_ROW_C;
					pProfile->bSectorZ = 0;
					break;
			}
		}
	}

	if ( SaveGameHeader.uiSavedGameVersion < 68 )
	{
		// correct bVehicleUnderRepairID for all mercs
		FOR_ALL_NON_PLANNING_SOLDIERS(s) s->bVehicleUnderRepairID = -1;
	}

	if ( SaveGameHeader.uiSavedGameVersion < 73 )
	{
		if( LaptopSaveInfo.fMercSiteHasGoneDownYet )
			LaptopSaveInfo.fFirstVisitSinceServerWentDown = 2;
	}


	//Update the MERC merc contract lenght.  Before save version 77 the data was stored in the SOLDIERTYPE,
	//after 77 the data is stored in the profile
	if ( SaveGameHeader.uiSavedGameVersion < 77 )
	{
		UpdateMercMercContractInfo();
	}


	if ( SaveGameHeader.uiSavedGameVersion <= 89 )
	{
		// ARM: A change was made in version 89 where refuel site availability now also depends on whether the player has
		// airspace control over that sector.  To update the settings immediately, must call it here.
		UpdateRefuelSiteAvailability();
	}

	if( SaveGameHeader.uiSavedGameVersion < 91 )
	{
		//update the amount of money that has been paid to speck
		CalcAproximateAmountPaidToSpeck();
	}

	gfLoadedGame = TRUE;

	uiRelEndPerc = 100;
	SetRelativeStartAndEndPercentage( 0, uiRelStartPerc, uiRelEndPerc, L"Done!" );
	RenderProgressBar( 0, 100 );

	RemoveLoadingScreenProgressBar();

	SetMusicMode( gMusicModeToPlay );

#ifndef JA2TESTVERSION
	RESET_CHEAT_LEVEL( );
#endif

#ifdef JA2BETAVERSION
	if( fDisableDueToBattleRoster || fDisableMapInterfaceDueToBattle )
	{
		gubReportMapscreenLock = 1;
	}
#endif

	// reset to 0
	guiSaveGameVersion=0;

	// reset once-per-convo records for everyone in the loaded sector
	ResetOncePerConvoRecordsForAllNPCsInLoadedSector();

	if ( !(gTacticalStatus.uiFlags & INCOMBAT) )
	{
		// fix lingering attack busy count problem on loading saved game by resetting a.b.c
		// if we're not in combat.
		gTacticalStatus.ubAttackBusyCount = 0;
	}

	// fix squads
	CheckSquadMovementGroups();

	//The above function LightSetBaseLevel adjusts ALL the level node light values including the merc node,
	//we must reset the values
	HandlePlayerTogglingLightEffects( FALSE );


	return( TRUE );

load_failed:
	guiSaveGameVersion = 0;
	return FALSE;
}


static void SaveMercProfiles(HWFILE const hFile)
{
	UINT16	cnt;

	//Lopp through all the profiles to save
	for( cnt=0; cnt< NUM_PROFILES; cnt++)
	{
		gMercProfiles[ cnt ].uiProfileChecksum = ProfileChecksum( &(gMercProfiles[ cnt ]) );
#ifdef _WIN32 // XXX HACK000A
		BYTE Data[716];
#else
		BYTE Data[796];
#endif
		InjectMercProfile(Data,  &gMercProfiles[cnt]);
		if ( guiSavedGameVersion < 87 )
		{
			JA2EncryptedFileWrite(hFile, Data, sizeof(Data));
		}
		else
		{
			NewJA2EncryptedFileWrite(hFile, Data, sizeof(Data));
		}
	}
}


static void LoadSavedMercProfiles(HWFILE const hFile)
{
	UINT16	cnt;

	//Lopp through all the profiles to Load
	for( cnt=0; cnt< NUM_PROFILES; cnt++)
	{
#ifdef _WIN32 // XXX HACK000A
		BYTE Data[716];
#else
		BYTE Data[796];
#endif
		if ( guiSaveGameVersion < 87 )
		{
			JA2EncryptedFileRead(hFile, Data, sizeof(Data));
		}
		else
		{
			NewJA2EncryptedFileRead(hFile, Data, sizeof(Data));
		}
		ExtractMercProfile(Data,  &gMercProfiles[cnt]);
		if ( gMercProfiles[ cnt ].uiProfileChecksum != ProfileChecksum( &(gMercProfiles[ cnt ]) ) )
		{
			throw std::runtime_error("Merc profile checksum mismatch");
		}
	}
}


static void SaveSoldierStructure(HWFILE const hFile)
{
	UINT16	cnt;
	UINT8		ubOne = 1;
	UINT8		ubZero = 0;

	//Loop through all the soldier structs to save
	for( cnt=0; cnt< TOTAL_SOLDIERS; cnt++)
	{
		SOLDIERTYPE* const s = GetMan(cnt);
		//if the soldier isnt active, dont add them to the saved game file.
		if (!s->bActive)
		{
			// Save the byte specifing to NOT load the soldiers
			FileWrite(hFile, &ubZero, 1);
		}
		else
		{
			// Save the byte specifing to load the soldiers
			FileWrite(hFile, &ubOne, 1);

			// calculate checksum for soldier
			s->uiMercChecksum = MercChecksum(s);
			// Save the soldier structure
#ifdef _WIN32 // XXX HACK000A
			BYTE Data[2328];
#else
			BYTE Data[2352];
#endif
			InjectSoldierType(Data, s);
			if ( guiSavedGameVersion < 87 )
			{
				JA2EncryptedFileWrite(hFile, Data, sizeof(Data));
			}
			else
			{
				NewJA2EncryptedFileWrite(hFile, Data, sizeof(Data));
			}

			// Save all the pointer info from the structure

			SaveMercPath(hFile, s->pMercPath);

			//do we have a 	KEY_ON_RING									*pKeyRing;
			if (s->pKeyRing != NULL)
			{
				// write to the file saying we have the ....
				FileWrite(hFile, &ubOne, 1);

				// Now save the ....
				FileWrite(hFile, s->pKeyRing, NUM_KEYS * sizeof(KEY_ON_RING));
			}
			else
			{
				// write to the file saying we DO NOT have the Key ring
				FileWrite(hFile, &ubZero, 1);
			}
		}
	}
}


static void LoadSoldierStructure(HWFILE const hFile)
{
	UINT16	cnt;
	SOLDIERTYPE SavedSoldierInfo;
	UINT8		ubOne = 1;
	UINT8		ubActive = 1;
	UINT32	uiPercentage;

	//Loop through all the soldier and delete them all
	FOR_ALL_SOLDIERS(s) TacticalRemoveSoldier(s);

	//Loop through all the soldier structs to load
	for( cnt=0; cnt< TOTAL_SOLDIERS; cnt++)
	{

		//update the progress bar
		uiPercentage = (cnt * 100) / (TOTAL_SOLDIERS-1);

		RenderProgressBar( 0, uiPercentage );


		//Read in a byte to tell us whether or not there is a soldier loaded here.
		FileRead(hFile, &ubActive, 1);

		// if the soldier is not active, continue
		if( !ubActive )
		{
			continue;
		}

		// else if there is a soldier
		else
		{
#ifdef _WIN32 // XXX HACK000A
			BYTE Data[2328];
#else
			BYTE Data[2352];
#endif
			//Read in the saved soldier info into a Temp structure
			if ( guiSaveGameVersion < 87 )
			{
				JA2EncryptedFileRead(hFile, Data, sizeof(Data));
			}
			else
			{
				NewJA2EncryptedFileRead(hFile, Data, sizeof(Data));
			}
			ExtractSoldierType(Data, &SavedSoldierInfo);
			// check checksum
			if ( MercChecksum( &SavedSoldierInfo ) != SavedSoldierInfo.uiMercChecksum )
			{
				throw std::runtime_error("soldier checksum mismatch");
			}

			SOLDIERTYPE* const s = TacticalCreateSoldierFromExisting(&SavedSoldierInfo);
			Assert(s->ubID == cnt);

			// Load the pMercPath
			LoadMercPath(hFile, &s->pMercPath);

			//do we have a 	KEY_ON_RING									*pKeyRing;

			// Read the file to see if we have to load the keys
			FileRead(hFile, &ubOne, 1);

			if( ubOne )
			{
				// Now Load the ....
				FileRead(hFile, s->pKeyRing, NUM_KEYS * sizeof(KEY_ON_RING));
			}
			else
			{
				Assert(s->pKeyRing == NULL);
			}

			//if the soldier is an IMP character
			if (s->ubWhatKindOfMercAmI == MERC_TYPE__PLAYER_CHARACTER && s->bTeam == gbPlayerNum)
			{
				ResetIMPCharactersEyesAndMouthOffsets(s->ubProfile);
			}

			//if the saved game version is before x, calculate the amount of money paid to mercs
			if( guiSaveGameVersion < 83 )
			{
				//if the soldier is someone
				if (s->ubProfile != NO_PROFILE)
				{
					MERCPROFILESTRUCT* const p = &gMercProfiles[s->ubProfile];
					if (s->ubWhatKindOfMercAmI == MERC_TYPE__MERC)
					{
						p->uiTotalCostToDate = p->sSalary * p->iMercMercContractLength;
					}
					else
					{
						p->uiTotalCostToDate = p->sSalary * s->iTotalContractLength;
					}
				}
			}

#ifdef GERMAN
			// Fix neutral flags
			if ( guiSaveGameVersion < 94 )
			{
				if (s->bTeam == OUR_TEAM && s->bNeutral && s->bAssignment != ASSIGNMENT_POW)
				{
					// turn off neutral flag
					s->bNeutral = FALSE;
				}
			}
#endif
			// JA2Gold: fix next-to-previous attacker value
			if ( guiSaveGameVersion < 99 )
			{
				s->next_to_previous_attacker = NULL;
			}
		}
	}

	// Fix robot
	if ( guiSaveGameVersion <= 87 )
	{
		MERCPROFILESTRUCT* const robot_p = &gMercProfiles[ROBOT];
		if (robot_p->inv[VESTPOS] == SPECTRA_VEST)
		{
			// update this
			robot_p->inv[VESTPOS]   = SPECTRA_VEST_18;
			robot_p->inv[HELMETPOS] = SPECTRA_HELMET_18;
			robot_p->inv[LEGPOS]    = SPECTRA_LEGGINGS_18;
			robot_p->bAgility = 50;
			SOLDIERTYPE* const robot_s = FindSoldierByProfileID(ROBOT);
			if (robot_s)
			{
				robot_s->inv[VESTPOS].usItem   = SPECTRA_VEST_18;
				robot_s->inv[HELMETPOS].usItem = SPECTRA_HELMET_18;
				robot_s->inv[LEGPOS].usItem    = SPECTRA_LEGGINGS_18;
				robot_s->bAgility = 50;
			}
		}
	}
}


#ifdef JA2BETAVERSION
static void WriteTempFileNameToFile(const char* pFileName, UINT32 uiSizeOfFile, HWFILE hSaveFile);
#endif


void SaveFilesToSavedGame(char const* const pSrcFileName, HWFILE const hFile)
{
	AutoSGPFile hSrcFile(FileOpen(pSrcFileName, FILE_ACCESS_READ));

#ifdef JA2BETAVERSION
	guiNumberOfMapTempFiles++;		//Increment counter:  To determine where the temp files are crashing
#endif

	//Get the file size of the source data file
	UINT32 uiFileSize = FileGetSize( hSrcFile );

	// Write the the size of the file to the saved game file
	FileWrite(hFile, &uiFileSize, sizeof(UINT32));

	if (uiFileSize == 0) return;

	// Read the saource file into the buffer
	SGP::Buffer<UINT8> pData(uiFileSize);
	FileRead(hSrcFile, pData, uiFileSize);

	// Write the buffer to the saved game file
	FileWrite(hFile, pData, uiFileSize);

#ifdef JA2BETAVERSION
	//Write out the name of the temp file so we can track whcih ones get loaded, and saved
	WriteTempFileNameToFile(pSrcFileName, uiFileSize, hFile);
#endif
}


void LoadFilesFromSavedGame(char const* const pSrcFileName, HWFILE const hFile)
{
#ifdef JA2BETAVERSION
	++guiNumberOfMapTempFiles; //Increment counter:  To determine where the temp files are crashing
#endif

	AutoSGPFile hSrcFile(FileOpen(pSrcFileName, FILE_ACCESS_WRITE | FILE_CREATE_ALWAYS));

	// Read the size of the data
	UINT32 uiFileSize;
	FileRead(hFile, &uiFileSize, sizeof(UINT32));

	if (uiFileSize == 0) return;

	// Read into the buffer
	SGP::Buffer<UINT8> pData(uiFileSize);
	FileRead(hFile, pData, uiFileSize);

	// Write the buffer to the new file
	FileWrite(hSrcFile, pData, uiFileSize);

#ifdef JA2BETAVERSION
	WriteTempFileNameToFile(pSrcFileName, uiFileSize, hFile);
#endif
}


static void SaveTacticalStatusToSavedGame(HWFILE const hFile)
{
	InjectTacticalStatusTypeIntoFile(hFile);

	//
	//Save the current sector location to the saved game file
	//

	// save gWorldSectorX
	FileWrite(hFile, &gWorldSectorX, sizeof(gWorldSectorX));

	// save gWorldSectorY
	FileWrite(hFile, &gWorldSectorY, sizeof(gWorldSectorY));

	// save gbWorldSectorZ
	FileWrite(hFile, &gbWorldSectorZ, sizeof(gbWorldSectorZ));
}


static void LoadTacticalStatusFromSavedGame(HWFILE const hFile)
{
	ExtractTacticalStatusTypeFromFile(hFile);

	//
	//Load the current sector location to the saved game file
	//

	// Load gWorldSectorX
	FileRead(hFile, &gWorldSectorX, sizeof(gWorldSectorX));

	// Load gWorldSectorY
	FileRead(hFile, &gWorldSectorY, sizeof(gWorldSectorY));

	// Load gbWorldSectorZ
	FileRead(hFile, &gbWorldSectorZ, sizeof(gbWorldSectorZ));
}


static void SaveOppListInfoToSavedGame(HWFILE const hFile)
{
	UINT32	uiSaveSize=0;

	// Save the Public Opplist
	uiSaveSize = MAXTEAMS * TOTAL_SOLDIERS;
	FileWrite(hFile, gbPublicOpplist, uiSaveSize);

	// Save the Seen Oppenents
	uiSaveSize = TOTAL_SOLDIERS * TOTAL_SOLDIERS;
	FileWrite(hFile, gbSeenOpponents, uiSaveSize);

	// Save the Last Known Opp Locations
	uiSaveSize = TOTAL_SOLDIERS * TOTAL_SOLDIERS; // XXX TODO000F
	FileWrite(hFile, gsLastKnownOppLoc, uiSaveSize);

	// Save the Last Known Opp Level
	uiSaveSize = TOTAL_SOLDIERS * TOTAL_SOLDIERS;
	FileWrite(hFile, gbLastKnownOppLevel, uiSaveSize);

	// Save the Public Last Known Opp Locations
	uiSaveSize = MAXTEAMS * TOTAL_SOLDIERS; // XXX TODO000F
	FileWrite(hFile, gsPublicLastKnownOppLoc, uiSaveSize);

	// Save the Public Last Known Opp Level
	uiSaveSize = MAXTEAMS * TOTAL_SOLDIERS;
	FileWrite(hFile, gbPublicLastKnownOppLevel, uiSaveSize);

	// Save the Public Noise Volume
	uiSaveSize = MAXTEAMS;
	FileWrite(hFile, gubPublicNoiseVolume, uiSaveSize);

	// Save the Public Last Noise Gridno
	uiSaveSize = MAXTEAMS; // XXX TODO000F
	FileWrite(hFile, gsPublicNoiseGridno, uiSaveSize);
}


static void LoadOppListInfoFromSavedGame(HWFILE const hFile)
{
	UINT32	uiLoadSize=0;

	// Load the Public Opplist
	uiLoadSize = MAXTEAMS * TOTAL_SOLDIERS;
	FileRead(hFile, gbPublicOpplist, uiLoadSize);

	// Load the Seen Oppenents
	uiLoadSize = TOTAL_SOLDIERS * TOTAL_SOLDIERS;
	FileRead(hFile, gbSeenOpponents, uiLoadSize);

	// Load the Last Known Opp Locations
	uiLoadSize = TOTAL_SOLDIERS * TOTAL_SOLDIERS; // XXX TODO000F
	FileRead(hFile, gsLastKnownOppLoc, uiLoadSize);

	// Load the Last Known Opp Level
	uiLoadSize = TOTAL_SOLDIERS * TOTAL_SOLDIERS;
	FileRead(hFile, gbLastKnownOppLevel, uiLoadSize);

	// Load the Public Last Known Opp Locations
	uiLoadSize = MAXTEAMS * TOTAL_SOLDIERS; // XXX TODO000F
	FileRead(hFile, gsPublicLastKnownOppLoc, uiLoadSize);

	// Load the Public Last Known Opp Level
	uiLoadSize = MAXTEAMS * TOTAL_SOLDIERS;
	FileRead(hFile, gbPublicLastKnownOppLevel, uiLoadSize);

	// Load the Public Noise Volume
	uiLoadSize = MAXTEAMS;
	FileRead(hFile, gubPublicNoiseVolume, uiLoadSize);

	// Load the Public Last Noise Gridno
	uiLoadSize = MAXTEAMS; // XXX TODO000F
	FileRead(hFile, gsPublicNoiseGridno, uiLoadSize);
}


static void SaveWatchedLocsToSavedGame(HWFILE const hFile)
{
	UINT32	uiArraySize;
	UINT32	uiSaveSize=0;

	uiArraySize = TOTAL_SOLDIERS * NUM_WATCHED_LOCS;

	// save locations of watched points
	uiSaveSize = uiArraySize * sizeof( INT16 );
	FileWrite(hFile, gsWatchedLoc, uiSaveSize);

	uiSaveSize = uiArraySize * sizeof( INT8 );

	FileWrite(hFile, gbWatchedLocLevel, uiSaveSize);

	FileWrite(hFile, gubWatchedLocPoints, uiSaveSize);

	FileWrite(hFile, gfWatchedLocReset, uiSaveSize);
}


static void LoadWatchedLocsFromSavedGame(HWFILE const hFile)
{
	UINT32	uiArraySize;
	UINT32	uiLoadSize=0;

	uiArraySize = TOTAL_SOLDIERS * NUM_WATCHED_LOCS;

	uiLoadSize = uiArraySize * sizeof( INT16 );
	FileRead(hFile, gsWatchedLoc, uiLoadSize);

	uiLoadSize = uiArraySize * sizeof( INT8 );
	FileRead(hFile, gbWatchedLocLevel, uiLoadSize);

	FileRead(hFile, gubWatchedLocPoints, uiLoadSize);

	FileRead(hFile, gfWatchedLocReset, uiLoadSize);
}


void CreateSavedGameFileNameFromNumber(const UINT8 ubSaveGameID, char* const pzNewFileName)
{
	const char* const dir = g_savegame_dir;
	const char* const ext = g_savegame_ext;

	//if we are creating the QuickSave file
	if (ubSaveGameID == 0)
	{
		const char* const quick = g_quicksave_name;
#ifdef JA2BETAVERSION
		if (gfUseConsecutiveQuickSaveSlots &&
				guiCurrentQuickSaveNumber != 0)
		{
			sprintf(pzNewFileName, "%s/%s%02d.%s", dir, quick, guiCurrentQuickSaveNumber, ext);
		}
		else
#endif
		{
			sprintf(pzNewFileName, "%s/%s.%s", dir, quick, ext);
		}
	}
	else if (ubSaveGameID == SAVE__END_TURN_NUM)
	{
		//The name of the file
		sprintf(pzNewFileName, "%s/Auto%02d.%s", dir, guiLastSaveGameNum, ext);

		guiLastSaveGameNum = (guiLastSaveGameNum + 1) % 2;
	}
	else
	{
		sprintf(pzNewFileName, "%s/%s%02d.%s", dir, g_savegame_name, ubSaveGameID, ext);
	}
}


void SaveMercPath(HWFILE const f, PathSt const* const head)
{
	UINT32 n_nodes = 0;
	for (const PathSt* p = head; p != NULL; p = p->pNext) ++n_nodes;
	FileWrite(f, &n_nodes, sizeof(UINT32));

	for (const PathSt* p = head; p != NULL; p = p->pNext)
	{
		BYTE  data[20];
		BYTE* d = data;
		INJ_U32(d, p->uiSectorId)
		INJ_SKIP(d, 4)
		INJ_BOOL(d, p->fSpeed)
		INJ_SKIP(d, 11)
		Assert(d == endof(data));

		FileWrite(f, data, sizeof(data));
	}
}


void LoadMercPath(HWFILE const hFile, PathSt** const head)
{
	//Load the number of the nodes
	UINT32 uiNumOfNodes = 0;
	FileRead(hFile, &uiNumOfNodes, sizeof(UINT32));

	//load all the nodes
	PathSt* path = NULL;
	for (UINT32 cnt = 0; cnt < uiNumOfNodes; ++cnt)
	{
		PathSt* const n = MALLOC(PathSt);

		BYTE data[20];
		FileRead(hFile, data, sizeof(data));

		const BYTE* d = data;
		EXTR_U32(d, n->uiSectorId)
		EXTR_SKIP(d, 4)
		EXTR_BOOL(d, n->fSpeed)
		EXTR_SKIP(d, 11)
		Assert(d == endof(data));

		n->pPrev = path;
		n->pNext = NULL;

		//Put the node into the list
		if (path == NULL)
		{
			*head = n;
		}
		else
		{
			path->pNext = n;
		}
		path = n;
	}
}


#ifdef JA2BETAVERSION
static void InitSaveGameFilePosition(void)
{
	CHAR8		zFileName[128];
	sprintf(zFileName, "%s/SaveGameFilePos%2d.txt", g_savegame_dir, gubSaveGameLoc);
	FileDelete( zFileName );
}


static void SaveGameFilePosition(const HWFILE save, const char* const pMsg)
{
	CHAR8		zTempString[512];
	UINT32	uiStrLen=0;
	CHAR8		zFileName[128];

	sprintf(zFileName, "%s/SaveGameFilePos%2d.txt", g_savegame_dir, gubSaveGameLoc);

	// create the save game file
	AutoSGPFile hFile(FileOpen(zFileName, FILE_ACCESS_APPEND | FILE_OPEN_ALWAYS));

	const INT32 pos = FileGetPos(save);
	sprintf(zTempString, "%8d     %s\n", pos, pMsg);
	uiStrLen = strlen( zTempString );
	FileWrite(hFile, zTempString, uiStrLen);
}


static void InitLoadGameFilePosition(void)
{
	CHAR8		zFileName[128];
	sprintf(zFileName, "%s/LoadGameFilePos%2d.txt", g_savegame_dir, gubSaveGameLoc);
	FileDelete( zFileName );
}


static void LoadGameFilePosition(const HWFILE load, const char* const pMsg)
{
	CHAR8		zTempString[512];
	UINT32	uiStrLen=0;

	CHAR8		zFileName[128];
	sprintf(zFileName, "%s/LoadGameFilePos%2d.txt", g_savegame_dir, gubSaveGameLoc);

	// create the save game file
	AutoSGPFile hFile(FileOpen(zFileName, FILE_ACCESS_APPEND | FILE_OPEN_ALWAYS));

	const INT32 pos = FileGetPos(load);
	sprintf(zTempString, "%8d     %s\n", pos, pMsg);
	uiStrLen = strlen( zTempString );

	FileWrite(hFile, zTempString, uiStrLen);
}
#endif


static void SaveGeneralInfo(HWFILE const hFile)
{
	GENERAL_SAVE_INFO sGeneralInfo;
	memset( &sGeneralInfo, 0, sizeof( GENERAL_SAVE_INFO ) );

	sGeneralInfo.ubMusicMode = gubMusicMode;
	sGeneralInfo.uiCurrentUniqueSoldierId = guiCurrentUniqueSoldierId;
	sGeneralInfo.uiCurrentScreen = guiPreviousOptionScreen;

	sGeneralInfo.usSelectedSoldier = Soldier2ID(GetSelectedMan());
	sGeneralInfo.sRenderCenterX = gsRenderCenterX;
	sGeneralInfo.sRenderCenterY = gsRenderCenterY;
	sGeneralInfo.fAtLeastOneMercWasHired = gfAtLeastOneMercWasHired;
	sGeneralInfo.fHavePurchasedItemsFromTony_UNUSED = FALSE;

	sGeneralInfo.fShowItemsFlag			= fShowItemsFlag;
	sGeneralInfo.fShowTownFlag			= fShowTownFlag;
	sGeneralInfo.fShowMineFlag			= fShowMineFlag;
	sGeneralInfo.fShowAircraftFlag	= fShowAircraftFlag;
	sGeneralInfo.fShowTeamFlag			= fShowTeamFlag;

	sGeneralInfo.fHelicopterAvailable = fHelicopterAvailable;

	// helicopter vehicle id
	sGeneralInfo.iHelicopterVehicleId = iHelicopterVehicleId;

	// total owed by player
	sGeneralInfo.iTotalAccumulatedCostByPlayer = iTotalAccumulatedCostByPlayer;

	// whether or not skyrider is alive and well? and on our side yet?
	sGeneralInfo.fSkyRiderAvailable = fSkyRiderAvailable;

	// is the heli in the air?
	sGeneralInfo.fHelicopterIsAirBorne = fHelicopterIsAirBorne;

	// is the pilot returning straight to base?
	sGeneralInfo.fHeliReturnStraightToBase = fHeliReturnStraightToBase;

	// heli hovering
	sGeneralInfo.fHoveringHelicopter = fHoveringHelicopter;

	// time started hovering
	sGeneralInfo.uiStartHoverTime = uiStartHoverTime;

	// what state is skyrider's dialogue in in?
	sGeneralInfo.uiHelicopterSkyriderTalkState = guiHelicopterSkyriderTalkState;

	// the flags for skyrider events
	sGeneralInfo.fShowEstoniRefuelHighLight = fShowEstoniRefuelHighLight;
	sGeneralInfo.fShowOtherSAMHighLight = fShowOtherSAMHighLight;
	sGeneralInfo.fShowDrassenSAMHighLight = fShowDrassenSAMHighLight;
	sGeneralInfo.fShowCambriaHospitalHighLight = fShowCambriaHospitalHighLight;

	//The current state of the weather
	sGeneralInfo.uiEnvWeather = guiEnvWeather;

	sGeneralInfo.ubDefaultButton = gubDefaultButton;

	sGeneralInfo.fSkyriderEmptyHelpGiven = gfSkyriderEmptyHelpGiven;
	sGeneralInfo.ubHelicopterHitsTaken = gubHelicopterHitsTaken;
	sGeneralInfo.fSkyriderSaidCongratsOnTakingSAM = gfSkyriderSaidCongratsOnTakingSAM;
	sGeneralInfo.ubPlayerProgressSkyriderLastCommentedOn = gubPlayerProgressSkyriderLastCommentedOn;

	sGeneralInfo.fEnterMapDueToContract = fEnterMapDueToContract;
	sGeneralInfo.ubQuitType = 0;

	if( pContractReHireSoldier != NULL )
		sGeneralInfo.sContractRehireSoldierID = pContractReHireSoldier->ubID;
	else
		sGeneralInfo.sContractRehireSoldierID = -1;

	sGeneralInfo.GameOptions = gGameOptions;


	#ifdef JA2BETAVERSION
	//Everytime we save get, and set a seed value, when reload, seed again
	sGeneralInfo.uiSeedNumber = GetJA2Clock();
	srand( sGeneralInfo.uiSeedNumber );
	#endif

	//Save the Ja2Clock()
	sGeneralInfo.uiBaseJA2Clock = guiBaseJA2Clock;

	// Save the current tactical panel mode
	sGeneralInfo.sCurInterfacePanel = gsCurInterfacePanel;

	// Save the selected merc
	if( gpSMCurrentMerc )
		sGeneralInfo.ubSMCurrentMercID = gpSMCurrentMerc->ubID;
	else
		sGeneralInfo.ubSMCurrentMercID = 255;

	//save map screen disabling buttons
	sGeneralInfo.fDisableDueToBattleRoster = fDisableDueToBattleRoster;
	sGeneralInfo.fDisableMapInterfaceDueToBattle = fDisableMapInterfaceDueToBattle;

	// Save boxing info
	memcpy( &sGeneralInfo.sBoxerGridNo, &gsBoxerGridNo, NUM_BOXERS * sizeof( INT16 ) );
	for (UINT i = 0; i < lengthof(gBoxer); ++i)
	{
		sGeneralInfo.ubBoxerID[i] = Soldier2ID(gBoxer[i]);
	}
	memcpy( &sGeneralInfo.fBoxerFought, &gfBoxerFought, NUM_BOXERS * sizeof( BOOLEAN ) );

	//Save the helicopter status
	sGeneralInfo.fHelicopterDestroyed = fHelicopterDestroyed;

	sGeneralInfo.iSortStateForMapScreenList = giSortStateForMapScreenList;
	sGeneralInfo.fFoundTixa = fFoundTixa;

	sGeneralInfo.uiTimeOfLastSkyriderMonologue = guiTimeOfLastSkyriderMonologue;
	sGeneralInfo.fSkyRiderSetUp = fSkyRiderSetUp;

	memcpy( &sGeneralInfo.fRefuelingSiteAvailable, &fRefuelingSiteAvailable, NUMBER_OF_REFUEL_SITES * sizeof( BOOLEAN ) );


	//Meanwhile stuff
	sGeneralInfo.gCurrentMeanwhileDef = gCurrentMeanwhileDef;
	sGeneralInfo.gfMeanwhileTryingToStart = gfMeanwhileTryingToStart;
	sGeneralInfo.gfInMeanwhile = gfInMeanwhile;


	// list of dead guys for squads...in id values -> -1 means no one home
	memcpy( &sGeneralInfo.sDeadMercs, &sDeadMercs, sizeof( INT16 ) * NUMBER_OF_SQUADS * NUMBER_OF_SOLDIERS_PER_SQUAD );

	// level of public noises
	memcpy( &sGeneralInfo.gbPublicNoiseLevel, &gbPublicNoiseLevel, sizeof( INT8 ) * MAXTEAMS );

	//The screen count for the initscreen
	sGeneralInfo.gubScreenCount = gubScreenCount;


	//used for the mean while screen
	sGeneralInfo.uiMeanWhileFlags = uiMeanWhileFlags;

	//Imp portrait number
	sGeneralInfo.iPortraitNumber = iPortraitNumber;

	// location of first enocunter with enemy
	sGeneralInfo.sWorldSectorLocationOfFirstBattle = sWorldSectorLocationOfFirstBattle;


	//State of email flags
	sGeneralInfo.fUnReadMailFlag = fUnReadMailFlag;
	sGeneralInfo.fNewMailFlag = fNewMailFlag;
	sGeneralInfo.fOldUnReadFlag = FALSE; // XXX HACK000B
	sGeneralInfo.fOldNewMailFlag = fOldNewMailFlag;

	sGeneralInfo.fShowMilitia	= fShowMilitia;

	sGeneralInfo.fNewFilesInFileViewer = fNewFilesInFileViewer;

	sGeneralInfo.fLastBoxingMatchWonByPlayer = gfLastBoxingMatchWonByPlayer;

	memcpy( &sGeneralInfo.fSamSiteFound, &fSamSiteFound, NUMBER_OF_SAMS * sizeof( BOOLEAN ) );

	sGeneralInfo.ubNumTerrorists = gubNumTerrorists;
	sGeneralInfo.ubCambriaMedicalObjects = gubCambriaMedicalObjects;

	sGeneralInfo.fDisableTacticalPanelButtons = gfDisableTacticalPanelButtons;

	sGeneralInfo.sSelMapX						= sSelMapX;
	sGeneralInfo.sSelMapY						= sSelMapY;
	sGeneralInfo.iCurrentMapSectorZ	= iCurrentMapSectorZ;

	//Save the current status of the help screens flag that say wether or not the user has been there before
	sGeneralInfo.usHasPlayerSeenHelpScreenInCurrentScreen = gHelpScreen.usHasPlayerSeenHelpScreenInCurrentScreen;

	sGeneralInfo.ubBoxingMatchesWon = gubBoxingMatchesWon;
	sGeneralInfo.ubBoxersRests = gubBoxersRests;
	sGeneralInfo.fBoxersResting = gfBoxersResting;

	sGeneralInfo.ubDesertTemperature = gubDesertTemperature;
	sGeneralInfo.ubGlobalTemperature = gubGlobalTemperature;

	sGeneralInfo.sMercArriveSectorX	 = gsMercArriveSectorX;
	sGeneralInfo.sMercArriveSectorY	 = gsMercArriveSectorY;

	sGeneralInfo.fCreatureMeanwhileScenePlayed = gfCreatureMeanwhileScenePlayed;

	//save the global player num
	sGeneralInfo.ubPlayerNum = gbPlayerNum;

	//New stuff for the Prebattle interface / autoresolve
	sGeneralInfo.fPersistantPBI									= gfPersistantPBI;
	sGeneralInfo.ubEnemyEncounterCode						= gubEnemyEncounterCode;
	sGeneralInfo.ubExplicitEnemyEncounterCode		= gubExplicitEnemyEncounterCode;
	sGeneralInfo.fBlitBattleSectorLocator				= gfBlitBattleSectorLocator;
	sGeneralInfo.ubPBSectorX										= gubPBSectorX;
	sGeneralInfo.ubPBSectorY										= gubPBSectorY;
	sGeneralInfo.ubPBSectorZ										= gubPBSectorZ;
	sGeneralInfo.fCantRetreatInPBI							= gfCantRetreatInPBI;
	sGeneralInfo.fExplosionQueueActive					= gfExplosionQueueActive;

	sGeneralInfo.bSelectedInfoChar							= bSelectedInfoChar;

	sGeneralInfo.iHospitalTempBalance						= giHospitalTempBalance;
	sGeneralInfo.iHospitalRefund								= giHospitalRefund;
	sGeneralInfo.bHospitalPriceModifier					= gbHospitalPriceModifier;
  sGeneralInfo.fPlayerTeamSawJoey             = gfPlayerTeamSawJoey;
	sGeneralInfo.fMikeShouldSayHi								= gfMikeShouldSayHi;

	FileWrite(hFile, &sGeneralInfo, sizeof(GENERAL_SAVE_INFO));
}


static void LoadGeneralInfo(HWFILE const hFile)
{
	GENERAL_SAVE_INFO sGeneralInfo;
	memset( &sGeneralInfo, 0, sizeof( GENERAL_SAVE_INFO ) );


	//Load the current music mode
	FileRead(hFile, &sGeneralInfo, sizeof(GENERAL_SAVE_INFO));

	gMusicModeToPlay = sGeneralInfo.ubMusicMode;

	guiCurrentUniqueSoldierId = sGeneralInfo.uiCurrentUniqueSoldierId;

	guiScreenToGotoAfterLoadingSavedGame = static_cast<ScreenID>(sGeneralInfo.uiCurrentScreen); // XXX TODO001A unchecked conversion

	SetSelectedMan(ID2Soldier(sGeneralInfo.usSelectedSoldier));

	gsRenderCenterX = sGeneralInfo.sRenderCenterX;
	gsRenderCenterY = sGeneralInfo.sRenderCenterY;

	gfAtLeastOneMercWasHired = sGeneralInfo.fAtLeastOneMercWasHired;


	fShowItemsFlag		= sGeneralInfo.fShowItemsFlag;
	fShowTownFlag			= sGeneralInfo.fShowTownFlag;
	fShowMineFlag			= sGeneralInfo.fShowMineFlag;
	fShowAircraftFlag	= sGeneralInfo.fShowAircraftFlag;
	fShowTeamFlag			= sGeneralInfo.fShowTeamFlag;

	fHelicopterAvailable = sGeneralInfo.fHelicopterAvailable;

	// helicopter vehicle id
	iHelicopterVehicleId = sGeneralInfo.iHelicopterVehicleId;

	// total owed to player
	iTotalAccumulatedCostByPlayer = sGeneralInfo.iTotalAccumulatedCostByPlayer;

	// whether or not skyrider is alive and well? and on our side yet?
	fSkyRiderAvailable = sGeneralInfo.fSkyRiderAvailable;

	// is the heli in the air?
	fHelicopterIsAirBorne = sGeneralInfo.fHelicopterIsAirBorne;

	// is the pilot returning straight to base?
	fHeliReturnStraightToBase = sGeneralInfo.fHeliReturnStraightToBase;

	// heli hovering
	fHoveringHelicopter = sGeneralInfo.fHoveringHelicopter;

	// time started hovering
	uiStartHoverTime = sGeneralInfo.uiStartHoverTime;

	// what state is skyrider's dialogue in in?
	guiHelicopterSkyriderTalkState = sGeneralInfo.uiHelicopterSkyriderTalkState;

	// the flags for skyrider events
	fShowEstoniRefuelHighLight = sGeneralInfo.fShowEstoniRefuelHighLight;
	fShowOtherSAMHighLight = sGeneralInfo.fShowOtherSAMHighLight;
	fShowDrassenSAMHighLight = sGeneralInfo.fShowDrassenSAMHighLight;
	fShowCambriaHospitalHighLight = sGeneralInfo.fShowCambriaHospitalHighLight;

	//The current state of the weather
	guiEnvWeather = sGeneralInfo.uiEnvWeather;

	gubDefaultButton = sGeneralInfo.ubDefaultButton;

	gfSkyriderEmptyHelpGiven = sGeneralInfo.fSkyriderEmptyHelpGiven;
	gubHelicopterHitsTaken = sGeneralInfo.ubHelicopterHitsTaken;
	gfSkyriderSaidCongratsOnTakingSAM = sGeneralInfo.fSkyriderSaidCongratsOnTakingSAM;
	gubPlayerProgressSkyriderLastCommentedOn = sGeneralInfo.ubPlayerProgressSkyriderLastCommentedOn;

	fEnterMapDueToContract = sGeneralInfo.fEnterMapDueToContract;

	//if the soldier id is valid
	if( sGeneralInfo.sContractRehireSoldierID == -1 )
		pContractReHireSoldier = NULL;
	else
		pContractReHireSoldier = GetMan(sGeneralInfo.sContractRehireSoldierID);

	gGameOptions = sGeneralInfo.GameOptions;

	#ifdef JA2BETAVERSION
	//Reset the random 'seed' number
	srand( sGeneralInfo.uiSeedNumber );
	#endif

	//Restore the JA2 Clock
	guiBaseJA2Clock = sGeneralInfo.uiBaseJA2Clock;

	ResetJA2ClockGlobalTimers();


	// Restore the selected merc
	if( sGeneralInfo.ubSMCurrentMercID == 255 )
		gpSMCurrentMerc = NULL;
	else
		gpSMCurrentMerc = GetMan(sGeneralInfo.ubSMCurrentMercID);

	//Set the interface panel to the team panel
	ShutdownCurrentPanel( );

	//Restore the current tactical panel mode
	gsCurInterfacePanel = sGeneralInfo.sCurInterfacePanel;

	/*
	//moved to last stage in the LoadSaveGame() function
	//if we are in tactical
	if( guiScreenToGotoAfterLoadingSavedGame == GAME_SCREEN )
	{
		//Initialize the current panel
		InitializeCurrentPanel( );

		SelectSoldier(GetSelectedMan(), SELSOLDIER_FORCE_RESELECT);
	}
	*/

	//Load map screen disabling buttons
	fDisableDueToBattleRoster = sGeneralInfo.fDisableDueToBattleRoster;
	fDisableMapInterfaceDueToBattle = sGeneralInfo.fDisableMapInterfaceDueToBattle;

	memcpy( &gsBoxerGridNo, &sGeneralInfo.sBoxerGridNo, NUM_BOXERS * sizeof( INT16 ) );
	for (UINT i = 0; i < lengthof(gBoxer); ++i)
	{
		gBoxer[i] = ID2Soldier(sGeneralInfo.ubBoxerID[i]);
	}
	memcpy( &gfBoxerFought, &sGeneralInfo.fBoxerFought, NUM_BOXERS * sizeof( BOOLEAN ) );

	//Load the helicopter status
	fHelicopterDestroyed = sGeneralInfo.fHelicopterDestroyed;

	giSortStateForMapScreenList = sGeneralInfo.iSortStateForMapScreenList;
	fFoundTixa = sGeneralInfo.fFoundTixa;

	guiTimeOfLastSkyriderMonologue = sGeneralInfo.uiTimeOfLastSkyriderMonologue;
	fSkyRiderSetUp = sGeneralInfo.fSkyRiderSetUp;

	memcpy( &fRefuelingSiteAvailable, &sGeneralInfo.fRefuelingSiteAvailable, NUMBER_OF_REFUEL_SITES * sizeof( BOOLEAN ) );


	//Meanwhile stuff
	gCurrentMeanwhileDef = sGeneralInfo.gCurrentMeanwhileDef;
	gfMeanwhileTryingToStart = sGeneralInfo.gfMeanwhileTryingToStart;
	gfInMeanwhile = sGeneralInfo.gfInMeanwhile;

	// list of dead guys for squads...in id values -> -1 means no one home
	memcpy( &sDeadMercs, &sGeneralInfo.sDeadMercs, sizeof( INT16 ) * NUMBER_OF_SQUADS * NUMBER_OF_SOLDIERS_PER_SQUAD );

	// level of public noises
	memcpy( &gbPublicNoiseLevel, &sGeneralInfo.gbPublicNoiseLevel, sizeof( INT8 ) * MAXTEAMS );

	//the screen count for the init screen
	gubScreenCount = sGeneralInfo.gubScreenCount;

	//used for the mean while screen
	if ( guiSaveGameVersion < 71 )
	{
		uiMeanWhileFlags = sGeneralInfo.usOldMeanWhileFlags;
	}
	else
	{
		uiMeanWhileFlags = sGeneralInfo.uiMeanWhileFlags;
	}

	//Imp portrait number
	iPortraitNumber = sGeneralInfo.iPortraitNumber;

	// location of first enocunter with enemy
	sWorldSectorLocationOfFirstBattle = sGeneralInfo.sWorldSectorLocationOfFirstBattle;

	fShowMilitia	= sGeneralInfo.fShowMilitia;

	fNewFilesInFileViewer = sGeneralInfo.fNewFilesInFileViewer;

	gfLastBoxingMatchWonByPlayer = sGeneralInfo.fLastBoxingMatchWonByPlayer;

	memcpy( &fSamSiteFound, &sGeneralInfo.fSamSiteFound, NUMBER_OF_SAMS * sizeof( BOOLEAN ) );

	gubNumTerrorists = sGeneralInfo.ubNumTerrorists;
	gubCambriaMedicalObjects = sGeneralInfo.ubCambriaMedicalObjects;

	gfDisableTacticalPanelButtons = sGeneralInfo.fDisableTacticalPanelButtons;

	sSelMapX						= sGeneralInfo.sSelMapX;
	sSelMapY						= sGeneralInfo.sSelMapY;
	iCurrentMapSectorZ	= sGeneralInfo.iCurrentMapSectorZ;

	//State of email flags
	fUnReadMailFlag = sGeneralInfo.fUnReadMailFlag;
	fNewMailFlag = sGeneralInfo.fNewMailFlag;
	fOldNewMailFlag = sGeneralInfo.fOldNewMailFlag;

	//Save the current status of the help screens flag that say wether or not the user has been there before
	gHelpScreen.usHasPlayerSeenHelpScreenInCurrentScreen = sGeneralInfo.usHasPlayerSeenHelpScreenInCurrentScreen;

	gubBoxingMatchesWon = sGeneralInfo.ubBoxingMatchesWon;
	gubBoxersRests = sGeneralInfo.ubBoxersRests;
	gfBoxersResting = sGeneralInfo.fBoxersResting;

	gubDesertTemperature = sGeneralInfo.ubDesertTemperature;
	gubGlobalTemperature = sGeneralInfo.ubGlobalTemperature;

	gsMercArriveSectorX = sGeneralInfo.sMercArriveSectorX;
	gsMercArriveSectorY = sGeneralInfo.sMercArriveSectorY;

	gfCreatureMeanwhileScenePlayed = sGeneralInfo.fCreatureMeanwhileScenePlayed;

	//load the global player num
	gbPlayerNum = sGeneralInfo.ubPlayerNum;

	//New stuff for the Prebattle interface / autoresolve
	gfPersistantPBI									= sGeneralInfo.fPersistantPBI;
	gubEnemyEncounterCode						= sGeneralInfo.ubEnemyEncounterCode;
	gubExplicitEnemyEncounterCode		= sGeneralInfo.ubExplicitEnemyEncounterCode;
	gfBlitBattleSectorLocator				= sGeneralInfo.fBlitBattleSectorLocator;
	gubPBSectorX										= sGeneralInfo.ubPBSectorX;
	gubPBSectorY										= sGeneralInfo.ubPBSectorY;
	gubPBSectorZ										= sGeneralInfo.ubPBSectorZ;
	gfCantRetreatInPBI							= sGeneralInfo.fCantRetreatInPBI;
	gfExplosionQueueActive					= sGeneralInfo.fExplosionQueueActive;

	bSelectedInfoChar	= sGeneralInfo.bSelectedInfoChar;

	giHospitalTempBalance		= sGeneralInfo.iHospitalTempBalance;
	giHospitalRefund				= sGeneralInfo.iHospitalRefund;
	gbHospitalPriceModifier = sGeneralInfo.bHospitalPriceModifier;
  gfPlayerTeamSawJoey     = sGeneralInfo.fPlayerTeamSawJoey;
	gfMikeShouldSayHi				= sGeneralInfo.fMikeShouldSayHi;
}


static void SavePreRandomNumbersToSaveGameFile(HWFILE const hFile)
{
	//Save the Prerandom number index
	FileWrite(hFile, &guiPreRandomIndex, sizeof(UINT32));

	//Save the Prerandom number index
	FileWrite(hFile, guiPreRandomNums, sizeof(UINT32) * MAX_PREGENERATED_NUMS);
}


static void LoadPreRandomNumbersFromSaveGameFile(HWFILE const hFile)
{
	//Load the Prerandom number index
	FileRead(hFile, &guiPreRandomIndex, sizeof(UINT32));

	//Load the Prerandom number index
	FileRead(hFile, guiPreRandomNums, sizeof(UINT32) * MAX_PREGENERATED_NUMS);
}


static void LoadMeanwhileDefsFromSaveGameFile(HWFILE const hFile)
{
	if ( guiSaveGameVersion < 72 )
	{
		//Load the array of meanwhile defs
		FileRead(hFile, gMeanwhileDef, sizeof(MEANWHILE_DEFINITION) * (NUM_MEANWHILES - 1));
		// and set the last one
		memset( &(gMeanwhileDef[ NUM_MEANWHILES - 1]), 0, sizeof( MEANWHILE_DEFINITION ) );
	}
	else
	{
		//Load the array of meanwhile defs
		FileRead(hFile, gMeanwhileDef, sizeof(MEANWHILE_DEFINITION) * NUM_MEANWHILES);
	}
}


static void SaveMeanwhileDefsFromSaveGameFile(HWFILE const hFile)
{
	//Save the array of meanwhile defs
	FileWrite(hFile, &gMeanwhileDef, sizeof(MEANWHILE_DEFINITION) * NUM_MEANWHILES);
}


BOOLEAN DoesUserHaveEnoughHardDriveSpace()
{
	UINT32			uiBytesFree=0;

	uiBytesFree = GetFreeSpaceOnHardDriveWhereGameIsRunningFrom( );

	//check to see if there is enough hard drive space
	if( uiBytesFree < REQUIRED_FREE_SPACE )
	{
		return( FALSE );
	}

	return( TRUE );
}

#ifdef JA2BETAVERSION

static void InitShutDownMapTempFileTest(BOOLEAN fInit, const char* pNameOfFile, UINT8 ubSaveGameID)
{
	CHAR8		zFileName[128];
	CHAR8		zTempString[512];
	UINT32	uiStrLen;

	//strcpy( gzNameOfMapTempFile, pNameOfFile);
	sprintf( gzNameOfMapTempFile, "%s%d", pNameOfFile, ubSaveGameID );

	sprintf(zFileName, "%s/%s.txt", g_savegame_dir, gzNameOfMapTempFile);

	if( fInit )
	{
		guiNumberOfMapTempFiles = 0;		//Test:  To determine where the temp files are crashing
		guiSizeOfTempFiles = 0;

		FileDelete(zFileName);
	}
	else
	{
		// create the save game file
		AutoSGPFile hFile(FileOpen(zFileName, FILE_ACCESS_APPEND | FILE_OPEN_ALWAYS));

		sprintf( zTempString, "Number Of Files: %6d.  Size of all files: %6d.\n", guiNumberOfMapTempFiles, guiSizeOfTempFiles );
		uiStrLen = strlen( zTempString );
		FileWrite(hFile, zTempString, uiStrLen);
	}
}


static void WriteTempFileNameToFile(const char* pFileName, UINT32 uiSizeOfFile, HWFILE hSaveFile)
{
	CHAR8		zTempString[512];
	UINT32	uiStrLen=0;

	CHAR8		zFileName[128];

	guiSizeOfTempFiles += uiSizeOfFile;

	sprintf(zFileName, "%s/%s.txt", g_savegame_dir, gzNameOfMapTempFile);

	// create the save game file
	AutoSGPFile hFile(FileOpen(zFileName, FILE_ACCESS_APPEND | FILE_OPEN_ALWAYS));

	sprintf( zTempString, "%8d   %6d   %s\n", FileGetPos( hSaveFile ), uiSizeOfFile, pFileName );
	uiStrLen = strlen( zTempString );

	FileWrite(hFile, zTempString, uiStrLen);
}

#endif


void GetBestPossibleSectorXYZValues(INT16* const psSectorX, INT16* const psSectorY, INT8* const pbSectorZ)
{
	//if the current sector is valid
	if (gfWorldLoaded)
	{
		*psSectorX = gWorldSectorX;
		*psSectorY = gWorldSectorY;
		*pbSectorZ = gbWorldSectorZ;
		return;
	}

	if (iCurrentTacticalSquad != NO_CURRENT_SQUAD)
	{
		const SOLDIERTYPE* const s = Squad[iCurrentTacticalSquad][0];
		if (s != NULL && s->bAssignment != IN_TRANSIT)
		{
			*psSectorX = s->sSectorX;
			*psSectorY = s->sSectorY;
			*pbSectorZ = s->bSectorZ;
			return;
		}
	}

	//loop through all the mercs on the players team to find the one that is not moving
	CFOR_ALL_IN_TEAM(s, gbPlayerNum)
	{
		if (s->bAssignment != IN_TRANSIT && !s->fBetweenSectors)
		{
			//we found an alive, merc that is not moving
			*psSectorX = s->sSectorX;
			*psSectorY = s->sSectorY;
			*pbSectorZ = s->bSectorZ;
			return;
		}
	}

	// loop through all the mercs and find one that is moving
	CFOR_ALL_IN_TEAM(s, gbPlayerNum)
	{
		//we found an alive, merc that is not moving
		*psSectorX = s->sSectorX;
		*psSectorY = s->sSectorY;
		*pbSectorZ = s->bSectorZ;
		return;
	}

	// if we STILL havent found a merc, give up and use the -1, -1, -1
	*psSectorX = gWorldSectorX;
	*psSectorY = gWorldSectorY;
	*pbSectorZ = gbWorldSectorZ;
}


static void PauseBeforeSaveGame(void)
{
	//if we are not in the save load screen
	if( guiCurrentScreen != SAVE_LOAD_SCREEN )
	{
		//Pause the game
		PauseGame();
	}
}


static void UnPauseAfterSaveGame(void)
{
	//if we are not in the save load screen
	if( guiCurrentScreen != SAVE_LOAD_SCREEN )
	{
		//UnPause time compression
		UnPauseGame();
	}
}


static void TruncateStrategicGroupSizes(void)
{
	SECTORINFO *pSector;
	INT32 i;
	for( i = SEC_A1; i < SEC_P16; i++ )
	{
		pSector = &SectorInfo[ i ];
		if( pSector->ubNumAdmins + pSector->ubNumTroops + pSector->ubNumElites > MAX_STRATEGIC_TEAM_SIZE )
		{
			if( pSector->ubNumAdmins > pSector->ubNumTroops )
			{
				if( pSector->ubNumAdmins > pSector->ubNumElites )
				{
					pSector->ubNumAdmins = 20;
					pSector->ubNumTroops = 0;
					pSector->ubNumElites = 0;
				}
				else
				{
					pSector->ubNumAdmins = 0;
					pSector->ubNumTroops = 0;
					pSector->ubNumElites = 20;
				}
			}
			else if( pSector->ubNumTroops > pSector->ubNumElites )
			{
				if( pSector->ubNumTroops > pSector->ubNumAdmins )
				{
					pSector->ubNumAdmins = 0;
					pSector->ubNumTroops = 20;
					pSector->ubNumElites = 0;
				}
				else
				{
					pSector->ubNumAdmins = 20;
					pSector->ubNumTroops = 0;
					pSector->ubNumElites = 0;
				}
			}
			else
			{
				if( pSector->ubNumElites > pSector->ubNumTroops )
				{
					pSector->ubNumAdmins = 0;
					pSector->ubNumTroops = 0;
					pSector->ubNumElites = 20;
				}
				else
				{
					pSector->ubNumAdmins = 0;
					pSector->ubNumTroops = 20;
					pSector->ubNumElites = 0;
				}
			}
		}
		//militia
		if( pSector->ubNumberOfCivsAtLevel[0] + pSector->ubNumberOfCivsAtLevel[1] + pSector->ubNumberOfCivsAtLevel[2] > MAX_STRATEGIC_TEAM_SIZE )
		{
			if( pSector->ubNumberOfCivsAtLevel[0] > pSector->ubNumberOfCivsAtLevel[1] )
			{
				if( pSector->ubNumberOfCivsAtLevel[0] > pSector->ubNumberOfCivsAtLevel[2] )
				{
					pSector->ubNumberOfCivsAtLevel[0] = 20;
					pSector->ubNumberOfCivsAtLevel[1] = 0;
					pSector->ubNumberOfCivsAtLevel[2] = 0;
				}
				else
				{
					pSector->ubNumberOfCivsAtLevel[0] = 0;
					pSector->ubNumberOfCivsAtLevel[1] = 0;
					pSector->ubNumberOfCivsAtLevel[2] = 20;
				}
			}
			else if( pSector->ubNumberOfCivsAtLevel[1] > pSector->ubNumberOfCivsAtLevel[2] )
			{
				if( pSector->ubNumberOfCivsAtLevel[1] > pSector->ubNumberOfCivsAtLevel[0] )
				{
					pSector->ubNumberOfCivsAtLevel[0] = 0;
					pSector->ubNumberOfCivsAtLevel[1] = 20;
					pSector->ubNumberOfCivsAtLevel[2] = 0;
				}
				else
				{
					pSector->ubNumberOfCivsAtLevel[0] = 20;
					pSector->ubNumberOfCivsAtLevel[1] = 0;
					pSector->ubNumberOfCivsAtLevel[2] = 0;
				}
			}
			else
			{
				if( pSector->ubNumberOfCivsAtLevel[2] > pSector->ubNumberOfCivsAtLevel[1] )
				{
					pSector->ubNumberOfCivsAtLevel[0] = 0;
					pSector->ubNumberOfCivsAtLevel[1] = 0;
					pSector->ubNumberOfCivsAtLevel[2] = 20;
				}
				else
				{
					pSector->ubNumberOfCivsAtLevel[0] = 0;
					pSector->ubNumberOfCivsAtLevel[1] = 20;
					pSector->ubNumberOfCivsAtLevel[2] = 0;
				}
			}
		}
	}
	//Enemy groups
	FOR_ALL_ENEMY_GROUPS(pGroup)
	{
		if( pGroup->pEnemyGroup->ubNumAdmins + pGroup->pEnemyGroup->ubNumTroops + pGroup->pEnemyGroup->ubNumElites > MAX_STRATEGIC_TEAM_SIZE )
		{
			pGroup->ubGroupSize = 20;
			if( pGroup->pEnemyGroup->ubNumAdmins > pGroup->pEnemyGroup->ubNumTroops )
			{
				if( pGroup->pEnemyGroup->ubNumAdmins > pGroup->pEnemyGroup->ubNumElites )
				{
					pGroup->pEnemyGroup->ubNumAdmins = 20;
					pGroup->pEnemyGroup->ubNumTroops = 0;
					pGroup->pEnemyGroup->ubNumElites = 0;
				}
				else
				{
					pGroup->pEnemyGroup->ubNumAdmins = 0;
					pGroup->pEnemyGroup->ubNumTroops = 0;
					pGroup->pEnemyGroup->ubNumElites = 20;
				}
			}
			else if( pGroup->pEnemyGroup->ubNumTroops > pGroup->pEnemyGroup->ubNumElites )
			{
				if( pGroup->pEnemyGroup->ubNumTroops > pGroup->pEnemyGroup->ubNumAdmins )
				{
					pGroup->pEnemyGroup->ubNumAdmins = 0;
					pGroup->pEnemyGroup->ubNumTroops = 20;
					pGroup->pEnemyGroup->ubNumElites = 0;
				}
				else
				{
					pGroup->pEnemyGroup->ubNumAdmins = 20;
					pGroup->pEnemyGroup->ubNumTroops = 0;
					pGroup->pEnemyGroup->ubNumElites = 0;
				}
			}
			else
			{
				if( pGroup->pEnemyGroup->ubNumElites > pGroup->pEnemyGroup->ubNumTroops )
				{
					pGroup->pEnemyGroup->ubNumAdmins = 0;
					pGroup->pEnemyGroup->ubNumTroops = 0;
					pGroup->pEnemyGroup->ubNumElites = 20;
				}
				else
				{
					pGroup->pEnemyGroup->ubNumAdmins = 0;
					pGroup->pEnemyGroup->ubNumTroops = 20;
					pGroup->pEnemyGroup->ubNumElites = 0;
				}
			}
		}
	}
}


static void UpdateMercMercContractInfo(void)
{
	UINT8	ubCnt;

	for( ubCnt=BIFF; ubCnt<= BUBBA; ubCnt++ )
	{
		SOLDIERTYPE* const pSoldier = FindSoldierByProfileIDOnPlayerTeam(ubCnt);

		//if the merc is on the team
		if( pSoldier == NULL )
			continue;

		gMercProfiles[ ubCnt ].iMercMercContractLength = pSoldier->iTotalContractLength;

		pSoldier->iTotalContractLength = 0;
	}
}

INT8 GetNumberForAutoSave( BOOLEAN fLatestAutoSave )
{
	BOOLEAN	fFile1Exist, fFile2Exist;
	SGP_FILETIME	CreationTime1, LastAccessedTime1, LastWriteTime1;
	SGP_FILETIME	CreationTime2, LastAccessedTime2, LastWriteTime2;

	fFile1Exist = FALSE;
	fFile2Exist = FALSE;

	//The name of the file
	char zFileName1[256];
	sprintf(zFileName1, "%s/Auto%02d.%s", g_savegame_dir, 0, g_savegame_ext);
	char zFileName2[256];
	sprintf(zFileName2, "%s/Auto%02d.%s", g_savegame_dir, 1, g_savegame_ext);

	if( FileExists( zFileName1 ) )
	{
		AutoSGPFile hFile(FileOpen(zFileName1, FILE_ACCESS_READ));
		GetFileManFileTime( hFile, &CreationTime1, &LastAccessedTime1, &LastWriteTime1 );
		fFile1Exist = TRUE;
	}

	if( FileExists( zFileName2 ) )
	{
		AutoSGPFile hFile(FileOpen(zFileName2, FILE_ACCESS_READ));
		GetFileManFileTime( hFile, &CreationTime2, &LastAccessedTime2, &LastWriteTime2 );
		fFile2Exist = TRUE;
	}

	if( !fFile1Exist && !fFile2Exist )
		return( -1 );
	else if( fFile1Exist && !fFile2Exist )
	{
		if( fLatestAutoSave )
			return( 0 );
		else
			return( -1 );
	}
	else if( !fFile1Exist && fFile2Exist )
	{
		if( fLatestAutoSave )
			return( 1 );
		else
			return( -1 );
	}
	else
	{
		if( CompareSGPFileTimes( &LastWriteTime1, &LastWriteTime2 ) > 0 )
			return( 0 );
		else
			return( 1 );
	}
}


static void HandleOldBobbyRMailOrders(void)
{
	INT32 iCnt;
	INT32	iNewListCnt=0;

	if( LaptopSaveInfo.usNumberOfBobbyRayOrderUsed != 0 )
	{
		gpNewBobbyrShipments = MALLOCN(NewBobbyRayOrderStruct, LaptopSaveInfo.usNumberOfBobbyRayOrderUsed);

		giNumberOfNewBobbyRShipment = LaptopSaveInfo.usNumberOfBobbyRayOrderUsed;

		//loop through and add the old items to the new list
		for( iCnt=0; iCnt< LaptopSaveInfo.usNumberOfBobbyRayOrderItems; iCnt++ )
		{
			//if this slot is used
			if( LaptopSaveInfo.BobbyRayOrdersOnDeliveryArray[iCnt].fActive )
			{
				//copy over the purchase info
				memcpy( gpNewBobbyrShipments[ iNewListCnt ].BobbyRayPurchase,
								LaptopSaveInfo.BobbyRayOrdersOnDeliveryArray[iCnt].BobbyRayPurchase,
								sizeof( BobbyRayPurchaseStruct ) * MAX_PURCHASE_AMOUNT );

				gpNewBobbyrShipments[ iNewListCnt ].fActive = TRUE;
				gpNewBobbyrShipments[ iNewListCnt ].ubDeliveryLoc = BR_DRASSEN;
				gpNewBobbyrShipments[ iNewListCnt ].ubDeliveryMethod = 0;
				gpNewBobbyrShipments[ iNewListCnt ].ubNumberPurchases = LaptopSaveInfo.BobbyRayOrdersOnDeliveryArray[iCnt].ubNumberPurchases;
				gpNewBobbyrShipments[ iNewListCnt ].uiPackageWeight = 1;
				gpNewBobbyrShipments[ iNewListCnt ].uiOrderedOnDayNum = GetWorldDay();
				gpNewBobbyrShipments[ iNewListCnt ].fDisplayedInShipmentPage = TRUE;

				iNewListCnt++;
			}
		}

		//Clear out the old list
		LaptopSaveInfo.usNumberOfBobbyRayOrderUsed = 0;
		MemFree( LaptopSaveInfo.BobbyRayOrdersOnDeliveryArray );
		LaptopSaveInfo.BobbyRayOrdersOnDeliveryArray = NULL;
	}
}


static UINT32 CalcJA2EncryptionSet(SAVED_GAME_HEADER* pSaveGameHeader)
{
	UINT32	uiEncryptionSet = 0;

	uiEncryptionSet = pSaveGameHeader->uiSavedGameVersion;
	uiEncryptionSet *= pSaveGameHeader->uiFlags;
	uiEncryptionSet += pSaveGameHeader->iCurrentBalance;
	uiEncryptionSet *= (pSaveGameHeader->ubNumOfMercsOnPlayersTeam + 1);
	uiEncryptionSet += pSaveGameHeader->bSectorZ * 3;
	uiEncryptionSet += pSaveGameHeader->ubLoadScreenID;

	if ( pSaveGameHeader->fAlternateSector )
	{
		uiEncryptionSet += 7;
	}

	if ( pSaveGameHeader->uiRandom % 2 == 0 )
	{
		uiEncryptionSet++;

		if ( pSaveGameHeader->uiRandom % 7 == 0)
		{
			uiEncryptionSet++;
			if ( pSaveGameHeader->uiRandom % 23 == 0 )
			{
				uiEncryptionSet++;
			}
			if ( pSaveGameHeader->uiRandom % 79 == 0 )
			{
				uiEncryptionSet += 2;
			}
		}
	}

	#ifdef GERMAN
		uiEncryptionSet *= 11;
	#endif

	uiEncryptionSet = uiEncryptionSet % 10;

	uiEncryptionSet += pSaveGameHeader->uiDay / 10;

	uiEncryptionSet = uiEncryptionSet % 19;

	// now pick a different set of #s depending on what game options we've chosen
	if ( pSaveGameHeader->sInitialGameOptions.fGunNut )
	{
		uiEncryptionSet += BASE_NUMBER_OF_ROTATION_ARRAYS * 6;
	}

	if ( pSaveGameHeader->sInitialGameOptions.fSciFi )
	{
		uiEncryptionSet += BASE_NUMBER_OF_ROTATION_ARRAYS * 3;
	}

	switch( pSaveGameHeader->sInitialGameOptions.ubDifficultyLevel )
	{
		case DIF_LEVEL_EASY:
			uiEncryptionSet += 0;
			break;
		case DIF_LEVEL_MEDIUM:
			uiEncryptionSet += BASE_NUMBER_OF_ROTATION_ARRAYS;
			break;
		case DIF_LEVEL_HARD:
			uiEncryptionSet += BASE_NUMBER_OF_ROTATION_ARRAYS * 2;
			break;
	}

	return( uiEncryptionSet );
}
