// ALL CODE FOR  learning GAS


#include "AbilitySystem/Abilities/AuraBeamSpell.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/KismetSystemLibrary.h"

void UAuraBeamSpell::StoreMouseDataInfo(const FHitResult HitResult)
{
	if (HitResult.bBlockingHit)
	{
		MouseHitLocation = HitResult.ImpactPoint;
		MouseHitActor = HitResult.GetActor();
	}
	else
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void UAuraBeamSpell::StoreOwnerInfo()
{
	if (CurrentActorInfo)
	{
		OwnerPlayerController = CurrentActorInfo->PlayerController.Get();
		OwnerCharacter = Cast<ACharacter>(CurrentActorInfo->AvatarActor.Get());
	}
}

void UAuraBeamSpell::TraceFirstTarget(const FVector& BeamTargetLocation)
{
	check(OwnerCharacter);
	if (OwnerCharacter->Implements<UCombatInterface>())
	{
		if(USkeletalMeshComponent* Weapon = ICombatInterface::Execute_GetWeapon(OwnerCharacter))
		{
			TArray<AActor*> ActorsToIgnore;
			ActorsToIgnore.Add(OwnerCharacter);
			FHitResult HitResult;
			const FVector SocketLocation = Weapon->GetSocketLocation(FName("TipSocket"));
			UKismetSystemLibrary::SphereTraceSingle(
				OwnerCharacter, 
				SocketLocation, 
				BeamTargetLocation, 
				10.f, TraceTypeQuery1, 
				false, 
				ActorsToIgnore, 
				EDrawDebugTrace::None, 
				HitResult,
				true
			);
			if (HitResult.bBlockingHit)
			{
				MouseHitLocation = HitResult.ImpactPoint;
				MouseHitActor = HitResult.GetActor();
			}
		}
	}
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(MouseHitActor))
	{
		if (CombatInterface->GetOnDeathDelegate().IsAlreadyBound(this, &UAuraBeamSpell::AdditionalActorDied))
		{
			CombatInterface->GetOnDeathDelegate().RemoveDynamic(this, &UAuraBeamSpell::AdditionalActorDied);
		}
		if (!CombatInterface->GetOnDeathDelegate().IsAlreadyBound(this, &UAuraBeamSpell::PrimaryActorDied))
		{
			CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UAuraBeamSpell::PrimaryActorDied);
		}
	}
}

void UAuraBeamSpell::StoreAdditionalTargets(TArray<AActor*>& OutAdditionalTargets,int32 MaxTarget)
{
	check(MouseHitActor);
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetAvatarActorFromActorInfo());
	ActorsToIgnore.Add(MouseHitActor);
	TArray<AActor*> OverlappingActors;
	UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(
		GetAvatarActorFromActorInfo(),
		OverlappingActors,
		ActorsToIgnore,
		850.f,
		MouseHitActor->GetActorLocation()
	);
	int32 NumAdditionalTargets = FMath::Min(GetAbilityLevel()-1, MaxNumShockTargets);

	UAuraAbilitySystemLibrary::GetClosestTargets(MaxTarget, OverlappingActors, OutAdditionalTargets, MouseHitActor->GetActorLocation());

	for (AActor* Target : OutAdditionalTargets)
	{
		if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(Target))
		{
			if (CombatInterface->GetOnDeathDelegate().IsAlreadyBound(this, &UAuraBeamSpell::PrimaryActorDied))
			{
				CombatInterface->GetOnDeathDelegate().RemoveDynamic(this, &UAuraBeamSpell::PrimaryActorDied);
			}
			if (!CombatInterface->GetOnDeathDelegate().IsAlreadyBound(this, &UAuraBeamSpell::AdditionalActorDied))
			{
				CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UAuraBeamSpell::AdditionalActorDied);
			}
		}
	}
}
