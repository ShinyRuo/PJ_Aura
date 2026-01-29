#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ItemManager.generated.h"

class UDataTable;
class APickUpItem;
class UItem;

UCLASS(BlueprintType, Blueprintable)
class PJ_AURA_API UItemManager : public UObject
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure, Category = "ItemManager", meta = (WorldContext = "WorldContextObject"))
	static UItemManager* Get(const UObject* WorldContextObject);


	virtual void Initialize(UWorld* InWorld) ;

	/**
	 * 从对象池获取或创建一个 APickUpItem 实例，并在指定位置生成。
	 * @param ItemClass 要生成的物品的数据类。
	 * @param WorldLocation 生成的位置。
	 * @return 返回激活的 APickUpItem 实例，如果失败则返回 nullptr。
	 */
	APickUpItem* SpawnItemOnTheFloor(const FName& ItemID,const int32 ItemQuantity, const FVector& WorldLocation);

	/**
	 * 回收一个 APickUpItem 实例到对象池中。
	 * @param ItemToRelease 要回收的物品实例。
	 */
	void ReleaseItem(APickUpItem* ItemToRelease);

	UDataTable* GetItemDataTable() const { return ItemDataTable; }

	UDataTable* GetEquipmentDataTable() const { return EquipmentDataTable; }

private:
	/** 预热对象池，创建初始数量的 APickUpItem */
	void PrimePool();

	/** 生成一个新的唯一物品ID */
	int64 GenerateUniqueID();

	/** 初始池大小 */
	UPROPERTY(EditDefaultsOnly, Category = "ItemManager", meta = (AllowPrivateAccess = "true"))
	int32 InitialPoolSize = 50;

	/** 用于在世界中生成 APickUpItem 的蓝图类 */
	UPROPERTY(EditDefaultsOnly, Category = "ItemManager", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<APickUpItem> PickUpItemClass;

	/** 存储所有被管理的 APickUpItem 实例，包括活动和非活动的 */
	UPROPERTY()
	TArray<TObjectPtr<APickUpItem>> ItemPool;

	/** 存储当前可用的（非活动的）APickUpItem 实例 */
	UPROPERTY()
	TArray<TObjectPtr<APickUpItem>> AvailableItemsPool;

	/** 存储当前活动在世界中的物品，通过ID快速查找 */
	UPROPERTY()
	TMap<int64, TObjectPtr<APickUpItem>> ActiveItems;

	/** 用于生成唯一ID的计数器 */
	int64 NextItemID = 0;

	UPROPERTY(EditDefaultsOnly, Category = "ItemManager", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDataTable> ItemDataTable;

	UPROPERTY(EditDefaultsOnly, Category = "ItemManager", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDataTable> EquipmentDataTable;

	/** 保存对世界的弱引用，因为 UObject 本身没有 GetWorld() */
	UPROPERTY()
	TWeakObjectPtr<UWorld> World;
};