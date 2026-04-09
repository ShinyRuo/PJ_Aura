// ALL CODE FOR  learning GAS


#include "Game/AuraGameInstance.h"
#include "Kismet/GameplayStatics.h"


void UAuraGameInstance::Init()
{
	Super::Init();
	// 绑定地图加载完成的全局回调
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UAuraGameInstance::HandlePostLoadMapWithWorld);
}

void UAuraGameInstance::Shutdown()
{
	// 一定要移除绑定，避免悬挂引用
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);

	Super::Shutdown();
}


void UAuraGameInstance::HandlePostLoadMapWithWorld(UWorld* LoadedWorld)
{
	if (!LoadedWorld)
	{
		return;
	}

	// 可选：只在游戏世界生效（过滤编辑器情况）
	if (!LoadedWorld->IsGameWorld())
	{
		return;
	}

	// 获取当前地图名（去掉前缀）
	const FString MapName = UGameplayStatics::GetCurrentLevelName(LoadedWorld, /*bRemovePrefixString=*/ true);

	// 广播给蓝图/Widget，做小地图切换
	OnMapLoaded.Broadcast(MapName);
}