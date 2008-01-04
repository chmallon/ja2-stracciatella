#ifndef _INTERFACE_H
#define _INTERFACE_H

#include "Handle_UI.h"
#include "MouseSystem.h"
#include "Structure.h"

#define					MAX_UICOMPOSITES	4

#define	INTERFACE_START_Y     (SCREEN_HEIGHT - 120)
#define	INV_INTERFACE_START_Y (SCREEN_HEIGHT - 140)

#define	INTERFACE_START_X					0

// FLASH PORTRAIT CODES
#define		FLASH_PORTRAIT_STOP				0
#define		FLASH_PORTRAIT_START			1
#define		FLASH_PORTRAIT_WAITING		2
#define		FLASH_PORTRAIT_DELAY			150

// FLASH PORTRAIT PALETTE IDS
#define		FLASH_PORTRAIT_NOSHADE		0
#define		FLASH_PORTRAIT_STARTSHADE	1
#define		FLASH_PORTRAIT_ENDSHADE		2
#define		FLASH_PORTRAIT_DARKSHADE	3
#define		FLASH_PORTRAIT_GRAYSHADE	4
#define		FLASH_PORTRAIT_LITESHADE	5


// GLOBAL DEFINES FOR SOME UI FLAGS
#define		ARROWS_HIDE_UP							0x00000002
#define		ARROWS_HIDE_DOWN						0x00000004
#define		ARROWS_SHOW_UP_BESIDE				0x00000008
#define		ARROWS_SHOW_DOWN_BESIDE			0x00000020
#define		ARROWS_SHOW_UP_ABOVE_Y			0x00000040
#define		ARROWS_SHOW_DOWN_BELOW_Y		0x00000080
#define		ARROWS_SHOW_DOWN_BELOW_G		0x00000200
#define		ARROWS_SHOW_DOWN_BELOW_YG		0x00000400
#define		ARROWS_SHOW_DOWN_BELOW_GG		0x00000800
#define		ARROWS_SHOW_UP_ABOVE_G			0x00002000
#define		ARROWS_SHOW_UP_ABOVE_YG			0x00004000
#define		ARROWS_SHOW_UP_ABOVE_GG			0x00008000
#define		ARROWS_SHOW_UP_ABOVE_YY			0x00020000
#define		ARROWS_SHOW_DOWN_BELOW_YY		0x00040000
#define		ARROWS_SHOW_UP_ABOVE_CLIMB	0x00080000
#define		ARROWS_SHOW_UP_ABOVE_CLIMB2	0x00400000
#define		ARROWS_SHOW_UP_ABOVE_CLIMB3	0x00800000
#define		ARROWS_SHOW_DOWN_CLIMB			0x02000000

#define		ROOF_LEVEL_HEIGHT						50

#define			LOCATEANDSELECT_MERC			1
#define			LOCATE_MERC_ONCE					2


// Interface level enums
enum
{
	I_GROUND_LEVEL,
	I_ROOF_LEVEL,
	I_NUMLEVELS
};


BOOLEAN				gfSwitchPanel;
BOOLEAN				gfUIStanceDifferent;
UINT8					gbNewPanel;
INT16					gsCurInterfacePanel;


extern SGPVObject* guiCLOSE;
extern SGPVObject* guiDEAD;
extern SGPVObject* guiHATCH;
extern SGPVObject* guiGUNSM;
extern SGPVObject* guiP1ITEMS;
extern SGPVObject* guiP2ITEMS;
extern SGPVObject* guiP3ITEMS;
extern SGPVObject* guiCOMPANEL;
extern SGPVObject* guiCOMPANELB;
extern SGPVObject* guiRADIO;
extern SGPVObject* guiPORTRAITICONS;
extern SGPVObject* guiBURSTACCUM;


MOUSE_REGION		gViewportRegion;
MOUSE_REGION		gRadarRegion;

#define				MOVEMENT_MENU_LOOK			1
#define				MOVEMENT_MENU_ACTIONC		2
#define				MOVEMENT_MENU_HAND			3
#define				MOVEMENT_MENU_TALK			4
#define				MOVEMENT_MENU_RUN				5
#define				MOVEMENT_MENU_WALK			6
#define				MOVEMENT_MENU_SWAT			7
#define				MOVEMENT_MENU_PRONE			8


#define				DIRTYLEVEL0							0
#define				DIRTYLEVEL1							1
#define				DIRTYLEVEL2							2


typedef enum
{
	SM_PANEL,
	TEAM_PANEL,
	NUM_UI_PANELS

} InterfacePanelDefines;


BOOLEAN InitializeTacticalInterface(void);
BOOLEAN	fInterfacePanelDirty;
BOOLEAN gfPausedTacticalRenderFlags;
BOOLEAN gfPausedTacticalRenderInterfaceFlags;
INT16		gsInterfaceLevel;
BOOLEAN	gfInMovementMenu;


void PopupMovementMenu( UI_EVENT *pUIEvent );
void PopDownMovementMenu(void);
void RenderMovementMenu(void);
void CancelMovementMenu(void);


void PopDownOpenDoorMenu(void);
void RenderOpenDoorMenu(void);
BOOLEAN InitDoorOpenMenu(SOLDIERTYPE* pSoldier, BOOLEAN fClosingDoor);
BOOLEAN HandleOpenDoorMenu(void);
void CancelOpenDoorMenu(void);

void HandleInterfaceBackgrounds(void);


void DrawSelectedUIAboveGuy(SOLDIERTYPE* s);

void CreateCurrentTacticalPanelButtons(void);
void RemoveCurrentTacticalPanelButtons(void);
void SetCurrentTacticalPanelCurrentMerc(SOLDIERTYPE* s);
void SetCurrentInterfacePanel( UINT8 ubNewPanel );
BOOLEAN IsMercPortraitVisible(const SOLDIERTYPE* s);

BOOLEAN InitializeCurrentPanel(void);
void ShutdownCurrentPanel(void);

void ClearInterface(void);
void RestoreInterface(void);

void RenderArrows(void);
void EraseRenderArrows(void);

void EndDeadlockMsg(void);

void DirtyMercPanelInterface( SOLDIERTYPE *pSoldier, UINT8 ubDirtyLevel );


void EndUIMessage(void);
void BeginUIMessage(BOOLEAN fUseSkullIcon, const wchar_t* text);


// map screen version, for centering over the map area
void BeginMapUIMessage(INT16 delta_y, const wchar_t* text);


extern VIDEO_OVERLAY* g_ui_message_overlay;
UINT32				guiUIMessageTime;

enum
{
	NO_MESSAGE,
	COMPUTER_TURN_MESSAGE,
	COMPUTER_INTERRUPT_MESSAGE,
	PLAYER_INTERRUPT_MESSAGE,
	MILITIA_INTERRUPT_MESSAGE,
	AIR_RAID_TURN_MESSAGE,
	PLAYER_TURN_MESSAGE,

} MESSAGE_TYPES;

void HandleTopMessages(void);
BOOLEAN AddTopMessage( UINT8 ubType, const wchar_t *pzString );
BOOLEAN InTopMessageBarAnimation(void);
void EndTopMessage(void);

void InitEnemyUIBar( UINT8 ubNumEnemies, UINT8 ubDoneEnemies );

const wchar_t* GetSoldierHealthString(const SOLDIERTYPE* s);


void RenderAimCubeUI(void);

void ResetPhysicsTrajectoryUI(void);
void SetupPhysicsTrajectoryUI(void);
void EndPhysicsTrajectoryUI(void);
void BeginPhysicsTrajectoryUI( INT16 sGridNo, INT8 bLevel, BOOLEAN fBadCTGT );

void InitPlayerUIBar( BOOLEAN fInterrupt );

void ToggleTacticalPanels(void);

void DirtyTopMessage(void);

void BeginMultiPurposeLocator( INT16 sGridNo, INT8 bLevel, BOOLEAN fSlideTo );
void HandleMultiPurposeLocator(void);
void RenderTopmostMultiPurposeLocator(void);

void GetSoldierAboveGuyPositions(const SOLDIERTYPE* s, INT16* psX, INT16* psY, BOOLEAN fRadio);

extern SGPVObject* guiVEHINV;
extern BOOLEAN gfInOpenDoorMenu;
extern UINT32  guiUIMessageTimeDelay;
extern BOOLEAN gfTopMessageDirty;

#endif
