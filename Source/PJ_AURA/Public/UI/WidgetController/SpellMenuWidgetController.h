// ALL CODE FOR  learning GAS

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "SpellMenuWidgetController.generated.h"



USTRUCT()
struct FAbilityDescRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (MultiLine = "True"))
	TArray<FText> AbilityDescText;

};


DECLARE_DYNAMIC_DELEGATE(FUIAbilityEquipped);

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class PJ_AURA_API USpellMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature_Int32 SpellPointsChangedDelegate;

	UPROPERTY(BlueprintReadWrite)
	FUIAbilityEquipped UIAbilityEquipped;

	UFUNCTION(BlueprintCallable)
	void SpendSpellPointPressed(const FGameplayTag& AbilityTag);

	UFUNCTION(BlueprintCallable)
	bool GetAbilityDesc(const FGameplayTag& AbilityTag,FText& OutDesc, FText& OutNextLevelDesc);

	virtual void BindCallbackToDependencies() override;
	virtual void BroadcastInitalValue() override;

	static FText GetDescription_CurLevel(int32 Level);
	static FText GetDescription_NextLevel(int32 Level);
	static FText GetDescription_Locked(int32 Level);

	UFUNCTION(BlueprintCallable)
	void SpellRowGlobePressed( const FGameplayTag& AbilityTag, const FGameplayTag& SlotTag);

	void OnAbilityEquipped(const FGameplayTag& AbilityTag,
		const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PrevSlot,int32 AbilityLevel);


protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Desc")
	TObjectPtr<UDataTable> AbilityDescDataTable;
	template<typename T>
	T* GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag);

private:
	FGameplayTag SelectedTag;
};

template <typename T>
T* USpellMenuWidgetController::GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag)
{
	return DataTable->FindRow<T>(Tag.GetTagName(), TEXT(""));
}
