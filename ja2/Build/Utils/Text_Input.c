#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <wchar.h>
#include "Input.h"
#include "Font.h"
#include "English.h"
#include "VSurface.h"
#include "Video.h"
#include "Debug.h"
#include "Cursors.h"
#include "Text_Input.h"
#include "Timer_Control.h"
#include "VObject_Blitters.h"
#include "Font_Control.h"
#include "Sound_Control.h"
#include <string.h>
#include "MemMan.h"
#include "MouseSystem.h"


UINT16 *szClipboard;
BOOLEAN gfNoScroll = FALSE;

typedef struct TextInputColors
{
	//internal values that contain all of the colors for the text editing fields.
	UINT16 usFont;
	UINT16 usTextFieldColor;
	UINT8 ubForeColor, ubShadowColor;
	UINT8 ubHiForeColor, ubHiShadowColor, ubHiBackColor;
	//optional -- no bevelling by default
	BOOLEAN	fBevelling;
	UINT16 usBrighterColor, usDarkerColor;
	//optional -- cursor color defaults to black
	UINT16 usCursorColor;
	//optional colors for disabled fields (defaults to 25% darker shading)
	BOOLEAN fUseDisabledAutoShade;
	UINT8	ubDisabledForeColor;
	UINT8	ubDisabledShadowColor;
	UINT16 usDisabledTextFieldColor;
}TextInputColors;

TextInputColors *pColors = NULL;

//Internal nodes for keeping track of the text and user defined fields.
typedef struct TEXTINPUTNODE{
	UINT8 ubID;
	UINT16 usInputType;
	UINT8 ubMaxChars;
	wchar_t *szString;
	UINT8 ubStrLen;
	BOOLEAN fEnabled;
	BOOLEAN fUserField;
	MOUSE_REGION region;
	INPUT_CALLBACK InputCallback;
	struct TEXTINPUTNODE *next, *prev;
}TEXTINPUTNODE;

//Stack list containing the head nodes of each level.  Only the top level is the active level.
typedef struct STACKTEXTINPUTNODE
{
	TEXTINPUTNODE *head;
	TextInputColors *pColors;
	struct STACKTEXTINPUTNODE *next;
}STACKTEXTINPUTNODE;

STACKTEXTINPUTNODE *pInputStack = NULL;

//Internal list vars.  active always points to the currently edited field.
TEXTINPUTNODE *gpTextInputHead = NULL, *gpTextInputTail = NULL, *gpActive = NULL;

//Saving current mode
TEXTINPUTNODE *pSavedHead = NULL;
TextInputColors *pSavedColors = NULL;
UINT16 gusTextInputCursor = CURSOR_IBEAM;


//Saves the current text input mode by pushing it onto our stack, then starts a new
//one.
static void PushTextInputLevel(void)
{
	STACKTEXTINPUTNODE *pNewLevel;
	pNewLevel = (STACKTEXTINPUTNODE*)MemAlloc( sizeof( STACKTEXTINPUTNODE ) );
	Assert( pNewLevel );
	pNewLevel->head = gpTextInputHead;
	pNewLevel->pColors = pColors;
	pNewLevel->next = pInputStack;
	pInputStack = pNewLevel;
	DisableAllTextFields();
}


//After the currently text input mode is removed, we then restore the previous one
//automatically.  Assert failure in this function will expose cases where you are trigger
//happy with killing non-existant text input modes.
static void PopTextInputLevel(void)
{
	STACKTEXTINPUTNODE *pLevel;
	gpTextInputHead = pInputStack->head;
	pColors = pInputStack->pColors;
	pLevel = pInputStack;
	pInputStack = pInputStack->next;
	MemFree( pLevel );
	pLevel = NULL;
	EnableAllTextFields();
}


//flags for determining various editing modes.
BOOLEAN gfEditingText = FALSE;
BOOLEAN gfTextInputMode = FALSE;
BOOLEAN gfHiliteMode = FALSE;

//values that contain the hiliting positions and the cursor position.
UINT8 gubCursorPos = 0;
UINT8 gubStartHilite = 0;
UINT8 gubEndHilite = 0;

//allow the user to cut, copy, and paste just like windows.
UINT16 gszClipboardString[256];

//Simply initiates that you wish to begin inputting text.  This should only apply to screen
//initializations that contain fields that edit text.  It also verifies and clears any existing
//fields.  Your input loop must contain the function HandleTextInput and processed if the gfTextInputMode
//flag is set else process your regular input handler.  Note that this doesn't mean you are necessarily typing,
//just that there are text fields in your screen and may be inactive.  The TAB key cycles through your text fields,
//and special fields can be defined which will call a void functionName( UINT16 usFieldNum )
void InitTextInputMode()
{
	if( gpTextInputHead )
	{
		//Instead of killing all of the currently existing text input fields, they will now (Jan16 '97)
		//be pushed onto a stack, and preserved until we are finished with the new mode when they will
		//automatically be re-instated when the new text input mode is killed.
		PushTextInputLevel();
		//KillTextInputMode();
	}
	gpTextInputHead = NULL;
	pColors = (TextInputColors*)MemAlloc( sizeof( TextInputColors ) );
	Assert( pColors );
	gfTextInputMode = TRUE;
	gfEditingText = FALSE;
	pColors->fBevelling = FALSE;
	pColors->fUseDisabledAutoShade = TRUE;
	pColors->usCursorColor = 0;
}

//A hybrid version of InitTextInput() which uses a specific scheme.  JA2's editor uses scheme 1, so
//feel free to add new schemes.
void InitTextInputModeWithScheme( UINT8 ubSchemeID )
{
	InitTextInputMode();
	switch( ubSchemeID )
	{
		case DEFAULT_SCHEME:  //yellow boxes with black text, with bluish bevelling
			SetTextInputFont( (UINT16)FONT12POINT1 );
			Set16BPPTextFieldColor( Get16BPPColor(FROMRGB(250, 240, 188) ) );
			SetBevelColors( Get16BPPColor(FROMRGB(136, 138, 135)), Get16BPPColor(FROMRGB(24, 61, 81)) );
			SetTextInputRegularColors( FONT_BLACK, FONT_BLACK );
			SetTextInputHilitedColors( FONT_GRAY2, FONT_GRAY2, FONT_METALGRAY );
			break;
	}
}

//Clears any existing fields, and ends text input mode.
void KillTextInputMode()
{
	TEXTINPUTNODE *curr;
	if( !gpTextInputHead )
//		AssertMsg( 0, "Called KillTextInputMode() without any text input mode defined.");
		return;
	curr = gpTextInputHead;
	while( curr )
	{
		gpTextInputHead = gpTextInputHead->next;
		if( curr->szString )
		{
			MemFree( curr->szString );
			curr->szString = NULL;
			MSYS_RemoveRegion( &curr->region );
		}
		MemFree( curr );
		curr = gpTextInputHead;
	}
	MemFree( pColors );
	pColors = NULL;
	gpTextInputHead = NULL;
	if( pInputStack )
	{
		PopTextInputLevel();
		SetActiveField( 0 );
	}
	else
	{
		gfTextInputMode = FALSE;
		gfEditingText = FALSE;
	}

	if( !gpTextInputHead )
		gpActive = NULL;
}

//Kills all levels of text input modes.  When you init a second consecutive text input mode, without
//first removing them, the existing mode will be preserved.  This function removes all of them in one
//call, though doing so "may" reflect poor coding style, though I haven't thought about any really
//just uses for it :(
void KillAllTextInputModes()
{
	while( gpTextInputHead )
		KillTextInputMode();
}


static void MouseClickedInTextRegionCallback(MOUSE_REGION* reg, INT32 reason);
static void MouseMovedInTextRegionCallback(MOUSE_REGION* reg, INT32 reason);


//After calling InitTextInputMode, you want to define one or more text input fields.  The order
//of calls to this function dictate the TAB order from traversing from one field to the next.  This
//function adds mouse regions and processes them for you, as well as deleting them when you are done.
void AddTextInputField(INT16 sLeft, INT16 sTop, INT16 sWidth, INT16 sHeight, INT8 bPriority, const wchar_t* szInitText, UINT8 ubMaxChars, UINT16 usInputType)
{
	TEXTINPUTNODE *pNode;
	pNode = (TEXTINPUTNODE*)MemAlloc(sizeof(TEXTINPUTNODE));
	Assert(pNode);
	memset( pNode, 0, sizeof( TEXTINPUTNODE ) );
	pNode->next = NULL;
  if( !gpTextInputHead ) //first entry, so we start with text input.
	{
		gfEditingText = TRUE;
		gpTextInputHead = gpTextInputTail = pNode;
		pNode->prev = NULL;
		pNode->ubID = 0;
		gpActive = pNode;
	}
	else //add to the end of the list.
	{
		gpTextInputTail->next = pNode;
		pNode->prev = gpTextInputTail;
		pNode->ubID = (UINT8)(gpTextInputTail->ubID+1);
		gpTextInputTail = gpTextInputTail->next;
	}
	//Setup the information for the node
	pNode->usInputType = usInputType;	//setup the filter type
	//All 24hourclock inputtypes have 6 characters.  01:23 (null terminated)
	if( usInputType == INPUTTYPE_EXCLUSIVE_24HOURCLOCK )
		ubMaxChars = 6;
	//Allocate and copy the string.
	pNode->szString = (wchar_t*)MemAlloc( (ubMaxChars+1)*sizeof(wchar_t) );
	Assert( pNode->szString );
	if( szInitText )
	{
		pNode->ubStrLen = (UINT8)wcslen( szInitText );
		Assert( pNode->ubStrLen <= ubMaxChars );
		swprintf( pNode->szString, ubMaxChars + 1, szInitText );
	}
	else
	{
		pNode->ubStrLen = 0;
		swprintf( pNode->szString, ubMaxChars + 1, L"" );
	}
	pNode->ubMaxChars = ubMaxChars; //max string length

	//if this is the first field, then hilight it.
	if( gpTextInputHead == pNode )
	{
		gubStartHilite = 0;
		gubEndHilite = pNode->ubStrLen;
		gubCursorPos = pNode->ubStrLen;
		gfHiliteMode = TRUE;
	}
	pNode->fUserField = FALSE;
	pNode->fEnabled = TRUE;
	//Setup the region.
	MSYS_DefineRegion( &pNode->region, sLeft, sTop, (INT16)(sLeft+sWidth), (INT16)(sTop+sHeight), bPriority,
						 gusTextInputCursor, MouseMovedInTextRegionCallback, MouseClickedInTextRegionCallback );
	MSYS_SetRegionUserData( &pNode->region, 0, pNode->ubID );
}

//This allows you to insert special processing functions and modes that can't be determined here.  An example
//would be a file dialog where there would be a file list.  This file list would be accessed using the Win95
//convention by pressing TAB.  In there, your key presses would be handled differently and by adding a userinput
//field, you can make this hook into your function to accomplish this.  In a filedialog, alpha characters
//would be used to jump to the file starting with that letter, and setting the field in the text input
//field.  Pressing TAB again would place you back in the text input field.  All of that stuff would be handled
//externally, except for the TAB keys.
void AddUserInputField( INPUT_CALLBACK userFunction )
{
	TEXTINPUTNODE *pNode;
	pNode = (TEXTINPUTNODE*)MemAlloc(sizeof(TEXTINPUTNODE));
	Assert(pNode);
	pNode->next = NULL;
  if( !gpTextInputHead ) //first entry, so we don't start with text input.
	{
		gfEditingText = FALSE;
		gpTextInputHead = gpTextInputTail = pNode;
		pNode->prev = NULL;
		pNode->ubID = 0;
		gpActive = pNode;
	}
	else //add to the end of the list.
	{
		gpTextInputTail->next = pNode;
		pNode->prev = gpTextInputTail;
		pNode->ubID = (UINT8)(gpTextInputTail->ubID+1);
		gpTextInputTail = gpTextInputTail->next;
	}
	//Setup the information for the node
	pNode->fUserField = TRUE;
	pNode->szString = NULL;
	pNode->fEnabled = TRUE;
	//Setup the callback
	pNode->InputCallback = userFunction;
}


//Removes the specified field from the existing fields.  If it doesn't exist, then there will be an
//assertion failure.
static void RemoveTextInputField(UINT8 ubField)
{
	TEXTINPUTNODE *curr;
	curr = gpTextInputHead;
	while( curr )
	{
		if( curr->ubID == ubField )
		{
			if( curr == gpActive )
				SelectNextField();
			if( curr == gpTextInputHead )
				gpTextInputHead = gpTextInputHead->next;
			//Detach the node.
			if( curr->next )
				curr->next->prev = curr->prev;
			if( curr->prev )
				curr->prev->next = curr->next;
			if( curr->szString )
			{
				MemFree( curr->szString );
				curr->szString = NULL;
				MSYS_RemoveRegion( &curr->region );
			}
			MemFree( curr );
			curr = NULL;
			if( !gpTextInputHead )
			{
				gfTextInputMode = FALSE;
				gfEditingText = FALSE;
			}
			return;
		}
		curr = curr->next;
	}
	AssertMsg( 0, "Attempt to remove a text input field that doesn't exist.  Check your IDs." );
}

//Returns the gpActive field ID number.  It'll return -1 if no field is active.
INT16 GetActiveFieldID()
{
	if( gpActive )
		return gpActive->ubID;
	return -1;
}

//This is a useful call made from an external user input field.  Using the previous file dialog example, this
//call would be made when the user selected a different filename in the list via clicking or scrolling with
//the arrows, or even using alpha chars to jump to the appropriate filename.
void SetInputFieldStringWith16BitString( UINT8 ubField, const wchar_t *szNewText)
{
	TEXTINPUTNODE *curr;
  curr = gpTextInputHead;
	while( curr )
	{
		if( curr->ubID == ubField )
		{
			if( szNewText)
			{
				curr->ubStrLen = (UINT8)wcslen( szNewText );
				Assert( curr->ubStrLen <= curr->ubMaxChars );
				swprintf( curr->szString, curr->ubMaxChars + 1, szNewText );
			}
			else if( !curr->fUserField )
			{
				curr->ubStrLen = 0;
				swprintf( curr->szString, curr->ubMaxChars + 1, L"");
			}
			else
			{
				AssertMsg( 0, String( "Attempting to illegally set text into user field %d", curr->ubID ) );
			}
			return;
		}
		curr = curr->next;
	}
}

void SetInputFieldStringWith8BitString( UINT8 ubField, UINT8 *szNewText )
{
	TEXTINPUTNODE *curr;
  curr = gpTextInputHead;
	while( curr )
	{
		if( curr->ubID == ubField )
		{
			if( szNewText )
			{
				curr->ubStrLen = (UINT8)strlen( szNewText );
				Assert( curr->ubStrLen <= curr->ubMaxChars );
				swprintf(curr->szString, curr->ubMaxChars + 1, L"%s", szNewText);
			}
			else if( !curr->fUserField )
			{
				curr->ubStrLen = 0;
				swprintf( curr->szString, curr->ubMaxChars + 1, L"" );
			}
			else
			{
				AssertMsg( 0, String( "Attempting to illegally set text into user field %d", curr->ubID ) );
			}
			return;
		}
		curr = curr->next;
	}
}


//Allows external functions to access the strings within the fields at anytime.
static void Get8BitStringFromField(UINT8 ubField, UINT8* szString)
{
	TEXTINPUTNODE *curr;
  curr = gpTextInputHead;
	while( curr )
	{
		if( curr->ubID == ubField )
		{
			sprintf( szString, "%S", curr->szString );
			return;
		}
		curr = curr->next;
	}
	szString[0] = '\0';
}

void Get16BitStringFromField( UINT8 ubField, wchar_t *szString, size_t Length)
{
	TEXTINPUTNODE *curr;
  curr = gpTextInputHead;
	while( curr )
	{
		if( curr->ubID == ubField )
		{
			swprintf( szString, Length, curr->szString );
			return;
		}
		curr = curr->next;
	}
	szString[0] = '\0';
}

//Converts the field's string into a number, then returns that number
//returns -1 if blank or invalid.  Only works for positive numbers.
INT32 GetNumericStrictValueFromField( UINT8 ubField )
{
	wchar_t *ptr;
	wchar_t str[20];
	INT32 total;
	Get16BitStringFromField( ubField, str, lengthof(str));
	//Blank string, so return -1
	if( str[0] == '\0' )
		return -1;
	//Convert the string to a number.  Don't trust other functions.  This will
	//ensure that nonnumeric values automatically return -1.
	total = 0;
	ptr = str;
	while( *ptr != '\0' )									//if char is a valid char...
	{
		if( *ptr >= '0' && *ptr <= '9' )		//...make sure it is numeric...
		{	//Multiply prev total by 10 and add converted char digit value.
			total = total * 10 + (*ptr - '0');
		}
		else																//...else the string is invalid.
			return -1;
		ptr++;	//point to next char in string.
	}
	return total; //if we made it this far, then we have a valid number.
}

//Converts a number to a numeric strict value.  If the number is negative, the
//field will be blank.
void SetInputFieldStringWithNumericStrictValue( UINT8 ubField, INT32 iNumber )
{
	TEXTINPUTNODE *curr;
	curr = gpTextInputHead;
	while( curr )
	{
		if( curr->ubID == ubField )
		{
			AssertMsg(!curr->fUserField, String("Attempting to illegally set text into user field %d", curr->ubID));
			if( iNumber < 0 ) //negative number converts to blank string
				swprintf( curr->szString, curr->ubMaxChars + 1, L"" );
			else
			{
				INT32 iMax = (INT32)pow( 10.0, curr->ubMaxChars );
				if( iNumber > iMax ) //set string to max value based on number of chars.
					swprintf( curr->szString, curr->ubMaxChars + 1, L"%d", iMax - 1 );
				else	//set string to the number given
					swprintf( curr->szString, curr->ubMaxChars + 1, L"%d", iNumber );
			}
			curr->ubStrLen = (UINT8)wcslen( curr->szString );
			return;
		}
		curr = curr->next;
	}
}

//Sets the active field to the specified ID passed.
void SetActiveField( UINT8 ubField )
{
	TEXTINPUTNODE *curr;
	curr = gpTextInputHead;
	while( curr )
	{
		if( curr != gpActive && curr->ubID == ubField && curr->fEnabled )
		{
			gpActive = curr;
			if( gpActive->szString )
			{
				gubStartHilite = 0;
				gubEndHilite = gpActive->ubStrLen;
				gubCursorPos = gpActive->ubStrLen;
				gfHiliteMode = TRUE;
				gfEditingText = TRUE;
			}
			else
			{
				gfHiliteMode = FALSE;
				gfEditingText = FALSE;
				if( gpActive->InputCallback )
					(gpActive->InputCallback)(gpActive->ubID, TRUE);
			}
			return;
		}
		curr = curr->next;
	}
}


static void RenderInactiveTextFieldNode(TEXTINPUTNODE* pNode);


void SelectNextField()
{
	BOOLEAN fDone = FALSE;
	TEXTINPUTNODE *pStart;

	if( !gpActive )
		return;
	if( gpActive->szString )
		RenderInactiveTextFieldNode( gpActive );
	else if( gpActive->InputCallback )
		(gpActive->InputCallback)(gpActive->ubID, FALSE );
	pStart = gpActive;
	while( !fDone )
	{
		gpActive = gpActive->next;
		if( !gpActive )
			gpActive = gpTextInputHead;
		if( gpActive->fEnabled )
		{
			fDone = TRUE;
			if( gpActive->szString )
			{
				gubStartHilite = 0;
				gubEndHilite = gpActive->ubStrLen;
				gubCursorPos = gpActive->ubStrLen;
				gfHiliteMode = TRUE;
				gfEditingText = TRUE;
			}
			else
			{
				gfHiliteMode = FALSE;
				gfEditingText = FALSE;
				if( gpActive->InputCallback )
					(gpActive->InputCallback)(gpActive->ubID, TRUE);
			}
		}
		if( gpActive == pStart )
		{
			gfEditingText = FALSE;
			return;
		}
	}
}


static void SelectPrevField(void)
{
	BOOLEAN fDone = FALSE;
	TEXTINPUTNODE *pStart;

	if( !gpActive )
		return;
	if( gpActive->szString )
		RenderInactiveTextFieldNode( gpActive );
	else if( gpActive->InputCallback )
		(gpActive->InputCallback)(gpActive->ubID, FALSE );
	pStart = gpActive;
	while( !fDone )
	{
		gpActive = gpActive->prev;
		if( !gpActive )
			gpActive = gpTextInputTail;
		if( gpActive->fEnabled )
		{
			fDone = TRUE;
			if( gpActive->szString )
			{
				gubStartHilite = 0;
				gubEndHilite = gpActive->ubStrLen;
				gubCursorPos = gpActive->ubStrLen;
				gfHiliteMode = TRUE;
				gfEditingText = TRUE;
			}
			else
			{
				gfHiliteMode = FALSE;
				gfEditingText = FALSE;
				if( gpActive->InputCallback )
					(gpActive->InputCallback)(gpActive->ubID, TRUE);
			}
		}
		if( gpActive == pStart )
		{
			gfEditingText = FALSE;
			return;
		}
	}
}

//These allow you to customize the general color scheme of your text input boxes.  I am assuming that
//under no circumstances would a user want a different color for each field.  It follows the Win95 convention
//that all text input boxes are exactly the same color scheme.  However, these colors can be set at anytime,
//but will effect all of the colors.
void SetTextInputFont( UINT16 usFont )
{
	pColors->usFont = usFont;
}

void Set16BPPTextFieldColor( UINT16 usTextFieldColor )
{
	pColors->usTextFieldColor = usTextFieldColor;
}

void SetTextInputRegularColors( UINT8 ubForeColor, UINT8 ubShadowColor )
{
	pColors->ubForeColor = ubForeColor;
	pColors->ubShadowColor = ubShadowColor;
}

void SetTextInputHilitedColors( UINT8 ubForeColor, UINT8 ubShadowColor, UINT8 ubBackColor )
{
	pColors->ubHiForeColor = ubForeColor;
	pColors->ubHiShadowColor = ubShadowColor;
	pColors->ubHiBackColor = ubBackColor;
}


// optional color setups
static void SetDisabledTextFieldColors(UINT8 ubForeColor, UINT8 ubShadowColor, UINT16 usTextFieldColor)
{
	pColors->fUseDisabledAutoShade = FALSE;
	pColors->ubDisabledForeColor = ubForeColor;
	pColors->ubDisabledShadowColor = ubShadowColor;
	pColors->usDisabledTextFieldColor = usTextFieldColor;
}

void SetBevelColors( UINT16 usBrighterColor, UINT16 usDarkerColor )
{
	pColors->fBevelling = TRUE;
	pColors->usBrighterColor = usBrighterColor;
	pColors->usDarkerColor = usDarkerColor;
}

void SetCursorColor( UINT16 usCursorColor )
{
	pColors->usCursorColor = usCursorColor;
}


static void AddChar(UINT32 uiKey);
static void DeleteHilitedText(void);
static void HandleExclusiveInput(UINT32 uiKey);
static void RemoveChar(UINT8 ubArrayIndex);


//All CTRL and ALT keys combinations, F1-F12 keys, ENTER and ESC are ignored allowing
//processing to be done with your own input handler.  Otherwise, the keyboard event
//is absorbed by this input handler, if used in the appropriate manner.
//This call must be added at the beginning of your input handler in this format:
//while( DequeueEvent(&Event) )
//{
//	if(	!HandleTextInput( &Event ) && (your conditions...ex:  Event.usEvent == KEY_DOWN ) )
//  {
//		switch( Event.usParam )
//		{
//			//Normal key cases here.
//		}
//	}
//}
//It is only necessary for event loops that contain text input fields.
BOOLEAN HandleTextInput( InputAtom *Event )
{
	//Check the multitude of terminating conditions...

	//not in text input mode
	gfNoScroll = FALSE;
	if( !gfTextInputMode )
		return FALSE;
	//currently in a user field, so return unless TAB or SHIFT_TAB are pressed.
	if( !gfEditingText && Event->usParam != TAB && Event->usParam != SHIFT_TAB )
		return FALSE;
	//unless we are psycho typers, we only want to process these key events.
	if( Event->usEvent != KEY_DOWN && Event->usEvent != KEY_REPEAT )
		return FALSE;
	//ESC and ENTER must be handled externally, due to the infinite uses for it.
	//When editing text, ESC is equivalent to cancel, and ENTER is to confirm.
	if( Event->usParam == ESC )
		return FALSE;
	if( Event->usParam == ENTER )
	{
		PlayJA2Sample(REMOVING_TEXT, BTNVOLUME, 1, MIDDLEPAN);
		return FALSE;
	}
	//For any number of reasons, these ALT and CTRL combination key presses
	//will be processed externally
#if 0
	if( Event->usKeyState & CTRL_DOWN  )
	{
		if( Event->usParam == 'c' || Event->usParam == 'C' )
		{
			ExecuteCopyCommand();
			return TRUE;
		}
		else if( Event->usParam == 'x' || Event->usParam == 'X' )
		{
			ExecuteCutCommand();
			return TRUE;
		}
		else if( Event->usParam == 'v' || Event->usParam == 'V' )
		{
			ExecutePasteCommand();
			return TRUE;
		}
	}
#endif
	if( Event->usKeyState & ALT_DOWN || Event->usKeyState & CTRL_DOWN && Event->usParam != DEL )
		return FALSE;
	//F1-F12 regardless of state are processed externally as well.
	if (Event->usParam >= F1 && Event->usParam <= F12)
	{
		return FALSE;
	}
	//If we have met all of the conditions, we then have a valid key press
	//which will be handled universally for all text input fields
	switch( Event->usParam )
	{
		case TAB:
			//Always selects the next field, even when a user defined field is currently selected.
			//The order in which you add your text and user fields dictates the cycling order when
			//TAB is pressed.
			SelectNextField();
			break;
		case SHIFT_TAB:
			//Same as TAB, but selects fields in reverse order.
			SelectPrevField();
			break;
		case LEFTARROW:
			//Move the cursor to the left one position.  If there is selected text,
			//the cursor moves to the left of the block, and clears the block.
			gfNoScroll = TRUE;
			if( gfHiliteMode )
			{
				gfHiliteMode = FALSE;
				gubCursorPos = gubStartHilite;
				break;
			}
			if( gubCursorPos )
				gubCursorPos--;
			break;
		case RIGHTARROW:
			//Move the cursor to the right one position.  If there is selected text,
			//the block is cleared.
			gfNoScroll = TRUE;
			if( gfHiliteMode )
			{
				gfHiliteMode = FALSE;
				gubCursorPos = gubEndHilite;
				break;
			}
			if( gubCursorPos < gpActive->ubStrLen )
				gubCursorPos++;
			break;
		case END:
			//Any hilighting is cleared and the cursor moves to the end of the text.
			gfHiliteMode = FALSE;
			gubCursorPos = gpActive->ubStrLen;
			break;
		case HOME:
			//Any hilighting is cleared and the cursor moves to the beginning of the line.
			gfHiliteMode = FALSE;
			gubCursorPos = 0;
			break;
		case SHIFT_LEFTARROW:
			//Initiates or continues hilighting to the left one position.  If the cursor
			//is at the left end of the block, then the block decreases one position.
			gfNoScroll = TRUE;
			if( !gfHiliteMode )
			{
				gfHiliteMode = TRUE;
				gubStartHilite = gubCursorPos;
			}
			if( gubCursorPos )
				gubCursorPos--;
			gubEndHilite = gubCursorPos;
			break;
		case SHIFT_RIGHTARROW:
			//Initiates or continues hilighting to the right one position.  If the cursor
			//is at the right end of the block, then the block decreases one position.
			gfNoScroll = TRUE;
			if( !gfHiliteMode )
			{
				gfHiliteMode = TRUE;
				gubStartHilite = gubCursorPos;
			}
			if( gubCursorPos < gpActive->ubStrLen )
				gubCursorPos++;
			gubEndHilite = gubCursorPos;
			break;
		case SHIFT_END:
			//From the location of the anchored cursor for hilighting, the cursor goes to
			//the end of the text, selecting all text from the anchor to the end of the text.
			if( !gfHiliteMode )
			{
				gfHiliteMode = TRUE;
				gubStartHilite = gubCursorPos;
			}
			gubCursorPos = gpActive->ubStrLen;
			gubEndHilite = gubCursorPos;
			break;
		case SHIFT_HOME:
			//From the location of the anchored cursor for hilighting, the cursor goes to
			//the beginning of the text, selecting all text from the anchor to the beginning
			//of the text.
			if( !gfHiliteMode )
			{
				gfHiliteMode = TRUE;
				gubStartHilite = gubCursorPos;
			}
			gubCursorPos = 0;
			gubEndHilite = gubCursorPos;
			break;
		case DEL:
			//CTRL+DEL will delete the entire text field, regardless of hilighting.
			//DEL will either delete the selected text, or the character to the right
			//of the cursor if applicable.
			PlayJA2Sample(ENTERING_TEXT, BTNVOLUME, 1, MIDDLEPAN);
			if( Event->usKeyState & CTRL_DOWN )
			{
				gubStartHilite = 0;
				gubEndHilite = gpActive->ubStrLen;
				gfHiliteMode = TRUE;
				DeleteHilitedText();
			}
			else if( gfHiliteMode )
				PlayJA2Sample(ENTERING_TEXT, BTNVOLUME, 1, MIDDLEPAN);
			else
				RemoveChar( gubCursorPos );
			break;
		case BACKSPACE:
			//Will delete the selected text, or the character to the left of the cursor if applicable.
			if( gfHiliteMode)
			{
				PlayJA2Sample(ENTERING_TEXT, BTNVOLUME, 1, MIDDLEPAN);
				DeleteHilitedText();
			}
			else if( gubCursorPos > 0 )
			{
				PlayJA2Sample(ENTERING_TEXT, BTNVOLUME, 1, MIDDLEPAN);
				RemoveChar( --gubCursorPos );
			}
			break;
		default:  //check for typing keys
			if( gfHiliteMode )
				DeleteHilitedText();
			if( gpActive->usInputType >= INPUTTYPE_EXCLUSIVE_BASEVALUE )
				HandleExclusiveInput( Event->usParam );
			else
			{
				//Use abbreviations
				UINT32 key = Event->Char;
				UINT16 type = gpActive->usInputType;
				//Handle space key
				if( key == SPACE && type & INPUTTYPE_SPACES )
				{
					AddChar( key );
					return TRUE;
				}
				//Handle allowing minus key only at the beginning of a field.
				if( key == '-' && type & INPUTTYPE_FIRSTPOSMINUS && !gubCursorPos )
				{
					AddChar( key );
					return TRUE;
				}
				//Handle numerics
				if( key >= '0' && key <= '9' && type & INPUTTYPE_NUMERICSTRICT )
				{
					AddChar( key );
					return TRUE;
				}
				//Handle alphas
				if( type & INPUTTYPE_ALPHA )
				{
					if( key >= 'A' && key <= 'Z' )
					{
						if( type & INPUTTYPE_LOWERCASE )
							key -= 32;
						AddChar( key );
						return TRUE;
					}
					if( key >= 'a' && key <= 'z' )
					{
						if( type & INPUTTYPE_UPPERCASE )
							key += 32;
						AddChar( key );
						return TRUE;
					}
				}
				//Handle special characters
				if( type & INPUTTYPE_SPECIAL )
				{
					//More can be added, but not all of the fonts support these
					if( key >= 0x21 && key <= 0x2f || // ! " # $ % & ' ( ) * + , - . /
						  key >= 0x3a && key <= 0x40 || // : ; < = > ? @
							key >= 0x5b && key <= 0x5f || // [ \ ] ^ _
							key >= 0x7b && key <= 0x7d  ) // { | }
					{
						AddChar( key );
						return TRUE;
					}
				}
			}
			return TRUE;
	}
	return TRUE;
}


// All exclusive input types are handled in this function.
static void HandleExclusiveInput(UINT32 uiKey)
{
	switch( gpActive->usInputType )
	{
		case INPUTTYPE_EXCLUSIVE_DOSFILENAME: //dos file names
			if( uiKey >= 'A' && uiKey <= 'Z' ||
					uiKey >= 'a' && uiKey <= 'z' ||
					uiKey >= '0' && uiKey <= '9' ||
					uiKey == '_' || uiKey == '.' )
			{
				if( !gubCursorPos && uiKey >= '0' && uiKey <= '9' )
				{	//can't begin a new filename with a number
					return;
				}
				AddChar( uiKey );
			}
			break;
		case INPUTTYPE_EXCLUSIVE_COORDINATE:  //coordinates such as a9, z78, etc.
			if( !gubCursorPos ) //first char is an lower case alpha
			{
				if( uiKey >= 'a' && uiKey <= 'z' )
					AddChar( uiKey );
				else if( uiKey >= 'A' && uiKey <= 'Z' )
					AddChar( uiKey + 32 ); //convert to lowercase
			}
			else //subsequent chars are numeric
			{
				if( uiKey >= '0' && uiKey <= '9' )
					AddChar( uiKey );
			}
			break;
		case INPUTTYPE_EXCLUSIVE_24HOURCLOCK:
			if( !gubCursorPos )
			{
				if( uiKey >= '0' && uiKey <= '2' )
					AddChar( uiKey );
			}
			else if( gubCursorPos == 1 )
			{
				if( uiKey >= '0' && uiKey <= '9' )
				{
					if( gpActive->szString[ 0 ] == '2' && uiKey > '3' )
						break;
					AddChar( uiKey );
				}
				if( !gpActive->szString[ 2 ] )
					AddChar( ':' );
				else
					gubCursorPos++;
			}
			else if( gubCursorPos == 2 )
			{
				if( uiKey == ':' )
					AddChar( uiKey );
				else if( uiKey >= '0' && uiKey <= '9' )
				{
					AddChar( ':' );
					AddChar( uiKey );
				}
			}
			else if( gubCursorPos == 3 )
			{
				if( uiKey >= '0' && uiKey <= '5' )
					AddChar( uiKey );
			}
			else if( gubCursorPos == 4 )
			{
				if( uiKey >= '0' && uiKey <= '9' )
					AddChar( uiKey );
			}
			break;
	}
}


static void AddChar(UINT32 uiKey)
{
	PlayJA2Sample(ENTERING_TEXT, BTNVOLUME, 1, MIDDLEPAN);
	if( gpActive->ubStrLen >= gpActive->ubMaxChars )
	{	//max length reached.  Just replace the last character with new one.
		gpActive->ubStrLen = gpActive->ubMaxChars;
		gpActive->szString[ gpActive->ubStrLen-1 ] = (UINT16)uiKey;
		gpActive->szString[ gpActive->ubStrLen ] = '\0';
		return;
	}
	else if( gubCursorPos == gpActive->ubStrLen )
	{ //add character to end
		gpActive->szString[ gpActive->ubStrLen ] = (UINT16)uiKey;
		gpActive->szString[ gpActive->ubStrLen + 1 ] = '\0';
		gpActive->ubStrLen++;
		gubCursorPos = gpActive->ubStrLen;
	}
	else
	{ //insert character after cursor
		INT16 sChar;
		sChar = (INT16)(gpActive->ubStrLen + 1);
		while( sChar >= gubCursorPos )
		{
			gpActive->szString[ sChar + 1 ] = gpActive->szString[ sChar ];
			sChar--;
		}
		gpActive->szString[ gubCursorPos ] = (UINT16)uiKey;
		gpActive->ubStrLen++;
		gubCursorPos++;
	}
}


static void DeleteHilitedText(void)
{
	UINT8 ubCount;
	UINT8 ubStart, ubEnd;
	gfHiliteMode = FALSE;
	if( gubStartHilite != gubEndHilite )
	{
		if( gubStartHilite < gubEndHilite )
		{
			ubStart = gubStartHilite;
			ubEnd = gubEndHilite;
		}
		else
		{
			ubStart = gubEndHilite;
			ubEnd = gubStartHilite;
		}
		ubCount = (UINT8)(ubEnd - ubStart);
		while( ubCount--)
		{
			RemoveChar( ubStart );
		}
		gubStartHilite = gubEndHilite = 0;
		gubCursorPos = ubStart;
		gfHiliteMode = FALSE;
	}
}


static void RemoveChar(UINT8 ubArrayIndex)
{
	BOOLEAN fDeleting = FALSE;
	while( ubArrayIndex < gpActive->ubStrLen )
	{
		gpActive->szString[ ubArrayIndex ] = gpActive->szString[ ubArrayIndex + 1 ];
		ubArrayIndex++;
		fDeleting = TRUE;
	}
	//if we deleted a char, then decrement the strlen.
	if( fDeleting )
		gpActive->ubStrLen--;
}


//Internally used to continue highlighting text
static void MouseMovedInTextRegionCallback(MOUSE_REGION* reg, INT32 reason)
{
	TEXTINPUTNODE *curr;
	if( gfLeftButtonState )
	{
		if( reason & MSYS_CALLBACK_REASON_MOVE ||
				reason & MSYS_CALLBACK_REASON_LOST_MOUSE ||
				reason & MSYS_CALLBACK_REASON_GAIN_MOUSE )
		{
			INT32 iClickX, iCurrCharPos, iNextCharPos;
			UINT8 ubNewID;
			ubNewID = (UINT8)MSYS_GetRegionUserData( reg, 0 );
			if( ubNewID != gpActive->ubID )
			{ //deselect the current text edit region if applicable, then find the new one.
				RenderInactiveTextFieldNode( gpActive );
				curr = gpTextInputHead;
				while( curr )
				{
					if( curr->ubID == ubNewID )
					{
						gpActive = curr;
						break;
					}
					curr = curr->next;
				}
			}
			if( reason & MSYS_CALLBACK_REASON_LOST_MOUSE )
			{
				if( gusMouseYPos < reg->RegionTopLeftY )
				{
					gubEndHilite = 0;
					gfHiliteMode = TRUE;
					return;
				}
				else if( gusMouseYPos > reg->RegionBottomRightY )
				{
					gubEndHilite = gpActive->ubStrLen;
					gfHiliteMode = TRUE;
					return;
				}
			}

			//Calculate the cursor position.
			iClickX = gusMouseXPos - reg->RegionTopLeftX;
			iCurrCharPos = 0;
			gubCursorPos = 0;
			iNextCharPos = StringPixLengthArg(pColors->usFont, 1, L"%S", gpActive->szString) / 2;
			while( iCurrCharPos + (iNextCharPos-iCurrCharPos)/2 < iClickX && gubCursorPos < gpActive->ubStrLen )
			{
				gubCursorPos++;
				iCurrCharPos = iNextCharPos;
				iNextCharPos = StringPixLengthArg(pColors->usFont, gubCursorPos + 1, L"%S", gpActive->szString);
			}
			gubEndHilite = gubCursorPos;
			if( gubEndHilite != gubStartHilite )
				gfHiliteMode = TRUE;
		}
	}
}


//Internally used to calculate where to place the cursor.
static void MouseClickedInTextRegionCallback(MOUSE_REGION* reg, INT32 reason)
{
	TEXTINPUTNODE *curr;
	if( reason & MSYS_CALLBACK_REASON_LBUTTON_DWN )
	{
		INT32 iClickX, iCurrCharPos, iNextCharPos;
		UINT8 ubNewID;
		ubNewID = (UINT8)MSYS_GetRegionUserData( reg, 0 );
		if( ubNewID != gpActive->ubID )
		{ //deselect the current text edit region if applicable, then find the new one.
			RenderInactiveTextFieldNode( gpActive );
			curr = gpTextInputHead;
			while( curr )
			{
				if( curr->ubID == ubNewID )
				{
					gpActive = curr;
					break;
				}
				curr = curr->next;
			}
		}
		//Signifies that we are typing text now.
		gfEditingText = TRUE;
		//Calculate the cursor position.
		iClickX = gusMouseXPos - reg->RegionTopLeftX;
		iCurrCharPos = 0;
		gubCursorPos = 0;
		iNextCharPos = StringPixLengthArg(pColors->usFont, 1, L"%S", gpActive->szString) / 2;
		while( iCurrCharPos + (iNextCharPos-iCurrCharPos)/2 < iClickX && gubCursorPos < gpActive->ubStrLen )
		{
			gubCursorPos++;
			iCurrCharPos = iNextCharPos;
			iNextCharPos = StringPixLengthArg(pColors->usFont, gubCursorPos + 1, L"%S", gpActive->szString);
		}
		gubStartHilite = gubCursorPos;  //This value is the anchor
		gubEndHilite = gubCursorPos;		//The end will move with the cursor as long as it's down.
		gfHiliteMode = FALSE;

	}
}


static void RenderBackgroundField(TEXTINPUTNODE* pNode)
{
	UINT16 usColor;
	if( pColors->fBevelling )
	{
		ColorFillVideoSurfaceArea(FRAME_BUFFER,	pNode->region.RegionTopLeftX, pNode->region.RegionTopLeftY,
											pNode->region.RegionBottomRightX, pNode->region.RegionBottomRightY, pColors->usDarkerColor );
		ColorFillVideoSurfaceArea(FRAME_BUFFER,	pNode->region.RegionTopLeftX+1, pNode->region.RegionTopLeftY+1,
											pNode->region.RegionBottomRightX, pNode->region.RegionBottomRightY, pColors->usBrighterColor );
	}
	if( !pNode->fEnabled && !pColors->fUseDisabledAutoShade )
		usColor = pColors->usDisabledTextFieldColor;
	else
		usColor = pColors->usTextFieldColor;

	ColorFillVideoSurfaceArea(FRAME_BUFFER,	pNode->region.RegionTopLeftX+1, pNode->region.RegionTopLeftY+1,
										pNode->region.RegionBottomRightX-1, pNode->region.RegionBottomRightY-1, usColor );

	InvalidateRegion( pNode->region.RegionTopLeftX, pNode->region.RegionTopLeftY,
										pNode->region.RegionBottomRightX, pNode->region.RegionBottomRightY );

}


/* Required in your screen loop to update the values, as well as blinking the
 * cursor. */
static void RenderActiveTextField(void)
{
	UINT32 uiCursorXPos;
	UINT16 usOffset;
	if( !gpActive || !gpActive->szString )
		return;

	SaveFontSettings();
	SetFont( pColors->usFont );
	usOffset = (UINT16)(( gpActive->region.RegionBottomRightY - gpActive->region.RegionTopLeftY - GetFontHeight( pColors->usFont ) ) / 2);
	RenderBackgroundField( gpActive );
	if( gfHiliteMode && gubStartHilite != gubEndHilite )
	{ //Some or all of the text is hilighted, so we will use a different method.
		UINT16 i;
		UINT16 usStart, usEnd;
		//sort the hilite order.
		if( gubStartHilite < gubEndHilite )
		{
			usStart = gubStartHilite;
			usEnd = gubEndHilite;
		}
		else
		{
			usStart = gubEndHilite;
			usEnd = gubStartHilite;
		}
		//Traverse the string one character at a time, and draw the highlited part differently.
		for( i = 0; i < gpActive->ubStrLen; i++ )
		{
			uiCursorXPos = StringPixLengthArg(pColors->usFont, i, L"%S", gpActive->szString) + 3;
			if( i >= usStart && i < usEnd )
			{ //in highlighted part of text
				SetFontForeground( pColors->ubHiForeColor );
				SetFontShadow( pColors->ubHiShadowColor );
				SetFontBackground( pColors->ubHiBackColor );
			}
			else
			{ //in regular part of text
				SetFontForeground( pColors->ubForeColor );
				SetFontShadow( pColors->ubShadowColor );
				SetFontBackground( 0 );
			}
			mprintf(uiCursorXPos + gpActive->region.RegionTopLeftX, gpActive->region.RegionTopLeftY + usOffset, L"%C", gpActive->szString[i]);
		}
	}
	else
	{
		SetFontForeground( pColors->ubForeColor );
		SetFontShadow( pColors->ubShadowColor );
		SetFontBackground( 0 );
		mprintf(gpActive->region.RegionTopLeftX + 3, gpActive->region.RegionTopLeftY + usOffset, L"%S", gpActive->szString);
	}
	//Draw the cursor in the correct position.
	if( gfEditingText && gpActive->szString )
	{
		uiCursorXPos = StringPixLengthArg(pColors->usFont, gubCursorPos, L"%S", gpActive->szString) + 2;
		if( GetJA2Clock()%1000 < 500 )
		{	//draw the blinking ibeam cursor during the on blink period.
			ColorFillVideoSurfaceArea(FRAME_BUFFER,
				gpActive->region.RegionTopLeftX + uiCursorXPos,
				gpActive->region.RegionTopLeftY + usOffset,
				gpActive->region.RegionTopLeftX + uiCursorXPos + 1,
				gpActive->region.RegionTopLeftY + usOffset + GetFontHeight( pColors->usFont ), pColors->usCursorColor );
		}
	}
	RestoreFontSettings();
}

void RenderInactiveTextField( UINT8 ubID )
{
	UINT16 usOffset;
	TEXTINPUTNODE* pNode, *curr;
	curr = gpTextInputHead;
	pNode = NULL;
	while( curr )
	{
		if( curr->ubID == ubID )
		{
			pNode = curr;
			break;
		}
	}
	if( !pNode || !pNode->szString )
		return;
	SaveFontSettings();
	SetFont( pColors->usFont );
	usOffset = (UINT16)(( pNode->region.RegionBottomRightY - pNode->region.RegionTopLeftY - GetFontHeight( pColors->usFont ) ) / 2);
	SetFontForeground( pColors->ubForeColor );
	SetFontShadow( pColors->ubShadowColor );
	SetFontBackground( 0 );
	RenderBackgroundField( pNode );
	mprintf(pNode->region.RegionTopLeftX + 3, pNode->region.RegionTopLeftY + usOffset, L"%S", pNode->szString);
	RestoreFontSettings();
}


static void RenderInactiveTextFieldNode(TEXTINPUTNODE* pNode)
{
	UINT16 usOffset;
	if( !pNode || !pNode->szString )
		return;
	SaveFontSettings();
	SetFont( pColors->usFont );
	if( !pNode->fEnabled && pColors->fUseDisabledAutoShade )
	{ //use the color scheme specified by the user.
		SetFontForeground( pColors->ubDisabledForeColor );
		SetFontShadow( pColors->ubDisabledShadowColor );
	}
	else
	{
		SetFontForeground( pColors->ubForeColor );
		SetFontShadow( pColors->ubShadowColor );
	}
	usOffset = (UINT16)(( pNode->region.RegionBottomRightY - pNode->region.RegionTopLeftY - GetFontHeight( pColors->usFont ) ) / 2);
	SetFontBackground( 0 );
	RenderBackgroundField( pNode );
	mprintf(pNode->region.RegionTopLeftX + 3, pNode->region.RegionTopLeftY + usOffset, L"%S", pNode->szString);
	RestoreFontSettings();
	if( !pNode->fEnabled && pColors->fUseDisabledAutoShade )
	{
		UINT8 *pDestBuf;
		UINT32 uiDestPitchBYTES;
		SGPRect ClipRect;
		ClipRect.iLeft = pNode->region.RegionTopLeftX;
		ClipRect.iRight = pNode->region.RegionBottomRightX;
		ClipRect.iTop = pNode->region.RegionTopLeftY;
		ClipRect.iBottom = pNode->region.RegionBottomRightY;
		pDestBuf = LockVideoSurface( FRAME_BUFFER, &uiDestPitchBYTES );
		Blt16BPPBufferShadowRect( (UINT16*)pDestBuf, uiDestPitchBYTES, &ClipRect );
		UnLockVideoSurface( FRAME_BUFFER );
	}
}

//Use when you do a full interface update.
void RenderAllTextFields()
{
	STACKTEXTINPUTNODE *stackCurr;
	TEXTINPUTNODE *curr;
	//Render all of the other text input levels first,
	//if they exist at all.
	stackCurr = pInputStack;
	while( stackCurr )
	{
		curr = stackCurr->head;
		while( curr )
		{
			RenderInactiveTextFieldNode( curr );
			curr = curr->next;
		}
		stackCurr = stackCurr->next;
	}
	//Render the current text input level
	curr = gpTextInputHead;
	while( curr )
	{
		if( curr != gpActive )
			RenderInactiveTextFieldNode( curr );
		else
			RenderActiveTextField();
		curr = curr->next;
	}
}


static void EnableTextField(UINT8 ubID)
{
	TEXTINPUTNODE *curr;
  curr = gpTextInputHead;
	while( curr )
	{
		if( curr->ubID == ubID )
		{
			if( !curr->fEnabled )
			{
				if( !gpActive )
					gpActive = curr;
				MSYS_EnableRegion( &curr->region );
				curr->fEnabled = TRUE;
			}
			else
				return;
		}
		curr = curr->next;
	}
}

void DisableTextField( UINT8 ubID )
{
	TEXTINPUTNODE *curr;
  curr = gpTextInputHead;
	while( curr )
	{
		if( curr->ubID == ubID )
		{
			if( gpActive == curr )
				SelectNextField();
			if( curr->fEnabled )
			{
				MSYS_DisableRegion( &curr->region );
				curr->fEnabled = FALSE;
			}
			else
				return;
		}
		curr = curr->next;
	}
}

void EnableTextFields( UINT8 ubFirstID, UINT8 ubLastID )
{
	TEXTINPUTNODE *curr;
  curr = gpTextInputHead;
	while( curr )
	{
		if( curr->ubID >= ubFirstID && curr->ubID <= ubLastID )
		{
			if( gpActive == curr )
				SelectNextField();
			if( !curr->fEnabled )
			{
				MSYS_EnableRegion( &curr->region );
				curr->fEnabled = TRUE;
			}
		}
		curr = curr->next;
	}
}

void DisableTextFields( UINT8 ubFirstID, UINT8 ubLastID )
{
	TEXTINPUTNODE *curr;
  curr = gpTextInputHead;
	while( curr )
	{
		if( curr->ubID >= ubFirstID && curr->ubID <= ubLastID )
		{
			if( gpActive == curr )
				SelectNextField();
			if( curr->fEnabled )
			{
				MSYS_DisableRegion( &curr->region );
				curr->fEnabled = FALSE;
			}
		}
		curr = curr->next;
	}
}

void EnableAllTextFields()
{
	TEXTINPUTNODE *curr;
  curr = gpTextInputHead;
	while( curr )
	{
		if( !curr->fEnabled )
		{
			MSYS_EnableRegion( &curr->region );
			curr->fEnabled = TRUE;
		}
		curr = curr->next;
	}
	if( !gpActive )
		gpActive = gpTextInputHead;
}

void DisableAllTextFields()
{
	TEXTINPUTNODE *curr;
  curr = gpTextInputHead;
	while( curr )
	{
		if( curr->fEnabled )
		{
			MSYS_DisableRegion( &curr->region );
			curr->fEnabled = FALSE;
		}
		curr = curr->next;
	}
	gpActive = NULL;
}

BOOLEAN EditingText()
{
	return gfEditingText;
}

BOOLEAN TextInputMode()
{
	return gfTextInputMode;
}


//copy, cut, and paste hilighted text code
void InitClipboard()
{
	szClipboard = NULL;
}

void KillClipboard()
{
	if( szClipboard )
	{
		MemFree( szClipboard );
		szClipboard = NULL;
	}
}


static void ExecuteCopyCommand(void)
{
	UINT8 ubCount;
	UINT8 ubStart, ubEnd;
	if( !gpActive || !gpActive->szString )
		return;
	//Delete the current contents in the clipboard
	KillClipboard();
	//Calculate the start and end of the hilight
	if( gubStartHilite != gubEndHilite )
	{
		if( gubStartHilite < gubEndHilite )
		{
			ubStart = gubStartHilite;
			ubEnd = gubEndHilite;
		}
		else
		{
			ubStart = gubEndHilite;
			ubEnd = gubStartHilite;
		}
		ubCount = (UINT8)(ubEnd - ubStart);
		szClipboard = (UINT16*)MemAlloc( ( ubCount + 1 ) * 2 );
		Assert( szClipboard );
		for( ubCount = ubStart; ubCount < ubEnd; ubCount++ )
		{
			szClipboard[ ubCount-ubStart ] = gpActive->szString[ ubCount ];
		}
		szClipboard[ ubCount-ubStart ] = L'\0';
	}
}


static void ExecutePasteCommand(void)
{
	UINT8 ubCount;
	if( !gpActive || !szClipboard )
		return;
	DeleteHilitedText();

	ubCount = 0;
	while( szClipboard[ ubCount ] )
	{
		AddChar( szClipboard[ ubCount ] );
		ubCount++;
	}
}


static void ExecuteCutCommand(void)
{
	ExecuteCopyCommand();
	DeleteHilitedText();
}

//Saves the current text input mode, then removes it and activates the previous text input mode,
//if applicable.  The second function restores the settings.  Doesn't currently support nested
//calls.
void SaveAndRemoveCurrentTextInputMode()
{
	AssertMsg(pSavedHead == NULL, "Attempting to save text input stack head, when one already exists.");
	pSavedHead = gpTextInputHead;
	pSavedColors = pColors;
	if( pInputStack )
	{
		gpTextInputHead = pInputStack->head;
		pColors = pInputStack->pColors;
	}
	else
	{
		gpTextInputHead = NULL;
		pColors = NULL;
	}
}

void RestoreSavedTextInputMode()
{
	AssertMsg(pSavedHead != NULL, "Attempting to restore saved text input stack head, when one doesn't exist.");
  gpTextInputHead = pSavedHead;
	pColors = pSavedColors;
	pSavedHead = NULL;
	pSavedColors = NULL;
}


static UINT16 GetTextInputCursor(void)
{
	return gusTextInputCursor;
}

void SetTextInputCursor( UINT16 usNewCursor )
{
	STACKTEXTINPUTNODE *stackCurr;
	TEXTINPUTNODE *curr;
	if(	gusTextInputCursor == usNewCursor )
	{
		return;
	}
	gusTextInputCursor = usNewCursor;
	//Render all of the other text input levels first,
	//if they exist at all.
	stackCurr = pInputStack;
	while( stackCurr )
	{
		curr = stackCurr->head;
		while( curr )
		{
			MSYS_SetCurrentCursor( usNewCursor );
			curr = curr->next;
		}
		stackCurr = stackCurr->next;
	}
	//Render the current text input level
	curr = gpTextInputHead;
	while( curr )
	{
		MSYS_SetCurrentCursor( usNewCursor );
		curr = curr->next;
	}
}

//Utility functions for the INPUTTYPE_EXCLUSIVE_24HOURCLOCK input type.
UINT16 GetExclusive24HourTimeValueFromField( UINT8 ubField )
{
	TEXTINPUTNODE *curr;
	UINT16 usTime;
	curr = gpTextInputHead;
	while( curr )
	{
		if( curr->ubID == ubField )
		{
			if( curr->usInputType != INPUTTYPE_EXCLUSIVE_24HOURCLOCK )
				return 0xffff; //illegal!
			//First validate the hours 00-23
			if( curr->szString[0] == '2' && curr->szString[1] >= '0' &&		//20-23
					curr->szString[1] <='3' ||
					curr->szString[0] >= '0' && curr->szString[0] <= '1' &&		// 00-19
				  curr->szString[1] >= '0' && curr->szString[1] <= '9' )
			{ //Next, validate the colon, and the minutes 00-59
				if(	curr->szString[2] == ':' &&	curr->szString[5] == 0	 &&	//	:
						curr->szString[3] >= '0' && curr->szString[3] <= '5' &&	// 0-5
						curr->szString[4] >= '0' && curr->szString[4] <= '9' )	// 0-9
				{
					//Hours
					usTime = ((curr->szString[0]-0x30) * 10 + curr->szString[1]-0x30) * 60;
					//Minutes
					usTime += (curr->szString[3]-0x30) * 10 + curr->szString[4]-0x30;
					return usTime;
				}
			}
			// invalid
			return 0xffff;
		}
		curr = curr->next;
	}

	AssertMsg( FALSE, String( "GetExclusive24HourTimeValueFromField: Invalid field %d", ubField ) );
	return 0xffff;
}

//Utility functions for the INPUTTYPE_EXCLUSIVE_24HOURCLOCK input type.
void SetExclusive24HourTimeValue( UINT8 ubField, UINT16 usTime )
{
	TEXTINPUTNODE *curr;
	//First make sure the time is a valid time.  If not, then use 23:59
	if( usTime == 0xffff )
	{
		SetInputFieldStringWith16BitString( ubField, L"" );
		return;
	}
	usTime = min( 1439, usTime );
	curr = gpTextInputHead;
	while( curr )
	{
		if( curr->ubID == ubField )
		{
			AssertMsg(!curr->fUserField, String("Attempting to illegally set text into user field %d", curr->ubID ));
			curr->szString[0] = (usTime / 600) + 0x30;			//10 hours
			curr->szString[1] = (usTime / 60 % 10) + 0x30;	//1 hour
			usTime %= 60;																		//truncate the hours
			curr->szString[2] = ':';
			curr->szString[3] = (usTime / 10) + 0x30;				//10 minutes
			curr->szString[4] = (usTime % 10) + 0x30;				//1 minute;
			curr->szString[5] = 0;
			return;
		}
		curr = curr->next;
	}
}
