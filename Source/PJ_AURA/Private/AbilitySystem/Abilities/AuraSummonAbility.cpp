// ALL CODE FOR  learning GAS


#include "AbilitySystem/Abilities/AuraSummonAbility.h"

#include "Kismet/KismetSystemLibrary.h"

TArray<FVector> UAuraSummonAbility::GetSpawnLocations(const FVector Forward)
{
	TArray<FVector> SpawnLocations;
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	const float DeltaSpread = SpawnSpread / NumMinions;
	//const FVector RightOfSpread = Forward.RotateAngleAxis(SpawnSpread / 2.f, FVector::UpVector);//RotateAngleAxis 是顺时针旋转 
	const FVector LeftOfSpread = Forward.RotateAngleAxis(-SpawnSpread / 2.f, FVector::UpVector);

	for (int32 i = 0; i < NumMinions ;++i)
	{
		const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
		FVector ChosenSpawnLocation = Location + Direction * FMath::FRandRange(MinSpawnDistance, MaxSpawnDistance);

		FHitResult HitResult;
		//竖着找个地面
		GetWorld()->LineTraceSingleByChannel(HitResult, ChosenSpawnLocation + FVector(0.f, 0.f, 400.f), ChosenSpawnLocation - FVector(0.f, 0.f, 400.f), ECollisionChannel::ECC_Visibility);
		if (HitResult.bBlockingHit)
		{
			ChosenSpawnLocation = HitResult.ImpactPoint;
		}
		SpawnLocations.Add(ChosenSpawnLocation);
	}


	//UKismetSystemLibrary::DrawDebugArrow(GetAvatarActorFromActorInfo(), Location, Location + LeftOfSpread * MaxSpawnDistance, 4.f, FLinearColor::Green,3.f);
	//UKismetSystemLibrary::DrawDebugArrow(GetAvatarActorFromActorInfo(), Location, Location + RightOfSpread * MaxSpawnDistance, 4.f, FLinearColor::Gray,3.f);

	return  SpawnLocations;
}

TSubclassOf<APawn> UAuraSummonAbility::GetRandomMinionsClass()
{
	return MinionClasses[FMath::RandRange(0, MinionClasses.Num() - 1)];
}
