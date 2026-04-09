// ALL CODE FOR  learning GAS

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "AuraGameInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMapLoadedSignature, const FString&, MapName);


class UDataTable;
/**
 * 
 */
UCLASS()
class PJ_AURA_API UAuraGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UPROPERTY()
	FName PlayerStartTag = FName("PlayerStart");

	UPROPERTY()
	FString LoadSlotName = FString();

	UPROPERTY()
	int32 LoadSlotIndex = 0;

};
