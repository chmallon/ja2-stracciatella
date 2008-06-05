#include "Debug.h"
#include "FileMan.h"
#include "LoadSaveUndergroundSectorInfo.h"
#include "LoadSaveData.h"


BOOLEAN ExtractUndergroundSectorInfoFromFile(const HWFILE file, UNDERGROUND_SECTORINFO* const u)
try
{
	BYTE data[72];
	FileRead(file, data, sizeof(data));

	const BYTE* d = data;
	EXTR_U32(d, u->uiFlags)
	EXTR_U8(d, u->ubSectorX)
	EXTR_U8(d, u->ubSectorY)
	EXTR_U8(d, u->ubSectorZ)
	EXTR_U8(d, u->ubNumElites)
	EXTR_U8(d, u->ubNumTroops)
	EXTR_U8(d, u->ubNumAdmins)
	EXTR_U8(d, u->ubNumCreatures)
	EXTR_U8(d, u->fVisited)
	EXTR_SKIP(d, 4)
	EXTR_U32(d, u->uiTimeCurrentSectorWasLastLoaded)
	EXTR_PTR(d, u->next)
	EXTR_U8(d, u->ubAdjacentSectors)
	EXTR_U8(d, u->ubCreatureHabitat)
	EXTR_U8(d, u->ubElitesInBattle)
	EXTR_U8(d, u->ubTroopsInBattle)
	EXTR_U8(d, u->ubAdminsInBattle)
	EXTR_U8(d, u->ubCreaturesInBattle)
	EXTR_SKIP(d, 2)
	EXTR_U32(d, u->uiNumberOfWorldItemsInTempFileThatCanBeSeenByPlayer)
	EXTR_SKIP(d, 36)
	Assert(d == endof(data));

	return TRUE;
}
catch (...) { return FALSE; }


BOOLEAN InjectUndergroundSectorInfoIntoFile(const HWFILE file, const UNDERGROUND_SECTORINFO* const u)
{
	BYTE data[72];
	BYTE* d = data;
	INJ_U32(d, u->uiFlags)
	INJ_U8(d, u->ubSectorX)
	INJ_U8(d, u->ubSectorY)
	INJ_U8(d, u->ubSectorZ)
	INJ_U8(d, u->ubNumElites)
	INJ_U8(d, u->ubNumTroops)
	INJ_U8(d, u->ubNumAdmins)
	INJ_U8(d, u->ubNumCreatures)
	INJ_U8(d, u->fVisited)
	INJ_SKIP(d, 4)
	INJ_U32(d, u->uiTimeCurrentSectorWasLastLoaded)
	INJ_PTR(d, u->next)
	INJ_U8(d, u->ubAdjacentSectors)
	INJ_U8(d, u->ubCreatureHabitat)
	INJ_U8(d, u->ubElitesInBattle)
	INJ_U8(d, u->ubTroopsInBattle)
	INJ_U8(d, u->ubAdminsInBattle)
	INJ_U8(d, u->ubCreaturesInBattle)
	INJ_SKIP(d, 2)
	INJ_U32(d, u->uiNumberOfWorldItemsInTempFileThatCanBeSeenByPlayer)
	INJ_SKIP(d, 36)
	Assert(d == endof(data));

	return FileWrite(file, data, sizeof(data));
}
