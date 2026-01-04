// ALL CODE FOR wangjunyang learning GAS

#pragma once

#include "CoreMinimal.h"
#include "HighlightInterface.h"
#include "UObject/Interface.h"
#include "EnemyInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UEnemyInterface : public UHighlightInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PJ_AURA_API IEnemyInterface : public IHighlightInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	void SetCombatTarget(AActor* InCombatTarget);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	AActor* GetCombatTarget()const;
};
