// ALL CODE FOR wangjunyang learning GAS

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AuraGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class PJ_AURA_API UAuraGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly,Category="Input")
	FGameplayTag StartUpInputTag;

	UFUNCTION(BlueprintNativeEvent)
	FText MakeAbilityDesc(const FText& InFormat,const int32 Level);

protected:

	UFUNCTION(BlueprintCallable)
	float GetManaCost(float InLevel = 1.f) const;
	UFUNCTION(BlueprintCallable)
	float GetCooldown(float InLevel = 1.f) const;
};
