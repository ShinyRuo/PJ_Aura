#include "UI/WidgetController/InventoryWidgetController.h"
#include "Inventory/InventoryComponent.h"
#include "Inventory/Item.h"



void UInventoryWidgetController::Initialize(UInventoryComponent* InInventoryComponent)
{
    if (!InInventoryComponent) return;

    InventoryComponent = InInventoryComponent;
    InventoryWidth = InventoryComponent->InventoryWidth;
    InventoryHeight = InventoryComponent->InventoryHeight;

    // 初始化装备栏
    InitializeEquipment();

    UpdateEquipment();
}

void UInventoryWidgetController::BindCallbackToDependencies()
{
    if (!InventoryComponent) return;
    
    InventoryComponent->OnInventoryUpdateSignature.AddUniqueDynamic(this, &UInventoryWidgetController::UpdateInventory);
}

void UInventoryWidgetController::InitializeEquipment()
{
    // 假设装备栏有固定数量的槽位
    const int32 NumEquipmentSlots = 10; // 示例：10个装备槽
    EquipmentSlots.SetNum(NumEquipmentSlots);
}

bool UInventoryWidgetController::MoveItemToEquipment(UItem* Item, int32 EquipmentSlotIndex)
{
    if (!Item || !EquipmentSlots.IsValidIndex(EquipmentSlotIndex)) return false;

    // 检查装备槽是否为空
    if (EquipmentSlots[EquipmentSlotIndex] != nullptr) return false;

    // 从背包移除物品
    if (!InventoryComponent->RemoveItem(Item)) return false;

    // 放入装备槽
    EquipmentSlots[EquipmentSlotIndex] = Item;

    // 更新界面
    UpdateInventory();
    UpdateEquipment();

    return true;
}

bool UInventoryWidgetController::MoveItemToInventory(UItem* Item, int32 InventoryX, int32 InventoryY)
{
    if (!Item || !InventoryComponent->IsSpaceAvailable(Item, InventoryX, InventoryY)) return false;

    // 从装备栏移除物品
    int32 SlotIndex = EquipmentSlots.Find(Item);
    if (SlotIndex != INDEX_NONE)
    {
        EquipmentSlots[SlotIndex] = nullptr;
    }

    // 添加到背包
    if (!InventoryComponent->AddItem(Item, InventoryX, InventoryY)) return false;

    // 更新界面
    UpdateInventory();
    UpdateEquipment();

    return true;
}

void UInventoryWidgetController::DropItem(UItem* Item)
{
    if (!Item) return;

    // 从背包或装备栏移除物品
    InventoryComponent->RemoveItem(Item);
    int32 SlotIndex = EquipmentSlots.Find(Item);
    if (SlotIndex != INDEX_NONE)
    {
        EquipmentSlots[SlotIndex] = nullptr;
    }

    // 触发丢弃逻辑（例如生成物品掉落到地面）
    // TODO: 实现丢弃逻辑

    // 更新界面
    UpdateInventory();
    UpdateEquipment();
}

void UInventoryWidgetController::UpdateInventory()
{
    OnUIInventoryUpdateDelegate.Broadcast();
}

void UInventoryWidgetController::UpdateEquipment()
{
    // TODO: 通知界面更新装备栏显示
}