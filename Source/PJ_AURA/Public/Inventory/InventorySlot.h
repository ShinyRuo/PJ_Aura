#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "InventorySlot.generated.h"

USTRUCT(BlueprintType)
struct PJ_AURA_API FInventorySlot
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    TObjectPtr<UItem> Item; // 当前格子中的物品

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    int32 X; // 格子在网格中的 X 坐标

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    int32 Y; // 格子在网格中的 Y 坐标
};