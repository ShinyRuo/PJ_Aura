// ALL CODE FOR wangjunyang learning GAS

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "OverlayWidgetController.generated.h"


struct FAuraAbilityInfo;
class UAuraAbilitySystemComponent;
class UAbilityInfo;

USTRUCT()
struct FUIWidgetRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag MessageTag = FGameplayTag();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Message = FText();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class UAuraUserWidget> MessageWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* Image = nullptr;
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChangedSignature_Float, float, NewValue);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessageWidgetRowSignature, FUIWidgetRow, Row);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityInfoSignature, const FAuraAbilityInfo&, Info);

/**
 * 
 */
UCLASS(BlueprintType,Blueprintable)
class PJ_AURA_API UOverlayWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
public:
	virtual void BroadcastInitalValue() override;
	virtual void BindCallbackToDependencies() override;

	UPROPERTY(BlueprintAssignable,Category="GAS|Attributes")
	FOnAttributeChangedSignature_Float OnHealthChanged;//֪ͨUI

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature_Float OnMaxHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature_Float OnManaChanged;//֪ͨUI

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature_Float OnMaxManaChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|EXP")
	FOnAttributeChangedSignature_Float OnExpPercentChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Messages")
	FMessageWidgetRowSignature MessageWidgetRowDelegate;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Messages")
	FAbilityInfoSignature AbilityInfoDelegate;

protected:

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "Widget Data")
	TObjectPtr<UDataTable> MessageWidgetDataTable;
	template<typename T>
	T* GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget Data")
	TObjectPtr<UAbilityInfo> AbilityInfo;

	void OnInitializeStartupAbilities(UAuraAbilitySystemComponent* AuraASC);

	void OnExpChanged(int32 NewExp)const;
};

template <typename T>
T* UOverlayWidgetController::GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag)
{
	return DataTable->FindRow<T>(Tag.GetTagName(), TEXT(""));
}
