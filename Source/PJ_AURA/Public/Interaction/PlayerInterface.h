// ALL CODE FOR  learning GAS

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerInterface.generated.h"

class UItem;
// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PJ_AURA_API IPlayerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent)
	void AddToExp(int32 InExp);

	UFUNCTION(BlueprintNativeEvent)
	int32 GetExp();

	UFUNCTION(BlueprintNativeEvent)
	void LevelUp();

	UFUNCTION(BlueprintNativeEvent)
	int32 FindLevelForExp(int32 InExp);

	UFUNCTION(BlueprintNativeEvent)
	int32 GetAttributePointsRewards(int32 Level) const;

	UFUNCTION(BlueprintNativeEvent)
	int32 GetSpellPointsRewards(int32 Level) const;

	UFUNCTION(BlueprintNativeEvent)
	void AddToPlayerLevel(int32 InPlayLevel);

	UFUNCTION(BlueprintNativeEvent)
	void AddToAttributePoints(int32 InAttributePoints);

	UFUNCTION(BlueprintNativeEvent)
	void AddToSpellPoints(int32 InSpellPoints);

	UFUNCTION(BlueprintNativeEvent)
	int32 GetAttributePoints() const;

	UFUNCTION(BlueprintNativeEvent)
	int32 GetSpellPoints() const;

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	void ShowMagicCircle(UMaterialInterface* DecalMaterial = nullptr);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void HideMagicCircle();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SaveProgress(const FName& CheckPointTag);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void PickUpItem(UItem* PickUpItem);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void DropItem(UItem* DropItem);
	
};
