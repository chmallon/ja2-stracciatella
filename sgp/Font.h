#ifndef FONT_H
#define FONT_H

#include "Types.h"


#define		DEFAULT_SHADOW			2
#define		MILITARY_SHADOW			67
#define		NO_SHADOW						0

#ifdef JA2

// these are bogus! No palette is set yet!
// font foreground color symbols
#define		FONT_FCOLOR_WHITE			208
#define		FONT_FCOLOR_RED				162
#define		FONT_FCOLOR_NICERED		164
#define		FONT_FCOLOR_BLUE			203
#define		FONT_FCOLOR_GREEN			184
#define		FONT_FCOLOR_YELLOW		144
#define		FONT_FCOLOR_BROWN			184
#define		FONT_FCOLOR_ORANGE		76
#define		FONT_FCOLOR_PURPLE		160

// font background color symbols
#define		FONT_BCOLOR_WHITE			208
#define		FONT_BCOLOR_RED				162
#define		FONT_BCOLOR_BLUE			203
#define		FONT_BCOLOR_GREEN			184
#define		FONT_BCOLOR_YELLOW		144
#define		FONT_BCOLOR_BROWN			80
#define		FONT_BCOLOR_ORANGE		76
#define		FONT_BCOLOR_PURPLE		160

#else

// font foreground color symbols
#define		FONT_FCOLOR_WHITE			0x0000
#define		FONT_FCOLOR_RED				0x0000
#define		FONT_FCOLOR_BLUE			0x0000
#define		FONT_FCOLOR_GREEN			0x0000
#define		FONT_FCOLOR_YELLOW		0x0000
#define		FONT_FCOLOR_BROWN			0x0000
#define		FONT_FCOLOR_ORANGE		0x0000
#define		FONT_FCOLOR_PURPLE		0x0000

// font background color symbols
#define		FONT_BCOLOR_WHITE			0x0000
#define		FONT_BCOLOR_RED				0x0000
#define		FONT_BCOLOR_BLUE			0x0000
#define		FONT_BCOLOR_GREEN			0x0000
#define		FONT_BCOLOR_YELLOW		0x0000
#define		FONT_BCOLOR_BROWN			0x0000
#define		FONT_BCOLOR_ORANGE		0x0000
#define		FONT_BCOLOR_PURPLE		0x0000

// font glyphs for spell targeting types
#define		FONT_GLYPH_TARGET_POINT		0xFFF0
#define		FONT_GLYPH_TARGET_CONE		0xFFF1
#define		FONT_GLYPH_TARGET_SINGLE	0xFFF2
#define		FONT_GLYPH_TARGET_GROUP		0xFFF3
#define		FONT_GLYPH_TARGET_NONE		0xFFF4

#endif


#ifdef __cplusplus
extern "C" {
#endif

extern INT32		FontDefault;


void SetFontColors(UINT16 usColors);
void SetFontForeground(UINT8 ubForeground);
void SetFontBackground(UINT8 ubBackground);
void SetFontShadow(UINT8 ubBackground);

extern HVOBJECT	 GetFontObject(INT32 iFont);
extern UINT32		 gprintf(INT32 x, INT32 y, const wchar_t *pFontString, ...);
extern UINT32		 mprintf(INT32 x, INT32 y, const wchar_t *pFontString, ...);
UINT32 mprintf_buffer(UINT16* pDestBuf, UINT32 uiDestPitchBYTES, INT32 x, INT32 y, const wchar_t* pFontString, ...);
UINT32 mprintf_buffer_coded(UINT8* pDestBuf, UINT32 uiDestPitchBYTES, INT32 x, INT32 y, const wchar_t* pFontString, ...);
UINT32 mprintf_coded(INT32 x, INT32 y, const wchar_t* pFontString, ...);

/* Sets the destination buffer for printing to and the clipping rectangle. */
BOOLEAN SetFontDestBuffer(SGPVSurface* dst, INT32 x1, INT32 y1, INT32 x2, INT32 y2);

extern BOOLEAN	 SetFont(INT32 iFontIndex);

extern INT32		 LoadFontFile(const char *pFileName);
extern UINT16    GetFontHeight(INT32 FontNum);
extern BOOLEAN   InitializeFontManager(void);
extern void      ShutdownFontManager(void);
extern void			 UnloadFont(UINT32 FontIndex);

UINT32 GetCharWidth(HVOBJECT Font, wchar_t c);

extern INT16 StringPixLength(const wchar_t *string,INT32 UseFont);
extern void SaveFontSettings(void);
extern void RestoreFontSettings(void);

void FindFontRightCoordinates( INT16 sLeft, INT16 sTop, INT16 sWidth, INT16 sHeight, const wchar_t *pStr, INT32 iFontIndex, INT16 *psNewX, INT16 *psNewY );
void FindFontCenterCoordinates( INT16 sLeft, INT16 sTop, INT16 sWidth, INT16 sHeight, const wchar_t *pStr, INT32 iFontIndex, INT16 *psNewX, INT16 *psNewY );

#ifdef __cplusplus
}
#endif

#endif
