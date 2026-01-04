// ALL CODE FOR  learning GAS

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraPassiveAbility.generated.h"

/**
 * 
 */
UCLASS()
class PJ_AURA_API UAuraPassiveAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()
public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	//用 tag的方法来取消被动
	void ReceiveDeactivate(const FGameplayTag& AbilityTag);
};
