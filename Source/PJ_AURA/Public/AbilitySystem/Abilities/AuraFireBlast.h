// ALL CODE FOR  learning GAS

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AuraFireBlast.generated.h"

class AAuraFireBall;
/**
 * 
 */
UCLASS()
class PJ_AURA_API UAuraFireBlast : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	TArray<AAuraFireBall*> SpawnFireBalls();

protected:
	UPROPERTY(EditDefaultsOnly,Category="FireBlast")
	int32 NumFireBalls = 12;

private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AAuraFireBall> FireBallClass;
};
