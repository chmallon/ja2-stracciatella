#ifndef __SYS_GLOBALS_H
#define __SYS_GLOBALS_H

#define				SHOW_MIN_FPS			0
#define				SHOW_FULL_FPS			1

extern char    gubErrorText[200];
extern BOOLEAN gfAniEditMode;
extern BOOLEAN gfEditMode;
extern BOOLEAN fFirstTimeInGameScreen;
extern INT8    gbFPSDisplay;
extern BOOLEAN gfGlobalError;

extern UINT32	guiGameCycleCounter;

extern				BOOLEAN  SET_ERROR( const char *String, ...);

#ifdef JA2EDITOR
extern char g_filename[200];
#endif

#endif
