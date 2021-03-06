#ifdef JA2EDITOR
#ifndef __SECTOR_SUMMARY_H
#define __SECTOR_SUMMARY_H

#include "Input.h"


void CreateSummaryWindow(void);
void DestroySummaryWindow(void);
void RenderSummaryWindow(void);
void LoadWorldInfo(void);

void UpdateSectorSummary(const wchar_t* gszFilename, BOOLEAN fUpdate);

void SaveGlobalSummary(void);

extern BOOLEAN gfGlobalSummaryExists;

extern BOOLEAN gfSummaryWindowActive;

extern BOOLEAN gSectorExists[16][16];

extern UINT16 gusNumEntriesWithOutdatedOrNoSummaryInfo;

void AutoLoadMap(void);

BOOLEAN HandleSummaryInput(InputAtom*);

#endif
#endif
