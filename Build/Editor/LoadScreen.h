#ifdef JA2EDITOR

#ifndef _JA2_LOAD_SCREEN_
#define _JA2_LOAD_SCREEN_

#include "ScreenIDs.h"


typedef struct FDLG_LIST FDLG_LIST;
struct FDLG_LIST
{
	char       filename[260];
	FDLG_LIST* pNext;
	FDLG_LIST* pPrev;
};

FDLG_LIST* AddToFDlgList(FDLG_LIST* pList, const char* filename);


BOOLEAN ExternalLoadMap(const wchar_t* szFilename);
BOOLEAN ExternalSaveMap(const wchar_t* szFilename);

void SetErrorCatchString(wchar_t const* fmt, ...);

void     LoadSaveScreenInit(void);
ScreenID LoadSaveScreenHandle(void);

#endif
#endif
