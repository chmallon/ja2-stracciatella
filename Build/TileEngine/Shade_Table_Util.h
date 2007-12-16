#ifndef __SHADE_TABLE_UTIL_H
#define __SHADE_TABLE_UTIL_H

#include "TileDat.h"
#include "VObject.h"

void DetermineRGBDistributionSettings(void);
BOOLEAN LoadShadeTable( HVOBJECT pObj, UINT32 uiTileTypeIndex );
BOOLEAN SaveShadeTable( HVOBJECT pObj, UINT32 uiTileTypeIndex );

extern CHAR8 TileSurfaceFilenames[NUMBEROFTILETYPES][32];
extern BOOLEAN gfForceBuildShadeTables;

BOOLEAN DeleteShadeTableDir(void);


#endif
