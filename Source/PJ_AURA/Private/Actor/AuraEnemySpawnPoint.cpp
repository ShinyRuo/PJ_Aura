// ALL CODE FOR  learning GAS


#include "Actor/AuraEnemySpawnPoint.h"

void AAuraEnemySpawnPoint::SpawnEnemy()
{
	//延迟生成Actor，并设置其尝试调整位置但固定生成
	// SpawnActorDeferred 的最大意义：可以在敌人初始化之前设置数据，保证 BeginPlay 里能读到正确的值。
	AAuraEnemy* Enemy = GetWorld()->SpawnActorDeferred<AAuraEnemy>(
		EnemyClass,
		GetActorTransform(),
		nullptr,
		nullptr,
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);

	Enemy->SetPlayerLevel(EnemyLevel);
	Enemy->SetCharacterClass(CharacterClass);//PossessedBy中根据CharacterClass设置ability
	Enemy->FinishSpawning(GetActorTransform());//在这之后执行敌人初始化函数，保证 BeginPlay 里能读到正确的值。执行beginplay
	Enemy->SpawnDefaultController();//在这里生成AIController（如果蓝图中设置过），触发 PossessedBy 函数，在里面运行行为树
}