#include "Interface_Panels.h"
#include "Local.h"
#include "HImage.h"
#include "VObject.h"
#include "Interface_Utils.h"
#include "Render_Dirty.h"
#include "Interface.h"
#include "Interface_Control.h"
#include "SysUtil.h"
#include "Faces.h"
#include "Weapons.h"
#include "Overhead.h"
#include "Soldier_Macros.h"
#include "Line.h"
#include "WCheck.h"
#include "Vehicles.h"
#include "JAScreens.h"
#include "Video.h"
#include "VSurface.h"
#include "ScreenIDs.h"


#define			LIFE_BAR_SHADOW							FROMRGB( 108, 12, 12 )
#define			LIFE_BAR										FROMRGB( 200, 0, 0 )
#define			BANDAGE_BAR_SHADOW					FROMRGB( 156, 60, 60 )
#define			BANDAGE_BAR									FROMRGB( 222, 132, 132 )
#define			BLEEDING_BAR_SHADOW					FROMRGB( 128, 128, 60 )
#define			BLEEDING_BAR								FROMRGB( 240,  240, 20 )
#define			CURR_BREATH_BAR_SHADOW			FROMRGB( 8,		12, 118 ) // the MAX max breatth, always at 100%
#define			CURR_BREATH_BAR							FROMRGB( 8,		12, 160 )
#define     CURR_MAX_BREATH							FROMRGB( 0,		0,	0		) // the current max breath, black
#define     CURR_MAX_BREATH_SHADOW			FROMRGB( 0,		0,	0		)
#define			MORALE_BAR_SHADOW						FROMRGB( 8,		112, 12 )
#define			MORALE_BAR									FROMRGB( 8,		180, 12 )
#define			BREATH_BAR_SHADOW						FROMRGB( 60,	108, 108 ) // the lt blue current breath
#define			BREATH_BAR									FROMRGB( 113,	178, 218 )
#define			BREATH_BAR_SHAD_BACK				FROMRGB( 1,1,1 )
#define			FACE_WIDTH									48
#define			FACE_HEIGHT									43


// car portraits
enum{
	ELDORADO_PORTRAIT =0,
	HUMMER_PORTRAIT,
	ICE_CREAM_TRUCT_PORTRAIT,
	JEEP_PORTRAIT,
	NUMBER_CAR_PORTRAITS,
};

// the ids for the car portraits
static SGPVObject* giCarPortraits[4];

// the car portrait file names
const char *pbCarPortraitFileNames[] = {
	"INTERFACE/eldorado.sti",
	"INTERFACE/Hummer.sti",
	"INTERFACE/ice Cream Truck.sti",
	"INTERFACE/Jeep.sti",
};


// load int he portraits for the car faces that will be use in mapscreen
BOOLEAN LoadCarPortraitValues( void )
try
{
	INT32 iCounter = 0;

	if (giCarPortraits[0] != NO_VOBJECT) return FALSE;
	for( iCounter = 0; iCounter < NUMBER_CAR_PORTRAITS; iCounter++ )
	{
		giCarPortraits[iCounter] = AddVideoObjectFromFile(pbCarPortraitFileNames[iCounter]);
	}
	return( TRUE );
}
catch (...) { return FALSE; }


// get rid of the images we loaded for the mapscreen car portraits
void UnLoadCarPortraits( void )
{
	INT32 iCounter = 0;

	// car protraits loaded?
	if (giCarPortraits[0] == NO_VOBJECT) return;
	for( iCounter = 0; iCounter < NUMBER_CAR_PORTRAITS; iCounter++ )
	{
		DeleteVideoObject(giCarPortraits[iCounter]);
		giCarPortraits[iCounter] = NULL;
	}
}


static void DrawBar(UINT32 XPos, UINT32 YPos, UINT32 Height, UINT16 Color, UINT16 ShadowColor, UINT8* DestBuf)
{
	LineDraw(TRUE, XPos + 0, YPos, XPos + 0, YPos - Height, ShadowColor, DestBuf);
	LineDraw(TRUE, XPos + 1, YPos, XPos + 1, YPos - Height, Color,       DestBuf);
	LineDraw(TRUE, XPos + 2, YPos, XPos + 2, YPos - Height, ShadowColor, DestBuf);
}


static void DrawLifeUIBar(const SOLDIERTYPE* pSoldier, UINT32 XPos, UINT32 YPos, UINT32 MaxHeight, UINT8* pDestBuf)
{
	UINT32 Height;

	// FIRST DO MAX LIFE
	Height = MaxHeight * pSoldier->bLife / 100;
	DrawBar(XPos, YPos, Height, Get16BPPColor(LIFE_BAR), Get16BPPColor(LIFE_BAR_SHADOW), pDestBuf);

	// NOW DO BANDAGE
	// Calculate bandage
	UINT32 Bandage = pSoldier->bLifeMax - pSoldier->bLife - pSoldier->bBleeding;
	if (Bandage != 0)
	{
		YPos   -= Height;
		Height  = MaxHeight * Bandage / 100;
		DrawBar(XPos, YPos, Height, Get16BPPColor(BANDAGE_BAR), Get16BPPColor(BANDAGE_BAR_SHADOW), pDestBuf);
	}

	// NOW DO BLEEDING
	if (pSoldier->bBleeding != 0)
	{
		YPos   -= Height;
		Height  = MaxHeight * pSoldier->bBleeding / 100;
		DrawBar(XPos, YPos, Height, Get16BPPColor(BLEEDING_BAR), Get16BPPColor(BLEEDING_BAR_SHADOW), pDestBuf);
	}
}


static void DrawBreathUIBar(const SOLDIERTYPE* pSoldier, UINT32 XPos, UINT32 sYPos, UINT32 MaxHeight, UINT8* pDestBuf)
{
	UINT32 Height;

	if (pSoldier->bBreathMax <= 97)
	{
		Height = MaxHeight * (pSoldier->bBreathMax + 3) / 100;
		// the old background colors for breath max diff
		DrawBar(XPos, sYPos, Height, Get16BPPColor(BREATH_BAR_SHAD_BACK), Get16BPPColor(BREATH_BAR_SHAD_BACK), pDestBuf);
	}

	Height = MaxHeight * pSoldier->bBreathMax / 100;
	DrawBar(XPos, sYPos, Height, Get16BPPColor(CURR_MAX_BREATH), Get16BPPColor(CURR_MAX_BREATH_SHADOW), pDestBuf);

	// NOW DO BREATH
	Height = MaxHeight * pSoldier->bBreath / 100;
	DrawBar(XPos, sYPos, Height, Get16BPPColor(CURR_BREATH_BAR), Get16BPPColor(CURR_BREATH_BAR_SHADOW), pDestBuf);
}


static void DrawMoraleUIBar(const SOLDIERTYPE* pSoldier, UINT32 XPos, UINT32 YPos, UINT32 MaxHeight, UINT8* pDestBuf)
{
	UINT32 Height = MaxHeight * pSoldier->bMorale / 100;
	DrawBar(XPos, YPos, Height, Get16BPPColor(MORALE_BAR), Get16BPPColor(MORALE_BAR_SHADOW), pDestBuf);
}


void DrawSoldierUIBars(const SOLDIERTYPE* const pSoldier, const INT16 sXPos, const INT16 sYPos, const BOOLEAN fErase, SGPVSurface* const uiBuffer)
{
	const UINT32 BarWidth  =  3;
	const UINT32 BarHeight = 42;
	const UINT32 BreathOff =  6;
	const UINT32 MoraleOff = 12;

	// Erase what was there
	if (fErase)
	{
		RestoreExternBackgroundRect(sXPos, sYPos - BarHeight, MoraleOff + BarWidth, BarHeight + 1);
	}

	if (pSoldier->bLife == 0) return;

	if (!(pSoldier->uiStatusFlags & SOLDIER_ROBOT))
	{
		// DO MAX BREATH
		// brown guy
		UINT16 Region;
		if (guiCurrentScreen != MAP_SCREEN &&
				GetSelectedMan() == pSoldier &&
				gTacticalStatus.ubCurrentTeam == OUR_TEAM &&
				OK_INTERRUPT_MERC(pSoldier))
		{
			Region = 1; // gold, the second entry in the .sti
		}
		else
		{
			Region = 0; // brown, first entry
		}
		BltVideoObject(uiBuffer, guiBrownBackgroundForTeamPanel, Region, sXPos + BreathOff, sYPos - BarHeight);
	}

	SGPVSurface::Lock l(uiBuffer);
	SetClippingRegionAndImageWidth(l.Pitch(), 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	UINT8* const pDestBuf = l.Buffer<UINT8>();

	DrawLifeUIBar(pSoldier, sXPos, sYPos, BarHeight, pDestBuf);
	if (!(pSoldier->uiStatusFlags & SOLDIER_ROBOT))
	{
		DrawBreathUIBar(pSoldier, sXPos + BreathOff, sYPos, BarHeight, pDestBuf);
		if (!(pSoldier->uiStatusFlags & SOLDIER_VEHICLE))
		{
			DrawMoraleUIBar(pSoldier, sXPos + MoraleOff, sYPos, BarHeight, pDestBuf);
		}
	}
}


void DrawItemUIBarEx(const OBJECTTYPE* const o, const UINT8 ubStatus, const INT16 x, const INT16 y, INT16 max_h, const INT16 sColor1, const INT16 sColor2, SGPVSurface* const uiBuffer)
{
	INT16 value;
	// Adjust for ammo, other things
	const INVTYPE* const item = &Item[o->usItem];
	if (ubStatus >= DRAW_ITEM_STATUS_ATTACHMENT1)
	{
		value = o->bAttachStatus[ubStatus - DRAW_ITEM_STATUS_ATTACHMENT1];
	}
	else if (item->usItemClass & IC_AMMO)
	{
		value = 100 * o->ubShotsLeft[ubStatus] / Magazine[item->ubClassIndex].ubMagSize;
		if (value > 100) value = 100;
	}
	else if (item->usItemClass & IC_KEY)
	{
		value = 100;
	}
	else
	{
		value = o->bStatus[ubStatus];
	}

	{ SGPVSurface::Lock l(uiBuffer);
		SetClippingRegionAndImageWidth(l.Pitch(), 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		UINT8* const pDestBuf = l.Buffer<UINT8>();

		--max_h; // LineDraw() includes the end point
		const INT h = max_h * value / 100;
		LineDraw(TRUE, x,     y, x,     y - h, sColor1, pDestBuf);
		LineDraw(TRUE, x + 1, y, x + 1, y - h, sColor2, pDestBuf);
	}

	if (uiBuffer == guiSAVEBUFFER)
	{
		RestoreExternBackgroundRect(x, y - max_h, 2, max_h + 1);
	}
	else
	{
		InvalidateRegion(x, y - max_h, x + 2, y + 1);
	}
}


void RenderSoldierFace(const SOLDIERTYPE* const s, const INT16 sFaceX, const INT16 sFaceY)
{
	if (!s->bActive)
	{
		// Put close panel there
		BltVideoObject(guiSAVEBUFFER, guiCLOSE, 5, sFaceX, sFaceY);
		RestoreExternBackgroundRect(sFaceX, sFaceY, FACE_WIDTH, FACE_HEIGHT);
	}
	else if (s->uiStatusFlags & SOLDIER_VEHICLE)
	{
		// just draw the vehicle
		const UINT8 vehicle_type = pVehicleList[s->bVehicleID].ubVehicleType;
		BltVideoObject(guiSAVEBUFFER, giCarPortraits[vehicle_type], 0, sFaceX, sFaceY);
		RestoreExternBackgroundRect(sFaceX, sFaceY, FACE_WIDTH, FACE_HEIGHT);
	}
	else if (s->face->uiFlags & FACE_INACTIVE_HANDLED_ELSEWHERE) // OK, check if this face actually went active
	{
		ExternRenderFaceFromSoldier(guiSAVEBUFFER, s, sFaceX, sFaceY);
	}
	else
	{
		SetAutoFaceActiveFromSoldier(FRAME_BUFFER, guiSAVEBUFFER, s, sFaceX, sFaceY);
		RenderAutoFaceFromSoldier(s);
	}
}
