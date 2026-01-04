// ALL CODE FOR  learning GAS


#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"

void UAuraDamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
	const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, 1.f);

	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
	float ScaledDamage = Damage.GetValueAtLevel(GetAbilityLevel());//damage value
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DamageType, ScaledDamage);//damage tag
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor));
}

FDamageEffectParams UAuraDamageGameplayAbility::MakeDamageEffectParamFromClassDefaults(AActor* TargetActor,
	FVector RadialOrigin, bool bOverrideKnockbackDirection, FVector KnockbackDirectionOverride,
	bool bOverrideDeathimpulse, FVector DeathImpulseDirectionOverride, bool bOverridePitch, float PitchOverride) const
{
	FDamageEffectParams Params;
	Params.WorldContextObject = GetAvatarActorFromActorInfo();
	Params.DamageGameplayEffectClass = DamageEffectClass;
	Params.SourceAbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	Params.TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	Params.AbilityLevel = GetAbilityLevel();
	Params.BaseDamage = GetDamageByLevel(Params.AbilityLevel);
	Params.DamageType = DamageType;
	Params.DebuffChance = DebuffChance;
	Params.DebuffDamage = DebuffDamage;
	Params.DebuffDuration = DebuffDuration;
	Params.DebuffFrequency = DebuffFrequency;
	Params.DeathImpulseMagnitude = DeathImpulseMagnitude;
	Params.KnockbackForceMagnitude = KnockbackForceMagnitude;
	Params.KnockbackChance = KnockbackChance;
	FRotator Rotation = FRotator::ZeroRotator;
	//默认的施力方向 Knockback与Deathimpulse 都是从技能 释放者 到target 方向 然后pitch角45度
	if (IsValid(TargetActor))
	{
		Rotation = (TargetActor->GetActorLocation() - GetAvatarActorFromActorInfo()->GetActorLocation()).Rotation();//roll是0 因为无法从一个方向vector得知roll
	}
	Rotation.Pitch = 45.f; // default
	if (bOverridePitch)
	{
		Rotation.Pitch = PitchOverride;
	}
	const FVector ToTarget = Rotation.Vector();

	if (bOverrideKnockbackDirection)
	{
		KnockbackDirectionOverride.Normalize();
		Params.KnockbackForce = KnockbackDirectionOverride * KnockbackForceMagnitude;
		if (bOverridePitch)
		{
			FRotator KnockbackRotation = KnockbackDirectionOverride.Rotation();
			KnockbackRotation.Pitch = PitchOverride;
			Params.KnockbackForce = KnockbackRotation.Vector() * KnockbackForceMagnitude;
		}
	}
	else
	{
		Params.KnockbackForce = ToTarget * KnockbackForceMagnitude;
	}
	if (bOverrideDeathimpulse)
	{
		DeathImpulseDirectionOverride.Normalize();
		Params.DeathImpulse = DeathImpulseDirectionOverride * DeathImpulseMagnitude;
		if (bOverridePitch)
		{
			FRotator DeathImpulseRotation = DeathImpulseDirectionOverride.Rotation();
			DeathImpulseRotation.Pitch = PitchOverride;
			Params.DeathImpulse = DeathImpulseRotation.Vector() * DeathImpulseMagnitude;
		}
	}
	else
	{
		Params.DeathImpulse = ToTarget * DeathImpulseMagnitude;
	}

	if (bIsRadialDamage)
	{
		Params.bIsRadialDamage = true;
		Params.RadialDamageOrigin = RadialOrigin;
		Params.RadialDamageInnerRadius = RadialDamageInnerRadius;
		Params.RadialDamageOuterRadius = RadialDamageOuterRadius;
	}
	return Params;
}


float UAuraDamageGameplayAbility::GetDamageByLevel(float InLevel) const
{
	return Damage.GetValueAtLevel(InLevel);
}
