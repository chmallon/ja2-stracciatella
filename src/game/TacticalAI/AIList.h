#ifndef AILIST_H
#define AILIST_H

#include "JA2Types.h"


SOLDIERTYPE* RemoveFirstAIListEntry(void);
extern BOOLEAN BuildAIListForTeam( INT8 bTeam );
BOOLEAN MoveToFrontOfAIList(SOLDIERTYPE* s);

#endif
