// ALL CODE FOR wangjunyang learning GAS


#include "AuraAssetManager.h"
#include "AuraGameplayTags.h"
#include <AbilitySystemGlobals.h>

UAuraAssetManager& UAuraAssetManager::Get()
{
	check(GEngine);
	UAuraAssetManager* AuraAssetManager = Cast<UAuraAssetManager>(GEngine->AssetManager);
	return *AuraAssetManager;
}

void UAuraAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	FAuraGameplayTags::InitializeNativeGameplayTags();

	//TargetDataCache 会在这里面初始化
	UAbilitySystemGlobals::Get().InitGlobalData();

}
