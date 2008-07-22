#include <stdarg.h>
#include "HImage.h"
#include "Local.h"
#include "Types.h"
#include "MemMan.h"
#include "Font.h"
#include "Debug.h"
#include "TranslationTable.h"
#include "VSurface.h"
#include "VObject.h"
#include "VObject_Blitters.h"


#define MAX_FONTS 25


// Destination printing parameters
Font                FontDefault      = 0;
static SGPVSurface* FontDestBuffer;
static SGPRect      FontDestRegion   = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
static UINT16       FontForeground16 = 0;
static UINT16       FontBackground16 = 0;
static UINT16       FontShadow16     = DEFAULT_SHADOW;

// Temp, for saving printing parameters
static Font         SaveFontDefault      = 0;
static SGPVSurface* SaveFontDestBuffer   = NULL;
static SGPRect      SaveFontDestRegion   = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
static UINT16       SaveFontForeground16 = 0;
static UINT16       SaveFontShadow16     = 0;
static UINT16       SaveFontBackground16 = 0;


/* Sets both the foreground and the background colors of the current font. The
 * top byte of the parameter word is the background color, and the bottom byte
 * is the foreground. */
void SetFontColors(UINT16 usColors)
{
	UINT8 ubForeground = usColors & 0xFF;
	UINT8 ubBackground = (usColors >> 8) & 0xFF;

	SetFontForeground(ubForeground);
	SetFontBackground(ubBackground);
}


/* Sets the foreground color of the currently selected font. The parameter is
 * the index into the 8-bit palette. In 16BPP mode, the RGB values from the
 * palette are used to create the pixel color. Note that if you change fonts,
 * the selected foreground/background colors will stay at what they are
 * currently set to. */
void SetFontForeground(UINT8 ubForeground)
{
	if (!FontDefault) return;
	const SGPPaletteEntry* const c = &FontDefault->Palette()[ubForeground];
	FontForeground16 = Get16BPPColor(FROMRGB(c->r, c->g, c->b));
}


void SetFontShadow(UINT8 ubShadow)
{
	if (!FontDefault) return;
	const SGPPaletteEntry* const c = &FontDefault->Palette()[ubShadow];
	FontShadow16 = Get16BPPColor(FROMRGB(c->r, c->g, c->b));

	if (ubShadow != 0 && FontShadow16 == 0) FontShadow16 = 1;
}


/* Sets the Background color of the currently selected font. The parameter is
 * the index into the 8-bit palette. In 16BPP mode, the RGB values from the
 * palette are used to create the pixel color. If the background value is zero,
 * the background of the font will be transparent.  Note that if you change
 * fonts, the selected foreground/background colors will stay at what they are
 * currently set to. */
void SetFontBackground(UINT8 ubBackground)
{
	if (!FontDefault) return;
	const SGPPaletteEntry* const c = &FontDefault->Palette()[ubBackground];
	FontBackground16 = Get16BPPColor(FROMRGB(c->r, c->g, c->b));
}


/* Loads a font from an ETRLE file */
Font LoadFontFile(const char *filename)
{
	Assert(filename != NULL);
	Assert(strlen(filename));

	Font const font = AddVideoObjectFromFile(filename);
	if (!FontDefault) FontDefault = font;
	return font;
}


/* Deletes the video object of a particular font. Frees up the memory and
 * resources allocated for it. */
void UnloadFont(Font const font)
{
	Assert(font);
	DeleteVideoObject(font);
}


/* Returns the width of a given character in the font. */
static UINT32 GetWidth(HVOBJECT hSrcVObject, INT16 ssIndex)
{
	Assert(hSrcVObject != NULL);

	// Get Offsets from Index into structure
	ETRLEObject const* const pTrav = hSrcVObject->SubregionProperties(ssIndex);
	return pTrav->usWidth + pTrav->sOffsetX;
}


/* Returns the length of a string in pixels, depending on the font given. */
INT16 StringPixLength(const wchar_t *string, Font const font)
{
	if (string == NULL) return 0;

	UINT32 Cur = 0;
	for (const wchar_t* curletter = string; *curletter != L'\0'; curletter++)
	{
		Cur += GetCharWidth(font, *curletter);
	}
	return Cur;
}


/* Saves the current font printing settings into temporary locations. */
void SaveFontSettings(void)
{
	SaveFontDefault      = FontDefault;
	SaveFontDestBuffer   = FontDestBuffer;
	SaveFontDestRegion   = FontDestRegion;
	SaveFontForeground16 = FontForeground16;
	SaveFontShadow16     = FontShadow16;
	SaveFontBackground16 = FontBackground16;
}


/* Restores the last saved font printing settings from the temporary lactions */
void RestoreFontSettings(void)
{
	FontDefault      = SaveFontDefault;
	FontDestBuffer   = SaveFontDestBuffer;
	FontDestRegion   = SaveFontDestRegion;
	FontForeground16 = SaveFontForeground16;
	FontShadow16     = SaveFontShadow16;
	FontBackground16 = SaveFontBackground16;
}


/* Returns the height of a given character in the font. */
static UINT32 GetHeight(HVOBJECT hSrcVObject, INT16 ssIndex)
{
	Assert(hSrcVObject != NULL);

	// Get Offsets from Index into structure
	ETRLEObject const* const pTrav = hSrcVObject->SubregionProperties(ssIndex);
	return pTrav->usHeight + pTrav->sOffsetY;
}


/* Returns the height of the first character in a font. */
UINT16 GetFontHeight(Font const font)
{
	Assert(font);
	return GetHeight(font, 0);
}


/* Given a wide char, this function returns the index of the glyph. Returns 0
 * - the index of 'A' (or ' ', depending on data files) - if no glyph exists for
 * the requested wide char. */
static INT16 GetIndex(wchar_t c)
{
	UINT16 Idx = 0;
	if (c < lengthof(TranslationTable)) Idx = TranslationTable[c];
#if defined RUSSIAN
	if (Idx == 0 && c != L' ')
#else
	if (Idx == 0 && c != L'A')
#endif
	{
		DebugMsg(TOPIC_FONT_HANDLER, DBG_LEVEL_0, String("Error: Invalid character given U%04X", c));
	}
	return Idx;
}


UINT32 GetCharWidth(HVOBJECT Font, wchar_t c)
{
	return GetWidth(Font, GetIndex(c));
}


/* Sets the current font number. */
void SetFont(Font const font)
{
	Assert(font);
	FontDefault = font;
}


void SetFontDestBuffer(SGPVSurface* const dst, const INT32 x1, const INT32 y1, const INT32 x2, const INT32 y2)
{
	Assert(x2 > x1);
	Assert(y2 > y1);

	FontDestBuffer         = dst;
	FontDestRegion.iLeft   = x1;
	FontDestRegion.iTop    = y1;
	FontDestRegion.iRight  = x2;
	FontDestRegion.iBottom = y2;
}


void SetFontDestBuffer(SGPVSurface* const dst)
{
	SetFontDestBuffer(dst, 0, 0, dst->Width(), dst->Height());
}


void FindFontRightCoordinates(INT16 sLeft, INT16 sTop, INT16 sWidth, INT16 sHeight, const wchar_t* pStr, Font const font, INT16* psNewX, INT16* psNewY)
{
	// Compute the coordinates to right justify the text
	INT16 xp = sWidth - StringPixLength(pStr, font) + sLeft;
	INT16 yp = (sHeight - GetFontHeight(font)) / 2 + sTop;

	*psNewX = xp;
	*psNewY = yp;
}


void FindFontCenterCoordinates(INT16 sLeft, INT16 sTop, INT16 sWidth, INT16 sHeight, const wchar_t* pStr, Font const font, INT16* psNewX, INT16* psNewY)
{
	// Compute the coordinates to center the text
	INT16 xp = (sWidth - StringPixLength(pStr, font) + 1) / 2 + sLeft;
	INT16 yp = (sHeight - GetFontHeight(font)) / 2 + sTop;

	*psNewX = xp;
	*psNewY = yp;
}


/* Prints to the currently selected destination buffer, at the X/Y coordinates
 * specified, using the currently selected font. Other than the X/Y coordinates,
 * the parameters are identical to printf. The resulting string may be no longer
 * than 512 word-characters. */
UINT32 gprintf(INT32 x, INT32 y, const wchar_t* pFontString, ...)
{
	Assert(pFontString != NULL);

	va_list argptr;
	va_start(argptr, pFontString);
	wchar_t	string[512];
	vswprintf(string, lengthof(string), pFontString, argptr);
	va_end(argptr);

	INT32 destx = x;
	INT32 desty = y;

	SGPVSurface::Lock l(FontDestBuffer);
	UINT16* const pDestBuf         = l.Buffer<UINT16>();
	UINT32  const uiDestPitchBYTES = l.Pitch();

	Font const font = FontDefault;
	for (const wchar_t* curletter = string; *curletter != L'\0'; curletter++)
	{
		wchar_t transletter = GetIndex(*curletter);
		Blt8BPPDataTo16BPPBufferTransparentClip(pDestBuf, uiDestPitchBYTES, font, destx, desty, transletter, &FontDestRegion);
		destx += GetWidth(font, transletter);
	}

	return 0;
}


static void mprint_buffer(UINT16* const pDestBuf, UINT32 const uiDestPitchBYTES, INT32 x, INT32 const y, wchar_t const* str)
{
	Font const font = FontDefault;
	for (; *str != L'\0'; ++str)
	{
		wchar_t const glyph = GetIndex(*str);
		Blt8BPPDataTo16BPPBufferMonoShadowClip(pDestBuf, uiDestPitchBYTES, font, x, y, glyph, &FontDestRegion, FontForeground16, FontBackground16, FontShadow16);
		x += GetWidth(font, glyph);
	}
}


void MPrint(INT32 const x, INT32 const y, wchar_t const* const str)
{
	SGPVSurface::Lock l(FontDestBuffer);
	mprint_buffer(l.Buffer<UINT16>(), l.Pitch(), x, y, str);
}


/* Prints to the currently selected destination buffer, at the X/Y coordinates
 * specified, using the currently selected font. Other than the X/Y coordinates,
 * the parameters are identical to printf. The resulting string may be no longer
 * than 512 word-characters. Uses monochrome font color settings */
void mprintf(INT32 const x, INT32 const y, wchar_t const* const fmt, ...)
{
	wchar_t str[512];
	va_list ap;
	va_start(ap, fmt);
	vswprintf(str, lengthof(str), fmt, ap);
	va_end(ap);
	MPrint(x, y, str);
}


void mprintf_buffer(UINT16* const pDestBuf, UINT32 const uiDestPitchBYTES, INT32 const x, INT32 const y, wchar_t const* const fmt, ...)
{
	wchar_t str[512];
	va_list ap;
	va_start(ap, fmt);
	vswprintf(str, lengthof(str), fmt, ap);
	va_end(ap);
	mprint_buffer(pDestBuf, uiDestPitchBYTES, x, y, str);
}


static UINT32 vmprintf_buffer_coded(UINT16* const pDestBuf, const UINT32 uiDestPitchBYTES, const INT32 x, const INT32 y, const wchar_t* const pFontString, va_list ArgPtr)
{
	Assert(pFontString != NULL);

	wchar_t	string[512];
	vswprintf(string, lengthof(string), pFontString, ArgPtr);

	INT32 destx = x;
	INT32 desty = y;

	UINT16 usOldForeColor = FontForeground16;

	Font const font = FontDefault;
	for (const wchar_t* curletter = string; *curletter != 0; curletter++)
	{
		if (*curletter == 180)
		{
			curletter++;
			SetFontForeground(*curletter);
			curletter++;
		}
		else if (*curletter == 181)
		{
			FontForeground16 = usOldForeColor;
			curletter++;
		}

		wchar_t transletter = GetIndex(*curletter);
		Blt8BPPDataTo16BPPBufferMonoShadowClip(pDestBuf, uiDestPitchBYTES, font, destx, desty, transletter, &FontDestRegion, FontForeground16, FontBackground16, FontShadow16);
		destx += GetWidth(font, transletter);
	}

	return 0;
}


void mprintf_buffer_coded(UINT16* const pDestBuf, const UINT32 uiDestPitchBYTES, const INT32 x, const INT32 y, const wchar_t* const pFontString, ...)
{
	va_list ArgPtr;
	va_start(ArgPtr, pFontString);
	vmprintf_buffer_coded(pDestBuf, uiDestPitchBYTES, x, y, pFontString, ArgPtr);
	va_end(ArgPtr);
}


void mprintf_coded(INT32 x, INT32 y, const wchar_t* pFontString, ...)
{
	SGPVSurface::Lock l(FontDestBuffer);
	UINT16* const pDestBuf         = l.Buffer<UINT16>();
	UINT32        uiDestPitchBYTES = l.Pitch();

	va_list ArgPtr;
	va_start(ArgPtr, pFontString);
	vmprintf_buffer_coded(pDestBuf, uiDestPitchBYTES, x, y, pFontString, ArgPtr);
	va_end(ArgPtr);
}


void InitializeFontManager(void)
{
	FontDefault    = 0;
	FontDestBuffer = BACKBUFFER;

	FontDestRegion.iLeft   = 0;
	FontDestRegion.iTop    = 0;
	FontDestRegion.iRight  = SCREEN_WIDTH;
	FontDestRegion.iBottom = SCREEN_HEIGHT;
}
