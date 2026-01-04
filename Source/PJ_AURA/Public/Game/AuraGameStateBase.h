// ALL CODE FOR  learning GAS

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "AuraGameStateBase.generated.h"

class UItemManager; // 向前声明

UCLASS()
class PJ_AURA_API AAuraGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	AAuraGameStateBase();

	virtual void BeginPlay() override;
	// 添加一个公共的获取器，方便外部访问
	UItemManager* GetItemManager() const { return ItemManager; }


protected:
	// 使用 UPROPERTY 来确保它被正确地进行垃圾回收和复制
	UPROPERTY( BlueprintReadOnly, Category = "Managers")
	TObjectPtr<UItemManager> ItemManager;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UItemManager> ItemManagerClass;
};
