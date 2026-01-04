// ALL CODE FOR wangjunyang learning GAS

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AuraGameModeBase.generated.h"

class UItemManager;
class ULoadScreenSaveGame;
class USaveGame;
class UMVVM_LoadSlot;
class UAbilityInfo;
class UCharacterClassInfo;
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class PJ_AURA_API AAuraGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly,Category = "Character Class Default")
	TObjectPtr<UCharacterClassInfo> CharacterClassInfo;

	UPROPERTY(EditDefaultsOnly, Category = "AbilityInfo")
	TObjectPtr<UAbilityInfo> AbilityInfo;


	void SaveSlotData(UMVVM_LoadSlot* LoadSlot,int32 SlotIndex);

	ULoadScreenSaveGame* GetSaveSlotData(const FString& SlotName, int32 SlotIndex) const;

	static void DeleteSlot(const FString& SlotName, int32 SlotIndex);

	ULoadScreenSaveGame* RetrieveInGameSaveData()const;

	void SaveInGameProgressData(ULoadScreenSaveGame* SaveObject) const;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<USaveGame> LoadScreenSaveGameClass;

	UPROPERTY(EditDefaultsOnly)
	FString DefaultMapName;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UWorld> DefaultMap;

	UPROPERTY(EditDefaultsOnly)
	FName DefaultPlayerStartTag ;

	UPROPERTY(EditDefaultsOnly)
	TMap<FString, TSoftObjectPtr<UWorld>> Maps;

	void TravelToMap(UMVVM_LoadSlot* Slot);

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	
	
protected:
	virtual void BeginPlay() override;

};
