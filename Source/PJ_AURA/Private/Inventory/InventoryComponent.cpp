#include "Inventory/InventoryComponent.h"

#include "NavigationSystem.h"
#include "Actor/PickUpItem.h"
#include "Game/ItemManager.h"
#include "Game/LoadScreenSaveGame.h"
#include "Inventory/Item.h"
#include "Net/UnrealNetwork.h"
#include "Player/AuraPlayerState.h"

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
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME_CONDITION(UInventoryComponent, Slots, COND_OwnerOnly);
    DOREPLIFETIME_CONDITION(UInventoryComponent, InventoryWidth, COND_OwnerOnly);
    DOREPLIFETIME_CONDITION(UInventoryComponent, InventoryHeight, COND_OwnerOnly);
}

void UInventoryComponent::OnRep_Slots()
{
    // 更新客户端 UI 或其他逻辑
    OnInventoryUpdateSignature.Broadcast();
}

void UInventoryComponent::OnRep_InventorySize()
{
    MaxCapacity = InventoryWidth * InventoryHeight;
    Slots.SetNum(MaxCapacity);
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
                Slots[Index].X = 0;
                Slots[Index].Y = 0;
            }
        }
    }

    // 更新当前容量
    CurrentCapacity -= ItemData->dimensions.X * ItemData->dimensions.Y;
    OnInventoryUpdateSignature.Broadcast(); // 在服务器上广播更新

    return true;
}


void UInventoryComponent::Server_DiscardItem_Implementation(int32 FromX, int32 FromY)
{
    UItem* ItemToDiscard = GetItemAt(FromX, FromY);
    if (ItemToDiscard)
    {
        // 1. 从背包中移除物品
        const bool bRemoved = RemoveItemByPosition(FromX, FromY);
        if (!bRemoved) return; // 如果移除失败，则中止

        // 2. 在角色身边生成掉落物
        if (UWorld* World = GetWorld())
        {
            AActor* OwnerActor = GetOwner();
            if (!OwnerActor) return;
            FVector CharacterLocation = FVector::ZeroVector;
            FVector DropLocation = CharacterLocation;
            if (const APlayerState* PlayerState = Cast<APlayerState>(OwnerActor))
            {
                if (const APawn* Pawn = PlayerState->GetPawn())
                {
                    CharacterLocation = Pawn->GetActorLocation();
                }
            }
            // 3. 计算随机生成位置
            FNavLocation NavLocation;
            if (UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(World))
            {
                // 在角色周围半径200的圆内寻找一个随机的可导航点
                const bool bFoundPoint = NavSys->GetRandomPointInNavigableRadius(CharacterLocation, 200.f, NavLocation);
                if (bFoundPoint)
                {
                    DropLocation = NavLocation.Location;
                }
            }
            if (UItemManager::Get(this))
            {
                APickUpItem* DropItem =  UItemManager::Get(this)->SpawnItemOnTheFloor(ItemToDiscard->ItemID, ItemToDiscard->Quantity, CharacterLocation);
                DropItem->OnItemDropped(DropLocation);
            }

            
        }
    }
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

void UInventoryComponent::LoadItemSlots(const FSavedInventory& SavedInventory)
{
    for ( const FSavedItemSlot& SavedOneSlot: SavedInventory.ItemSlots)
    {
        UItem* NewItem = NewObject<UItem>(this);
        NewItem->ItemID = SavedOneSlot.ItemID;
        NewItem->Quantity = SavedOneSlot.Quantity;
        const FS_ItemData* ItemData = NewItem->GetItemData(this);
        if (!ItemData) return;
        int32 X = SavedOneSlot.X;
        int32 Y = SavedOneSlot.Y;

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
        CurrentCapacity += X*Y;
    }
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

void UInventoryComponent::Server_MoveItem_Implementation(int32 FromX, int32 FromY, int32 ToX, int32 ToY)
{
    if (!GetOwner()->HasAuthority()) return;

    UItem* ItemToMove = GetItemAt(FromX, FromY);
    if (!ItemToMove) return;

    // 如果起始和目标位置相同，则不执行任何操作
    if (FromX == ToX && FromY == ToY) return;

    // 1. 找到 FromX, FromY 对应的 ItemData 并收集占用的槽位信息 A
    const FS_ItemData* ItemData = ItemToMove->GetItemData(this);
    if (!ItemData) return;

    TArray<int32> SourceIndices;
    for (int32 j = 0; j < ItemData->dimensions.Y; ++j)
    {
        for (int32 i = 0; i < ItemData->dimensions.X; ++i)
        {
            SourceIndices.Add((FromY + j) * InventoryWidth + (FromX + i));
        }
    }

    // 2. 检测 ToX, ToY 位置是否有足够的空间放下该物品，判断的时候忽略自己，并收集目标槽位信息 B
    TArray<int32> TargetIndices;
    bool bIsSpaceAvailable = true;
    for (int32 j = 0; j < ItemData->dimensions.Y; ++j)
    {
        for (int32 i = 0; i < ItemData->dimensions.X; ++i)
        {
            const int32 CurrentX = ToX + i;
            const int32 CurrentY = ToY + j;

            // 检查是否越界
            if (CurrentX < 0 || CurrentX >= InventoryWidth || CurrentY < 0 || CurrentY >= InventoryHeight)
            {
                bIsSpaceAvailable = false;
                break;
            }

            const int32 TargetIndex = CurrentY * InventoryWidth + CurrentX;
            TargetIndices.Add(TargetIndex);

            // 检查目标槽位是否被其他物品占用
            UItem* ExistingItem = Slots[TargetIndex].Item;
            if (ExistingItem != nullptr && ExistingItem != ItemToMove)
            {
                bIsSpaceAvailable = false;
                break;
            }
        }
        if (!bIsSpaceAvailable) break;
    }

    // 如果空间不足，则中止操作
    if (!bIsSpaceAvailable) return;

    // 3. 将 A 位置的槽位清空，将 B 位置的槽位设置为 ItemData
    // 清空 A 位置
    for (const int32 Index : SourceIndices)
    {
        if (Slots.IsValidIndex(Index))
        {
            Slots[Index].Item = nullptr;
            Slots[Index].X = 0;
            Slots[Index].Y = 0;
        }
    }

    // 填充 B 位置
    for (const int32 Index : TargetIndices)
    {
        if (Slots.IsValidIndex(Index))
        {
            Slots[Index].Item = ItemToMove;
            Slots[Index].X = ToX;
            Slots[Index].Y = ToY;
        }
    }

    // 在服务器上广播更新，以便服务器本地UI（如果有）可以立即响应。
    // 客户端将通过 OnRep_Slots 自动接收更新。
    OnInventoryUpdateSignature.Broadcast();
}
