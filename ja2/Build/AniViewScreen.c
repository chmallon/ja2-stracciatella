#include "SGP.h"
#include "Gameloop.h"
#include "HImage.h"
#include "VObject.h"
#include "WCheck.h"
#include "WorldDef.h"
#include "RenderWorld.h"
#include "Input.h"
#include "Font.h"
#include "ScreenIDs.h"
#include "Overhead.h"
#include "Isometric_Utils.h"
#include "SysUtil.h"
#include "Font_Control.h"
#include "Radar_Screen.h"
#include "Soldier_Control.h"
#include "Animation_Control.h"
#include "Animation_Data.h"
#include "Timer_Control.h"
#include "Render_Dirty.h"
#include "Sys_Globals.h"
#include "Interface.h"
#include "Soldier_Ani.h"
#include "English.h"
#include "FileMan.h"
#include "MessageBoxScreen.h"
#include "Video.h"
#include "MemMan.h"


BOOLEAN	gfAniEditMode = FALSE;
static UINT16		usStartAnim = 0;
static UINT8		ubStartHeight = 0;
static SOLDIERTYPE *pSoldier;

static BOOLEAN fOKFiles = FALSE;
static UINT8	 ubNumStates = 0;
static UINT16   *pusStates = NULL;
static INT8   ubCurLoadedState = 0;


static void CycleAnimations(void)
{
	INT32 cnt;

	// FInd the next animation with start height the same...
	for ( cnt = usStartAnim + 1; cnt < NUMANIMATIONSTATES; cnt++ )
	{
		if ( gAnimControl[ cnt ].ubHeight == ubStartHeight )
		{
			usStartAnim = ( UINT8) cnt;
			EVENT_InitNewSoldierAnim( pSoldier, usStartAnim, 0 , TRUE );
			return;
		}
	}

	usStartAnim = 0;
	EVENT_InitNewSoldierAnim( pSoldier, usStartAnim, 0 , TRUE );
}


UINT32 AniEditScreenInit(void)
{

  return TRUE;
}

// The ShutdownGame function will free up/undo all things that were started in InitializeGame()
// It will also be responsible to making sure that all Gaming Engine tasks exit properly

UINT32 AniEditScreenShutdown(void)
{

	return TRUE;
}


static void BuildListFile(void);


UINT32  AniEditScreenHandle(void)
{
  InputAtom					InputEvent;
	static BOOLEAN		fFirstTime = TRUE;
	static UINT16			usOldState;
	static BOOLEAN		fToggle = FALSE;
	static BOOLEAN		fToggle2 = FALSE;

	// Make backups
	if ( fFirstTime )
	{
		gfAniEditMode = TRUE;

		usStartAnim   = 0;
		ubStartHeight = ANIM_STAND;

		fFirstTime = FALSE;
		fToggle		 = FALSE;
		fToggle2   = FALSE;
		ubCurLoadedState = 0;

		pSoldier = MercPtrs[ gusSelectedSoldier ];

		gTacticalStatus.uiFlags |= LOADING_SAVED_GAME;

		EVENT_InitNewSoldierAnim( pSoldier, usStartAnim, 0 , TRUE );

		BuildListFile( );

	}



	/////////////////////////////////////////////////////
	StartFrameBufferRender( );

	RenderWorld( );

	ExecuteBaseDirtyRectQueue( );


	/////////////////////////////////////////////////////
	EndFrameBufferRender( );


	SetFont( LARGEFONT1 );
	mprintf( 0,0,L"SOLDIER ANIMATION VIEWER" );
	gprintfdirty( 0,0,L"SOLDIER ANIMATION VIEWER" );


	mprintf( 0,20,L"Current Animation: %ls %ls", gAnimControl[ usStartAnim ].zAnimStr, gAnimSurfaceDatabase[ pSoldier->usAnimSurface ].Filename );
	gprintfdirty( 0,20,L"Current Animation: %ls %ls", gAnimControl[ usStartAnim ].zAnimStr, gAnimSurfaceDatabase[ pSoldier->usAnimSurface ].Filename );


	switch( ubStartHeight )
	{
		case ANIM_STAND:

			mprintf( 0,40,L"Current Stance: STAND" );
			break;

		case ANIM_CROUCH:

			mprintf( 0,40,L"Current Stance: CROUCH" );
			break;

		case ANIM_PRONE:

			mprintf( 0,40,L"Current Stance: PRONE" );
			break;
	}
	gprintfdirty( 0,40,L"Current Animation: %ls", gAnimControl[ usStartAnim ].zAnimStr );


	if ( fToggle )
	{
		mprintf( 0,60,L"FORCE ON" );
		gprintfdirty( 0,60,L"FORCE OFF" );
	}

	if ( fToggle2 )
	{
		mprintf( 0,70,L"LOADED ORDER ON" );
		gprintfdirty( 0,70,L"LOADED ORDER ON" );

		mprintf( 0,90,L"LOADED ORDER : %ls", gAnimControl[ pusStates[ ubCurLoadedState ] ].zAnimStr );
		gprintfdirty( 0,90,L"LOADED ORDER : %ls", gAnimControl[ pusStates[ ubCurLoadedState ] ].zAnimStr );

	}

  if (DequeueEvent(&InputEvent) == TRUE)
  {
    if (InputEvent.usEvent == KEY_DOWN && InputEvent.usParam == SDLK_ESCAPE)
    {
			 fFirstTime = TRUE;

			 gfAniEditMode = FALSE;

	  	 fFirstTimeInGameScreen = TRUE;

			 gTacticalStatus.uiFlags &= (~LOADING_SAVED_GAME);

			 if ( fOKFiles )
			 {
					 MemFree( pusStates );
			 }

			 fOKFiles = FALSE;

			 return( GAME_SCREEN );
    }

		if (InputEvent.usEvent == KEY_UP && InputEvent.usParam == SDLK_SPACE)
		{
			if ( !fToggle && !fToggle2 )
			{
				CycleAnimations( );
			}
		}

		if ((InputEvent.usEvent == KEY_UP) && (InputEvent.usParam == 's' ))
		{
			if ( !fToggle )
			{
				UINT16 usAnim=0;
				usOldState = usStartAnim;

				switch( ubStartHeight )
				{
					case ANIM_STAND:

						usAnim = STANDING;
						break;

					case ANIM_CROUCH:

						usAnim = CROUCHING;
						break;

					case ANIM_PRONE:

						usAnim = PRONE;
						break;
				}

				EVENT_InitNewSoldierAnim( pSoldier, usAnim, 0 , TRUE );
			}
			else
			{
				EVENT_InitNewSoldierAnim( pSoldier, usOldState, 0 , TRUE );
			}

			fToggle = !fToggle;
		}

		if ((InputEvent.usEvent == KEY_UP) && (InputEvent.usParam == 'l' ))
		{
			if ( !fToggle2 )
			{
				usOldState = usStartAnim;

				EVENT_InitNewSoldierAnim( pSoldier, pusStates[ ubCurLoadedState ], 0 , TRUE );
			}
			else
			{
				EVENT_InitNewSoldierAnim( pSoldier, usOldState, 0 , TRUE );
			}

			fToggle2 = !fToggle2;
		}

		if (InputEvent.usEvent == KEY_UP && InputEvent.usParam == SDLK_PAGEUP)
		{
			 if ( fOKFiles && fToggle2 )
			 {
					ubCurLoadedState++;

					if ( ubCurLoadedState == ubNumStates )
					{
						ubCurLoadedState = 0;
					}

					EVENT_InitNewSoldierAnim( pSoldier, pusStates[ ubCurLoadedState ], 0 , TRUE );

			 }
		}

		if (InputEvent.usEvent == KEY_UP && InputEvent.usParam == SDLK_PAGEDOWN)
		{
			 if ( fOKFiles && fToggle2 )
			 {
					ubCurLoadedState--;

					if ( ubCurLoadedState == 0 )
					{
						ubCurLoadedState = ubNumStates;
					}

					EVENT_InitNewSoldierAnim( pSoldier, pusStates[ ubCurLoadedState ], 0 , TRUE );
			 }
		}

		if ((InputEvent.usEvent == KEY_UP) && (InputEvent.usParam == 'c' ))
		{
			// CLEAR!
			usStartAnim = 0;
			EVENT_InitNewSoldierAnim( pSoldier, usStartAnim, 0 , TRUE );
		}

		if (InputEvent.usEvent == KEY_UP && InputEvent.usParam == SDLK_RETURN)
		{
			if ( ubStartHeight == ANIM_STAND )
			{
				ubStartHeight = ANIM_CROUCH;
			}
			else if ( ubStartHeight == ANIM_CROUCH )
			{
				ubStartHeight = ANIM_PRONE;
			}
			else
			{
				ubStartHeight = ANIM_STAND;
			}
		}

  }


  return( ANIEDIT_SCREEN );

}


static UINT16 GetAnimStateFromName(const char* zName)
{
	INT32 cnt;

	// FInd the next animation with start height the same...
	for ( cnt = 0; cnt < NUMANIMATIONSTATES; cnt++ )
	{
		if (strcasecmp(gAnimControl[cnt].zAnimStr, zName) == 0) return cnt;
	}

	return( 5555 );
}


static void BuildListFile(void)
{
	FILE *infoFile;
	char currFilename[128];
	int numEntries = 0;
	int	cnt;
	UINT16 usState;
	wchar_t zError[128];


	//Verify the existance of the header text file.
	infoFile = fopen( "ANITEST.DAT", "rb");
	if(!infoFile)
	{
		return;
	}
	//count STIs inside header and verify each one's existance.
	while( !feof( infoFile ) )
	{
		fgets( currFilename, 128, infoFile );
		//valid entry in header, continue on...

		numEntries++;
	}
	fseek( infoFile, 0, SEEK_SET ); //reset header file

	// Allocate array
	pusStates = MemAlloc( sizeof( UINT16 ) * numEntries );

	fOKFiles = TRUE;

	cnt = 0;
	while( !feof( infoFile ) )
	{
		fgets( currFilename, 128, infoFile );

		// Remove newline
		currFilename[ strlen( currFilename ) -1 ] = '\0';
		currFilename[ strlen( currFilename ) -1 ] = '\0';

		usState = GetAnimStateFromName( currFilename );

		if ( usState != 5555 )
		{
			cnt++;
			ubNumStates	= (UINT8)cnt;
			pusStates[ cnt ] = usState;
		}
		else
		{
			swprintf( zError, lengthof(zError), L"Animation str %ls is not known: ", currFilename );
			DoMessageBox( MSG_BOX_BASIC_STYLE, zError, ANIEDIT_SCREEN, ( UINT8 )MSG_BOX_FLAG_YESNO, NULL, NULL );
			fclose( infoFile );
			return;
		}
	}

}
