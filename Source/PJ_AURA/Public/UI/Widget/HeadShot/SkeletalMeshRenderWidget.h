// ALL CODE FOR  learning GAS

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/HeadShot/RenderActorWidget.h"
#include "SkeletalMeshRenderWidget.generated.h"

/**
 * 
 */
UCLASS()
class PJ_AURA_API USkeletalMeshRenderWidget : public URenderActorWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
private:
	virtual void SpawnRenderActor() override;
	virtual ARenderActor* GetRenderActor() const override;
	UPROPERTY(EditDefaultsOnly, Category = "SKeletal Mesh Render")
	TSubclassOf<class ASkeletalMeshRenderActor> SKeletalMeshRenderActorClass;

	UPROPERTY()
	ASkeletalMeshRenderActor* SkeletalMeshRenderActor;
};
