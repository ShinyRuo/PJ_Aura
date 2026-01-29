#include "Inventory/Equipment.h"
#include "Engine/DataTable.h"
#include "Game/ItemManager.h"
#include "Net/UnrealNetwork.h"

void UEquipment::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

const FS_EquipmentData* UEquipment::GetEquipmentData(const UObject* ContextObject) const
{
	// 如果已经缓存了指针，并且ID没有变，直接返回
	if (CachedEquipmentDataPtr && CachedEquipmentDataPtr->ID == ItemID)
	{
		return CachedEquipmentDataPtr;
	}

	// 如果没有上下文对象，无法查找
	if (!ContextObject) return nullptr;
	if (!UItemManager::Get(ContextObject))return nullptr;
	const UDataTable* ItemDataTable = UItemManager::Get(ContextObject)->GetEquipmentDataTable();
	if (ItemDataTable)
	{
		// 在数据表中查找行
		CachedEquipmentDataPtr = ItemDataTable->FindRow<FS_EquipmentData>(ItemID, TEXT("UItem::GetItemData"));
		return CachedEquipmentDataPtr;
	}

	return nullptr;
}

E_EquipmentType UEquipment::GetEquipmentType() const
{
	if (const FS_EquipmentData* Data = GetEquipmentData(this))
	{
		return Data->EquipmentType;
	}
	return E_EquipmentType::EET_MAX;
}

int32 UEquipment::GetEquipmentLevelRequirement() const
{
	if (const FS_EquipmentData* Data = GetEquipmentData(this))
	{
		return Data->EquipmentLevelRequirement;
	}
	return 0;
}

const TMap<FGameplayTag, float>& UEquipment::GetAttributeRequirements() const
{
	if (const FS_EquipmentData* Data = GetEquipmentData(this))
	{
		return Data->AttributeRequirements;
	}
	static const TMap<FGameplayTag, float> EmptyMap;
	return EmptyMap;
}

const TMap<FGameplayTag, float>& UEquipment::GetAddedAttributes() const
{
	if (const FS_EquipmentData* Data = GetEquipmentData(this))
	{
		return Data->AddedAttributes;
	}
	static const TMap<FGameplayTag, float> EmptyMap;
	return EmptyMap;
}

