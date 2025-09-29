// ALL CODE FOR wangjunyang learning GAS


#include "AbilitySystem/AuraAttributeSet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "Interaction/PlayerInterface.h"
#include "Player/AuraPlayerController.h"

//map [ tags ] = UAuraAttributeSet::GetxxxAttribute
#define BIND_ATTRIBUTE_SIGNATURE(SecTag,AttributeName)\
TagsToAttributes.Add(FAuraGameplayTags::Get().Attributes_##SecTag##_##AttributeName, Get##AttributeName##Attribute); 
	

UAuraAttributeSet::UAuraAttributeSet()
{
	BIND_ATTRIBUTE_SIGNATURE(Primary, Strength)
	BIND_ATTRIBUTE_SIGNATURE(Primary, Intelligence)
	BIND_ATTRIBUTE_SIGNATURE(Primary, Resilience)
	BIND_ATTRIBUTE_SIGNATURE(Primary, Vigor)
	BIND_ATTRIBUTE_SIGNATURE(Secondary, Armor)
	BIND_ATTRIBUTE_SIGNATURE(Secondary, ArmorPenetration)
	BIND_ATTRIBUTE_SIGNATURE(Secondary, BlockChance)
	BIND_ATTRIBUTE_SIGNATURE(Secondary, CriticalHitChance)
	BIND_ATTRIBUTE_SIGNATURE(Secondary, CriticalHitDamage)
	BIND_ATTRIBUTE_SIGNATURE(Secondary, CriticalHitResistance)
	BIND_ATTRIBUTE_SIGNATURE(Secondary, HealthRegeneration)
	BIND_ATTRIBUTE_SIGNATURE(Secondary, ManaRegeneration)
	BIND_ATTRIBUTE_SIGNATURE(Secondary, MaxHealth)
	BIND_ATTRIBUTE_SIGNATURE(Secondary, MaxMana)

	BIND_ATTRIBUTE_SIGNATURE(Secondary, Resistance_Fire)
	BIND_ATTRIBUTE_SIGNATURE(Secondary, Resistance_Lighting)
	BIND_ATTRIBUTE_SIGNATURE(Secondary, Resistance_Arcane)
	BIND_ATTRIBUTE_SIGNATURE(Secondary, Resistance_Physical)
}


void UAuraAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Resilience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Vigor, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ArmorPenetration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, BlockChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, HealthRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ManaRegeneration, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Resistance_Fire, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Resistance_Lighting, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Resistance_Arcane, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Resistance_Physical, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);

}

void UAuraAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	/*Super::PreAttributeChange(Attribute, NewValue);
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
	}*/
	// 确保属性值不为负
	if (Attribute == GetArmorAttribute() ||
		Attribute == GetArmorPenetrationAttribute() ||
		Attribute == GetBlockChanceAttribute() ||
		Attribute == GetCriticalHitChanceAttribute() ||
		Attribute == GetCriticalHitDamageAttribute() ||
		Attribute == GetCriticalHitResistanceAttribute() ||
		Attribute == GetHealthRegenerationAttribute() ||
		Attribute == GetManaRegenerationAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.0f);
	}

	// 百分比属性限制在0-100%
	if (Attribute == GetBlockChanceAttribute() ||
		Attribute == GetCriticalHitChanceAttribute() ||
		Attribute == GetCriticalHitResistanceAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, 100.0f);
	}
}

void UAuraAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
	}
}

void UAuraAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

}


void UAuraAttributeSet::SetEffectProperties(const  FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const
{
	// Source = causer of the effect, Target = target of the effect(owner of this AS)
	Props.EffectContextHandle = Data.EffectSpec.GetContext();
	Props.SourceASC = Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();//这里直接get了源头的ASC 省掉了很多细节
	if (IsValid(Props.SourceASC) && Props.SourceASC->AbilityActorInfo.IsValid() && Props.SourceASC->AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.SourceAvatarActor = Props.SourceASC->AbilityActorInfo->AvatarActor.Get();
		Props.SourceController = Props.SourceASC->AbilityActorInfo->PlayerController.Get();//通过SourceASC获取的Actor 和 Controller 所以创建特效的时候不用AddSource这里也能找到 我觉得应该是Props.EffectContextHandle.GetSourceObject()
		if (Props.SourceController == nullptr && Props.SourceAvatarActor != nullptr)
		{
			if (const APawn* Pawn = Cast<APawn>(Props.SourceAvatarActor))
			{
				Props.SourceController = Pawn->GetController();
			}
		}
		if (Props.SourceController)
		{
			Props.SourceCharacter = Cast<ACharacter>(Props.SourceController->GetPawn());
		}
	}
	Props.TargetASC = &Data.Target;
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.TargetAvatarActor = Props.TargetASC->AbilityActorInfo->AvatarActor.Get();
		Props.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		Props.TargetCharacter = Cast<ACharacter>(Props.TargetAvatarActor);
		Props.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Props.TargetAvatarActor);
	}
}

void UAuraAttributeSet::ShowFloatingText(const FEffectProperties& Properties, float Damage,bool bBlockedHit, bool bCriticalHit)const
{
	if (Properties.SourceCharacter != Properties.TargetCharacter)
	{
		AAuraPlayerController* PC = Cast<AAuraPlayerController>(Properties.SourceController);
		if (PC && Properties.TargetCharacter)
		{
			//Source is Aura
			PC->ShowDamageNumber(Damage, Properties.TargetCharacter, bBlockedHit, bCriticalHit);
			return;
		}
		AAuraPlayerController* TargetPC = Cast<AAuraPlayerController>(Properties.TargetController);
		if (TargetPC && Properties.SourceCharacter&& Properties.TargetCharacter)
		{
			//Target is Aura
			TargetPC->ShowDamageNumber(Damage, Properties.TargetCharacter, bBlockedHit, bCriticalHit);
			return;
		}
	}
}

void UAuraAttributeSet::SendExpEvent(const FEffectProperties& Properties)
{
	if (Properties.TargetAvatarActor && Properties.TargetAvatarActor->Implements<UCombatInterface>())
	{
		int32 TargetLevel = ICombatInterface::Execute_GetPlayerLevel(Properties.TargetAvatarActor);
		ECharacterClass CharacterClass = ICombatInterface::Execute_GetCharacterClass(Properties.TargetAvatarActor);

		const int32 ExpReward = UAuraAbilitySystemLibrary::GetExpRewardForClassAndLevel(Properties.TargetCharacter, CharacterClass, TargetLevel);

		const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
		FGameplayEventData Payload;
		Payload.EventTag = GameplayTags.Attributes_Meta_IncomingExp;
		Payload.EventMagnitude = ExpReward;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Properties.SourceAvatarActor, GameplayTags.Attributes_Meta_IncomingExp, Payload);
	}
}

void UAuraAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	FEffectProperties Properties;
	SetEffectProperties(Data, Properties);

	/*if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		UE_LOG(LogTemp, Warning, TEXT("Changed Health On %s,Health:%f"), *Properties.TargetAvatarActor->GetName(), GetHealth());
	}*/
	if (Data.EvaluatedData.Attribute ==  GetIncomingDamageAttribute())
	{
		const float LocalIncomingDamage = GetIncomingDamage();
		SetIncomingDamage(0.f);
		if (LocalIncomingDamage > 0.f)
		{
			const float NewHealth = GetHealth() - LocalIncomingDamage;
			SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));

			const bool bFatal = NewHealth <= 0.f;
			if (bFatal)
			{
				ICombatInterface* CombatInterface = Cast<ICombatInterface>(Properties.TargetAvatarActor);
				if (CombatInterface)
				{
					CombatInterface->Die();
				}
				SendExpEvent(Properties);
			}
			else
			{
				FGameplayTagContainer TagContainer;
				TagContainer.AddTag(FAuraGameplayTags::Get().Effect_HitReact);
				Properties.TargetASC->TryActivateAbilitiesByTag(TagContainer);//激活拥有这个tag的技能
			}
			//floating text
			const bool bBlock = UAuraAbilitySystemLibrary::IsBlockedHit(Properties.EffectContextHandle);
			const bool bCritical = UAuraAbilitySystemLibrary::IsCriticalHit(Properties.EffectContextHandle);

			ShowFloatingText(Properties, LocalIncomingDamage, bBlock, bCritical);
		}
	}
	else if(Data.EvaluatedData.Attribute == GetIncomingExpAttribute())
	{
		const float LocalIncomingExp = GetIncomingExp();
		SetIncomingExp(0.f);
		//UE_LOG(LogTemp, Warning, TEXT("Changed Exp On %s,Exp:%f"), *Properties.TargetAvatarActor->GetName(), LocalIncomingExp);
		if (Properties.SourceCharacter->Implements<UPlayerInterface>() && Properties.SourceCharacter->Implements<UCombatInterface>())
		{
			const int32 CurrentLevel = ICombatInterface::Execute_GetPlayerLevel(Properties.SourceCharacter);
			const int32 CurrentExp = IPlayerInterface::Execute_GetExp(Properties.SourceCharacter);
			const int32 NewLevel = IPlayerInterface::Execute_FindLevelForExp(Properties.SourceCharacter, CurrentExp + LocalIncomingExp);
			const int32 NumLevelUps = NewLevel - CurrentLevel;

			if (NumLevelUps > 0)
			{
				const int32 AttributePointsRewards = IPlayerInterface::Execute_GetAttributePointsRewards(Properties.SourceCharacter, CurrentLevel);
				const int32 SpellPointsRewards =  IPlayerInterface::Execute_GetSpellPointsRewards(Properties.SourceCharacter, CurrentLevel);
				IPlayerInterface::Execute_AddToPlayerLevel(Properties.SourceCharacter, NumLevelUps);
				IPlayerInterface::Execute_AddToAttributePoints(Properties.SourceCharacter, AttributePointsRewards);
				IPlayerInterface::Execute_AddToSpellPoints(Properties.SourceCharacter, SpellPointsRewards);

				SetVigor(GetVigor());
				SetIntelligence(GetIntelligence());//用来触发MaxHealth 与 MaxMana 的MMC 

				SetHealth(GetMaxHealth());
				SetMana(GetMaxMana());
				

				IPlayerInterface::Execute_LevelUp(Properties.SourceCharacter);

			}
			IPlayerInterface::Execute_AddToExp(Properties.SourceCharacter, LocalIncomingExp);

		}
	}
}

void UAuraAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldStrength) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Strength, OldStrength);
}

void UAuraAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Intelligence, OldIntelligence);
}

void UAuraAttributeSet::OnRep_Resilience(const FGameplayAttributeData& OldResilience) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Resilience, OldResilience);
}

void UAuraAttributeSet::OnRep_Vigor(const FGameplayAttributeData& OldVigor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Vigor, OldVigor);
}

void UAuraAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Health, OldHealth);
}

void UAuraAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxHealth, OldMaxHealth);
}

void UAuraAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Mana, OldMana);
}

void UAuraAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxMana, OldMaxMana);
}

void UAuraAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Armor, OldArmor);
}

void UAuraAttributeSet::OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ArmorPenetration, OldArmorPenetration);
}

void UAuraAttributeSet::OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, BlockChance, OldBlockChance);
}

void UAuraAttributeSet::OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitChance, OldCriticalHitChance);
}

void UAuraAttributeSet::OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitDamage, OldCriticalHitDamage);
}

void UAuraAttributeSet::OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitResistance, OldCriticalHitResistance);
}

void UAuraAttributeSet::OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, HealthRegeneration, OldHealthRegeneration);
}

void UAuraAttributeSet::OnRep_ManaRegeneration(const FGameplayAttributeData& OldManaRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ManaRegeneration, OldManaRegeneration);
}

void UAuraAttributeSet::OnRep_Resistance_Fire(const FGameplayAttributeData& OldResistance_Fire) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Resistance_Fire, OldResistance_Fire);
}

void UAuraAttributeSet::OnRep_Resistance_Arcane(const FGameplayAttributeData& OldResistance_Arcane) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Resistance_Arcane, OldResistance_Arcane);
}

void UAuraAttributeSet::OnRep_Resistance_Lighting(const FGameplayAttributeData& OldResistance_Lighting) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Resistance_Lighting, OldResistance_Lighting);
}

void UAuraAttributeSet::OnRep_Resistance_Physical(const FGameplayAttributeData& OldResistance_Physical) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Resistance_Physical, OldResistance_Physical);
}
