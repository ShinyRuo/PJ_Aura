// ALL CODE FOR  learning GAS


#include "Actor/PointCollection.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
APointCollection::APointCollection()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
    if (PointsNum > 0)
    {
		for (int32 i = 0; i < PointsNum; ++i)
		{
			ArrPoints.Add(CreateDefaultSubobject<USceneComponent>(FName(*FString::Printf(TEXT("Pt_%d"), i))));
		}
		SetRootComponent(ArrPoints[0]);
		for (int32 i = 1; i < PointsNum; ++i)
		{
			ArrPoints[i]->SetupAttachment(GetRootComponent());
		}
    }
	

}

TArray<USceneComponent*> APointCollection::GetGroundPoint(const FVector& GroundLocation, int32 NumPoints,
	float YawOverride)
{
	checkf(ArrPoints.Num() >= NumPoints, TEXT("Attempted to access ArrPoints out of bounds."));

	TArray<USceneComponent*> ArrayCopy;
	if (ArrPoints.IsEmpty()) return ArrayCopy;
	if (USceneComponent* Pt_0 = ArrPoints[0].Get()) //pt0为旋转中心点
	{
		for (TObjectPtr<USceneComponent>& TPt : ArrPoints)
		{
			USceneComponent* Pt = TPt.Get();
			if (ArrayCopy.Num() > NumPoints)
			{
				return ArrayCopy;
			}
			if (Pt != Pt_0)
			{
				FVector ToPoint = Pt->GetComponentLocation() - Pt_0->GetComponentLocation();
				ToPoint = ToPoint.RotateAngleAxis(YawOverride, FVector::UpVector);
				Pt->SetWorldLocation(Pt_0->GetComponentLocation() + ToPoint);
			}
			const FVector RaisedLocation = FVector(Pt->GetComponentLocation().X, Pt->GetComponentLocation().Y, Pt->GetComponentLocation().Z + 500.f);
			const FVector LowestLocation = FVector(Pt->GetComponentLocation().X, Pt->GetComponentLocation().Y, Pt->GetComponentLocation().Z - 500.f);

			FHitResult HitResult;
			TArray<AActor*> IgnoreActors;
			UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(this, IgnoreActors, TArray<AActor*>(), 1500.f, GetActorLocation());

			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActors(IgnoreActors);
			GetWorld()->LineTraceSingleByProfile(HitResult, RaisedLocation, LowestLocation, FName("BlockAll"),QueryParams);

			const FVector AdjustedLocation = FVector(Pt->GetComponentLocation().X, Pt->GetComponentLocation().Y, HitResult.ImpactPoint.Z);
			Pt->SetWorldLocation(AdjustedLocation);
			Pt->SetWorldRotation(UKismetMathLibrary::MakeRotFromZ(HitResult.ImpactNormal)); //比如 射线垂直向下检测到一个斜面 那么就旋转碎片到垂直斜面

			ArrayCopy.Add(Pt);
		}
	}
	return ArrayCopy;
}

// Called when the game starts or when spawned
void APointCollection::BeginPlay()
{
	Super::BeginPlay();
	
}


