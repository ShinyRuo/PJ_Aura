// ALL CODE FOR  learning GAS

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraProjectileSpell.h"
#include "AuraStoneShot.generated.h"

class AAuraFlyingStone;

/**
 * 石头射击技能 - 类似岩雀Q技能
 * 流程：
 * 1. 生成多个飞行石头围绕主角旋转
 * 2. 延迟后石头依次向鼠标方向发射
 * 3. 石头带加速度飞行并造成伤害
 */
UCLASS()
class PJ_AURA_API UAuraStoneShot : public UAuraProjectileSpell
{
	GENERATED_BODY()

public:
	UAuraStoneShot();

	// 生成石头
	UFUNCTION(BlueprintCallable, Category = "StoneShot")
	void SpawnStones();

	// 开始旋转所有石头
	UFUNCTION(BlueprintCallable, Category = "StoneShot")
	void StartStoneRotation();

	// 获取鼠标方向
	UFUNCTION(BlueprintCallable, Category = "StoneShot")
	FVector GetMouseDirection();

	UFUNCTION(BlueprintCallable, Category = "StoneShot")
	void LaunchAllStones();

protected:
	// 延迟后开始旋转
	void DelayedStartRotation();

	UPROPERTY(BlueprintReadWrite)
	FVector TargetLocation;

private:
	// 生成的石头数组
	UPROPERTY()
	TArray<AAuraFlyingStone*> SpawnedStones;

	// 鼠标方向
	UPROPERTY()
	FVector MouseDirection;

	// 是否已经开始发射
	UPROPERTY()
	bool bHasLaunchStarted = false;

	// 已发射石头计数
	UPROPERTY()
	int32 LaunchedCount = 0;

public:
	// ===== 可配置属性 =====
	
	// 石头数量
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StoneShot|Config", meta = (ExposeOnSpawn = true))
	int32 StoneCount = 3;

	// 围绕飞行的半径
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StoneShot|Config", meta = (ExposeOnSpawn = true))
	float OrbitRadius = 100.f;

	// 旋转速度(角度/秒)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StoneShot|Config", meta = (ExposeOnSpawn = true))
	float RotationSpeed = 360.f;

	// 延迟多久后开始发射(秒)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StoneShot|Config", meta = (ExposeOnSpawn = true))
	float LaunchDelay = 0.5f;

	// 每个石头发射的延迟间隔(秒)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StoneShot|Config", meta = (ExposeOnSpawn = true))
	float StoneLaunchInterval = 0.1f;

	// 石头初始速度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StoneShot|Config", meta = (ExposeOnSpawn = true))
	float LaunchInitialSpeed = 800.f;

	// 石头最大速度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StoneShot|Config", meta = (ExposeOnSpawn = true))
	float LaunchMaxSpeed = 2000.f;

	// 石头加速度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StoneShot|Config", meta = (ExposeOnSpawn = true))
	float LaunchAcceleration = 5000.f;

	// 石头最大飞行距离
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StoneShot|Config", meta = (ExposeOnSpawn = true))
	float MaxTravelDistance = 2000.f;
};