// ALL CODE FOR  learning GAS


#include "UI/Widget/HeadShot/SkeletalMeshRenderWidget.h"
#include "UI/Widget/HeadShot/SkeletalMeshRenderActor.h"
#include "GameFramework/Character.h"
#include "Components/SceneCaptureComponent2D.h"
#include "UI/Widget/HeadShot/RenderActorTargetInterface.h"

void USkeletalMeshRenderWidget::NativeConstruct()
{
	Super::NativeConstruct();
	ACharacter* PlayerCharacter = GetOwningPlayerPawn<ACharacter>();
	IRenderActorTargetInterface* PlayerCharacterRenderTargetInterface = Cast<IRenderActorTargetInterface>(PlayerCharacter);
	if (PlayerCharacter && SkeletalMeshRenderActor)
	{
		SkeletalMeshRenderActor->ConfigureSkeletalMesh(PlayerCharacter->GetMesh()->GetSkeletalMeshAsset(), PlayerCharacter->GetMesh()->GetAnimClass());
		USceneCaptureComponent2D* SceneCapture = SkeletalMeshRenderActor->GetCaptureComponent();
		if (PlayerCharacterRenderTargetInterface && SceneCapture)
		{
			SceneCapture->SetRelativeLocation(PlayerCharacterRenderTargetInterface->GetCaptureLocalPosition());
			SceneCapture->SetRelativeRotation(PlayerCharacterRenderTargetInterface->GetCaptureLocalRotation());
		}
	}
}

void USkeletalMeshRenderWidget::SpawnRenderActor()
{
	if (!SKeletalMeshRenderActorClass)
		return;

	UWorld* World = GetWorld();
	if (!World)
		return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	SkeletalMeshRenderActor = World->SpawnActor<ASkeletalMeshRenderActor>(SKeletalMeshRenderActorClass, SpawnParams);
}

ARenderActor* USkeletalMeshRenderWidget::GetRenderActor() const
{
	return SkeletalMeshRenderActor;
}

