#include "Font.h"
#include "Local.h"
#include "PopUpBox.h"
#include "SysUtil.h"
#include "Debug.h"
#include "VObject.h"
#include "Video.h"
#include "MemMan.h"
#include "VSurface.h"


#define MAX_POPUP_BOX_COUNT 20


typedef struct PopUpString {
	wchar_t* pString;
	UINT8 ubForegroundColor;
	UINT8 ubBackgroundColor;
	UINT8 ubHighLight;
	UINT8 ubShade;
	UINT8 ubSecondaryShade;
	BOOLEAN fHighLightFlag;
	BOOLEAN fShadeFlag;
	BOOLEAN fSecondaryShadeFlag;
} PopUpString;

struct PopUpBox
{
	SGPBox pos;
	UINT32 uiLeftMargin;
	UINT32 uiRightMargin;
	UINT32 uiBottomMargin;
	UINT32 uiTopMargin;
	UINT32 uiLineSpace;
	const SGPVObject* iBorderObjectIndex;
	SGPVSurface* iBackGroundSurface;
	UINT32 uiFlags;
	SGPVSurface* uiBuffer;
	UINT32 uiSecondColumnMinimunOffset;
	UINT32 uiSecondColumnCurrentOffset;
	UINT32 uiBoxMinWidth;
	BOOLEAN fUpdated;
	BOOLEAN fShowBox;
	UINT32  font;

	PopUpString* Text[MAX_POPUP_BOX_STRING_COUNT];
	PopUpString* pSecondColumnString[MAX_POPUP_BOX_STRING_COUNT];
};

static PopUpBox* PopUpBoxList[MAX_POPUP_BOX_COUNT];

#define FOR_ALL_POPUP_BOXES(iter)                                           \
	for (PopUpBox** iter = PopUpBoxList; iter != endof(PopUpBoxList); ++iter) \
		if (*iter == NULL) continue; else


#define BORDER_WIDTH  16
#define BORDER_HEIGHT  8
#define TOP_LEFT_CORNER     0
#define TOP_EDGE            4
#define TOP_RIGHT_CORNER    1
#define SIDE_EDGE           5
#define BOTTOM_LEFT_CORNER  2
#define BOTTOM_EDGE         4
#define BOTTOM_RIGHT_CORNER 3


void SetLineSpace(PopUpBox* const box, const UINT32 uiLineSpace)
{
	box->uiLineSpace = uiLineSpace;
}


UINT32 GetLineSpace(const PopUpBox* const box)
{
	// return number of pixels between lines for this box
	return box->uiLineSpace;
}


void SpecifyBoxMinWidth(PopUpBox* const box, INT32 iMinWidth)
{
	box->uiBoxMinWidth = iMinWidth;

	// check if the box is currently too small
	if (box->pos.w < iMinWidth) box->pos.w = iMinWidth;
}


PopUpBox* CreatePopUpBox(const SGPPoint Position, const UINT32 uiFlags, SGPVSurface* const buffer, const SGPVObject* const border, SGPVSurface* const background, const UINT32 margin_l, const UINT32 margin_t, const UINT32 margin_b, const UINT32 margin_r)
{
	// find first free box
	for (PopUpBox** i = PopUpBoxList; i != endof(PopUpBoxList); ++i)
	{
		if (*i == NULL)
		{
			PopUpBox* const box = MemAlloc(sizeof(*box));
			if (box == NULL) return NO_POPUP_BOX;
			memset(box, 0, sizeof(*box));

			SetBoxXY(box, Position.iX, Position.iY);
			box->uiFlags            = uiFlags;
			box->uiBuffer           = buffer;
			box->iBorderObjectIndex = border;
			box->iBackGroundSurface = background;
			box->uiLeftMargin       = margin_l;
			box->uiRightMargin      = margin_r;
			box->uiTopMargin        = margin_t;
			box->uiBottomMargin     = margin_b;

			*i = box;
			return box;
		}
	}

	// ran out of available popup boxes - probably not freeing them up right!
	Assert(0);
	return NO_POPUP_BOX;
}


UINT32 GetTopMarginSize(const PopUpBox* const box)
{
	// return size of top margin, for mouse region offsets
	return box->uiTopMargin;
}


void ShadeStringInBox(PopUpBox* const box, const INT32 iLineNumber)
{
	// shade iLineNumber Line in box indexed by hBoxHandle
	if (box->Text[iLineNumber] != NULL)
	{
		// shade line
		box->Text[iLineNumber]->fShadeFlag = TRUE;
	}
}


void UnShadeStringInBox(PopUpBox* const box, const INT32 iLineNumber)
{
	// unshade iLineNumber in box
	if (box->Text[iLineNumber] != NULL)
	{
		// shade line
		box->Text[iLineNumber]->fShadeFlag = FALSE;
	}
}


void SecondaryShadeStringInBox(PopUpBox* const box, const INT32 iLineNumber)
{
	// shade iLineNumber Line in box indexed by hBoxHandle
	if (box->Text[iLineNumber] != NULL)
	{
		// shade line
		box->Text[iLineNumber]->fSecondaryShadeFlag = TRUE;
	}
}


void UnSecondaryShadeStringInBox(PopUpBox* const box, const INT32 iLineNumber)
{
	// unshade iLineNumber in box indexed by hBoxHandle
	if (box->Text[iLineNumber] != NULL)
	{
		// shade line
		box->Text[iLineNumber]->fSecondaryShadeFlag = FALSE;
	}
}


void SetBoxXY(PopUpBox* const box, const INT16 x, const INT16 y)
{
	box->pos.x    = x;
	box->pos.y    = y;
	box->fUpdated = FALSE;
}


void SetBoxX(PopUpBox* const box, const INT16 x)
{
	box->pos.x    = x;
	box->fUpdated = FALSE;
}


void SetBoxY(PopUpBox* const box, const INT16 y)
{
	box->pos.y    = y;
	box->fUpdated = FALSE;
}


const SGPBox* GetBoxArea(const PopUpBox* const box)
{
	return &box->pos;
}


// adds a FIRST column string to the CURRENT popup box
void AddMonoString(PopUpBox* const box, const wchar_t* pString)
{
	INT32 iCounter = 0;

	// find first free slot in list
	for (iCounter = 0; iCounter < MAX_POPUP_BOX_STRING_COUNT && box->Text[iCounter] != NULL; iCounter++);

	if ( iCounter >= MAX_POPUP_BOX_STRING_COUNT )
	{
		// using too many text lines, or not freeing them up properly
		Assert(0);
		return;
	}

	PopUpString* pStringSt = MemAlloc(sizeof(*pStringSt));
	if (pStringSt == NULL)
		return;

	wchar_t* const pLocalString = MemAlloc(sizeof(*pLocalString) * (wcslen(pString) + 1));
	if (pLocalString == NULL)
		return;

	wcscpy(pLocalString, pString);

	box->Text[iCounter]                      = pStringSt;
	box->Text[iCounter]->pString             = pLocalString;
	box->Text[iCounter]->fShadeFlag          = FALSE;
	box->Text[iCounter]->fHighLightFlag      = FALSE;
	box->Text[iCounter]->fSecondaryShadeFlag = FALSE;

	box->fUpdated = FALSE;
}


static void RemoveBoxSecondaryText(PopUpBox*, INT32 hStringHandle);


void AddSecondColumnMonoString(PopUpBox* const box, const wchar_t* const pString)
{
	INT32 iCounter=0;

	// find the LAST USED text string index
	for (iCounter = 0; iCounter + 1 < MAX_POPUP_BOX_STRING_COUNT && box->Text[iCounter + 1] != NULL; iCounter++);

	if ( iCounter >= MAX_POPUP_BOX_STRING_COUNT )
	{
		// using too many text lines, or not freeing them up properly
		Assert(0);
		return;
	}

	PopUpString* pStringSt = MemAlloc(sizeof(*pStringSt));
	if (pStringSt == NULL)
		return;

	wchar_t* const pLocalString = MemAlloc(sizeof(*pLocalString) * (wcslen(pString) + 1));
	if (pLocalString == NULL)
		return;

	wcscpy(pLocalString, pString);

	RemoveBoxSecondaryText(box, iCounter);

	box->pSecondColumnString[iCounter]                 = pStringSt;
	box->pSecondColumnString[iCounter]->pString        = pLocalString;
	box->pSecondColumnString[iCounter]->fShadeFlag     = FALSE;
	box->pSecondColumnString[iCounter]->fHighLightFlag = FALSE;
}


UINT32 GetNumberOfLinesOfTextInBox(const PopUpBox* const box)
{
	INT32 iCounter = 0;

	// count number of lines
	// check string size
	for( iCounter = 0; iCounter < MAX_POPUP_BOX_STRING_COUNT; iCounter++ )
	{
		if (box->Text[iCounter] == NULL) break;
	}

	return( iCounter );
}


void SetBoxFont(PopUpBox* const box, const UINT32 font)
{
	box->font     = font;
	box->fUpdated = FALSE;
}


void SetBoxSecondColumnMinimumOffset(PopUpBox* const box, const UINT32 uiWidth)
{
	box->uiSecondColumnMinimunOffset = uiWidth;
}


UINT32 GetBoxFont(const PopUpBox* const box)
{
	return box->font;
}


// set the foreground color of this string in this pop up box
void SetBoxLineForeground(PopUpBox* const box, const INT32 iStringValue, const UINT8 ubColor)
{
	box->Text[iStringValue]->ubForegroundColor = ubColor;
}


void SetBoxSecondaryShade(PopUpBox* const box, const UINT8 ubColor)
{
	UINT32 uiCounter;
	for ( uiCounter = 0; uiCounter < MAX_POPUP_BOX_STRING_COUNT; uiCounter++ )
	{
		if (box->Text[uiCounter] != NULL)
		{
			box->Text[uiCounter]->ubSecondaryShade = ubColor;
		}
	}
}


void SetBoxForeground(PopUpBox* const box, UINT8 ubColor)
{
	UINT32 uiCounter;
	for ( uiCounter = 0; uiCounter < MAX_POPUP_BOX_STRING_COUNT; uiCounter++ )
	{
		if (box->Text[uiCounter] != NULL)
		{
			box->Text[uiCounter]->ubForegroundColor = ubColor;
		}
	}
}


void SetBoxBackground(PopUpBox* const box, UINT8 ubColor)
{
	UINT32 uiCounter;
	for ( uiCounter = 0; uiCounter < MAX_POPUP_BOX_STRING_COUNT; uiCounter++ )
	{
		if (box->Text[uiCounter] != NULL)
		{
			box->Text[uiCounter]->ubBackgroundColor = ubColor;
		}
	}
}


void SetBoxHighLight(PopUpBox* const box, UINT8 ubColor)
{
	UINT32 uiCounter;
	for ( uiCounter = 0; uiCounter < MAX_POPUP_BOX_STRING_COUNT; uiCounter++ )
	{
		if (box->Text[uiCounter] != NULL)
		{
			box->Text[uiCounter]->ubHighLight = ubColor;
		}
	}
}


void SetBoxShade(PopUpBox* const box, UINT8 ubColor)
{
	UINT32 uiCounter;
	for ( uiCounter = 0; uiCounter < MAX_POPUP_BOX_STRING_COUNT; uiCounter++ )
	{
		if (box->Text[uiCounter] != NULL)
		{
			box->Text[uiCounter]->ubShade = ubColor;
		}
	}
}

void SetBoxSecondColumnForeground(PopUpBox* const box, const UINT8 ubColor)
{
	UINT32 iCounter = 0;
	for (iCounter = 0; iCounter < MAX_POPUP_BOX_STRING_COUNT; iCounter++)
	{
		if (box->pSecondColumnString[iCounter])
		{
			box->pSecondColumnString[iCounter]->ubForegroundColor = ubColor;
		}
	}
}


void SetBoxSecondColumnBackground(PopUpBox* const box, const UINT8 ubColor)
{
	UINT32 iCounter = 0;
	for (iCounter = 0; iCounter < MAX_POPUP_BOX_STRING_COUNT; iCounter++)
	{
		if (box->pSecondColumnString[iCounter])
		{
			box->pSecondColumnString[iCounter]->ubBackgroundColor = ubColor;
		}
	}
}


void SetBoxSecondColumnHighLight(PopUpBox* const box, const UINT8 ubColor)
{
	UINT32 iCounter = 0;
	for (iCounter = 0; iCounter < MAX_POPUP_BOX_STRING_COUNT; iCounter++)
	{
		if (box->pSecondColumnString[iCounter])
		{
			box->pSecondColumnString[iCounter]->ubHighLight = ubColor;
		}
	}
}


void SetBoxSecondColumnShade(PopUpBox* const box, const UINT8 ubColor)
{
	UINT32 iCounter = 0;
	for (iCounter = 0; iCounter < MAX_POPUP_BOX_STRING_COUNT; iCounter++)
	{
		if (box->pSecondColumnString[iCounter])
		{
			box->pSecondColumnString[iCounter]->ubShade = ubColor;
		}
	}
}


void HighLightBoxLine(PopUpBox* const box, const INT32 iLineNumber)
{
	// highlight iLineNumber Line in box
	PopUpString* const line = box->Text[iLineNumber];
	if (line != NULL)
	{
		line->fHighLightFlag = TRUE;
	}
}


BOOLEAN GetBoxShadeFlag(const PopUpBox* const box, const INT32 iLineNumber)
{
	if (box->Text[iLineNumber] != NULL)
	{
		return box->Text[iLineNumber]->fShadeFlag;
	}
	return( FALSE );
}


void UnHighLightBox(PopUpBox* const box)
{
	INT32 iCounter = 0;

	for (iCounter = 0; iCounter < MAX_POPUP_BOX_STRING_COUNT; iCounter++)
	{
		if (box->Text[iCounter]) box->Text[iCounter]->fHighLightFlag = FALSE;
	}
}


static void RemoveBoxPrimaryText(PopUpBox*, INT32 hStringHandle);


void RemoveAllBoxStrings(PopUpBox* const box)
{
	for (UINT32 i = 0; i < MAX_POPUP_BOX_STRING_COUNT; ++i)
	{
		RemoveBoxPrimaryText(box, i);
		RemoveBoxSecondaryText(box, i);
	}
	box->fUpdated = FALSE;
}


void RemoveBox(PopUpBox* const box)
{
	FOR_ALL_POPUP_BOXES(i)
	{
		if (*i == box)
		{
			*i = NULL;
			RemoveAllBoxStrings(box);
			MemFree(box);
			return;
		}
	}
	Assert(0);
}


void ShowBox(PopUpBox* const box)
{
	if (box->fShowBox == FALSE)
	{
		box->fShowBox = TRUE;
		box->fUpdated = FALSE;
	}
}


void HideBox(PopUpBox* const box)
{
	if (box->fShowBox)
	{
		box->fShowBox = FALSE;
		box->fUpdated = FALSE;
	}
}


void DisplayBoxes(SGPVSurface* const uiBuffer)
{
	FOR_ALL_POPUP_BOXES(i)
	{
		DisplayOnePopupBox(*i, uiBuffer);
	}
}


static void DrawBox(const PopUpBox*);
static void DrawBoxText(const PopUpBox*);


void DisplayOnePopupBox(PopUpBox* const box, SGPVSurface* const uiBuffer)
{
	if (!box->fUpdated && box->fShowBox && box->uiBuffer == uiBuffer)
	{
		box->fUpdated = TRUE;
		if (box->uiFlags & POPUP_BOX_FLAG_RESIZE) ResizeBoxToText(box);
		DrawBox(box);
		DrawBoxText(box);
	}
}


void ForceUpDateOfBox(PopUpBox* const box)
{
	box->fUpdated = FALSE;
}


static void DrawBox(const PopUpBox* const box)
{
	const UINT16 x = box->pos.x;
	const UINT16 y = box->pos.y;
	UINT16       w = box->pos.w;
	const UINT16 h = box->pos.h;

	// make sure it will fit on screen!
	Assert(x + w < SCREEN_WIDTH);
	Assert(y + h < SCREEN_HEIGHT);

	// subtract 4 because the 2 2-pixel corners are handled separately
	const UINT32 uiNumTilesWide = (w - 4) / BORDER_WIDTH;
	const UINT32 uiNumTilesHigh = (h - 4) / BORDER_HEIGHT;

	SGPVSurface* const dst = box->uiBuffer;

	// blit in texture first, then borders
	const SGPRect clip = { 0, 0, w, h };
	BltVideoSurface(dst, box->iBackGroundSurface, x, y, &clip);

	const SGPVObject* const border = box->iBorderObjectIndex;

	// blit in 4 corners (they're 2x2 pixels)
	BltVideoObject(dst, border, TOP_LEFT_CORNER,     x,         y);
	BltVideoObject(dst, border, TOP_RIGHT_CORNER,    x + w - 2, y);
	BltVideoObject(dst, border, BOTTOM_RIGHT_CORNER, x + w - 2, y + h - 2);
	BltVideoObject(dst, border, BOTTOM_LEFT_CORNER,  x,         y + h - 2);

	// blit in edges
	if (uiNumTilesWide > 0)
	{
		// full pieces
		for (UINT32 i = 0; i < uiNumTilesWide; ++i)
		{
			const INT32 lx = x + 2 + i * BORDER_WIDTH;
			BltVideoObject(dst, border, TOP_EDGE,    lx, y);
			BltVideoObject(dst, border, BOTTOM_EDGE, lx, y + h - 2);
		}

		// partial pieces
		const INT32 lx = x + w - 2 - BORDER_WIDTH;
		BltVideoObject(dst, border, TOP_EDGE,    lx, y);
		BltVideoObject(dst, border, BOTTOM_EDGE, lx, y + h - 2);
	}
	if (uiNumTilesHigh > 0)
	{
		// full pieces
		for (UINT32 i = 0; i < uiNumTilesHigh; ++i)
		{
			const INT32 ly = y + 2 + i * BORDER_HEIGHT;
			BltVideoObject(dst, border, SIDE_EDGE, x,         ly);
			BltVideoObject(dst, border, SIDE_EDGE, x + w - 2, ly);
		}

		// partial pieces
		const INT32 ly = y + h - 2 - BORDER_HEIGHT;
		BltVideoObject(dst, border, SIDE_EDGE, x,         ly);
		BltVideoObject(dst, border, SIDE_EDGE, x + w - 2, ly);
	}

	InvalidateRegion(x, y, x + w, y + h);
}


static void DrawBoxText(const PopUpBox* const box)
{
	const UINT32 font = box->font;
	const INT32  tlx  = box->pos.x + box->uiLeftMargin;
	const INT32  tly  = box->pos.y + box->uiTopMargin;
	const INT32  brx  = box->pos.x + box->pos.w - box->uiRightMargin;
	const INT32  bry  = box->pos.y + box->pos.h - box->uiBottomMargin;
	const INT32  w    = box->pos.w - (box->uiRightMargin + box->uiLeftMargin + 2);
	const INT32  h    = GetFontHeight(font);

	SetFont(font);
	SetFontDestBuffer(box->uiBuffer, tlx - 1, tly, brx, bry);

	for (UINT32 i = 0; i < MAX_POPUP_BOX_STRING_COUNT; ++i)
	{
		// there is text in this line?
		const PopUpString* const text = box->Text[i];
		if (text)
		{
			// are we highlighting?...shading?..or neither
			if (text->fHighLightFlag)
			{
				SetFontForeground(text->ubHighLight);
			}
			else if (text->fSecondaryShadeFlag)
			{
				SetFontForeground(text->ubSecondaryShade);
			}
			else if (text->fShadeFlag)
			{
				SetFontForeground(text->ubShade);
			}
			else
			{
				SetFontForeground(text->ubForegroundColor);
			}

			SetFontBackground(text->ubBackgroundColor);

			const INT32 y = tly + i * (h + box->uiLineSpace);
			INT16 uX;
			INT16 uY;
			if (box->uiFlags & POPUP_BOX_FLAG_CENTER_TEXT)
			{
				FindFontCenterCoordinates(tlx, y, w, h, text->pString, font, &uX, &uY);
			}
			else
			{
				uX = tlx;
				uY = y;
			}
			mprintf(uX, uY, L"%ls", text->pString);
		}

		// there is secondary text in this line?
		const PopUpString* const second = box->pSecondColumnString[i];
		if (second)
		{
			// are we highlighting?...shading?..or neither
			if (second->fHighLightFlag)
			{
				SetFontForeground(second->ubHighLight);
			}
			else if (second->fShadeFlag)
			{
				SetFontForeground(second->ubShade);
			}
			else
			{
				SetFontForeground(second->ubForegroundColor);
			}

			SetFontBackground(second->ubBackgroundColor);

			const INT32 y = tly + i * (h + box->uiLineSpace);
			INT16 uX;
			INT16 uY;
			if (box->uiFlags & POPUP_BOX_FLAG_CENTER_TEXT)
			{
				FindFontCenterCoordinates(tlx, y, w, h, second->pString, font, &uX, &uY);
			}
			else
			{
				uX = tlx + box->uiSecondColumnCurrentOffset;
				uY = y;
			}
			mprintf(uX, uY, L"%ls", second->pString);
		}
	}

	if (box->uiBuffer != guiSAVEBUFFER)
	{
		InvalidateRegion(tlx - 1, tly, brx, bry);
	}

	SetFontDestBuffer(FRAME_BUFFER, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}


void ResizeBoxToText(PopUpBox* const box)
{
	const UINT32 font = box->font;
	UINT32 max_lw = 0; // width of left  column
	UINT32 max_rw = 0; // width of right column
	UINT i;
	for (i = 0; i < MAX_POPUP_BOX_STRING_COUNT; ++i)
	{
		const PopUpString* const l = box->Text[i];
		if (l == NULL) break;

		const UINT32 lw = StringPixLength(l->pString, font);
		if (lw > max_lw) max_lw = lw;

		const PopUpString* const r = box->pSecondColumnString[i];
		if (r != NULL)
		{
			const UINT32 rw = StringPixLength(r->pString, font);
			if (rw > max_rw) max_rw = rw;
		}
	}

	const UINT32 r_off = max_lw + box->uiSecondColumnMinimunOffset;
	box->uiSecondColumnCurrentOffset = r_off;

	UINT32 w = box->uiLeftMargin + r_off + max_rw + box->uiRightMargin;;
	if (w < box->uiBoxMinWidth) w = box->uiBoxMinWidth;
	box->pos.w = w;

	box->pos.h = box->uiTopMargin + i * (GetFontHeight(font) + box->uiLineSpace) + box->uiBottomMargin;
}


BOOLEAN IsBoxShown(const PopUpBox* const box)
{
	if (box == NULL) return FALSE;
	return box->fShowBox;
}


void MarkAllBoxesAsAltered( void )
{
	FOR_ALL_POPUP_BOXES(i)
	{
		ForceUpDateOfBox(*i);
	}
}


void HideAllBoxes( void )
{
	FOR_ALL_POPUP_BOXES(i)
	{
		HideBox(*i);
	}
}


static void RemoveBoxPrimaryText(PopUpBox* const Box, const INT32 hStringHandle)
{
	Assert(Box != NULL);
	Assert( hStringHandle < MAX_POPUP_BOX_STRING_COUNT );

	// remove & release primary text
	if (Box->Text[hStringHandle] != NULL)
	{
		if (Box->Text[hStringHandle]->pString)
		{
			MemFree(Box->Text[hStringHandle]->pString);
		}

		MemFree(Box->Text[hStringHandle]);
		Box->Text[hStringHandle] = NULL;
	}
}


static void RemoveBoxSecondaryText(PopUpBox* const Box, const INT32 hStringHandle)
{
	Assert(Box != NULL);
	Assert( hStringHandle < MAX_POPUP_BOX_STRING_COUNT );

	// remove & release secondary strings
	if (Box->pSecondColumnString[hStringHandle] != NULL)
	{
		if (Box->pSecondColumnString[hStringHandle]->pString)
		{
			MemFree(Box->pSecondColumnString[hStringHandle]->pString);
		}

		MemFree(Box->pSecondColumnString[hStringHandle]);
		Box->pSecondColumnString[hStringHandle] = NULL;
	}
}
