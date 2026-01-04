// ALL CODE FOR  learning GAS

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraProjectileSpell.h"
#include "AuraFirebolt.generated.h"

/**
 * 
 */
UCLASS()
class PJ_AURA_API UAuraFirebolt : public UAuraProjectileSpell
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SpawnProjectiles(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag, bool bOverridePitch, float PitchOverride, AActor* HomingTarget);
protected:
	UPROPERTY(EditDefaultsOnly,Category = "Firebolt")
	float ProjectileSpread = 90.f;

	UPROPERTY(EditDefaultsOnly, Category = "Firebolt")
	float MaxNumProjectiles = 5;

	UPROPERTY(EditDefaultsOnly, Category = "Firebolt")
	float HomingAccelerationMin = 1600.f;

	UPROPERTY(EditDefaultsOnly, Category = "Firebolt")
	float HomingAccelerationMax = 3200.f;

	UPROPERTY(EditDefaultsOnly, Category = "Firebolt")
	bool bLaunchHomingProjectile = true;
};
