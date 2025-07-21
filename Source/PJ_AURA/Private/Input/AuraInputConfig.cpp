// ALL CODE FOR wangjunyang learning GAS


#include "Input/AuraInputConfig.h"

const UInputAction* UAuraInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for(const FAuraInputAction& Aia : AbilityInputActions)
	{
		if (Aia.InputAction && Aia.InputTag == InputTag) return Aia.InputAction;
	}
	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find AbilityInputAction For Tag[%s],on InputCpnfig[%s]"), *InputTag.ToString(), *GetNameSafe(this));
	}
	return nullptr;
}
