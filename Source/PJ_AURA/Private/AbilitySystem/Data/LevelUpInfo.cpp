// ALL CODE FOR  learning GAS


#include "AbilitySystem/Data/LevelUpInfo.h"

int32 ULevelUpInfo::FindLevelForXp(int32 XP) const
{
	int32 Level = 1;
	bool bSearching = true;
	while (bSearching)
	{
		// start from 0
		// LevelUpInformation[1] = Level 1 info
		// LevelUpInformation[2] = Level 2 info
		if (LevelUpInformation.Num()-1 <= Level) break;
		if (XP >= LevelUpInformation[Level].LevelUpRequirement)
		{
			++Level;
		}
		else
		{
			bSearching = false;
		}

	}
	return Level;
}
