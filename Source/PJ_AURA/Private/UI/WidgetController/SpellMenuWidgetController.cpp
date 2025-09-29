// ALL CODE FOR  learning GAS


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerState.h"

void USpellMenuWidgetController::SpendSpellPointPressed(const FGameplayTag& AbilityTag)
{
	GetAuraASC()->ServerSpendSpellPoint(AbilityTag);
}

bool USpellMenuWidgetController::GetAbilityDesc(const FGameplayTag& AbilityTag, FText& OutDesc,
	FText& OutNextLevelDesc)
{
	if (const FGameplayAbilitySpec* AbilitySpec = GetAuraASC()->GetSpecFromAbilityTags(AbilityTag))
	{
		if (UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec->Ability))
		{
			//get level and find in DT_AbilityDesc
			if (FAbilityDescRow* AbilityDescRow = GetDataTableRowByTag<FAbilityDescRow>(AbilityDescDataTable, AbilityTag))
			{
				int32 TargetLevel = AbilitySpec->Level;
				TargetLevel = FMath::Clamp(TargetLevel - 1, 0, AbilityDescRow->AbilityDescText.Num() - 1);
				OutDesc = AuraAbility->MakeAbilityDesc(AbilityDescRow->AbilityDescText[TargetLevel], AbilitySpec->Level); // input format out useful string
				if (TargetLevel < AbilityDescRow->AbilityDescText.Num() - 1)TargetLevel++;
				OutNextLevelDesc =AuraAbility->MakeAbilityDesc(AbilityDescRow->AbilityDescText[TargetLevel], AbilitySpec->Level+1); // input format out useful string
			}
			return true;
		}
	}

	//Locked Ability
	const FAuraAbilityInfo& Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
	OutDesc = GetDescription_Locked(Info.LevelRequirement);
	OutNextLevelDesc = FText();
	return true;
}

void USpellMenuWidgetController::BindCallbackToDependencies()
{
	GetAuraASC()->AbilityStatusChanged.AddLambda(
		[this](const FGameplayTag&  AbilityTag, const FGameplayTag&  StatusTag, int32 Level)
		{
			if (AbilityInfo)
			{
				FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
				Info.StatusTag = StatusTag;
				Info.AbilityLevel = Level;
				AbilityInfoDelegate.Broadcast(Info);
			}
		}
	);

	GetAuraASC()->AbilityEquipped.AddUObject(this, &USpellMenuWidgetController::OnAbilityEquipped);

	GetAuraPS()->OnSpellPointsChangedDelegate.AddLambda(
		[this](int32 NewSpellPoints)
		{
			SpellPointsChangedDelegate.Broadcast(NewSpellPoints);
		}
	);
}

void USpellMenuWidgetController::BroadcastInitalValue()
{
	BroadcastAbilityInfo();
	SpellPointsChangedDelegate.Broadcast(GetAuraPS()->GetSpellPoints());
}


FText USpellMenuWidgetController::GetDescription_CurLevel(int32 Level)
{
	FText FormatString = NSLOCTEXT("AbilitySystem", "GetDescription_CurLevel","<Default>Default Description,Level: </><Level>{0}</>");
	return FText::Format(FormatString, FText::AsNumber(Level));
}

FText USpellMenuWidgetController::GetDescription_NextLevel(int32 Level)
{
	FText FormatString = NSLOCTEXT("AbilitySystem", "GetDescription_NextLevel", "<Default>Default Description,Next Level: </><Level>{0}</>");
	return FText::Format(FormatString, FText::AsNumber(Level));
}

FText USpellMenuWidgetController::GetDescription_Locked(int32 Level)
{
	FText FormatString = NSLOCTEXT("AbilitySystem", "GetDescription_NextLevel", "<Default>SpellLocked Until Level: </><Level>{0}</>");
	return FText::Format(FormatString, FText::AsNumber(Level));
}

void USpellMenuWidgetController::SpellRowGlobePressed(const FGameplayTag& AbilityTag, const FGameplayTag& SlotTag)
{
	GetAuraASC()->ServerEquipAbility(AbilityTag, SlotTag);
}

void USpellMenuWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status,
	const FGameplayTag& Slot, const FGameplayTag& PrevSlot, int32 AbilityLevel)
{
	FAuraAbilityInfo LastSlotInfo;
	LastSlotInfo.StatusTag = FAuraGameplayTags::Get().Abilities_Status_Unlocked;
	LastSlotInfo.InputTag = PrevSlot;
	LastSlotInfo.AbilityType = FAuraGameplayTags::Get().Abilities_Type_None;
	LastSlotInfo.AbilityLevel = 0;
	//empty last slot if this ability is already equiped
	AbilityInfoDelegate.Broadcast(LastSlotInfo);

	FAuraAbilityInfo CurSlotInfo = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
	CurSlotInfo.StatusTag = Status;
	CurSlotInfo.InputTag = Slot;
	CurSlotInfo.AbilityLevel = AbilityLevel;
	AbilityInfoDelegate.Broadcast(CurSlotInfo);

	UIAbilityEquipped.Execute();
}
