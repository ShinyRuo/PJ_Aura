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

void UInventoryWidgetController::TryMoveBagItem(int32 FromX, int32 FromY, int32 ToX, int32 ToY)
{
    if (!InventoryComponent) return;

    // 客户端直接调用服务器RPC，将移动请求发送给服务器处理
    InventoryComponent->Server_MoveItem(FromX-1, FromY-1, ToX-1, ToY-1);
}

void UInventoryWidgetController::TryDropItem(int32 FromX, int32 FromY)
{
    if (!InventoryComponent) return;
    
    InventoryComponent->Server_DiscardItem(FromX-1, FromY-1);
}


void UInventoryWidgetController::UpdateInventory()
{
    OnUIInventoryUpdateDelegate.Broadcast();
}

void UInventoryWidgetController::UpdateEquipment()
{
    // TODO: 通知界面更新装备栏显示
}