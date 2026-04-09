// ALL CODE FOR  learning GAS

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Chararctor/AuraEnemy.h"
#include "Engine/TargetPoint.h"
#include "AuraEnemySpawnPoint.generated.h"

/**
 * 
 */
UCLASS()
class PJ_AURA_API AAuraEnemySpawnPoint : public ATargetPoint
{
	GENERATED_BODY()
public:
	//生成敌人
	UFUNCTION(BlueprintCallable)
	void SpawnEnemy();

	//需要生成的敌人蓝图类，在类前面加class就不需要额外的前向申明
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Class")
	TSubclassOf<AAuraEnemy> EnemyClass;

	//需要生成的敌人的等级
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Class")
	int32 EnemyLevel = 1;

	//敌人的职业类型
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Class")
	ECharacterClass CharacterClass = ECharacterClass::Warrior;

};
