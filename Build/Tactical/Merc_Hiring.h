#ifndef _MERC_HIRING_H_
#define _MERC_HIRING_H_

#include "Soldier_Control.h"


//
// Used with the HireMerc function
//
#define		MERC_HIRE_OVER_20_MERCS_HIRED		-1
#define		MERC_HIRE_FAILED								0
#define		MERC_HIRE_OK										1

#define			MERC_ARRIVE_TIME_SLOT_1		( 7 * 60 + 30 )		// 7:30 a.m.
#define			MERC_ARRIVE_TIME_SLOT_2		( 13 * 60 + 30 )	// 1:30 pm
#define			MERC_ARRIVE_TIME_SLOT_3		( 19 * 60 + 30 )	// 7:30 pm


// ATE: This define has been moved to be a function so that
// we pick the most appropriate time of day to use...
//#define		MERC_ARRIVAL_TIME_OF_DAY				 (7 * 60 + 30)		// 7:30 am


typedef struct
{
	UINT8		ubProfileID;
	INT16		sSectorX;
	INT16		sSectorY;
	INT8		bSectorZ;
	INT16		iTotalContractLength;
	BOOLEAN fCopyProfileItemsOver;
	UINT32	uiTimeTillMercArrives;
	UINT8		ubInsertionCode;
	UINT16	usInsertionData;
	BOOLEAN	fUseLandingZoneForArrival;

} MERC_HIRE_STRUCT;

// ATE: Globals that dictate where the mercs will land once being hired
extern INT16	gsMercArriveSectorX;
extern INT16	gsMercArriveSectorY;


INT8		HireMerc( MERC_HIRE_STRUCT *pHireMerc);
void    MercArrivesCallback(SOLDIERTYPE* s);
BOOLEAN IsMercHireable( UINT8 ubMercID );
BOOLEAN IsMercDead( UINT8 ubMercID );
BOOLEAN IsTheSoldierAliveAndConcious( 	SOLDIERTYPE		*pSoldier );
void		HandleMercArrivesQuotes( SOLDIERTYPE *pSoldier );
void		UpdateAnyInTransitMercsWithGlobalArrivalSector( );


UINT32	GetMercArrivalTimeOfDay( );

#endif
