// ALL CODE FOR  learning GAS


#include "CheckPoint/MapEntrance.h"

#include "Components/SphereComponent.h"
#include "Game/AuraGameModeBase.h"
#include "Interaction/PlayerInterface.h"
#include "Kismet/GameplayStatics.h"

AMapEntrance::AMapEntrance(const FObjectInitializer& Initializer):Super(Initializer)
{
	Sphere->SetupAttachment(MoveToComponent);
}

void AMapEntrance::LoadActor_Implementation()
{

}

void AMapEntrance::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("AMapEntrance::OnSphereOverlap"));

	if (OtherActor->Implements<UPlayerInterface>())
	{
		IPlayerInterface::Execute_SaveProgress(OtherActor, DestinationPlayStartUpName, DestinationMap.ToSoftObjectPath().GetAssetName());
		//ÇÐ»»µØÍ¼
		UGameplayStatics::OpenLevelBySoftObjectPtr(this,DestinationMap);
	}
}
