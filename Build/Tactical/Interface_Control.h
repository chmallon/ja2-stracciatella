#ifndef __INTERFACE_CONTROL_H
#define __INTERFACE_CONTROL_H

#include "Types.h"

#define		INTERFACE_MAPSCREEN							0x00000001
#define		INTERFACE_NORENDERBUTTONS				0x00000002
#define		INTERFACE_SHOPKEEP_INTERFACE		0x00000008

extern UINT32 guiTacticalInterfaceFlags;


void SetTacticalInterfaceFlags( UINT32 uiFlags );

void SetUpInterface(void);
void ResetInterface(void);
void RenderTopmostTacticalInterface(void);
void RenderTacticalInterface(void);

void RenderTacticalInterfaceWhileScrolling(void);

void EraseInterfaceMenus( BOOLEAN fIgnoreUIUnLock );

void ResetInterfaceAndUI(void);

BOOLEAN AreWeInAUIMenu(void);

void HandleTacticalPanelSwitch(void);

BOOLEAN InterfaceOKForMeanwhilePopup(void);

extern BOOLEAN gfRerenderInterfaceFromHelpText;

#endif
