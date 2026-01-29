#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Inventory/Item.h"
#include "Equipment.generated.h"



USTRUCT(BlueprintType)
struct FS_EquipmentData : public FTableRowBase
{
	GENERATED_BODY()

	/** The unique identifier for the item */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "ID", MakeStructureDefaultValue = "None"))
	FName ID;

	/** 装备类型 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	E_EquipmentType EquipmentType;

	/** 装备等级需求 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	int32 EquipmentLevelRequirement;

	/** 装备属性需求 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	TMap<FGameplayTag, float> AttributeRequirements;

	/** 装备增加的属性 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	TMap<FGameplayTag, float> AddedAttributes;
};

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class PJ_AURA_API UEquipment : public UItem
{
	GENERATED_BODY()
		
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	const FS_EquipmentData* GetEquipmentData(const UObject* ContextObject) const;

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	E_EquipmentType GetEquipmentType() const;

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	int32 GetEquipmentLevelRequirement() const;

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	const TMap<FGameplayTag, float>& GetAttributeRequirements() const;

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	const TMap<FGameplayTag, float>& GetAddedAttributes() const;

private:
	// 用于缓存查找到的数据，避免重复查找，标记为mutable以便在const函数中修改
	mutable const FS_EquipmentData* CachedEquipmentDataPtr = nullptr;
};