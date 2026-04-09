// ALL CODE FOR  learning GAS

#pragma once

#include "CoreMinimal.h"
#include "CheckPoint/CheckPoint.h"
#include "MapEntrance.generated.h"

/**
 * 
 */
UCLASS()
class PJ_AURA_API AMapEntrance : public ACheckPoint
{
	GENERATED_BODY()
public:
	explicit  AMapEntrance(const FObjectInitializer& Initializer);
	virtual void LoadActor_Implementation() override;

protected:
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UWorld> DestinationMap;

	UPROPERTY(EditAnywhere)
	FName DestinationPlayStartUpName;

	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
