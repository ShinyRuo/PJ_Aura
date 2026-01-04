#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "UObject/NoExportTypes.h"
#include "Item.generated.h"

class USoundCue;

UENUM(BlueprintType)
enum class EItemType : uint8
{
	UsableItem,
	Equipment,
	Socketable,
	QuestItem,
	Misc,
	Container,
};

/** A struct representing the data of an item in the inventory. */
USTRUCT(BlueprintType)
struct FS_ItemData : public FTableRowBase
{
	GENERATED_BODY()
public:
	/** The unique identifier for the item */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "ID", MakeStructureDefaultValue = "None"))
	FName ID;

	/** The display name of the item */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "name"))
	FText name;

	/** A brief subtitle for the item */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "subTitle"))
	FText subTitle;

	/** A brief description of the item */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "description"))
	FText description;

	/** The type of item (e.g., Usable, Equipment) */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "itemType", MakeStructureDefaultValue = "NewEnumerator0"))
	TEnumAsByte<EItemType> itemType;

	/** The dimensions of the item (width and height) */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "dimensions", MakeStructureDefaultValue = "(X=1,Y=1)"))
	FIntPoint dimensions;

	/** The image representing the item */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "image", MakeStructureDefaultValue = "None"))
	TObjectPtr<UTexture2D> image;

	/** The rotated image of the item */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "rotatedImage", MakeStructureDefaultValue = "None"))
	TObjectPtr<UTexture2D> rotatedImage;

	/** The maximum number of items that can be stacked */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "maxStack", MakeStructureDefaultValue = "1"))
	int32 maxStack;

	/** The selling price of the item */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "price", MakeStructureDefaultValue = "1"))
	int32 price;

	/** The duration (in seconds) the item remains on the ground before despawning */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "lifeTimeOnGround", MakeStructureDefaultValue = "60.000000"))
	double lifeTimeOnGround;

	/** The sound that plays when the item is picked up */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "pickUpSound", MakeStructureDefaultValue = "None"))
	TObjectPtr<USoundCue> pickUpSound;

	/** The sound that plays when the item is dropped */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "dropSound", MakeStructureDefaultValue = "None"))
	TObjectPtr<USoundCue> dropSound;

	/** The static mesh used for item pickup visualization */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "pickUpStaticMesh", MakeStructureDefaultValue = "None"))
	TObjectPtr<UStaticMesh> pickUpStaticMesh;

	/** The skeletal mesh used for item pickup visualization */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "pickUpSkeletalMesh", MakeStructureDefaultValue = "None"))
	TObjectPtr<USkeletalMesh> pickUpSkeletalMesh;

	/** The blueprint class used for custom item pickup behavior */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "customPickUpBP", MakeStructureDefaultValue = "None"))
	TObjectPtr<UClass> customPickUpBP;
};


UCLASS(Blueprintable, BlueprintType)
class PJ_AURA_API UItem : public UObject
{
	GENERATED_BODY()

public:


	//~ Begin UObject Interface
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool IsSupportedForNetworking() const override;
	//~ End UObject Interface


	// -- 运行时数据 (Runtime Data) --

	/** 物品的ID，用于查找其静态数据 */
	UPROPERTY(ReplicatedUsing = OnRep_ItemID,EditAnywhere, BlueprintReadWrite, Category = "ItemData")
	FName ItemID;

	/** 当前的堆叠数量 */
	UPROPERTY(ReplicatedUsing = OnRep_Quantity ,EditAnywhere, BlueprintReadWrite, Category = "ItemData")
	int32 Quantity;

	UFUNCTION()
	void OnRep_ItemID();

	UFUNCTION()
	void OnRep_Quantity();


	// 用于在子对象的RepNotify中通知其所有者
	void NotifyOwningActor();

	// -- 访问器 (Accessors) --

	/**
	 * 获取此物品的静态数据。
	 * @param ContextObject 用于查找数据表的上下文对象 (例如，可以是持有此物品的组件或Actor)。
	 * @return 返回一个指向FS_ItemData的常量指针，如果找不到则返回nullptr。
	 */
	const FS_ItemData* GetItemData(const UObject* ContextObject) const;

	UStaticMesh* GetStaticMesh() const;

	USkeletalMesh* GetSkeletalMesh() const;

	USoundCue* GetPickUpSound() const;

	USoundCue* GetDropSound() const;

	float GetLifeTimeOnGround() const;

	UFUNCTION(BlueprintCallable, Category = "ItemData")
	FString GetItemDescription(UObject* ContextObject) const;

private:
	// 用于缓存查找到的数据，避免重复查找，标记为mutable以便在const函数中修改
	mutable const FS_ItemData* CachedItemDataPtr = nullptr;
};