#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "InventoryWidgetController.generated.h"

class UInventoryComponent;
class UItem;


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUIInventoryUpdate);


/**
 * 控制背包界面的逻辑层
 */
UCLASS(Blueprintable, BlueprintType)
class PJ_AURA_API UInventoryWidgetController : public UAuraWidgetController
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable, Category = "Inventory|Update")
	FOnUIInventoryUpdate OnUIInventoryUpdateDelegate;

    /* AuraWidgetController */
	virtual void BindCallbackToDependencies() override;

    /** 初始化背包控制器 */
    UFUNCTION(BlueprintCallable, Category = "Inventory|Initialization")
    void Initialize(UInventoryComponent* InInventoryComponent);

    /** 从背包拖动物品到装备栏 */
    UFUNCTION(BlueprintCallable, Category = "Inventory|Interaction")
    bool MoveItemToEquipment(UItem* Item, int32 EquipmentSlotIndex);

    /** 从装备栏拖动物品到背包 */
    UFUNCTION(BlueprintCallable, Category = "Inventory|Interaction")
    bool MoveItemToInventory(UItem* Item, int32 InventoryX, int32 InventoryY);

    /*在背包内移动道具*/
    UFUNCTION(BlueprintCallable, Category = "Inventory|Interaction")
    void TryMoveBagItem(int32 FromX, int32 FromY, int32 ToX, int32 ToY);

    UFUNCTION(BlueprintCallable, Category = "Inventory|Interaction")
    void TryDropItem(int32 FromX, int32 FromY);

    /** 更新背包界面 */
    UFUNCTION(BlueprintCallable, Category = "Inventory|Update")
    void UpdateInventory();

    /** 更新装备栏界面 */
    UFUNCTION(BlueprintCallable, Category = "Inventory|Update")
    void UpdateEquipment();

    /** 背包组件 */
    UPROPERTY(BlueprintReadOnly, Category = "Inventory")
    TObjectPtr<UInventoryComponent> InventoryComponent;

    /** 装备栏数据 */
    UPROPERTY(BlueprintReadOnly, Category = "Inventory")
    TArray<UItem*> EquipmentSlots;

    /** 背包网格宽度 */
    UPROPERTY(BlueprintReadOnly, Category = "Inventory")
    int32 InventoryWidth;

    /** 背包网格高度 */
    UPROPERTY(BlueprintReadOnly, Category = "Inventory")
    int32 InventoryHeight;

    /** 初始化装备栏 */
    void InitializeEquipment();
};