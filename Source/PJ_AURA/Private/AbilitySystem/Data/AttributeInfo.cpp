// ALL CODE FOR wangjunyang learning GAS


#include "AbilitySystem/Data/AttributeInfo.h"

FAuraAttributeInfo UAttributeInfo::FindAttributeInfoForTag(const FGameplayTag& AttributeTag, bool bLogNotFound)
{
	for(const FAuraAttributeInfo& Info :AttributeInformation)
	{
		if(Info.AttributeTag == AttributeTag)
		{
			return Info;
		}
	}
	if(bLogNotFound)
	{
		UE_LOG(LogTemp,Error,TEXT("Can't find Info for AttributeTag[%s] On AttributInfo[%s]"), *AttributeTag.ToString(),*GetNameSafe(this))
	}

	return FAuraAttributeInfo();
}
