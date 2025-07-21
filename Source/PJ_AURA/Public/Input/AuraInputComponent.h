// ALL CODE FOR wangjunyang learning GAS

#pragma once

#include "CoreMinimal.h"
#include "AuraInputConfig.h"
#include "EnhancedInputComponent.h"
#include "AuraInputComponent.generated.h"

/**
 * 
 */
UCLASS()
class PJ_AURA_API UAuraInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()
public:
	template<class UserClass,typename PressedFuncType,typename ReleaseFuncType,typename HeldFuncType>
	void BindAbilityActions(const UAuraInputConfig* InputConfig, UserClass* Object, 
		PressedFuncType PressedFunc, ReleaseFuncType ReleaseFunc, HeldFuncType HeldFunc);
};

template <class UserClass, typename PressedFuncType, typename ReleaseFuncType, typename HeldFuncType>
void UAuraInputComponent::BindAbilityActions(const UAuraInputConfig* InputConfig, UserClass* Object,
	PressedFuncType PressedFunc, ReleaseFuncType ReleaseFunc, HeldFuncType HeldFunc)
{
	check(InputConfig);
	for (const FAuraInputAction& Aia : InputConfig->AbilityInputActions)
	{
		if (Aia.InputAction&& Aia.InputTag.IsValid())
		{
			if (PressedFunc)
			{
				BindAction(Aia.InputAction, ETriggerEvent::Started, Object, PressedFunc, Aia.InputTag);
			}
			if (ReleaseFunc)
			{
				BindAction(Aia.InputAction, ETriggerEvent::Completed, Object, ReleaseFunc, Aia.InputTag);
			}
			if (HeldFunc)
			{
				//Triggered every tick -> run HeldFunc every tick
				BindAction(Aia.InputAction, ETriggerEvent::Triggered, Object, HeldFunc, Aia.InputTag);
			}
		}
	}
}
