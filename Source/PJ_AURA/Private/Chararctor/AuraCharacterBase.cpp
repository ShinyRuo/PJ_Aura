// ALL CODE FOR wangjunyang learning GAS


#include "Chararctor/AuraCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"
#include "AbilitySystem/Passive/PassiveNiagaraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "PJ_AURA/PJ_AURA.h"

AAuraCharacterBase::AAuraCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	BurnDebuffComponent = CreateDefaultSubobject<UDebuffNiagaraComponent>("BurnDebuffComponent");
	BurnDebuffComponent->SetupAttachment(GetRootComponent());
	BurnDebuffComponent->DebuffTag = FAuraGameplayTags::Get().Debuff_Burn;

	StunDebuffComponent = CreateDefaultSubobject<UDebuffNiagaraComponent>("StunDebuffComponent");
	StunDebuffComponent->SetupAttachment(GetRootComponent());
	StunDebuffComponent->DebuffTag = FAuraGameplayTags::Get().Debuff_Stun;

	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Ignore);
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket")); //给weapon的sk加一个socket
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	EffectAttachComponent = CreateDefaultSubobject<USceneComponent>("EffectAttachPoint");
	EffectAttachComponent->SetupAttachment(GetRootComponent());

	HaloOfProtectionNiagaraComponent = CreateDefaultSubobject<UPassiveNiagaraComponent>("HaloOfProtectionComponent");
	HaloOfProtectionNiagaraComponent->SetupAttachment(EffectAttachComponent);

	LifeSiphonNiagaraComponent = CreateDefaultSubobject<UPassiveNiagaraComponent>("LifeSiphonComponent");
	LifeSiphonNiagaraComponent->SetupAttachment(EffectAttachComponent);

	ManaSiphonNiagaraComponent = CreateDefaultSubobject<UPassiveNiagaraComponent>("ManaSiphonComponent");
	ManaSiphonNiagaraComponent->SetupAttachment(EffectAttachComponent);

}

void AAuraCharacterBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	EffectAttachComponent->SetWorldRotation(FRotator::ZeroRotator); // 旋转不受父组件影响
}

void AAuraCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AAuraCharacterBase, bIsStunned);
	DOREPLIFETIME(AAuraCharacterBase, bIsBurned);
	DOREPLIFETIME(AAuraCharacterBase, bIsInShocked);
}

UAnimMontage* AAuraCharacterBase::GetHitReactMontage_Implementation()
{
	return HitReactMontage;
}

TArray<FTaggedMontage> AAuraCharacterBase::GetAttackMontages_Implementation()
{
	return AttackMontages;
}

UNiagaraSystem* AAuraCharacterBase::GetHitEffect_Implementation()
{
	return BloodEffect;
}

FTaggedMontage AAuraCharacterBase::GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag)
{
	for (FTaggedMontage TaggedMontage : AttackMontages)
	{
		if (TaggedMontage.MontageTag == MontageTag)
		{
			return TaggedMontage;
		}
	}
	return FTaggedMontage();
}

int32 AAuraCharacterBase::GetMinionCount_Implementation()
{
	return MinionCount;
}

void AAuraCharacterBase::IncrementMinionCount_Implementation(int32 Amount)
{
	MinionCount += Amount;
}

ECharacterClass AAuraCharacterBase::GetCharacterClass_Implementation()
{
	return CharacterClass;
}

FOnASCRegistered& AAuraCharacterBase::GetOnASCRegisteredDelegate()
{
	return OnAscRegistered;
}

FOnDeath& AAuraCharacterBase::GetOnDeathDelegate()
{
	return OnDeath;
}

USkeletalMeshComponent* AAuraCharacterBase::GetWeapon_Implementation()
{
	return Weapon;
}

void AAuraCharacterBase::SetBeingShocked_Implementation(bool InShocked)
{
	bIsInShocked = InShocked;
}

bool AAuraCharacterBase::IsBeingShocked_Implementation() const
{
	return bIsInShocked;
}

FOnDamageSignature& AAuraCharacterBase::GetOnDamageSignature()
{
	return OnDamageDelegate;
}


float AAuraCharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
                                     AActor* DamageCauser)
{
	const float DamageTaken = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	OnDamageDelegate.Broadcast(DamageTaken);
	return DamageTaken;
}

void AAuraCharacterBase::Die(const FVector& DeathImpulse)
{
	Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	MulticastHandleDeath(DeathImpulse);
}

void AAuraCharacterBase::MulticastHandleDeath_Implementation(const FVector& DeathImpulse)
{
	UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation(), GetActorRotation());

	Weapon->SetSimulatePhysics(true);
	Weapon->SetEnableGravity(true);
	Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	Weapon->AddImpulse(DeathImpulse*0.1f, NAME_None, true);

	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	GetMesh()->AddImpulse(DeathImpulse,NAME_None,true);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Dissolve();
	bDead = true;
	BurnDebuffComponent->Deactivate();
	StunDebuffComponent->Deactivate();
	OnDeath.Broadcast(this);
}

void AAuraCharacterBase::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bIsStunned = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bIsStunned ? 0.f : BaseWalkSpeed;
}

void AAuraCharacterBase::BurnTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bIsBurned = NewCount > 0;
}

void AAuraCharacterBase::OnRep_Stunned()
{

}

void AAuraCharacterBase::OnRep_Burned()
{
}

void AAuraCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

FVector AAuraCharacterBase::GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag)
{
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_Weapon) && IsValid(Weapon))
	{
		return Weapon->GetSocketLocation(WeaponTipSocketName);
	}
	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_LeftHand) )
	{
		return GetMesh()->GetSocketLocation(LeftHandSocketName);
	}
	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_RightHand))
	{
		return GetMesh()->GetSocketLocation(RightHandSocketName);
	}
	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_Hit))
	{
		return GetMesh()->GetSocketLocation(HitSocketName);
	}
	return FVector();
}

bool AAuraCharacterBase::IsDead_Implementation() const
{
	return bDead;
}

AActor* AAuraCharacterBase::GetAvatar_Implementation() 
{
	return this;
}

void AAuraCharacterBase::InitAbilityActorInfo()
{
}

void AAuraCharacterBase::ApplyEffectToSelf(const TSubclassOf<UGameplayEffect> GameEffectClass, const float Level) const
{
	check(IsValid(GetAbilitySystemComponent())); 
	check(GameEffectClass);
	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	ContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(GameEffectClass, Level, ContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), GetAbilitySystemComponent());

}

void AAuraCharacterBase::InitializeDefaultAttributes() const
{
	ApplyEffectToSelf(DefaultPrimaryAttribute, 1.f);
	ApplyEffectToSelf(DefaultSecondaryAttribute, 1.f);
	ApplyEffectToSelf(DefaultVitalAttribute, 1.f);
}

void AAuraCharacterBase::AddCharacterAbilities()
{
	UAuraAbilitySystemComponent* AuraASC = CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent);
	if (!HasAuthority())return;
	AuraASC->AddCharacterAbilities(StartupAbilities);
	AuraASC->AddCharacterPassiveAbilities(StartupPassiveAbilities);
}

void AAuraCharacterBase::Dissolve()
{
	if (IsValid(CharDissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(CharDissolveMaterialInstance, this);
		GetMesh()->SetMaterial(0, DynamicMatInst);//本项目mesh只有一种material
		StartDissolveTimeline(DynamicMatInst);
	}
	if (IsValid(WeaponDissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* WeaponDynamicMatInst = UMaterialInstanceDynamic::Create(WeaponDissolveMaterialInstance, Weapon);
		Weapon->SetMaterial(0, WeaponDynamicMatInst);//本项目mesh只有一种material
		StartWeaponDissolveTimeline(WeaponDynamicMatInst);
	}
}


UAbilitySystemComponent* AAuraCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

