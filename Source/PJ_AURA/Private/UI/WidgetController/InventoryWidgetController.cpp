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
    InventoryComponent->OnEquipmentUpdateSignature.AddUniqueDynamic(this, &UInventoryWidgetController::UpdateEquipment);
}

void UInventoryWidgetController::InitializeEquipment()
{
}

void UInventoryWidgetController::TryMoveBagItem(int32 FromX, int32 FromY, int32 ToX, int32 ToY)
{
    if (!InventoryComponent) return;

    // 客户端直接调用服务器RPC，将移动请求发送给服务器处理
    InventoryComponent->Server_MoveItem(FromX-1, FromY-1, ToX-1, ToY-1);
}

void UInventoryWidgetController::TryMoveBagItemFromEquip(int32 FromEquipSlotIndex, int32 ToX, int32 ToY)
{
    if (!InventoryComponent) return;
	E_EquipmentSlots FromEquipSlot = static_cast<E_EquipmentSlots>(FromEquipSlotIndex);
    InventoryComponent->Server_UnEquipItem(FromEquipSlot, ToX-1, ToY-1);
}

void UInventoryWidgetController::TryEquipItem(UItem* Item, E_EquipmentSlots EquipmentSlot)
{
    if (!InventoryComponent) return;
    if (!Item) return;

    // 仅处理装备类型的物品
    if (Item->GetItemType() != EItemType::Equipment) return;

    int32 FoundX = 0;
    int32 FoundY = 0;
    // FindItemPosition 返回的是 0-based 坐标
    if (InventoryComponent->FindItemPosition(Item, FoundX, FoundY))
    {
        // 将请求发送到服务器，由服务器执行装备逻辑并同步到客户端
        InventoryComponent->Server_EquipItem(FoundX, FoundY, EquipmentSlot);
    }
}

void UInventoryWidgetController::TryDropItem(int32 FromX, int32 FromY)
{
    if (!InventoryComponent) return;
    
    InventoryComponent->Server_DiscardItem(FromX-1, FromY-1);
}

void UInventoryWidgetController::TryDropEquip(E_EquipmentSlots EquipmentSlot)
{
    if (!InventoryComponent) return;

    InventoryComponent->Server_DiscardEquip(EquipmentSlot);
}


void UInventoryWidgetController::UpdateInventory()
{
    OnUIInventoryUpdateDelegate.Broadcast();
}

void UInventoryWidgetController::UpdateEquipment()
{
    OnUIEquipmentUpdateDelegate.Broadcast();
}