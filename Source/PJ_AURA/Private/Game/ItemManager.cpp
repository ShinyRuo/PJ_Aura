#include "Game/ItemManager.h"
#include "Actor/PickUpItem.h"
#include "Inventory/Item.h"
#include "Engine/World.h"
#include "Game/AuraGameStateBase.h"
#include "Kismet/GameplayStatics.h"

UItemManager* UItemManager::Get( const UObject* WorldContextObject)
{
	// 通过 GameState 获取 ItemManager
	if ( AAuraGameStateBase* GameState = Cast<AAuraGameStateBase>(UGameplayStatics::GetGameState(WorldContextObject)))
	{
		return GameState->GetItemManager();
	}
	return nullptr;
}

void UItemManager::Initialize(UWorld* InWorld)
{
	World = InWorld;
	// 只在服务器上预热对象池
	if (!GetWorld()->IsNetMode(NM_Client))
	{
		PrimePool();
	}
}

APickUpItem* UItemManager::SpawnItemOnTheFloor(const FName& ItemID, const int32 ItemQuantity, const FVector& WorldLocation)
{
	if (GetWorld()->IsNetMode(NM_Client)) return nullptr;

	if ( !PickUpItemClass)
	{
		return nullptr;
	}

	APickUpItem* PickUpItem = nullptr;

	// 1. 检查可用池
	if (!AvailableItemsPool.IsEmpty())
	{
		PickUpItem = AvailableItemsPool.Pop();
	}
	else // 2. 如果池子已空，则扩容
	{
		PickUpItem = GetWorld()->SpawnActor<APickUpItem>(PickUpItemClass, FVector::ZeroVector, FRotator::ZeroRotator);
		if (PickUpItem)
		{
			ItemPool.Add(PickUpItem); // 添加到主池进行追踪
		}
	}

	if (PickUpItem)
	{
		// 3. 激活物品并分配ID
		const int64 NewID = GenerateUniqueID();
		PickUpItem->ActivateItem(NewID, ItemID, ItemQuantity, WorldLocation);
		ActiveItems.Add(NewID, PickUpItem);
	}

	return PickUpItem;
}

void UItemManager::ReleaseItem(APickUpItem* ItemToRelease)
{
	if (GetWorld()->IsNetMode(NM_Client)) return ;

	if (!ItemToRelease || ItemToRelease->ItemID == -1)
	{
		return;
	}

	// 从活动列表中移除
	ActiveItems.Remove(ItemToRelease->ItemID);

	// 停用物品并将其放回可用池
	ItemToRelease->DeactivateItem();
	AvailableItemsPool.Add(ItemToRelease);
}

void UItemManager::PrimePool()
{
	if (!PickUpItemClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemManager: PickUpItemClass is not set. Cannot prime the pool."));
		return;
	}

	for (int32 i = 0; i < InitialPoolSize; ++i)
	{
		APickUpItem* NewItem = GetWorld()->SpawnActor<APickUpItem>(PickUpItemClass, FVector::ZeroVector, FRotator::ZeroRotator);
		if (NewItem)
		{
			NewItem->DeactivateItem(); // 初始为非活动状态
			ItemPool.Add(NewItem);
			AvailableItemsPool.Add(NewItem);
		}
	}
}

int64 UItemManager::GenerateUniqueID()
{
	return ++NextItemID;
}