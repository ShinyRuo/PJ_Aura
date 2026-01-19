	// ALL CODE FOR  learning GAS


#include "Game/AuraGameStateBase.h"
#include "Game/ItemManager.h" // 包含头文件
#include "Net/UnrealNetwork.h"

AAuraGameStateBase::AAuraGameStateBase()
{
}

UItemManager* AAuraGameStateBase::GetItemManager() 
{
	if(!ItemManager)
	{
		ItemManager = NewObject<UItemManager>(this, ItemManagerClass);
		ItemManager->Initialize(GetWorld());
	}
	return ItemManager;
}

