// ALL CODE FOR  learning GAS

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "Interaction/HighlightInterface.h"
#include "Interaction/SaveInterface.h"
#include "CheckPoint.generated.h"

class USphereComponent;
/**
 * 
 */
UCLASS()
class PJ_AURA_API ACheckPoint : public APlayerStart,public ISaveInterface,public IHighlightInterface
{
	GENERATED_BODY()
public:
	ACheckPoint(const FObjectInitializer& ObjectInitializer);

	/*Save interface*/
	virtual bool ShouldLoadTransform_Implementation()override {return false;}
	virtual void LoadActor_Implementation() override;
	/*End Save interface*/

	/*highlight interface*/
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;
	/*End highlight interface*/

	UFUNCTION(BlueprintNativeEvent)
	void SetMoveToLocation(FVector& OutDestinationPosition);

protected:
	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
	void CheckPointReached(UMaterialInstanceDynamic* DynamicInstance);

	void HandleGlowEffects();

	UPROPERTY(BlueprintReadOnly, SaveGame)
	bool bReached = false;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> CheckPointMesh;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Sphere;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> MoveToComponent;
};
