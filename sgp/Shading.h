#ifndef _SHADING_H_
#define _SHADING_H_

#include "VObject.h"	// For HVOBJECT_SHADE_TABLES

#ifdef __cplusplus
extern "C" {
#endif

void BuildShadeTable(void);
void BuildIntensityTable(void);
void SetShadeTablePercent( FLOAT uiShadePercent );

extern UINT16	IntensityTable[65536];
extern UINT16	ShadeTable[65536];
extern UINT16	White16BPPPalette[ 256 ];
extern FLOAT   guiShadePercent;
extern FLOAT   guiBrightPercent;

#ifdef __cplusplus
}
#endif

#define DEFAULT_SHADE_LEVEL		4

#endif
