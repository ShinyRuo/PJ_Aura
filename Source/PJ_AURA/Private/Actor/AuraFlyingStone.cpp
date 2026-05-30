// ALL CODE FOR  learning GAS

#include "Actor/AuraFlyingStone.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

AAuraFlyingStone::AAuraFlyingStone()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// 关闭原有的投射物运动组件的控制(我们将手动控制)
	bReplicates = true;
	
	LaunchAcceleration = 3000.f;
	MaxSpeedCap = 2000.f;
	
	StoneOwner = nullptr;
	StoneIndex = 0;
	TotalStones = 3;
	CurrentAngle = 0.f;
	OrbitRadius = 100.f;
	RotationSpeed = 360.f;
	OrbitHeight = 50.f;
	bIsRotating = false;
	bIsLaunched = false;
}

void AAuraFlyingStone::BeginPlay()
{
	Super::BeginPlay();
	
	// 禁用ProjectileMovement组件的自动更新
	if (ProjectileMovement)
	{
		ProjectileMovement->bAutoActivate = false;
	}
}

void AAuraFlyingStone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!StoneOwner) return;
	
	if (bIsLaunched)
	{
		// 发射状态：使用ProjectileMovement组件
		UpdateLaunchedMovement(DeltaTime);
	}
	else if (bIsRotating)
	{
		// 旋转状态：围绕主人旋转
		UpdateRotationMovement(DeltaTime);
		
		// 检查是否旋转到正前方，如果是则发射
		if (CheckIfFacingForward())
		{
			LaunchStone(800.f, 2000.f, 5000.f, 2000.f);
		}
	}
	else
	{
		// 卫星静止状态：跟随主人移动
		UpdateSatelliteStaticMovement();
	}
}

void AAuraFlyingStone::InitializeSatellite(AActor* InOwner, int32 InStoneIndex, int32 InTotalStones, float InOrbitRadius, float InOrbitHeight, FVector InTargetLocation)
{
	StoneOwner = InOwner;
	StoneIndex = InStoneIndex;
	TotalStones = InTotalStones;
	OrbitRadius = InOrbitRadius;
	OrbitHeight = InOrbitHeight;
	TargetLocation = InTargetLocation;
	OrbitHeight = InOrbitHeight;
	
	// 根据石头索引计算初始角度(均匀分布在圆周上)
	CurrentAngle = (360.f / TotalStones) * StoneIndex;
	
	// 初始位置设置为围绕主人
	if (StoneOwner)
	{
		SetActorLocation(StoneOwner->GetActorLocation());
	}
	
	bIsRotating = false;
	bIsLaunched = false;
}

void AAuraFlyingStone::StartRotation(float InRotationSpeed)
{
	bIsRotating = true;
	bIsLaunched = false;
	RotationSpeed = InRotationSpeed;
}

void AAuraFlyingStone::UpdateSatelliteStaticMovement()
{
	if (!StoneOwner) return;
	
	// 计算石头在主人周围的圆周位置（静止状态）
	const FVector OwnerLocation = StoneOwner->GetActorLocation();
	const float AngleRad = FMath::DegreesToRadians(CurrentAngle);
	
	const float OffsetX = FMath::Cos(AngleRad) * OrbitRadius;
	const float OffsetY = FMath::Sin(AngleRad) * OrbitRadius;
	
	FVector NewLocation = OwnerLocation;
	NewLocation.X += OffsetX;
	NewLocation.Y += OffsetY;
	NewLocation.Z += OrbitHeight;
	
	SetActorLocation(NewLocation);
	
	// 石头朝向外边
	FVector DirectionToOwner = (OwnerLocation - NewLocation).GetSafeNormal();
	if (!DirectionToOwner.IsZero())
	{
		FRotator LookAtRotator = DirectionToOwner.Rotation();
		SetActorRotation(LookAtRotator);
	}
}

void AAuraFlyingStone::UpdateRotationMovement(float DeltaTime)
{
	if (!StoneOwner) return;
	
	// 更新当前角度（逆时针旋转）
	CurrentAngle -= RotationSpeed * DeltaTime;
	if (CurrentAngle < 0.f)
	{
		CurrentAngle += 360.f;
	}
	else if (CurrentAngle >= 360.f)
	{
		CurrentAngle -= 360.f;
	}
	
	// 计算石头在主人周围的圆周位置
	const FVector OwnerLocation = StoneOwner->GetActorLocation();
	const float AngleRad = FMath::DegreesToRadians(CurrentAngle);
	
	const float OffsetX = FMath::Cos(AngleRad) * OrbitRadius;
	const float OffsetY = FMath::Sin(AngleRad) * OrbitRadius;
	
	FVector NewLocation = OwnerLocation;
	NewLocation.X += OffsetX;
	NewLocation.Y += OffsetY;
	NewLocation.Z += OrbitHeight;
	
	SetActorLocation(NewLocation);
	
	// 石头朝向应该指向主人
	FVector DirectionToOwner = (NewLocation - OwnerLocation).GetSafeNormal();
	if (!DirectionToOwner.IsZero())
	{
		FRotator LookAtRotator = DirectionToOwner.Rotation();
		SetActorRotation(LookAtRotator);
	}
}

bool AAuraFlyingStone::CheckIfFacingForward() const
{
	if (!StoneOwner) return false;
	
	// 获取主人的前方方向
	FVector OwnerToTarget = TargetLocation - StoneOwner->GetActorLocation();
	OwnerToTarget.Z = 0.f; // 忽略高度差
	OwnerToTarget.Normalize();
	
	// 计算石头的方向
	FVector StoneForward = GetActorRotation().Vector();
	StoneForward.Z = 0.f; // 忽略高度差
	StoneForward.Normalize();
	
	// 计算两个方向的角度差
	float DotProduct = FVector::DotProduct(OwnerToTarget, StoneForward);
	float AngleDifference = FMath::RadiansToDegrees(FMath::Acos(FMath::Clamp(DotProduct, -1.0f, 1.0f)));
	
	// 如果角度差很小（小于5度），认为已经旋转到正前方
	return AngleDifference < 5.0f;
}

void AAuraFlyingStone::LaunchStone(float InLaunchSpeed, float InMaxSpeed, float InAcceleration, float InMaxDistance)
{
	if (bIsLaunched) return;
	
	bIsRotating = false;
	bIsLaunched = true;
	MaxDistance = InMaxDistance;
	StartLocation = GetActorLocation();
	LaunchAcceleration = InAcceleration;
	MaxSpeedCap = InMaxSpeed;
	
	// 计算发射方向：朝向目标位置
	FVector LaunchDirection = (TargetLocation - GetActorLocation()).GetSafeNormal();
	
	// 如果目标位置无效，则朝向主人正前方
	if (LaunchDirection.IsZero())
	{
		LaunchDirection = StoneOwner->GetActorForwardVector();
	}
	
	// 启用ProjectileMovement组件
	if (ProjectileMovement)
	{
		ProjectileMovement->bAutoActivate = true;
		ProjectileMovement->InitialSpeed = InLaunchSpeed;
		ProjectileMovement->MaxSpeed = InLaunchSpeed;
		ProjectileMovement->ProjectileGravityScale = 0.f; // 不受重力影响
		ProjectileMovement->Velocity = LaunchDirection * InLaunchSpeed;
	}
	
	// 设置朝向目标位置
	SetActorRotation(LaunchDirection.Rotation());
}

void AAuraFlyingStone::UpdateLaunchedMovement(float DeltaTime)
{
	if (!ProjectileMovement || !bIsLaunched) return;
	
	// 检查是否超过最大距离
	float DistanceTraveled = FVector::Distance(StartLocation, GetActorLocation());
	if (DistanceTraveled >= MaxDistance)
	{
		// 超过最大距离，销毁石头
		OnHit();
		Destroy();
		return;
	}
	
	// 逐渐加速
	float CurrentSpeed = ProjectileMovement->Velocity.Size();
	if (CurrentSpeed < MaxSpeedCap)
	{
		float NewSpeed = FMath::Min(CurrentSpeed + LaunchAcceleration * DeltaTime, MaxSpeedCap);
		FVector CurrentDirection = ProjectileMovement->Velocity.GetSafeNormal();
		ProjectileMovement->Velocity = CurrentDirection * NewSpeed;
		ProjectileMovement->MaxSpeed = NewSpeed;
	}
}