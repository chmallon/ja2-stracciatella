#ifndef _SAVE_LOAD_SCREEN__H_
#define _SAVE_LOAD_SCREEN__H_

#include "JA2Types.h"


#define		NUM_SAVE_GAMES											11


//This flag is used to diferentiate between loading a game and saveing a game.
// gfSaveGame=TRUE		For saving a game
// gfSaveGame=FALSE		For loading a game
extern BOOLEAN gfSaveGame;

//if there is to be a slot selected when entering this screen
extern INT8			gbSetSlotToBeSelected;

extern	BOOLEAN			gbSaveGameArray[ NUM_SAVE_GAMES ];

extern	BOOLEAN			gfCameDirectlyFromGame;

UINT32	SaveLoadScreenHandle( void );
UINT32	SaveLoadScreenInit( void );

BOOLEAN		DoSaveLoadMessageBox( UINT8 ubStyle, const wchar_t *zString, UINT32 uiExitScreen, UINT16 usFlags, MSGBOX_CALLBACK ReturnCallback );

BOOLEAN DoQuickSave(void);
BOOLEAN DoQuickLoad(void);

BOOLEAN IsThereAnySavedGameFiles(void);

void			DeleteSaveGameNumber( UINT8 ubSaveGameSlotID );

#if defined JA2BETAVERSION
void ErrorDetectedInSaveCallback(UINT8 bValue);
#endif

#endif
