#include "Inventory/InventoryComponent.h"
#include "Inventory/Item.h"
#include "Net/UnrealNetwork.h"

UInventoryComponent::UInventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    InventoryWidth = 10;
    InventoryHeight = 5;
    MaxCapacity = InventoryWidth * InventoryHeight; // 默认最大容量
    CurrentCapacity = 0; // 初始已使用容量为 0
    SetIsReplicatedByDefault(true); // 确保组件是可复制的
}

void UInventoryComponent::BeginPlay()
{
    Super::BeginPlay();
    Slots.SetNum(MaxCapacity);
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME_CONDITION(UInventoryComponent, Slots, COND_OwnerOnly);
}

void UInventoryComponent::OnRep_Slots()
{
    // 更新客户端 UI 或其他逻辑
    OnInventoryUpdateSignature.Broadcast();
}


//int32 X, int32 Y : 物品在背包网格中的起始位置（左上角坐标）。
bool UInventoryComponent::AddItem(UItem* Item, int32 X, int32 Y)
{
    if (!GetOwner()->HasAuthority()) return false; // 确保只在服务器上执行

    if (!Item || !IsSpaceAvailable(Item, X, Y)) return false;

    const FS_ItemData* ItemData = Item->GetItemData(this);
    if (!ItemData) return false;

    // 检查是否超出容量
    int32 ItemSize = ItemData->dimensions.X * ItemData->dimensions.Y;
    if (CurrentCapacity + ItemSize > MaxCapacity)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot add item: Exceeds inventory capacity!"));
        return false;
    }

    // --- 新增：创建物品的副本 ---
    UItem* NewItem = DuplicateObject<UItem>(Item, this);
    if (!NewItem)
    {
        // 如果复制失败，则直接返回
        return false;
    }

    // 添加物品到背包
    for (int32 i = 0; i < ItemData->dimensions.X; ++i)
    {
        for (int32 j = 0; j < ItemData->dimensions.Y; ++j)
        {
            int32 Index = (Y + j) * InventoryWidth + (X + i);
            Slots[Index].Item = NewItem;
            Slots[Index].X = X;
            Slots[Index].Y = Y;
        }
    }

    // 更新当前容量
    CurrentCapacity += ItemSize;
    OnInventoryUpdateSignature.Broadcast(); // 在服务器上广播更新

    return true;
}

bool UInventoryComponent::RemoveItemByPosition(int32 X, int32 Y)
{
    if (!GetOwner()->HasAuthority()) return false; // 确保只在服务器上执行

    UItem* Item = GetItemAt(X, Y);
    if (!Item) return false;

    const FS_ItemData* ItemData = Item->GetItemData(this);
    if (!ItemData) return false; // 虽然不太可能发生，但做好检查

    // 移除物品
    for (int32 i = 0; i < ItemData->dimensions.X; ++i)
    {
        for (int32 j = 0; j < ItemData->dimensions.Y; ++j)
        {
            int32 Index = (Y + j) * InventoryWidth + (X + i);
            if (Slots.IsValidIndex(Index) && Slots[Index].Item == Item)
            {
                Slots[Index].Item = nullptr;
            }
        }
    }

    // 更新当前容量
    CurrentCapacity -= ItemData->dimensions.X * ItemData->dimensions.Y;
    OnInventoryUpdateSignature.Broadcast(); // 在服务器上广播更新

    return true;
}

UItem* UInventoryComponent::GetItemAt(int32 X, int32 Y) const
{
    int32 Index = Y * InventoryWidth + X;
    return Slots.IsValidIndex(Index) ? Slots[Index].Item : nullptr;
}

bool UInventoryComponent::IsSpaceAvailable(UItem* Item, int32 X, int32 Y) 
{
    if (!Item) return false;

    const FS_ItemData* ItemData = Item->GetItemData(this);
    if (!ItemData) return false;

    for (int32 i = 0; i < ItemData->dimensions.X; ++i)
    {
        for (int32 j = 0; j < ItemData->dimensions.Y; ++j)
        {
            int32 CurrentX = X + i;
            int32 CurrentY = Y + j;

            // 检查是否越界
            if (CurrentX < 0 || CurrentX >= InventoryWidth || CurrentY < 0 || CurrentY >= InventoryHeight)
            {
                return false;
            }

            int32 Index = CurrentY * InventoryWidth + CurrentX;
            if (Slots[Index].Item != nullptr)
            {
                return false;
            }
        }
    }
    return true;
}

bool UInventoryComponent::CanAddItem(UItem* Item) 
{
    if (!Item) return false;

    const FS_ItemData* ItemData = Item->GetItemData(this);
    if (!ItemData) return false;

    // 1. 检查是否可以堆叠
    if (ItemData->maxStack > 1)
    {
        for (const FInventorySlot& Slot : Slots)
        {
            if (Slot.Item)
            {
                const FS_ItemData* ExistingItemData = Slot.Item->GetItemData(this);
                if (ExistingItemData && ExistingItemData->ID == ItemData->ID && Slot.Item->Quantity < ExistingItemData->maxStack)
                {
                    return true; // 找到了可以堆叠的位置
                }
            }
        }
    }

    // 2. 检查是否有新的空槽位
    for (int32 Y = 0; Y < InventoryHeight; ++Y)
    {
        for (int32 X = 0; X < InventoryWidth; ++X)
        {
            if (IsSpaceAvailable(Item, X, Y))
            {
                return true; // 找到了一个空的槽位
            }
        }
    }

    return false; // 既不能堆叠，也没有空位
}

bool UInventoryComponent::FindEmptySlotAndAddItem(UItem* Item)
{
	if (!Item) return false;

    const FS_ItemData* ItemData = Item->GetItemData(this);
    if (!ItemData) return false;

    // 1. 尝试堆叠
    if (ItemData->maxStack > 1)
    {
        for (FInventorySlot& Slot : Slots)
        {
            if (Slot.Item)
            {
                const FS_ItemData* ExistingItemData = Slot.Item->GetItemData(this);
                if (ExistingItemData && ExistingItemData->ID == ItemData->ID && Slot.Item->Quantity < ExistingItemData->maxStack)
                {
                    int32 CanAddAmount = ExistingItemData->maxStack - Slot.Item->Quantity;
                    int32 AmountToAdd = FMath::Min(Item->Quantity, CanAddAmount);

                    Slot.Item->Quantity += AmountToAdd;
                    Item->Quantity -= AmountToAdd;

                    OnInventoryUpdateSignature.Broadcast(); // 广播更新

                    if (Item->Quantity <= 0)
                    {
                        return true; // 物品已完全堆叠
                    }
                }
            }
        }
    }

    // 2. 如果还有剩余或物品不可堆叠，则寻找新槽位
    if (Item->Quantity > 0)
    {
        for (int32 Y = 0; Y < InventoryHeight; ++Y)
        {
            for (int32 X = 0; X < InventoryWidth; ++X)
            {
                if (IsSpaceAvailable(Item, X, Y))
                {
                    AddItem(Item, X, Y);
                    return true;
                }
            }
        }
    }
    return false;
}

bool UInventoryComponent::FindItemPosition(UItem* Item, int32& OutX, int32& OutY) const
{
    if (!Item) return false;

    for (int32 Y = 0; Y < InventoryHeight; ++Y)
    {
        for (int32 X = 0; X < InventoryWidth; ++X)
        {
            int32 Index = Y * InventoryWidth + X;
            if (Slots.IsValidIndex(Index) && Slots[Index].Item == Item)
            {
                OutX = X;
                OutY = Y;
                return true;
            }
        }
    }

    return false;
}

bool UInventoryComponent::RemoveItem(UItem* Item)
{
    if (!GetOwner()->HasAuthority()) return false; // 确保只在服务器上执行

    if (!Item) return false;

    int32 X, Y;
    if (!FindItemPosition(Item, X, Y)) return false;

    return RemoveItemByPosition(X, Y);
}