#ifdef JA2EDITOR

#include "Button_System.h"
#include "FileMan.h"
#include "Font.h"
#include "Font_Control.h"
#include "HImage.h"
#include "Local.h"
#include "RenderWorld.h"
#include "Render_Dirty.h"
#include "LoadScreen.h"
#include "SelectWin.h"
#include "EditorDefines.h"
#include "MessageBox.h"
#include "Text_Input.h"
#include "Soldier_Create.h"
#include "Soldier_Init_List.h"
#include "EditorBuildings.h"
#include "Editor_Taskbar_Utils.h"
#include "Editor_Undo.h"
#include "EditScreen.h"
#include "StrategicMap.h"
#include "Editor_Modes.h"
#include "Map_Information.h"
#include "Sys_Globals.h"
#include "Sector_Summary.h"
#include "NewSmooth.h"
#include "Simple_Render_Utils.h"
#include "Animated_ProgressBar.h"
#include "EditorMercs.h"
#include "Lighting.h"
#include "EditorMapInfo.h"
#include "Environment.h"
#include "Edit_Sys.h"
#include "EditorItems.h"
#include "English.h"
#include "GameLoop.h"
#include "Message.h"
#include "Pits.h"
#include "Item_Statistics.h"
#include "Scheduling.h"
#include "Debug.h"
#include "JAScreens.h"
#include "MemMan.h"
#include "MessageBoxScreen.h"
#include "ScreenIDs.h"
#include "Timer_Control.h"
#include "VObject.h"
#include "VSurface.h"
#include "Video.h"
#include "WorldDef.h"


BOOLEAN gfErrorCatch = FALSE;
wchar_t gzErrorCatchString[256] = L"";


enum{
	DIALOG_NONE,
	DIALOG_SAVE,
	DIALOG_LOAD,
	DIALOG_CANCEL,
	DIALOG_DELETE
};

static INT32 iTotalFiles;
static INT32 iTopFileShown;
static INT32 iCurrFileShown;
static INT32	iLastFileClicked;
static INT32 iLastClickTime;

static wchar_t gzFilename[31];

static FDLG_LIST* FileList = NULL;

static INT32 iFDlgState = DIALOG_NONE;
static INT32 iFileDlgButtons[7];

static BOOLEAN gfLoadError;
static BOOLEAN gfReadOnly;
static BOOLEAN gfFileExists;
static BOOLEAN gfIllegalName;
static BOOLEAN gfDeleteFile;
static BOOLEAN gfNoFiles;

static BOOLEAN fEnteringLoadSaveScreen = TRUE;

static MOUSE_REGION BlanketRegion;

static char gszCurrFilename[80];

enum{
	IOSTATUS_NONE,
	INITIATE_MAP_SAVE,
	SAVING_MAP,
	INITIATE_MAP_LOAD,
	LOADING_MAP
};
static INT8 gbCurrentFileIOStatus; // 1 init saving message, 2 save, 3 init loading message, 4 load, 0 none


UINT32 LoadSaveScreenInit(void)
{
	gfUpdateSummaryInfo = TRUE;
	fEnteringLoadSaveScreen = TRUE;
	return TRUE;
}


static void CreateFileDialog(const wchar_t* zTitle);
static void TrashFDlgList(FDLG_LIST* pList);


static void LoadSaveScreenEntry(void)
{
	fEnteringLoadSaveScreen = FALSE;
	gbCurrentFileIOStatus	= IOSTATUS_NONE;

	gfReadOnly = FALSE;
	gfFileExists = FALSE;
	gfLoadError = FALSE;
	gfIllegalName = FALSE;
	gfDeleteFile = FALSE;
	gfNoFiles = FALSE;

	// setup filename dialog box
	// (*.dat and *.map) as file filter

	// If user clicks on a filename in the window, then turn off string focus and re-init the string with the new name.
	// If user hits ENTER or presses OK, then continue with the file loading/saving

	if( FileList )
		TrashFDlgList( FileList );

	iTopFileShown = iTotalFiles = 0;
	FindFileInfo* const find_info = FindFiles("MAPS/*.dat");
	if (find_info != NULL)
	{
		for (;;)
		{
			const char* const filename = FindFilesNext(find_info);
			if (filename == NULL) break;

			FileList = AddToFDlgList(FileList, filename);
			++iTotalFiles;
		}
		FindFilesFree(find_info);
	}

	swprintf(gzFilename, lengthof(gzFilename), L"%hs", g_filename);

	CreateFileDialog(iCurrentAction == ACTION_SAVE_MAP ? L"Save Map (*.dat)" : L"Load Map (*.dat)");

	if( !iTotalFiles )
	{
		gfNoFiles = TRUE;
		if( iCurrentAction == ACTION_LOAD_MAP )
			DisableButton( iFileDlgButtons[0] );
	}

	iLastFileClicked = -1;
	iLastClickTime = 0;

}


static void RemoveFileDialog(void);
static BOOLEAN RemoveFromFDlgList(FDLG_LIST** head, FDLG_LIST* node);
static BOOLEAN ValidFilename(void);


static UINT32 ProcessLoadSaveScreenMessageBoxResult(void)
{
	FDLG_LIST *curr, *temp;
	gfRenderWorld = TRUE;
	RemoveMessageBox();
	if( gfIllegalName )
	{
		fEnteringLoadSaveScreen = TRUE;
		RemoveFileDialog();
		MarkWorldDirty();
		return gfMessageBoxResult ? LOADSAVE_SCREEN : EDIT_SCREEN;
	}
	if( gfDeleteFile )
	{
		if( gfMessageBoxResult )
		{ //delete file
			INT32 x;
			curr = FileList;
			for( x = 0; x < iCurrFileShown && x < iTotalFiles && curr; x++ )
			{
				curr = curr->pNext;
			}
			if( curr )
			{
				FileDelete( gszCurrFilename );

				//File is deleted so redo the text fields so they show the
				//next file in the list.
				temp = curr->pNext;
				if( !temp )
					temp = curr->pPrev;
				if( !temp )
					wcscpy( gzFilename, L"" );
				else
					swprintf(gzFilename, lengthof(gzFilename), L"%hs", temp->filename);
				if( ValidFilename() )
				{
					SetInputFieldStringWith16BitString( 0, gzFilename );
				}
				else
				{
					SetInputFieldStringWith16BitString( 0, L"" );
					wcscpy( gzFilename, L"" );
				}
				RemoveFromFDlgList( &FileList, curr );
				iTotalFiles--;
				if( !iTotalFiles )
				{
					gfNoFiles = TRUE;
					if( iCurrentAction == ACTION_LOAD_MAP )
						DisableButton( iFileDlgButtons[0] );
				}
				if( iCurrFileShown >= iTotalFiles )
					iCurrFileShown--;
				if( iCurrFileShown < iTopFileShown )
					iTopFileShown -= 8;
				if( iTopFileShown < 0 )
					iTopFileShown = 0;
			}
		}
		MarkWorldDirty();
		RenderWorld();
		gfDeleteFile = FALSE;
		iFDlgState = DIALOG_NONE;
		return LOADSAVE_SCREEN;
	}
	if( gfLoadError )
	{
		fEnteringLoadSaveScreen = TRUE;
		return gfMessageBoxResult ? LOADSAVE_SCREEN : EDIT_SCREEN;
	}
	if( gfReadOnly )
	{ //file is readonly.  Result will determine if the file dialog stays up.
		fEnteringLoadSaveScreen = TRUE;
		RemoveFileDialog();
		return gfMessageBoxResult ? LOADSAVE_SCREEN : EDIT_SCREEN;
	}
	if( gfFileExists )
	{
		if( gfMessageBoxResult )
		{ //okay to overwrite file
			RemoveFileDialog();
			gbCurrentFileIOStatus = INITIATE_MAP_SAVE;
			return LOADSAVE_SCREEN;
		}
		fEnteringLoadSaveScreen = TRUE;
		RemoveFileDialog();
		return EDIT_SCREEN ;
	}
	Assert( 0 );
	return LOADSAVE_SCREEN;
}


static void DrawFileDialog(void);
static BOOLEAN ExtractFilenameFromFields(void);
static void HandleMainKeyEvents(InputAtom* pEvent);
static UINT32 ProcessFileIO(void);


UINT32 LoadSaveScreenHandle(void)
{
	FDLG_LIST *FListNode;
	INT32 x;
	InputAtom DialogEvent;

	if( fEnteringLoadSaveScreen )
	{
		LoadSaveScreenEntry();
	}

	if( gbCurrentFileIOStatus ) //loading or saving map
	{
		UINT32 uiScreen;
		uiScreen = ProcessFileIO();
		if( uiScreen == EDIT_SCREEN && gbCurrentFileIOStatus == LOADING_MAP )
			RemoveProgressBar( 0 );
		return uiScreen;
	}

	if( gubMessageBoxStatus )
	{
		if( MessageBoxHandled() )
			return ProcessLoadSaveScreenMessageBoxResult();
		return LOADSAVE_SCREEN;
	}

	//handle all key input.
	while( DequeueEvent(&DialogEvent) )
	{
		if( !HandleTextInput(&DialogEvent) && (DialogEvent.usEvent == KEY_DOWN || DialogEvent.usEvent == KEY_REPEAT) )
		{
			HandleMainKeyEvents( &DialogEvent );
		}
	}

	DrawFileDialog();

	// Skip to first filename to show
	FListNode = FileList;
	for(x=0;x<iTopFileShown && x<iTotalFiles && FListNode != NULL;x++)
	{
		FListNode = FListNode->pNext;
	}

	// Show up to 8 filenames in the window
	SetFont( FONT12POINT1 );
	if( gfNoFiles )
	{
		SetFontForeground( FONT_LTRED );
		SetFontBackground( 142 );
	  MPrint(226, 126, L"NO FILES IN /MAPS DIRECTORY");
	}
	else for(x=iTopFileShown;x<(iTopFileShown+8) && x<iTotalFiles && FListNode != NULL; x++)
	{
		if( !EditingText() && x == iCurrFileShown  )
		{
			SetFontForeground( FONT_GRAY2 );
			SetFontBackground( FONT_METALGRAY );
		}
		else
		{
			SetFontForeground( FONT_BLACK );
			SetFontBackground( 142 );
		}
		mprintf(186, 73 + (x - iTopFileShown) * 15, L"%hs", FListNode->filename);
		FListNode = FListNode->pNext;
	}

	RenderAllTextFields();

	InvalidateScreen();

	ExecuteBaseDirtyRectQueue();
	EndFrameBufferRender();

	switch( iFDlgState )
	{
		case DIALOG_CANCEL:
			RemoveFileDialog();
			fEnteringLoadSaveScreen = TRUE;
			return EDIT_SCREEN;

		case DIALOG_DELETE:
		{
			sprintf( gszCurrFilename, "MAPS/%ls", gzFilename );
			const UINT32 attr = FileGetAttributes(gszCurrFilename);
			if (attr != FILE_ATTR_ERROR)
			{
				wchar_t str[40];
				if (attr & FILE_ATTR_READONLY)
				{
					swprintf(str, lengthof(str), L" Delete READ-ONLY file %ls? ", gzFilename);
				}
				else
					swprintf(str, lengthof(str), L" Delete file %ls? ", gzFilename);
				gfDeleteFile = TRUE;
				CreateMessageBox( str );
			}
			return LOADSAVE_SCREEN;
		}

		case DIALOG_SAVE:
			if( !ExtractFilenameFromFields() )
			{
				CreateMessageBox( L" Illegal filename.  Try another filename? " );
				gfIllegalName = TRUE;
				iFDlgState = DIALOG_NONE;
				return LOADSAVE_SCREEN;
			}
			sprintf( gszCurrFilename, "MAPS/%ls", gzFilename );
			if ( FileExists( gszCurrFilename ) )
			{
				gfFileExists = TRUE;
				gfReadOnly = FALSE;
				const UINT32 attr = FileGetAttributes(gszCurrFilename);
				if (attr != FILE_ATTR_ERROR && attr & FILE_ATTR_READONLY)
				{
					gfReadOnly = TRUE;
				}
				if( gfReadOnly )
					CreateMessageBox( L" File is read only!  Choose a different name? " );
				else
					CreateMessageBox( L" File exists, Overwrite? " );
				return( LOADSAVE_SCREEN );
			}
			RemoveFileDialog();
			gbCurrentFileIOStatus = INITIATE_MAP_SAVE;
			return LOADSAVE_SCREEN ;
		case DIALOG_LOAD:
			if( !ExtractFilenameFromFields() )
			{
				CreateMessageBox( L" Illegal filename.  Try another filename? " );
				gfIllegalName = TRUE;
				iFDlgState = DIALOG_NONE;
				return LOADSAVE_SCREEN;
			}
			RemoveFileDialog();
			CreateProgressBar( 0, 118, 183, 522, 202 );
			DefineProgressBarPanel( 0, 65, 79, 94, 100, 155, 540, 235 );
			wchar_t zOrigName[60];
			swprintf(zOrigName, lengthof(zOrigName), L"Loading map:  %ls", gzFilename);
			SetProgressBarTitle( 0, zOrigName, BLOCKFONT2, FONT_RED, FONT_NEARBLACK );
			gbCurrentFileIOStatus = INITIATE_MAP_LOAD;
			return LOADSAVE_SCREEN ;
		default:
			iFDlgState = DIALOG_NONE;
	}
	iFDlgState = DIALOG_NONE;
	return LOADSAVE_SCREEN ;
}


static INT32 MakeButtonArrow(const char* const gfx, const INT16 y, const GUI_CALLBACK click)
{
	const INT32 btn = QuickCreateButtonImg(gfx, -1, 1, 2, 3, 4, 426, y, MSYS_PRIORITY_HIGH, click);
	SpecifyDisabledButtonStyle(btn, DISABLED_STYLE_SHADED);
	return btn;
}


static void FDlgCancelCallback(GUI_BUTTON* butn, INT32 reason);
static void FDlgDwnCallback(GUI_BUTTON* butn, INT32 reason);
static void FDlgNamesCallback(GUI_BUTTON* butn, INT32 reason);
static void FDlgOkCallback(GUI_BUTTON* butn, INT32 reason);
static void FDlgUpCallback(GUI_BUTTON* butn, INT32 reason);
static void FileDialogModeCallback(UINT8 ubID, BOOLEAN fEntering);
static void UpdateWorldInfoCallback(GUI_BUTTON* b, INT32 reason);


static void CreateFileDialog(const wchar_t* zTitle)
{

	iFDlgState = DIALOG_NONE;

	DisableEditorTaskbar();

	MSYS_DefineRegion( &BlanketRegion, 0, 0, gsVIEWPORT_END_X, gsVIEWPORT_END_Y, MSYS_PRIORITY_HIGH - 5, 0, 0, 0 );

	//Okay and cancel buttons
	iFileDlgButtons[0] = CreateTextButton(L"Okay",   FONT12POINT1, FONT_BLACK, FONT_BLACK, 354, 225, 50, 30, MSYS_PRIORITY_HIGH, FDlgOkCallback);
	iFileDlgButtons[1] = CreateTextButton(L"Cancel", FONT12POINT1, FONT_BLACK, FONT_BLACK, 406, 225, 50, 30, MSYS_PRIORITY_HIGH, FDlgCancelCallback);

	//Scroll buttons
	iFileDlgButtons[2] = MakeButtonArrow("EDITOR/uparrow.sti",    92, FDlgUpCallback);
	iFileDlgButtons[3] = MakeButtonArrow("EDITOR/downarrow.sti", 182, FDlgDwnCallback);

	//File list window
	iFileDlgButtons[4] = CreateHotSpot(179 + 4, 69 + 3, 179 + 4 + 240, 69 + 120 + 3, MSYS_PRIORITY_HIGH - 1, FDlgNamesCallback);
	//Title button
	iFileDlgButtons[5] = CreateLabel(zTitle, HUGEFONT, FONT_LTKHAKI, FONT_DKKHAKI, 179, 39, 281, 30, MSYS_PRIORITY_HIGH - 2);

	iFileDlgButtons[6] = -1;
	if( iCurrentAction == ACTION_SAVE_MAP )
	{	//checkboxes
		//The update world info checkbox
		iFileDlgButtons[6] = CreateCheckBoxButton( 183, 229, "EDITOR/smcheckbox.sti", MSYS_PRIORITY_HIGH, UpdateWorldInfoCallback );
		if( gfUpdateSummaryInfo )
			ButtonList[ iFileDlgButtons[6] ]->uiFlags |= BUTTON_CLICKED_ON;
	}

	//Add the text input fields
	InitTextInputModeWithScheme( DEFAULT_SCHEME );
	//field 1 (filename)
	AddTextInputField( /*233*/183, 195, 190, 20, MSYS_PRIORITY_HIGH, gzFilename, 30, INPUTTYPE_EXCLUSIVE_DOSFILENAME );
	//field 2 -- user field that allows mouse/key interaction with the filename list
	AddUserInputField( FileDialogModeCallback );
}


static void UpdateWorldInfoCallback(GUI_BUTTON* b, INT32 reason)
{
	if( reason & MSYS_CALLBACK_REASON_LBUTTON_UP )
		gfUpdateSummaryInfo = b->uiFlags & BUTTON_CLICKED_ON ? TRUE : FALSE;
}


//This is a hook into the text input code.  This callback is called whenever the user is currently
//editing text, and presses Tab to transfer to the file dialog mode.  When this happens, we set the text
//field to the currently selected file in the list which is already know.
static void FileDialogModeCallback(UINT8 ubID, BOOLEAN fEntering)
{
	INT32 x;
	FDLG_LIST *FListNode;
	if( fEntering )
	{
		// Skip to first filename
		FListNode = FileList;
		for(x=0;x<iTopFileShown && x<iTotalFiles && FListNode != NULL;x++)
		{
			FListNode = FListNode->pNext;
		}
		// Find the already selected filename
		for(x = iTopFileShown; x < iTopFileShown + 8 && x < iTotalFiles && FListNode != NULL; x++ )
		{
			if( iCurrFileShown == (x-iTopFileShown) )
			{
				FListNode->filename[30] = '\0';
				SetInputFieldStringWith8BitString(0, FListNode->filename);
				return;
			}
			FListNode = FListNode->pNext;
		}
	}
}


static void RemoveFileDialog(void)
{
	INT32 x;

	MSYS_RemoveRegion( &BlanketRegion );

	for(x=0; x<6; x++)
	{
		RemoveButton(iFileDlgButtons[x]);
	}

	if( iFileDlgButtons[6] != -1 )
	{
		RemoveButton( iFileDlgButtons[6] );
	}

	TrashFDlgList( FileList );
	FileList = NULL;

	InvalidateScreen( );

	EnableEditorTaskbar();
	KillTextInputMode();
	MarkWorldDirty();
	RenderWorld();
	EndFrameBufferRender();
}


static void DrawFileDialog(void)
{
	ColorFillVideoSurfaceArea(FRAME_BUFFER,	179, 69, (179+281), 261, Get16BPPColor(FROMRGB(136, 138, 135)) );
	ColorFillVideoSurfaceArea(FRAME_BUFFER,	180, 70, (179+281), 261, Get16BPPColor(FROMRGB(24, 61, 81)) );
	ColorFillVideoSurfaceArea(FRAME_BUFFER,	180, 70, (179+280), 260, Get16BPPColor(FROMRGB(65, 79, 94)) );

	ColorFillVideoSurfaceArea(FRAME_BUFFER, (179+4), (69+3), (179+4+240), (69+123), Get16BPPColor(FROMRGB(24, 61, 81)) );
	ColorFillVideoSurfaceArea(FRAME_BUFFER, (179+5), (69+4), (179+4+240), (69+123), Get16BPPColor(FROMRGB(136, 138, 135)) );
	ColorFillVideoSurfaceArea(FRAME_BUFFER, (179+5), (69+4), (179+3+240), (69+122), Get16BPPColor(FROMRGB(250, 240, 188)) );

	MarkButtonsDirty();
	RenderButtons();
	RenderButtonsFastHelp();

	SetFont( FONT10ARIAL );
	SetFontForeground( FONT_LTKHAKI );
	SetFontShadow( FONT_DKKHAKI );
	SetFontBackground( FONT_BLACK );
	MPrint(183, 217, L"Filename");

	if( iFileDlgButtons[6] != -1 )
	{
		MPrint(200, 231, L"Update world info");
	}
}


//The callback calls this function passing the relative y position of where
//the user clicked on the hot spot.
static void SelectFileDialogYPos(UINT16 usRelativeYPos)
{
	INT16 sSelName;
	INT32 x;
	FDLG_LIST *FListNode;

	sSelName = usRelativeYPos / 15;

	//This is a field in the text editmode, but clicked via mouse.
	SetActiveField( 1 );

	// Skip to first filename
	FListNode = FileList;
	for(x=0;x<iTopFileShown && x<iTotalFiles && FListNode != NULL;x++)
	{
		FListNode = FListNode->pNext;
	}

	for(x=iTopFileShown;x<(iTopFileShown+8) && x<iTotalFiles && FListNode != NULL; x++)
	{
		if( (INT32)sSelName == (x-iTopFileShown) )
		{
			INT32 iCurrClickTime;
			iCurrFileShown = x;
			FListNode->filename[30] = '\0';
			swprintf(gzFilename, lengthof(gzFilename), L"%hs", FListNode->filename);
			if( ValidFilename() )
			{
				SetInputFieldStringWith16BitString( 0, gzFilename );
			}
			else
			{
				SetInputFieldStringWith16BitString( 0, L"" );
				wcscpy( gzFilename, L"" );
			}

			RenderInactiveTextField( 0 );

			//Calculate and process any double clicking...
			iCurrClickTime = GetJA2Clock();
			if( iCurrClickTime - iLastClickTime < 400 && x == iLastFileClicked )
			{ //Considered a double click, so activate load/save this filename.
				iFDlgState = iCurrentAction == ACTION_SAVE_MAP ? DIALOG_SAVE : DIALOG_LOAD;
			}
			iLastClickTime = iCurrClickTime;
			iLastFileClicked = x;
		}
		FListNode = FListNode->pNext;
	}
}


FDLG_LIST* AddToFDlgList(FDLG_LIST* const pList, const char* const filename)
{
	// Add to start of list
	if ( pList == NULL )
	{
		FDLG_LIST* const pNode = MALLOC(FDLG_LIST);
		strlcpy(pNode->filename, filename, lengthof(pNode->filename));
		pNode->pPrev = pNode->pNext = NULL;
		return(pNode);
	}

	// Add and sort alphabetically without regard to case -- function limited to 10 chars comparison
	if (strcasecmp(pList->filename, filename) > 0)
	{
		// filename is smaller than pList (i.e. Insert before)
		FDLG_LIST* const pNode = MALLOC(FDLG_LIST);
		strlcpy(pNode->filename, filename, lengthof(pNode->filename));
		pNode->pNext = pList;
		pNode->pPrev = pList->pPrev;
		pList->pPrev = pNode;
		return(pNode);
	}
	else
	{
		pList->pNext = AddToFDlgList(pList->pNext, filename);
		pList->pNext->pPrev = pList;
	}
	return(pList);
}


static BOOLEAN RemoveFromFDlgList(FDLG_LIST** head, FDLG_LIST* node)
{
	FDLG_LIST *curr;
	curr = *head;
	while( curr )
	{
		if( curr == node )
		{
			if( *head == node )
				*head = (*head)->pNext;
			if( curr->pPrev )
				curr->pPrev->pNext = curr->pNext;
			if( curr->pNext )
				curr->pNext->pPrev = curr->pPrev;
			MemFree( node );
			node = NULL;
			return TRUE;
		}
		curr = curr->pNext;
	}
	return FALSE; //wasn't deleted
}


static void TrashFDlgList(FDLG_LIST* pList)
{
	FDLG_LIST *pNode;

	while(pList != NULL)
	{
		pNode = pList;
		pList = pList->pNext;
		MemFree(pNode);
	}
}


static void SetTopFileToLetter(UINT16 usLetter)
{
	UINT32 x;
	FDLG_LIST *curr;
	FDLG_LIST *prev;
	UINT16 usNodeLetter;

	// Skip to first filename
	x = 0;
	curr = prev = FileList;
	while( curr )
	{
		usNodeLetter = curr->filename[0]; //first letter of filename.
		if( usNodeLetter < 'a' )
			usNodeLetter += 32; //convert uppercase to lower case A=65, a=97
		if( usLetter <= usNodeLetter )
			break;
		prev = curr;
		curr = curr->pNext;
		x++;
	}
	if( FileList )
	{
		iCurrFileShown = x;
		iTopFileShown = x;
		if( iTopFileShown > iTotalFiles - 7 )
			iTopFileShown = iTotalFiles - 7;
		SetInputFieldStringWith8BitString(0, prev->filename);
	}
}


static void HandleMainKeyEvents(InputAtom* pEvent)
{
	INT32 iPrevFileShown = iCurrFileShown;
	//Replace Alt-x press with ESC.
	if( pEvent->usKeyState & ALT_DOWN && pEvent->usParam == 'x' )
		pEvent->usParam = SDLK_ESCAPE;
	switch( pEvent->usParam )
	{
		case SDLK_RETURN:
			if( gfNoFiles && iCurrentAction == ACTION_LOAD_MAP )
				break;
			iFDlgState = iCurrentAction == ACTION_SAVE_MAP ? DIALOG_SAVE : DIALOG_LOAD;
			break;

		case SDLK_ESCAPE:
			iFDlgState = DIALOG_CANCEL;
			break;

		case SDLK_PAGEUP:
			if( iTopFileShown > 7 )
			{
				iTopFileShown -= 7;
				iCurrFileShown -= 7;
			}
			else
			{
				iTopFileShown = 0;
				iCurrFileShown = 0;
			}
			break;

		case SDLK_PAGEDOWN:
			iTopFileShown += 7;
			iCurrFileShown += 7;
			if( iTopFileShown > iTotalFiles-7 )
				iTopFileShown = iTotalFiles - 7;
			if( iCurrFileShown >= iTotalFiles )
				iCurrFileShown = iTotalFiles - 1;
			break;

		case SDLK_UP:
			if( iCurrFileShown > 0 )
				iCurrFileShown--;
			if( iTopFileShown > iCurrFileShown )
				iTopFileShown = iCurrFileShown;
			break;

		case SDLK_DOWN:
			iCurrFileShown++;
			if( iCurrFileShown >= iTotalFiles )
				iCurrFileShown = iTotalFiles - 1;
			else if( iTopFileShown < iCurrFileShown-7 )
				iTopFileShown++;
			break;

		case SDLK_HOME:
			iTopFileShown = 0;
			iCurrFileShown = 0;
			break;

		case SDLK_END:
			iTopFileShown = iTotalFiles-7;
			iCurrFileShown = iTotalFiles-1;
			break;

		case SDLK_DELETE: iFDlgState = DIALOG_DELETE; break;

		default:
			//This case handles jumping the file list to display the file with the letter pressed.
			if (pEvent->usParam >= SDLK_a && pEvent->usParam <= SDLK_z)
			{
				SetTopFileToLetter( (UINT16)pEvent->usParam );
			}
			break;
	}
	//Update the text field if the file value has changed.
	if( iCurrFileShown != iPrevFileShown )
	{
		INT32 x;
		FDLG_LIST *curr;
		x = 0;
		curr = FileList;
		while( curr && x != iCurrFileShown )
		{
			curr = curr->pNext;
			x++;
		}
		if( curr )
		{
			SetInputFieldStringWith8BitString(0, curr->filename);
			swprintf(gzFilename, lengthof(gzFilename), L"%hs", curr->filename);
		}
	}
}


static BOOLEAN ValidCoordinate(void);


//editor doesn't care about the z value.  It uses it's own methods.
static void SetGlobalSectorValues(const wchar_t* szFilename)
{
	const wchar_t* pStr;
	if( ValidCoordinate() )
	{
		//convert the coordinate string into into the actual global sector coordinates.
		if( gzFilename[0] >= 'A' && gzFilename[0] <= 'P' )
			gWorldSectorY = gzFilename[0] - 'A' + 1;
		else
			gWorldSectorY = gzFilename[0] - 'a' + 1;
		if( gzFilename[1] == '1' && gzFilename[2] >= '0' && gzFilename[2] <= '6' )
			gWorldSectorX = ( gzFilename[1] - 0x30 ) * 10 + ( gzFilename[2] - 0x30 );
		else
			gWorldSectorX = ( gzFilename[1] - 0x30 );
		pStr = wcsstr( gzFilename, L"_b" );
		if( pStr )
		{
			if( pStr[ 2 ] >= '1' && pStr[ 2 ] <= '3' )
			{
				gbWorldSectorZ = (INT8)(pStr[ 2 ] - 0x30);
			}
		}
	}
	else
	{
		gWorldSectorX = -1;
		gWorldSectorY = -1;
		gbWorldSectorZ = 0;
	}
}


static void InitErrorCatchDialog(void)
{
	DoMessageBox(MSG_BOX_BASIC_STYLE, gzErrorCatchString, EDIT_SCREEN, MSG_BOX_FLAG_OK, NULL, NULL);
	gfErrorCatch = FALSE;
}


//Because loading and saving the map takes a few seconds, we want to post a message
//on the screen and then update it which requires passing the screen back to the main loop.
//When we come back for the next frame, we then actually save or load the map.  So this
//process takes two full screen cycles.
static UINT32 ProcessFileIO(void)
{
	INT16 usStartX, usStartY;
	char ubNewFilename[50];
	switch( gbCurrentFileIOStatus )
	{
		case INITIATE_MAP_SAVE:	//draw save message
			SaveFontSettings();
			SetFont( HUGEFONT );
			SetFontForeground( FONT_LTKHAKI );
			SetFontShadow( FONT_DKKHAKI );
			SetFontBackground( 0 );
			wchar_t zOrigName[60];
			swprintf(zOrigName, lengthof(zOrigName), L"Saving map:  %ls", gzFilename);
			usStartX = (SCREEN_WIDTH - StringPixLength(zOrigName, HUGEFONT)) / 2;
			usStartY = 180 - GetFontHeight(HUGEFONT) / 2;
			MPrint(usStartX, usStartY, zOrigName);

			InvalidateScreen( );
			EndFrameBufferRender( );
			gbCurrentFileIOStatus = SAVING_MAP;
			return LOADSAVE_SCREEN;
		case SAVING_MAP: //save map
			sprintf( ubNewFilename, "%ls", gzFilename );
			RaiseWorldLand();
			if( gfShowPits )
				RemoveAllPits();
			OptimizeSchedules();
			if ( !SaveWorld( ubNewFilename ) )
			{
				if( gfErrorCatch )
				{
					InitErrorCatchDialog();
					return EDIT_SCREEN;
				}
				return ERROR_SCREEN;
			}
			if( gfShowPits )
				AddAllPits();

			SetGlobalSectorValues( gzFilename );

			if( gfGlobalSummaryExists )
				UpdateSectorSummary( gzFilename, gfUpdateSummaryInfo );

			iCurrentAction = ACTION_NULL;
			gbCurrentFileIOStatus = IOSTATUS_NONE;
			gfRenderWorld = TRUE;
			gfRenderTaskbar = TRUE;
			fEnteringLoadSaveScreen = TRUE;
			RestoreFontSettings();
			if( gfErrorCatch )
			{
				InitErrorCatchDialog();
				return EDIT_SCREEN;
			}
			if( gMapInformation.ubMapVersion != gubMinorMapVersion )
				ScreenMsg( FONT_MCOLOR_RED, MSG_ERROR, L"Map data has just been corrupted!!!  What did you just do?  KM : 0" );
			return EDIT_SCREEN;
		case INITIATE_MAP_LOAD: //draw load message
			SaveFontSettings();
			gbCurrentFileIOStatus = LOADING_MAP;
			if( gfEditMode && iCurrentTaskbar == TASK_MERCS )
				IndicateSelectedMerc( SELECT_NO_MERC );
			SpecifyItemToEdit( NULL, -1 );
			return LOADSAVE_SCREEN;
		case LOADING_MAP: //load map
			DisableUndo();
			sprintf( ubNewFilename, "%ls", gzFilename );

			RemoveMercsInSector( );

			if( !LoadWorld( ubNewFilename ) )
			{ //Want to override crash, so user can do something else.
				EnableUndo();
				SetPendingNewScreen( LOADSAVE_SCREEN );
				gbCurrentFileIOStatus = IOSTATUS_NONE;
				gfGlobalError = FALSE;
				gfLoadError = TRUE;
				//RemoveButton( iTempButton );
				CreateMessageBox( L" Error loading file.  Try another filename?" );
				return LOADSAVE_SCREEN;
			}
			SetGlobalSectorValues( gzFilename );

			RestoreFontSettings();

			//Load successful, update necessary information.

			//ATE: Any current mercs are transfered here...
			//UpdateMercsInSector( gWorldSectorX, gWorldSectorY, gbWorldSectorZ );

			AddSoldierInitListTeamToWorld( ENEMY_TEAM,		255 );
			AddSoldierInitListTeamToWorld( CREATURE_TEAM, 255 );
			AddSoldierInitListTeamToWorld( MILITIA_TEAM,	255 );
			AddSoldierInitListTeamToWorld( CIV_TEAM,			255 );
			iCurrentAction = ACTION_NULL;
			gbCurrentFileIOStatus = IOSTATUS_NONE;
			if( !gfCaves && !gfBasement )
			{
				gusLightLevel = 12;
				if( ubAmbientLightLevel != 4 )
				{
					ubAmbientLightLevel = 4;
					LightSetBaseLevel( ubAmbientLightLevel );
				}
			}
			else
				gusLightLevel = (UINT16)(EDITOR_LIGHT_MAX - ubAmbientLightLevel );
			gEditorLightColor = g_light_color;
			gfRenderWorld = TRUE;
			gfRenderTaskbar = TRUE;
			fEnteringLoadSaveScreen = TRUE;
			InitJA2SelectionWindow();
			ShowEntryPoints();
			EnableUndo();
			RemoveAllFromUndoList();
			SetEditorSmoothingMode( gMapInformation.ubEditorSmoothingType );
			if( gMapInformation.ubEditorSmoothingType == SMOOTHING_CAVES )
				AnalyseCaveMapForStructureInfo();

			AddLockedDoorCursors();
			gubCurrRoomNumber = gubMaxRoomNumber;
			UpdateRoofsView();
			UpdateWallsView();
			ShowLightPositionHandles();
			SetMercTeamVisibility( ENEMY_TEAM, gfShowEnemies );
			SetMercTeamVisibility( CREATURE_TEAM, gfShowCreatures );
			SetMercTeamVisibility( MILITIA_TEAM, gfShowRebels );
			SetMercTeamVisibility( CIV_TEAM, gfShowCivilians );
			BuildItemPoolList();
			if( gfShowPits )
				AddAllPits();

			if( iCurrentTaskbar == TASK_MAPINFO )
			{ //We have to temporarily remove the current textinput mode,
				//update the disabled text field values, then restore the current
				//text input fields.
				SaveAndRemoveCurrentTextInputMode();
				UpdateMapInfoFields();
				RestoreSavedTextInputMode();
			}
			return EDIT_SCREEN;
	}
	gbCurrentFileIOStatus = IOSTATUS_NONE;
	return LOADSAVE_SCREEN;
}


//LOADSCREEN
static void FDlgNamesCallback(GUI_BUTTON* butn, INT32 reason)
{
	if( reason & (MSYS_CALLBACK_REASON_LBUTTON_UP) )
	{
		SelectFileDialogYPos( butn->Area.RelativeYPos );
	}
}


static void FDlgOkCallback(GUI_BUTTON* butn, INT32 reason)
{
	if( reason & (MSYS_CALLBACK_REASON_LBUTTON_UP) )
	{
		iFDlgState = iCurrentAction == ACTION_SAVE_MAP ? DIALOG_SAVE : DIALOG_LOAD;
	}
}


static void FDlgCancelCallback(GUI_BUTTON* butn, INT32 reason)
{
	if( reason & (MSYS_CALLBACK_REASON_LBUTTON_UP) )
	{
		iFDlgState = DIALOG_CANCEL;
	}
}


static void FDlgUpCallback(GUI_BUTTON* butn, INT32 reason)
{
	if( reason & (MSYS_CALLBACK_REASON_LBUTTON_UP) )
	{
		if(iTopFileShown > 0)
			iTopFileShown--;
	}
}


static void FDlgDwnCallback(GUI_BUTTON* butn, INT32 reason)
{
	if( reason & (MSYS_CALLBACK_REASON_LBUTTON_UP) )
	{
		if( (iTopFileShown+7) < iTotalFiles )
			iTopFileShown++;
	}
}


static BOOLEAN ExtractFilenameFromFields(void)
{
	Get16BitStringFromField(0, gzFilename, lengthof(gzFilename));
	return ValidFilename();
}


static BOOLEAN ValidCoordinate(void)
{
	if( gzFilename[0] >= 'A' && gzFilename[0] <= 'P' ||
		gzFilename[0] >= 'a' && gzFilename[0] <='p' )
	{
		UINT16 usTotal; // XXX HACK000E
		if( gzFilename[1] == '1' && gzFilename[2] >= '0' && gzFilename[2] <= '6' )
		{
			usTotal = ( gzFilename[1] - 0x30 ) * 10 + ( gzFilename[2] - 0x30 );
		}
		else if( gzFilename[1] >= '1' && gzFilename[1] <= '9')
		{
			if( gzFilename[2] < '0' || gzFilename[2] > '9' )
			{
				usTotal = ( gzFilename[1] - 0x30 );
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			abort(); // XXX HACK000E
		}
		if( usTotal >= 1 && usTotal <= 16 )
		{
			return TRUE;
		}
	}
	return FALSE;
}


static BOOLEAN ValidFilename(void)
{
	const wchar_t* pDest;
	if( gzFilename[0] != '\0' );
	{
		pDest = wcsstr( gzFilename, L".dat" );
		if( !pDest )
			pDest = wcsstr( gzFilename, L".DAT" );
		if( pDest && pDest != gzFilename && pDest[4] == '\0' )
			return TRUE;
	}
	return FALSE;
}

BOOLEAN ExternalLoadMap(const wchar_t* szFilename)
{
	Assert( szFilename );
	if( !wcslen( szFilename ) )
		return FALSE;
	wcscpy( gzFilename, szFilename );
	if( !ValidFilename() )
		return FALSE;
	gbCurrentFileIOStatus = INITIATE_MAP_LOAD;
	ProcessFileIO(); //always returns loadsave_screen and changes iostatus to loading_map.
	ExecuteBaseDirtyRectQueue();
	EndFrameBufferRender();
	RefreshScreen();
	if( ProcessFileIO() == EDIT_SCREEN )
		return TRUE;
	return FALSE;
}

BOOLEAN ExternalSaveMap(const wchar_t* szFilename)
{
	Assert( szFilename );
	if( !wcslen( szFilename ) )
		return FALSE;
	wcscpy( gzFilename, szFilename );
	if( !ValidFilename() )
		return FALSE;
	gbCurrentFileIOStatus = INITIATE_MAP_SAVE;
	if( ProcessFileIO() == ERROR_SCREEN )
		return FALSE;
	ExecuteBaseDirtyRectQueue();
	EndFrameBufferRender();
	RefreshScreen();
	if( ProcessFileIO() == EDIT_SCREEN )
		return TRUE;
	return FALSE;
}

#endif
