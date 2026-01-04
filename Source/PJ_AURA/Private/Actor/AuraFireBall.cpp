// ALL CODE FOR  learning GAS


#include "Actor/AuraFireBall.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemGlobals.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Components/AudioComponent.h"
#include "GameplayCueManager.h"

AActor* AAuraFireBall::GetReturnActor() const
{
	return ReturnToActor.Get();
}

void AAuraFireBall::BeginPlay()
{
	Super::BeginPlay();
	StartOutgoingTimeline();
}

void AAuraFireBall::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValidOverlap(OtherActor)) return;
	if (HasAuthority())
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			const FVector DeathImpulse = GetActorForwardVector() * DamageEffectParams.DeathImpulseMagnitude;
			DamageEffectParams.DeathImpulse = DeathImpulse;
			DamageEffectParams.KnockbackForce = FVector::ZeroVector;
			DamageEffectParams.TargetAbilitySystemComponent = TargetASC;
			UAuraAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
		}
	}
	else
		bHit = true;
}

void AAuraFireBall::OnHit()
{

	if (GetOwner())
	{
		FGameplayCueParameters CueParameters;
		CueParameters.Location = GetActorLocation();
		//client在destroy的时候会调用
		UGameplayCueManager::ExecuteGameplayCue_NonReplicated(GetOwner(), FAuraGameplayTags::Get().GameplayCue_FireBlast, CueParameters);
	}

	if (LoopingSoundComponent)
	{
		LoopingSoundComponent->Stop();
		LoopingSoundComponent->DestroyComponent();
	}
	bHit = true;
}
