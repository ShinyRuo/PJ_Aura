// ALL CODE FOR  learning GAS

#pragma once

#include "CoreMinimal.h"
#include "Actor/AuraProjectile.h"
#include "AuraFlyingStone.generated.h"

/**
 * 飞行石头类 - 具有三种状态：
 * 1. 卫星静止状态：均匀分布在主角周围圆环上，与主角保持相对静止
 * 2. 旋转状态：围绕主角逆时针旋转
 * 3. 发射状态：旋转到正前方后，朝前方加速飞行
 */
UCLASS()
class PJ_AURA_API AAuraFlyingStone : public AAuraProjectile
{
	GENERATED_BODY()

public:
	AAuraFlyingStone();

	// 初始化卫星状态（静止在圆环上）
	UFUNCTION(BlueprintCallable, Category = "FlyingStone")
	void InitializeSatellite(AActor* InOwner, int32 StoneIndex, int32 TotalStones, float InOrbitRadius, float InOrbitHeight,FVector InTargetLocation);

	// 开始旋转（从静止状态切换到旋转状态）
	UFUNCTION(BlueprintCallable, Category = "FlyingStone")
	void StartRotation(float InRotationSpeed);

	// 切换到发射状态，朝前方加速飞行
	UFUNCTION(BlueprintCallable, Category = "FlyingStone")
	void LaunchStone(float InLaunchSpeed, float InMaxSpeed, float InAcceleration, float InMaxDistance);

	// 获取当前石头索引
	UFUNCTION(BlueprintPure, Category = "FlyingStone")
	int32 GetStoneIndex() const { return StoneIndex; }

	// 获取当前状态
	UFUNCTION(BlueprintPure, Category = "FlyingStone")
	bool IsInLaunchState() const { return bIsLaunched; }

	UFUNCTION(BlueprintPure, Category = "FlyingStone")
	bool IsInRotationState() const { return bIsRotating; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	// 更新卫星静止状态（跟随主人移动）
	void UpdateSatelliteStaticMovement();

	// 更新旋转状态（围绕主人旋转）
	void UpdateRotationMovement(float DeltaTime);

	// 更新发射状态（加速飞行）
	void UpdateLaunchedMovement(float DeltaTime);

	// 检查是否旋转到正前方
	bool CheckIfFacingForward() const;

	// 卫星相关参数
	UPROPERTY()
	AActor* StoneOwner;

	UPROPERTY()
	int32 StoneIndex = 0;

	UPROPERTY()
	int32 TotalStones = 3;

	UPROPERTY()
	float CurrentAngle = 0.f;

	UPROPERTY()
	float OrbitRadius = 100.f;

	UPROPERTY()
	float OrbitHeight = 50.f;

	// 旋转状态相关
	UPROPERTY()
	bool bIsRotating = false;

	UPROPERTY()
	float RotationSpeed = 360.f;

	// 发射状态相关
	UPROPERTY()
	bool bIsLaunched = false;

	UPROPERTY()
	float MaxDistance = 2000.f;

	UPROPERTY()
	FVector StartLocation;

	UPROPERTY()
	FVector TargetLocation;

	// 发射加速度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FlyingStone", meta = (AllowPrivateAccess = "true"))
	float LaunchAcceleration = 3000.f;

	// 最大速度上限
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FlyingStone", meta = (AllowPrivateAccess = "true"))
	float MaxSpeedCap = 2000.f;
};