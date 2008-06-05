#include "Font_Control.h"
#include "MessageBoxScreen.h"
#include "Types.h"
#include "GameSettings.h"
#include "FileMan.h"
#include "Sound_Control.h"
#include "SaveLoadScreen.h"
#include "Music_Control.h"
#include "Options_Screen.h"
#include "Overhead.h"
#include "GameVersion.h"
#include "LibraryDataBase.h"
#include "Debug.h"
#include "HelpScreen.h"
#include "Meanwhile.h"
#include "Cheats.h"
#include "Message.h"
#include "Campaign.h"
#include "StrategicMap.h"
#include "Queen_Command.h"
#include "ScreenIDs.h"
#include "Random.h"
#include "SGP.h"
#include "SaveLoadGame.h"
#include	"Text.h"


#define				GAME_SETTINGS_FILE		"../Ja2.set"

GAME_SETTINGS		gGameSettings;
GAME_OPTIONS		gGameOptions;


//Change this number when we want any who gets the new build to reset the options
#define				GAME_SETTING_CURRENT_VERSION		522


static void InitGameSettings(void);


void LoadGameSettings(void)
{
	try
	{
		AutoSGPFile f(FileOpen(GAME_SETTINGS_FILE, FILE_ACCESS_READ));
		if (!f) goto fail;

		GAME_SETTINGS* const g = &gGameSettings;
		if (FileGetSize(f) != sizeof(*g))                              goto fail;
		FileRead(f, g, sizeof(*g));
		if (g->uiSettingsVersionNumber < GAME_SETTING_CURRENT_VERSION) goto fail;

		// Do checking to make sure the settings are valid
		if (g->bLastSavedGameSlot < 0 || NUM_SAVE_GAMES <= g->bLastSavedGameSlot) g->bLastSavedGameSlot = -1;
		if (g->ubMusicVolumeSetting > HIGHVOLUME) g->ubMusicVolumeSetting = MIDVOLUME;
		if (g->ubSoundEffectsVolume > HIGHVOLUME) g->ubSoundEffectsVolume = MIDVOLUME;
		if (g->ubSpeechVolume       > HIGHVOLUME) g->ubSpeechVolume       = MIDVOLUME;

		// Make sure that at least subtitles or speech is enabled
		if (!g->fOptions[TOPTION_SUBTITLES] && !g->fOptions[TOPTION_SPEECH])
		{
			g->fOptions[TOPTION_SUBTITLES] = TRUE;
			g->fOptions[TOPTION_SPEECH   ] = TRUE;
		}

		// Set the settings
		SetSoundEffectsVolume(g->ubSoundEffectsVolume);
		SetSpeechVolume(g->ubSpeechVolume);
		MusicSetVolume(g->ubMusicVolumeSetting);

		// If the user doesn't want the help screens present
		if (g->fHideHelpInAllScreens)
		{
			gHelpScreen.usHasPlayerSeenHelpScreenInCurrentScreen = 0;
		}
		else
		{
			// Set it so that every screens help will come up the first time (the 'x' will be set)
			gHelpScreen.usHasPlayerSeenHelpScreenInCurrentScreen = 0xffff;
		}
		return;
	}
	catch (...) { /* Handled below */ }

fail:
	InitGameSettings();
}


void SaveGameSettings(void)
{
	AutoSGPFile f(FileOpen(GAME_SETTINGS_FILE, FILE_ACCESS_WRITE | FILE_CREATE_ALWAYS));
	if (!f) return;

	// Record the current settings into the game settins structure
	GAME_SETTINGS* const g = &gGameSettings;
	g->ubSoundEffectsVolume    = GetSoundEffectsVolume();
	g->ubSpeechVolume          = GetSpeechVolume();
	g->ubMusicVolumeSetting    = MusicGetVolume();
	strcpy(g->zVersionNumber, g_version_number);
	g->uiSettingsVersionNumber = GAME_SETTING_CURRENT_VERSION;

	// Write the game settings to disk
	FileWrite(f, g, sizeof(*g));
}


static void InitGameSettings(void)
{
	GAME_SETTINGS* const g = &gGameSettings;
	memset(g, 0, sizeof(*g));

	// Init the Game Settings
	g->bLastSavedGameSlot   = -1;
	g->ubMusicVolumeSetting = 63;
	g->ubSoundEffectsVolume = 63;
	g->ubSpeechVolume       = 63;

	// Set the settings
	SetSoundEffectsVolume(g->ubSoundEffectsVolume);
	SetSpeechVolume(g->ubSpeechVolume);
	MusicSetVolume(g->ubMusicVolumeSetting);

	g->fOptions[TOPTION_SUBTITLES]                 = TRUE;
	g->fOptions[TOPTION_SPEECH]                    = TRUE;
	g->fOptions[TOPTION_KEY_ADVANCE_SPEECH]        = FALSE;
	g->fOptions[TOPTION_RTCONFIRM]                 = FALSE;
	g->fOptions[TOPTION_HIDE_BULLETS]              = FALSE;
	g->fOptions[TOPTION_TRACKING_MODE]             = TRUE;
	g->fOptions[TOPTION_MUTE_CONFIRMATIONS]        = FALSE;
	g->fOptions[TOPTION_ANIMATE_SMOKE]             = TRUE;
	g->fOptions[TOPTION_BLOOD_N_GORE]              = TRUE;
	g->fOptions[TOPTION_DONT_MOVE_MOUSE]           = FALSE;
	g->fOptions[TOPTION_OLD_SELECTION_METHOD]      = FALSE;
	g->fOptions[TOPTION_ALWAYS_SHOW_MOVEMENT_PATH] = FALSE;

	g->fOptions[TOPTION_SLEEPWAKE_NOTIFICATION]    = TRUE;

	g->fOptions[TOPTION_USE_METRIC_SYSTEM]         = FALSE;

	g->fOptions[TOPTION_MERC_ALWAYS_LIGHT_UP]      = FALSE;
	g->fOptions[TOPTION_SMART_CURSOR]              = FALSE;

	g->fOptions[TOPTION_SNAP_CURSOR_TO_DOOR]       = TRUE;
	g->fOptions[TOPTION_GLOW_ITEMS]                = TRUE;
	g->fOptions[TOPTION_TOGGLE_TREE_TOPS]          = TRUE;
	g->fOptions[TOPTION_TOGGLE_WIREFRAME]          = TRUE;
	g->fOptions[TOPTION_3D_CURSOR]                 = FALSE;
	// JA2Gold
	g->fOptions[TOPTION_MERC_CASTS_LIGHT]          = TRUE;

	g->ubSizeOfDisplayCover = 4;
	g->ubSizeOfLOS          = 4;

	// Since we just set the settings, save them
	SaveGameSettings();
}


void InitGameOptions()
{
	memset( &gGameOptions, 0, sizeof( GAME_OPTIONS ) );

	//Init the game options
	gGameOptions.fGunNut					 = FALSE;
	gGameOptions.fSciFi						 = TRUE;
	gGameOptions.ubDifficultyLevel = DIF_LEVEL_EASY;
	//gGameOptions.fTurnTimeLimit		 = FALSE;
	gGameOptions.fIronManMode			 = FALSE;

}


void CDromEjectionErrorMessageBoxCallBack( UINT8 bExitValue )
{
	if( bExitValue == MSG_BOX_RETURN_OK )
	{
		guiPreviousOptionScreen = GAME_SCREEN;

		//if we are in a game, save the game
		if( gTacticalStatus.fHasAGameBeenStarted )
		{
			SaveGame( SAVE__ERROR_NUM, pMessageStrings[ MSG_CDROM_SAVE ] );
		}

 		//quit the game
		gfProgramIsRunning = FALSE;
	}
}


void DisplayGameSettings( )
{
	//Display the version number
	ScreenMsg(FONT_MCOLOR_LTYELLOW, MSG_INTERFACE, L"%ls: %hs (%hs)", pMessageStrings[MSG_VERSION], g_version_label, g_version_number);

	//Display the difficulty level
	ScreenMsg(FONT_MCOLOR_LTYELLOW, MSG_INTERFACE, L"%ls: %ls", gzGIOScreenText[GIO_DIF_LEVEL_TEXT], gzGIOScreenText[gGameOptions.ubDifficultyLevel + GIO_EASY_TEXT - 1]);

	//Iron man option
	ScreenMsg(FONT_MCOLOR_LTYELLOW, MSG_INTERFACE, L"%ls: %ls", gzGIOScreenText[GIO_GAME_SAVE_STYLE_TEXT], gzGIOScreenText[GIO_SAVE_ANYWHERE_TEXT + gGameOptions.fIronManMode]);

	// Gun option
	ScreenMsg(FONT_MCOLOR_LTYELLOW, MSG_INTERFACE, L"%ls: %ls", gzGIOScreenText[GIO_GUN_OPTIONS_TEXT], gzGIOScreenText[gGameOptions.fGunNut ? GIO_GUN_NUT_TEXT : GIO_REDUCED_GUNS_TEXT]);

	//Sci fi option
	ScreenMsg(FONT_MCOLOR_LTYELLOW, MSG_INTERFACE, L"%ls: %ls", gzGIOScreenText[GIO_GAME_STYLE_TEXT], gzGIOScreenText[GIO_REALISTIC_TEXT + gGameOptions.fSciFi]);

	//Timed Turns option
	// JA2Gold: no timed turns
	//ScreenMsg(FONT_MCOLOR_LTYELLOW, MSG_INTERFACE, L"%ls: %ls", gzGIOScreenText[GIO_TIMED_TURN_TITLE_TEXT], gzGIOScreenText[GIO_NO_TIMED_TURNS_TEXT + gGameOptions.fTurnTimeLimit]);

	if (CHEATER_CHEAT_LEVEL())
	{
		ScreenMsg(FONT_MCOLOR_LTYELLOW, MSG_INTERFACE, gzLateLocalizedString[58], CurrentPlayerProgressPercentage(), HighestPlayerProgressPercentage());
	}
}

BOOLEAN MeanwhileSceneSeen( UINT8 ubMeanwhile )
{
	UINT32	uiCheckFlag;

	if ( ubMeanwhile > 32 || ubMeanwhile > NUM_MEANWHILES )
	{
		return( FALSE );
	}

	uiCheckFlag = 0x1 << ubMeanwhile;

	if ( gGameSettings.uiMeanwhileScenesSeenFlags & uiCheckFlag )
	{
		return( TRUE );
	}
	else
	{
		return( FALSE );
	}
}

BOOLEAN SetMeanwhileSceneSeen( UINT8 ubMeanwhile )
{
	UINT32	uiCheckFlag;

	if ( ubMeanwhile > 32 || ubMeanwhile > NUM_MEANWHILES )
	{
		// can't set such a flag!
		return( FALSE );
	}
	uiCheckFlag = 0x1 << ubMeanwhile;
	gGameSettings.uiMeanwhileScenesSeenFlags |= uiCheckFlag;
	return( TRUE );
}

BOOLEAN	CanGameBeSaved()
{
	//if the iron man mode is on
	if( gGameOptions.fIronManMode )
	{
		//if we are in turn based combat
		if( (gTacticalStatus.uiFlags & TURNBASED) && (gTacticalStatus.uiFlags & INCOMBAT) )
		{
			//no save for you
			return( FALSE );
		}

		//if there are enemies in the current sector
		if( gWorldSectorX != -1 && gWorldSectorY != -1 &&
				gWorldSectorX != 0 && gWorldSectorY != 0 &&
				NumEnemiesInAnySector( gWorldSectorX, gWorldSectorY, gbWorldSectorZ ) > 0 )
		{
			//no save for you
			return( FALSE );
		}

		//All checks failed, so we can save
		return( TRUE );
	}
	else
	{
		return( TRUE );
	}
}
