// ALL CODE FOR  learning GAS


#include "AbilitySystem/Abilities/AuraFireBlast.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Actor/AuraFireBall.h"

TArray<AAuraFireBall*> UAuraFireBlast::SpawnFireBalls()
{
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
	TArray<AAuraFireBall*> FireBalls;


	if (!bIsServer) return FireBalls;

	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	TArray<FRotator> Rotators = UAuraAbilitySystemLibrary::EvenlySpacedRotators(Forward, FVector::UpVector, 360.f, NumFireBalls);
	for (const FRotator& Rot : Rotators)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(Location);
		SpawnTransform.SetRotation(Rot.Quaternion());
		AAuraFireBall* FireBall = GetWorld()->SpawnActorDeferred<AAuraFireBall>(
			FireBallClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			CurrentActorInfo->PlayerController->GetPawn(),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn
		);
		if (FireBall)
		{
			FireBall->DamageEffectParams = MakeDamageEffectParamFromClassDefaults();
			FireBall->ExplosionDamageParams = MakeDamageEffectParamFromClassDefaults();
			FireBall->SetOwner(GetAvatarActorFromActorInfo());
			FireBall->ReturnToActor = GetAvatarActorFromActorInfo();
			FireBalls.Add(FireBall);
			FireBall->FinishSpawning(SpawnTransform);
		}
	}
	return FireBalls;
}
