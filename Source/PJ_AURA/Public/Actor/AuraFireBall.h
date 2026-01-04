// ALL CODE FOR  learning GAS

#pragma once

#include "CoreMinimal.h"
#include "Actor/AuraProjectile.h"
#include "AuraFireBall.generated.h"

/**
 * 
 */
UCLASS()
class PJ_AURA_API AAuraFireBall : public AAuraProjectile
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent)
	void StartOutgoingTimeline();
	TWeakObjectPtr<AActor> ReturnToActor;

	UFUNCTION(BlueprintCallable)
	AActor* GetReturnActor() const;
	UPROPERTY(BlueprintReadWrite)
	FDamageEffectParams ExplosionDamageParams;
protected:
	virtual void BeginPlay() override;
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnHit() override;

};
