// ALL CODE FOR  learning GAS

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "DamageTextComponent.generated.h"

/**
 * 
 */
UCLASS()
class PJ_AURA_API UDamageTextComponent : public UWidgetComponent
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)
	void SetDamageText(float Damage);
};
