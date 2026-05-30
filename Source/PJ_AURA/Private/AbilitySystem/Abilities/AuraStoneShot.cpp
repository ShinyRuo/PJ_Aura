// ALL CODE FOR  learning GAS


#include "AbilitySystem/Abilities/AuraStoneShot.h"
#include "Actor/AuraFlyingStone.h"
#include "AbilitySystem/AbilityTasks/TargetDataUnderMouse.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/Pawn.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TimerManager.h"

UAuraStoneShot::UAuraStoneShot()
{
	// 设置默认值
	StoneCount = 3;
	OrbitRadius = 100.f;
	RotationSpeed = 360.f;
	LaunchDelay = 0.5f;
	StoneLaunchInterval = 0.1f;
	LaunchInitialSpeed = 800.f;
	LaunchMaxSpeed = 2000.f;
	LaunchAcceleration = 5000.f;
	MaxTravelDistance = 2000.f;
}


void UAuraStoneShot::SpawnStones()
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor)
	{
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), false, false);
		return;
	}

	SpawnedStones.Empty();

	// 生成指定数量的石头
	for (int32 i = 0; i < StoneCount; ++i)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(AvatarActor->GetActorLocation());
		SpawnTransform.SetRotation(AvatarActor->GetActorRotation().Quaternion());

		// 使用SpawnActorDeferred延迟初始化
		AAuraFlyingStone* Stone = GetWorld()->SpawnActorDeferred<AAuraFlyingStone>(
			ProjectileClass,
			SpawnTransform,
			AvatarActor,
			Cast<APawn>(AvatarActor),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn
		);

		if (Stone)
		{
			// 在BeginPlay之前设置属性
			Stone->DamageEffectParams = MakeDamageEffectParamFromClassDefaults();
			
			// 初始化石头为卫星静止状态
			Stone->InitializeSatellite(AvatarActor, i, StoneCount, OrbitRadius, 50.f,TargetLocation);

			// 完成初始化，触发BeginPlay
			Stone->FinishSpawning(SpawnTransform);

			SpawnedStones.Add(Stone);
		}
	}
}

void UAuraStoneShot::DelayedStartRotation()
{
	// 开始所有石头的旋转
	StartStoneRotation();
}

void UAuraStoneShot::StartStoneRotation()
{
	// 开始所有石头的旋转
	for (AAuraFlyingStone* Stone : SpawnedStones)
	{
		if (Stone && Stone->IsValidLowLevel())
		{
			Stone->StartRotation(RotationSpeed);
		}
	}
}

FVector UAuraStoneShot::GetMouseDirection()
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor)
	{
		return FVector::ForwardVector;
	}

	APlayerController* PC = Cast<APlayerController>(AvatarActor->GetOwner());
	if (!PC)
	{
		PC = Cast<APlayerController>(Cast<APawn>(AvatarActor)->GetController());
	}

	if (PC)
	{
		FHitResult HitResult;
		PC->GetHitResultUnderCursor(ECC_Visibility, false, HitResult);

		FVector MouseWorldLocation = HitResult.Location;
		FVector ActorLocation = AvatarActor->GetActorLocation();

		return (MouseWorldLocation - ActorLocation).GetSafeNormal();
	}

	return AvatarActor->GetActorForwardVector();
}

void UAuraStoneShot::LaunchAllStones()
{
	DelayedStartRotation();
}

