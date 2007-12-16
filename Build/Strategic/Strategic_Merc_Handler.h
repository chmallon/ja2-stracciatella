#ifndef _STRATEGIC_MERC_HANDLER_H_
#define _STRATEGIC_MERC_HANDLER_H_


void StrategicHandlePlayerTeamMercDeath( SOLDIERTYPE *pSoldier );
void MercDailyUpdate(void);
void MercsContractIsFinished(SOLDIERTYPE* s);
void RPCWhineAboutNoPay(SOLDIERTYPE* s);
void MercComplainAboutEquipment( UINT8 ubProfileID );
BOOLEAN SoldierHasWorseEquipmentThanUsedTo( SOLDIERTYPE *pSoldier );
void UpdateBuddyAndHatedCounters( void );
void HourlyCamouflageUpdate( void );
#endif
