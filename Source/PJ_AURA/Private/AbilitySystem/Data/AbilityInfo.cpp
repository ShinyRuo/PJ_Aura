// ALL CODE FOR  learning GAS


#include "AbilitySystem/Data/AbilityInfo.h"

#include "PJ_AURA/AuraLogChannels.h"

FAuraAbilityInfo UAbilityInfo::FindAbilityInfoForTag(const FGameplayTag& AbilityTag, bool bLogNotFound) const
{
	for (const FAuraAbilityInfo& Info : AbilityInformation)
	{
		if (Info.AbilityTag == AbilityTag)
		{
			return Info;
		}
	}
	if (bLogNotFound)
	{
		UE_LOG(LogAura, Error, TEXT("Can't find Info for AbilityTag[%s] On AbilityInformation[%s]"), *AbilityTag.ToString(), *GetNameSafe(this))
	}

	return FAuraAbilityInfo();
}
