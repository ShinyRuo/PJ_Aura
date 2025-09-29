// ALL CODE FOR wangjunyang learning GAS


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Player/AuraPlayerState.h"

void UAttributeMenuWidgetController::BindCallbackToDependencies()
{
	UAuraAttributeSet* AS = GetAuraAS();
	
	check(AttributeInfo);
	for(auto& Pair :AS->TagsToAttributes)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
			[this,Pair](const FOnAttributeChangeData& Data)
			{
				BroadcastAttributeInfo(Pair.Key, Pair.Value());
			}
		);
	}

	AAuraPlayerState* AuraPlayerState = GetAuraPS();
	AuraPlayerState->OnAttributePointsChangedDelegate.AddLambda(
		[this](int32 NewAttributePoints)
		{
			AttributePointsChangedDelegate.Broadcast(NewAttributePoints);
		}
	);
}

void UAttributeMenuWidgetController::BroadcastInitalValue()
{
	UAuraAttributeSet* AS = GetAuraAS();
	check(AttributeInfo);
	for(auto& Pair : AS->TagsToAttributes)
	{
		BroadcastAttributeInfo(Pair.Key, Pair.Value());
	}

	AAuraPlayerState* AuraPlayerState = GetAuraPS();
	AttributePointsChangedDelegate.Broadcast(AuraPlayerState->GetAttributePoints());

}

void UAttributeMenuWidgetController::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	UAuraAbilitySystemComponent* AuraASC = GetAuraASC();
	if (AuraASC)
	{
		AuraASC->UpgradeAttribute(AttributeTag);
	}
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag,
                                                            const FGameplayAttribute Attribute) const
{
	FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(AttributeTag);
	//Info.AttributeValue = Data.NewValue;
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet); //得到当前属性值
	AttributeInfoDelegate.Broadcast(Info);
}
