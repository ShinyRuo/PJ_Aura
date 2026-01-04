// ALL CODE FOR  learning GAS

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoadScreenWidget.generated.h"

/**
 * 
 */
UCLASS()
class PJ_AURA_API ULoadScreenWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)
	void BlueprintInitializeWidget();
};
