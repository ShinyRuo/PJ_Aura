// ALL CODE FOR  learning GAS

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PointCollection.generated.h"

UCLASS()
class PJ_AURA_API APointCollection : public AActor
{
	GENERATED_BODY()

public:
	APointCollection();

	UFUNCTION(BlueprintPure)
	TArray<USceneComponent*> GetGroundPoint(const FVector& GroundLocation, int32 NumPoints, float YawOverride = 0.f);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly,VisibleAnywhere)
	TArray<TObjectPtr<USceneComponent>> ArrPoints;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	int32 PointsNum = 10;
};
