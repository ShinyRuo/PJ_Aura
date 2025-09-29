// ALL CODE FOR wangjunyang learning GAS


#include "UI/WidgetController/AuraWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"

void UAuraWidgetController::SetWidgetControllerParams(const FWidgetContorllerParams& WCParams)
{
	PlayerController = WCParams.PlayerController;
	PlayerState = WCParams.PlayerState;
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
	AttributeSet = WCParams.AttributeSet;
}

void UAuraWidgetController::BroadcastInitalValue()
{

}

void UAuraWidgetController::BindCallbackToDependencies()
{
}

AAuraPlayerController* UAuraWidgetController::GetAuraPC() 
{
	if (!KeptAuraPlayerController)
	{
		KeptAuraPlayerController = Cast<AAuraPlayerController>(PlayerController);
	}
	return KeptAuraPlayerController;
}

AAuraPlayerState* UAuraWidgetController::GetAuraPS() 
{
	if (!KeptAuraPlayerState)
	{
		KeptAuraPlayerState = Cast<AAuraPlayerState>(PlayerState);
	}
	return KeptAuraPlayerState;
}

UAuraAbilitySystemComponent* UAuraWidgetController::GetAuraASC() 
{
	if (!KeptAuraAbilitySystemComponent)
	{
		KeptAuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent);
	}
	return KeptAuraAbilitySystemComponent;
}

UAuraAttributeSet* UAuraWidgetController::GetAuraAS() 
{
	if (!KeptAuraAttributeSet)
	{
		KeptAuraAttributeSet = Cast<UAuraAttributeSet>(AttributeSet);
	}
	return KeptAuraAttributeSet;
}

void UAuraWidgetController::BroadcastAbilityInfo()
{
	if ( !GetAuraASC()->bStartupAbilitiesGiven) return;

	FForEachAbility BroadcastDelegate;//当作匿名函数用
	BroadcastDelegate.BindLambda([this](const FGameplayAbilitySpec& AbilitySpec)
		{
			FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(UAuraAbilitySystemComponent::GetAbilityTagFromSpec(AbilitySpec));
			Info.InputTag = UAuraAbilitySystemComponent::GetAbilityInputTagFromSpec(AbilitySpec);
			Info.StatusTag = UAuraAbilitySystemComponent::GetAbilityStatusTagFromSpec(AbilitySpec);
			Info.AbilityLevel = AbilitySpec.Level;
			AbilityInfoDelegate.Broadcast(Info);
		}
	);
	GetAuraASC()->ForEachAbility(BroadcastDelegate);
}
