// ALL CODE FOR  learning GAS


#include "AuraAbilityTypes.h"

bool FAuraGameplayEffectContext::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	uint32 RepBits = 0;
	if (Ar.IsSaving())
	{
		if (bReplicateInstigator && Instigator.IsValid())
		{
			RepBits |= 1 << 0;
		}
		if (bReplicateEffectCauser && EffectCauser.IsValid())
		{
			RepBits |= 1 << 1;
		}
		if (AbilityCDO.IsValid())
		{
			RepBits |= 1 << 2;
		}
		if (bReplicateSourceObject && SourceObject.IsValid())
		{
			RepBits |= 1 << 3;
		}
		if (Actors.Num() > 0)
		{
			RepBits |= 1 << 4;
		}
		if (HitResult.IsValid())
		{
			RepBits |= 1 << 5;
		}
		if (bHasWorldOrigin)
		{
			RepBits |= 1 << 6;
		}
		if (bIsCriticalHit)
		{
			RepBits |= 1 << 7;
		}
		if (bIsBlockedHit)
		{
			RepBits |= 1 << 8;
		}
		if (bIsSuccessfulDebuff)
		{
			RepBits |= 1 << 9;
		}
		if (!DeathImpulse.IsZero())
		{
			RepBits |= 1 << 10;
		}
		if (!KnockbackForce.IsZero())
		{
			RepBits |= 1 << 11;
		}
		if (bIsRadialDamage)
		{
			RepBits |= 1 << 12;
		}
	}

	Ar.SerializeBits(&RepBits, 13);

	if (RepBits & (1 << 0))
	{
		Ar << Instigator;
	}
	if (RepBits & (1 << 1))
	{
		Ar << EffectCauser;
	}
	if (RepBits & (1 << 2))
	{
		Ar << AbilityCDO;
	}
	if (RepBits & (1 << 3))
	{
		Ar << SourceObject;
	}
	if (RepBits & (1 << 4))
	{
		SafeNetSerializeTArray_Default<31>(Ar, Actors);
	}
	if (RepBits & (1 << 5))
	{
		if (Ar.IsLoading())
		{
			if (!HitResult.IsValid())
			{
				HitResult = MakeShared<FHitResult>();
			}
		}
		HitResult->NetSerialize(Ar, Map, bOutSuccess);
	}
	if (RepBits & (1 << 6))
	{
		Ar << WorldOrigin;
		bHasWorldOrigin = true;
	}
	else
	{
		bHasWorldOrigin = false;
	}
	if (RepBits & (1 << 7))
	{
		bIsCriticalHit = true;
	}
	else
	{
		bIsCriticalHit = false;
	}
	if (RepBits & (1 << 8))
	{
		bIsBlockedHit = true;
	}
	else
	{
		bIsBlockedHit = false;
	}
	if (RepBits & (1 << 9))
	{
		bIsSuccessfulDebuff = true;
		Ar << DebuffDamage;
		Ar << DebuffDuration;
		Ar << DebuffFrequency;
		if (Ar.IsLoading())
		{
			if (!DamageType.IsValid())
			{
				DamageType = MakeShared<FGameplayTag>();
			}
			DamageType->NetSerialize(Ar, Map, bOutSuccess);
		}
	}
	else
	{
		bIsSuccessfulDebuff = false;
	}
	if (RepBits & (1 << 10))
	{
		DeathImpulse.NetSerialize(Ar, Map, bOutSuccess);
	}
	if (RepBits & (1 << 11))
	{
		KnockbackForce.NetSerialize(Ar, Map, bOutSuccess);
	}
	if (RepBits & (1 << 12))
	{
		bIsRadialDamage = true;
		Ar << RadialDamageOrigin;
		Ar << RadialDamageInnerRadius;
		Ar << RadialDamageOuterRadius;
	}
	else
		bIsRadialDamage = false;


	if (Ar.IsLoading())
	{
		AddInstigator(Instigator.Get(), EffectCauser.Get()); // Just to initialize InstigatorAbilitySystemComponent
	}

	bOutSuccess = true;
	return true;
}
