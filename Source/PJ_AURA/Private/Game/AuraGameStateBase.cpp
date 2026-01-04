	// ALL CODE FOR  learning GAS


#include "Game/AuraGameStateBase.h"
#include "Game/ItemManager.h" // 包含头文件
#include "Net/UnrealNetwork.h"

AAuraGameStateBase::AAuraGameStateBase()
{
}

void AAuraGameStateBase::BeginPlay()
{
	Super::BeginPlay();


	ItemManager = NewObject<UItemManager>(this, ItemManagerClass);
	ItemManager->Initialize(GetWorld());
}

