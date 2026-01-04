#include "Inventory/Item.h"

#include "Actor/PickUpItem.h"
#include "Game/AuraGameInstance.h"
#include "Game/ItemManager.h"
#include "Kismet/GameplayStatics.h" // 需要包含以使用查找游戏实例等功能
#include "Net/UnrealNetwork.h"


void UItem::OnRep_ItemID()
{
    NotifyOwningActor();
}

void UItem::OnRep_Quantity()
{
    NotifyOwningActor();
}

void UItem::NotifyOwningActor()
{
    // 获取拥有此UObject的Actor
    if (APickUpItem* OwningItem = Cast<APickUpItem>(GetOuter()))
    {
        // 调用Actor的更新函数
        OwningItem->OnRep_Item();
    }
}

const FS_ItemData* UItem::GetItemData(const UObject* ContextObject) const
{
    // 如果已经缓存了指针，并且ID没有变，直接返回
    if (CachedItemDataPtr && CachedItemDataPtr->ID == ItemID)
    {
        return CachedItemDataPtr;
    }

    // 如果没有上下文对象，无法查找
    if (!ContextObject) return nullptr;
    if (!UItemManager::Get(ContextObject))return nullptr;
    const UDataTable* ItemDataTable = UItemManager::Get(ContextObject)->GetItemDataTable();
    if (ItemDataTable)
    {
        // 在数据表中查找行
        CachedItemDataPtr = ItemDataTable->FindRow<FS_ItemData>(ItemID, TEXT("UItem::GetItemData"));
        return CachedItemDataPtr;
    }

    return nullptr;
}

UStaticMesh* UItem::GetStaticMesh() const
{
    const FS_ItemData* Data = GetItemData(this);
    return Data ? Data->pickUpStaticMesh : nullptr;
}

USkeletalMesh* UItem::GetSkeletalMesh() const
{
    const FS_ItemData* Data = GetItemData(this);
    return Data ? Data->pickUpSkeletalMesh : nullptr;
}

USoundCue* UItem::GetPickUpSound() const
{
    const FS_ItemData* Data = GetItemData(this);
    return Data ? Data->pickUpSound : nullptr;
}

USoundCue* UItem::GetDropSound() const
{
    const FS_ItemData* Data = GetItemData(this);
    return Data ? Data->dropSound : nullptr;
}

float UItem::GetLifeTimeOnGround() const
{
    const FS_ItemData* Data = GetItemData(this);
    return Data ? Data->lifeTimeOnGround : 0.0f;
}

FString UItem::GetItemDescription(UObject* ContextObject) const
{
    const FS_ItemData* Data = GetItemData(ContextObject);
    if (Data)
    {
        return Data->description.ToString();
    }
    return FString();
}



void UItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UItem, ItemID);
    DOREPLIFETIME(UItem, Quantity);
}

bool UItem::IsSupportedForNetworking() const
{
    return true;
}