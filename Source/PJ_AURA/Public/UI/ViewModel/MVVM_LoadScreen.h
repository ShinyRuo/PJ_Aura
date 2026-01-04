// ALL CODE FOR  learning GAS

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "MVVM_LoadScreen.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSlotSelected, int32, SelectedIndex);


class UMVVM_LoadSlot;
/**
 * 
 */
UCLASS()
class PJ_AURA_API UMVVM_LoadScreen : public UMVVMViewModelBase
{
	GENERATED_BODY()


public:

	void InitializeLoadSlots();

	UPROPERTY(BlueprintAssignable)
	FSlotSelected SlotSelected;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMVVM_LoadSlot> LoadSlotViewModelClass;

	UFUNCTION(BlueprintPure)
	UMVVM_LoadSlot* GetLoadSlotViewModelByIndex(int32 Index) const;

	UFUNCTION(BlueprintPure)
	UMVVM_LoadSlot* GetSelectedSlot()const;

	UFUNCTION(BlueprintCallable)
	void NewSlotButtonPressed(int32 Slot, const FString& EnteredName);

	UFUNCTION(BlueprintCallable)
	void NewGameButtonPressed(int32 Slot);

	UFUNCTION(BlueprintCallable)
	void SelectSlotButtonPressed(int32 Slot);

	UFUNCTION(BlueprintCallable)
	void DeleteSlotButtonPressed();

	UFUNCTION(BlueprintCallable)
	void PlayButtonPressed();

	void LoadData();

private:
	UPROPERTY()
	TMap<int32, UMVVM_LoadSlot*> LoadSlots;
	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> LoadSlot_0;
	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> LoadSlot_1;
	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> LoadSlot_2;

	UPROPERTY()
	int32 SelectedSlotIndex = -1;
};
