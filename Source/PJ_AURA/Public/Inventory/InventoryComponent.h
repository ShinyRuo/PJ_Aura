#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventorySlot.h"
#include "Inventory/Item.h"
#include "InventoryComponent.generated.h"

struct FGameplayTag;
class UItem;
class UEquipment;

struct FSavedInventory;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEquipmentUpdate);


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PJ_AURA_API UInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UInventoryComponent();

    virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

    FOnInventoryUpdate OnInventoryUpdateSignature;
    FOnEquipmentUpdate OnEquipmentUpdateSignature;

    UPROPERTY(ReplicatedUsing = OnRep_InventorySize, EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    int32 InventoryWidth; // 背包网格宽度

    UPROPERTY(ReplicatedUsing = OnRep_InventorySize, EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    int32 InventoryHeight; // 背包网格高度

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    int32 MaxCapacity; // 背包最大容量

    UPROPERTY(BlueprintReadOnly, Category = "Inventory")
    int32 CurrentCapacity; // 当前已使用的容量

    UPROPERTY(ReplicatedUsing = OnRep_Slots, EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    TArray<FInventorySlot> Slots; // 背包格子

    UPROPERTY(ReplicatedUsing = OnRep_Equipment, VisibleAnywhere, Category = "Inventory")
    TArray<TObjectPtr<UEquipment>> EquipmentSlots;

    UItem* DuplicateItemByItemType(UItem* Item);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool AddItem(UItem* Item, int32 X, int32 Y,bool DuplicateItem = true);

   

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool RemoveItemByPosition(int32 X, int32 Y);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool FindItemPosition(UItem* Item, int32& OutX, int32& OutY) const;

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool RemoveItem(UItem* Item);

    UFUNCTION(Server, Reliable)
    void Server_MoveItem(int32 FromX, int32 FromY, int32 ToX, int32 ToY);

    UFUNCTION(Server, Reliable)
    void Server_DiscardItem(int32 FromX, int32 FromY);

    UFUNCTION(Server, Reliable)
    void Server_DiscardEquip(E_EquipmentSlots Slot);

    void DropItemFromOwner(UItem* ItemToDrop);

    UFUNCTION(Server, Reliable)
    void Server_EquipItem(int32 FromX, int32 FromY, E_EquipmentSlots Slot);

    UFUNCTION(Server, Reliable)
    void Server_UnEquipItem(E_EquipmentSlots Slot, int32 ToX, int32 ToY);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    UItem* GetItemAt(int32 X, int32 Y) const;

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    UEquipment* GetEquipmentOnSlot(E_EquipmentSlots Slot);

    bool IsSpaceAvailable(UItem* Item, int32 X, int32 Y);

    UFUNCTION(BlueprintPure, Category = "Inventory")
    bool CanAddItem(UItem* Item);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool FindEmptySlotAndAddItem(UItem* Item,bool bDuplicateItem = true);

    void LoadItemSlots(const FSavedInventory& SavedInventory);

    void GetAllEquipmentAddedAttributes(TMap<FGameplayTag, float>& OutAttributesMap) const;

    UFUNCTION()
    void OnRep_InventorySize();

    UFUNCTION()
    void OnRep_Slots();

    UFUNCTION()
    void OnRep_Equipment();

protected:
    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	bool RemoveItemByPositionInternal(int32 X, int32 Y, bool bMarkGarbage=true);
};