// ALL CODE FOR  learning GAS

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "WaitCooldownChange.generated.h"
struct FActiveGameplayEffectHandle;
struct FGameplayEffectSpec;
class UAbilitySystemComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCooldownChangeSignature, float, TimeRemaining);
/**
 * 其实就是effect的添加和删除时候加了两个回调
 * 用UBlueprintAsyncActionBase只是为了在蓝图节点中显示得更清晰
 * 这个类并不是执行一个task 在执行完了回调 只是用委托的方式Listen两个动作
 */
UCLASS(BlueprintType,meta = (ExposedAsyncProxy = "AsyncTask"))
class PJ_AURA_API UWaitCooldownChange : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FCooldownChangeSignature CooldownStart;

	UPROPERTY(BlueprintAssignable)
	FCooldownChangeSignature CooldownEnd;

	UFUNCTION(BlueprintCallable,meta = (BlueprintInternalUseOnly = "true"))
	static UWaitCooldownChange* WaitForCooldownChange(UAbilitySystemComponent* AbilitySystemComponent,const FGameplayTag& InCooldownTag);

	UFUNCTION(BlueprintCallable)
	void EndTask();

protected:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> ASC;

	FGameplayTag CooldownTag;
	//call back on cooldown start or end
	void CooldownTagChanged(const FGameplayTag InCooldownTag, int32 NewCount);
	void OnActiveEffectAdded(UAbilitySystemComponent* TargetASC, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveEffectHandle);
};
