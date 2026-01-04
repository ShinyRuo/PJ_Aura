// ALL CODE FOR  learning GAS


#include "AbilitySystem/Abilities/AuraFirebolt.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Actor/AuraProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/KismetSystemLibrary.h"

void UAuraFirebolt::SpawnProjectiles(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag,
                                     bool bOverridePitch, float PitchOverride, AActor* HomingTarget)
{
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bIsServer) return;

	const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(), SocketTag);
	FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
	if (bOverridePitch)Rotation.Pitch = PitchOverride;

	const FVector Forward = Rotation.Vector();
	const int32 NumProjectiles = FMath::Min(MaxNumProjectiles, GetAbilityLevel());

	TArray<FRotator> Rotations = UAuraAbilitySystemLibrary::EvenlySpacedRotators(Forward, FVector::UpVector, ProjectileSpread, NumProjectiles);

	FTransform SpawnTransform;
	const bool bTargetingCombatActor = HomingTarget && HomingTarget->Implements<UCombatInterface>();
	USceneComponent* HomingTargetComponent = nullptr;
	for (const FRotator& Rot : Rotations)
	{
		SpawnTransform.SetLocation(SocketLocation);
		SpawnTransform.SetRotation(Rot.Quaternion());
		AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
			ProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetAvatarActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		Projectile->ProjectileMovement->bIsHomingProjectile = bLaunchHomingProjectile;

		if (bLaunchHomingProjectile)
		{
			if (!IsValid(HomingTargetComponent))
			{
				if (bTargetingCombatActor)
				{
					HomingTargetComponent = HomingTarget->GetRootComponent();
				}
				else
				{
					HomingTargetComponent = NewObject<USceneComponent>(USceneComponent::StaticClass());
					HomingTargetComponent->SetWorldLocation(ProjectileTargetLocation);
				}
			}

			//client需要同步 ProjectileMovement->HomingTargetComponent 的话
			//  Projectile 需要设置rep 还要设置rep movement
			Projectile->ProjectileMovement->HomingTargetComponent = HomingTargetComponent;

			Projectile->ProjectileMovement->HomingAccelerationMagnitude = FMath::RandRange(HomingAccelerationMin, HomingAccelerationMax);

		}

		Projectile->DamageEffectParams = MakeDamageEffectParamFromClassDefaults();
		
		
		Projectile->FinishSpawning(SpawnTransform);
	}
	
	//UKismetSystemLibrary::DrawDebugArrow(GetAvatarActorFromActorInfo(), SocketLocation, SocketLocation + Rotation.Vector() * 100.f, 5, FLinearColor::White, 120, 2);
}
