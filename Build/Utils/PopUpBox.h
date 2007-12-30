#ifndef __POPUP_BOX
#define __POPUP_BOX

#include "JA2Types.h"

#define MAX_POPUP_BOX_STRING_COUNT 50		// worst case = 45: move menu with 20 soldiers, each on different squad + overhead

// PopUpBox Flags
#define POPUP_BOX_FLAG_CENTER_TEXT 2
#define POPUP_BOX_FLAG_RESIZE 4

#define NO_POPUP_BOX NULL

PopUpBox* CreatePopUpBox(SGPRect Dimensions, SGPPoint Position, UINT32 uiFlags);

void SetMargins(PopUpBox*, UINT32 uiLeft, UINT32 uiTop, UINT32 uiBottom, UINT32 uiRight);
UINT32 GetTopMarginSize(const PopUpBox*);
void SetLineSpace(PopUpBox*, UINT32 uiLineSpace);
UINT32 GetLineSpace(const PopUpBox*);
void SetBoxBuffer(PopUpBox*, SGPVSurface*);
void SetBoxPosition(PopUpBox*, SGPPoint Position);
void GetBoxPosition(const PopUpBox*, SGPPoint* Position);
UINT32 GetNumberOfLinesOfTextInBox(const PopUpBox*);
void SetBoxSize(PopUpBox*, SGPRect Dimensions);
void GetBoxSize(const PopUpBox*, SGPRect* dimensions);
void SetBorderType(PopUpBox*, const SGPVObject* border);
void SetBackGroundSurface(PopUpBox*, SGPVSurface*);
void AddMonoString(PopUpBox*, const wchar_t* pString);
void SetBoxFont(PopUpBox*, UINT32 uiFont);
UINT32 GetBoxFont(const PopUpBox*);
void SetBoxForeground(PopUpBox*, UINT8 ubColor);
void SetBoxBackground(PopUpBox*, UINT8 ubColor);
void SetBoxHighLight(PopUpBox*, UINT8 ubColor);
void SetBoxShade(PopUpBox*, UINT8 ubColor);
void ShadeStringInBox(PopUpBox*, INT32 iLineNumber);
void UnShadeStringInBox(PopUpBox*, INT32 iLineNumber);
void HighLightBoxLine(PopUpBox*, INT32 iLineNumber);
void UnHighLightBox(PopUpBox*);
void RemoveAllBoxStrings(PopUpBox*);
void RemoveBox(PopUpBox*);
void ShowBox(PopUpBox*);
void HideBox(PopUpBox*);
void DisplayBoxes(SGPVSurface* buffer);
void DisplayOnePopupBox(PopUpBox*, SGPVSurface* buffer);

// resize this box to the text it contains
void ResizeBoxToText(PopUpBox*);

// force update/redraw of this boxes background
void ForceUpDateOfBox(PopUpBox*);

// force redraw of ALL boxes
void MarkAllBoxesAsAltered( void );

// is the box being displayed?
BOOLEAN IsBoxShown(const PopUpBox*);

// is this line in the current box set to a shaded state ?
BOOLEAN GetBoxShadeFlag(const PopUpBox*, INT32 iLineNumber);

// set boxes foreground color
void SetBoxLineForeground(PopUpBox*, INT32 iStringValue, UINT8 ubColor);

// hide all visible boxes
void HideAllBoxes( void );

// add a second column monochrome string
void AddSecondColumnMonoString(PopUpBox*, const wchar_t* pString);

// set the 2nd column font for this box
void SetBoxSecondColumnFont(PopUpBox*, UINT32 uiFont);

// set the minimum offset
void SetBoxSecondColumnMinimumOffset(PopUpBox*, UINT32 uiWidth);

// now on a box wide basis, one if recomened to use this function after adding all the strings..rather than on an individual basis
void SetBoxSecondColumnForeground(PopUpBox*, UINT8 ubColor);
void SetBoxSecondColumnBackground(PopUpBox*, UINT8 ubColor);
void SetBoxSecondColumnHighLight(PopUpBox*, UINT8 ubColor);
void SetBoxSecondColumnShade(PopUpBox*, UINT8 ubColor);

// secondary shades for boxes
void UnSecondaryShadeStringInBox(PopUpBox*, INT32 iLineNumber);
void SecondaryShadeStringInBox(PopUpBox*, INT32 iLineNumber);
void SetBoxSecondaryShade(PopUpBox*, UINT8 ubColor);

// min width for box
void SpecifyBoxMinWidth(PopUpBox*, INT32 iMinWidth);

#endif
