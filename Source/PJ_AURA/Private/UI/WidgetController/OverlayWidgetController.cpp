// ALL CODE FOR wangjunyang learning GAS


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Player/AuraPlayerState.h"

void UOverlayWidgetController::BroadcastInitalValue()
{
	const UAuraAttributeSet* AuraAttributeSet = GetAuraAS();
	//委托2 UOverlayWidgetController -> 蓝图UI
	OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());
	OnManaChanged.Broadcast(AuraAttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(AuraAttributeSet->GetMaxMana());

	
}
void UOverlayWidgetController::BindCallbackToDependencies()
{
	AAuraPlayerState* AuraPlayerState = GetAuraPS();
	AuraPlayerState->OnExpChangedDelegate.AddUObject(this, &UOverlayWidgetController::OnExpChanged);
	AuraPlayerState->OnLevelChangedDelegate.AddLambda(
		[this](int32 NewLevel)
		{
			OnPlayerLevelChangedDelegate.Broadcast(NewLevel);
		}
	);

	const UAuraAttributeSet* AuraAttributeSet =GetAuraAS();
	// 委托1  OnRep_xxx -> GAMEPLAYATTRIBUTE_REPNOTIFY 
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetHealthAttribute())
		.AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				//委托2 UOverlayWidgetController -> 蓝图UI
				OnHealthChanged.Broadcast(Data.NewValue);
			}
		);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxHealthAttribute())
		.AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue);
			}
		);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetManaAttribute())
		.AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnManaChanged.Broadcast(Data.NewValue);
			}
		);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxManaAttribute())
		.AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxManaChanged.Broadcast(Data.NewValue);
			}
		);
	if (UAuraAbilitySystemComponent* AuraASC = GetAuraASC())
	{
		if (AuraASC->bStartupAbilitiesGiven)
		{
			//先bind委托 或是先GiveAbility 的时机不确定
			//BindCallbackToDependencies 实在初始化UOverlayWidgetController时候才执行的
			BroadcastAbilityInfo();
		}
		else
		{
			AuraASC->AbilitiesGivenDelegate.AddUObject(this, &UOverlayWidgetController::BroadcastAbilityInfo);
		}

		AuraASC->AbilityEquipped.AddUObject(this, &UOverlayWidgetController::OnAbilityEquipped);

		AuraASC->EffectAssetTags.AddLambda(
			[this](const FGameplayTagContainer& AssetTags)
			{
				for (const FGameplayTag Tag : AssetTags)
				{
					FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
					if (!Tag.MatchesTag(MessageTag)) continue;

					//GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Blue, FString::Printf(TEXT("GE Tag %s"), *Tag.ToString()));;
					const FUIWidgetRow* Row = this->GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);
					if (Row)
						MessageWidgetRowDelegate.Broadcast(*Row);
				}
			}
		);
	}
}


void UOverlayWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status,
	const FGameplayTag& Slot, const FGameplayTag& PrevSlot, int32 AbilityLevel)
{
	FAuraAbilityInfo LastSlotInfo;
	LastSlotInfo.InputTag = PrevSlot;
	LastSlotInfo.AbilityType = FAuraGameplayTags::Get().Abilities_Type_None;
	AbilityInfoDelegate.Broadcast(LastSlotInfo);


	FAuraAbilityInfo CurSlotInfo = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
	CurSlotInfo.InputTag = Slot;
	AbilityInfoDelegate.Broadcast(CurSlotInfo);
}

void UOverlayWidgetController::OnExpChanged(int32 NewExp) 
{
	AAuraPlayerState* AuraPlayerState = GetAuraPS();
	const ULevelUpInfo* LevelUpInfo = AuraPlayerState->LevelUpInfo;
	checkf(LevelUpInfo, TEXT("Unable to find LevelUpInfo,Please fill out AuraPlayerState Blueprint"));

	int32 Level = LevelUpInfo->FindLevelForXp(NewExp);
	int32 MaxLevel = LevelUpInfo->LevelUpInformation.Num() - 1;

	if (Level <= MaxLevel && Level > 0)
	{
		const int32  LevelUpRequirement = LevelUpInfo->LevelUpInformation[Level].LevelUpRequirement;
		const int32  PreviousLevelUpRequirement = LevelUpInfo->LevelUpInformation[Level-1].LevelUpRequirement;
		const int32  DeltaLevelUpRequirement = LevelUpRequirement - PreviousLevelUpRequirement;
		const int32  XPForThisLevel = NewExp - PreviousLevelUpRequirement;

		const float ExpBarPercent = static_cast<float>(XPForThisLevel) / static_cast<float>(DeltaLevelUpRequirement);

		OnExpPercentChangedDelegate.Broadcast(ExpBarPercent);
	}
}

