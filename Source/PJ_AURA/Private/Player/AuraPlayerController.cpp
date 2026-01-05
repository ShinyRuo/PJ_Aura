// ALL CODE FOR wangjunyang learning GAS


#include "Player/AuraPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "EnhancedInputSubsystems.h"
#include "GameplayTagContainer.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Actor/MagicCircle.h"
#include "Chararctor/AuraCharacter.h"
#include "Components/DecalComponent.h"
#include "Components/SplineComponent.h"
#include "Input/AuraInputComponent.h"
#include "Interaction/EnemyInterface.h"
#include "GameFramework/Character.h"
#include "PJ_AURA/PJ_AURA.h"
#include "UI/Widget/DamageTextComponent.h"
#include <AbilitySystem/AuraAbilitySystemLibrary.h>

#include "Actor/PickUpItem.h"


AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
}

void AAuraPlayerController::PickingItem()
{
	// 如果玩家正在向一个物品移动
	if (TargetPickUpItem.IsValid())
	{
		APawn* ControlledPawn = GetPawn();
		if (ControlledPawn)
		{
			const float Distance = (TargetPickUpItem->GetActorLocation() - ControlledPawn->GetActorLocation()).Length();
			if (Distance <= PickUpRange)
			{
				// 到达范围内，执行拾取并停止移动
				if (AAuraCharacter* AuraCharacter = Cast<AAuraCharacter>(ControlledPawn))
				{
					AuraCharacter->OnPickUpItemBegin(TargetPickUpItem.Get());
				}

				// 停止寻路
				bAutoRunning = false;

				// 清除目标
				TargetPickUpItem = nullptr;
			}
		}
	}
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();

	AutoRun();

	UpdateMagicCircleLocation();

	PickingItem();
}

void AAuraPlayerController::ShowMagicCircle(UMaterialInterface* DecalMaterial)
{
	if (!IsValid(MagicCircle))
	{
		MagicCircle = GetWorld()->SpawnActor<AMagicCircle>(MagicCircleClass);
		if (DecalMaterial)
		{
			MagicCircle->MagicCircleDecal->SetMaterial(0,DecalMaterial);
		}
	}
}

void AAuraPlayerController::HideMagicCircle()
{
	if (IsValid(MagicCircle))
	{
		MagicCircle->Destroy();
	}
}

void AAuraPlayerController::ShowDamageNumber_Implementation(float DamageAmount, ACharacter* TargetCharacter, bool bBlockedHit, bool bCriticalHit)
{
	if (IsValid(TargetCharacter) && DamageTextComponentClass&&IsLocalController())
	{
		UDamageTextComponent* DamageText = NewObject<UDamageTextComponent>(TargetCharacter, DamageTextComponentClass);
		DamageText->RegisterComponent();//不在构造函数中创建CDO的组件就得这样动态注册
		DamageText->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		/*按需构造” 机制
		* 即使不手动调用 InitWidget()，当满足以下条件时，WidgetComponent 会自动触发 Widget 的构造并显示 UI：
		*	组件已通过 RegisterComponent() 注册到世界中。
		*	组件的可见性已开启（SetVisibility(true)）。
		*	组件进入摄像机的视锥体范围（即镜头能 “看到” 该组件所在的 3D 位置）。
		*	此时，引擎会在渲染时检测到该组件需要显示，自动调用内部逻辑构造 Widget 并渲染到屏幕上。
		* 手动调用 InitWidget() 的作用
		*	手动调用 InitWidget() 的核心价值是强制提前构造 Widget，确保在以下场景中 UI 能正常工作：
		*	需要在组件进入视锥体前就初始化 Widget 数据（如提前设置文本、进度条值）。
		*	组件位置可能始终在视锥体之外，但需要通过代码访问 Widget 实例（如获取子控件）。
		*/
		DamageText->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		DamageText->SetDamageText(DamageAmount,  bBlockedHit,  bCriticalHit);
	}
}

void AAuraPlayerController::AutoRun()
{
	if (!bAutoRunning) return;
	if (APawn* ControlledPawn = GetPawn())
	{
		const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(ControlledPawn->GetActorLocation(), ESplineCoordinateSpace::World);
		const FVector Direction = Spline->FindDirectionClosestToWorldLocation(LocationOnSpline, ESplineCoordinateSpace::World);
		ControlledPawn->AddMovementInput(Direction);

		const float DistanceToDestination = (LocationOnSpline - CachedDestination).Length();
		if (DistanceToDestination <= AutoRunAcceptanceRadius)
		{
			bAutoRunning = false;
		}
	}
}

void AAuraPlayerController::UpdateMagicCircleLocation()
{
	if (IsValid(MagicCircle))
	{
		MagicCircle->SetActorLocation(CursorHit.ImpactPoint);
	}
}

void AAuraPlayerController::ToggleDebugWidget()
{
	// 仅在服务器上执行
	if (!HasAuthority())
	{
		return;
	}

	if (!bCtrlKeyDown)
	{
		return;
	}

	if (!DebugWidgetClass)
	{
		// 在蓝图中设置 WBP_DebugWidget
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("DebugWidgetClass 未在玩家控制器中设置!"));
		return;
	}
	if (DebugWidgetInstance.IsValid())
	{
		if (DebugWidgetInstance->IsVisible())
		{
			DebugWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
		}
		else
		{
			DebugWidgetInstance->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else
	{
		UAuraUserWidget* Widget = UAuraAbilitySystemLibrary::ShowAuraWidget(this, FName("DebugPanel"), DebugWidgetClass);
		DebugWidgetInstance = Cast<UDebugWidget>(Widget);
	}
	
}

void AAuraPlayerController::CursorTrace()
{
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_CursorTrace))
	{
		if (LastActor)
			LastActor->UnHighlightActor();
		if (ThisActor)
			ThisActor->HighlightActor();

		LastActor = nullptr;
		ThisActor = nullptr;
	}
	//在MagicCircle存在的情况下 使用ExcludePlayers通道 来让mousehit 忽略enemy player 等等 会影响贴花投射的actor
	const ECollisionChannel TraceChannel = IsValid(MagicCircle) ? ECC_ExcludePlayers : ECC_Visibility;
	GetHitResultUnderCursor(TraceChannel, false, CursorHit);
	if (!CursorHit.bBlockingHit) return;

	LastActor = ThisActor;
	ThisActor = Cast<IHighlightInterface>(CursorHit.GetActor());
	if (LastActor != ThisActor) //todo 内存不安全
	{
		if (LastActor)
			LastActor->UnHighlightActor();

		if (ThisActor)
			ThisActor->HighlightActor();
	}
}

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputPressed)) return;

	if (InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		bTargeting = ThisActor ? true : false;
		bAutoRunning = false;
	}
	if (GetASC())
	{
		GetASC()->AbilityInputTagPressed(InputTag);
	}
}

void AAuraPlayerController::AutoRunToCachedDestination(const APawn* ControlledPawn)
{
	if (UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, ControlledPawn->GetActorLocation(), CachedDestination))
	{
		Spline->ClearSplinePoints();
		for (const FVector& PointLoc : NavPath->PathPoints)
		{
			Spline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World);
			//DrawDebugSphere(GetWorld(), PointLoc, 80.f, 8, FColor::Green, false, 5.f);
		}
		if (!NavPath->PathPoints.IsEmpty())
			CachedDestination = NavPath->PathPoints.Last();
		bAutoRunning = true;
	}
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputReleased)) return;


	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB) )
	{
		if (InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_RMB))
		{
			CameraRotatingHoldingSec  = 0.f;
			SetIsRotatingCamera(false);
		}
		if (GetASC())
		{
			GetASC()->AbilityInputTagReleased(InputTag);
		}
	}
	else if (!bTargeting && ! bShiftKeyDown)
	{
		const APawn* ControlledPawn = GetPawn();
		if (FollowTime <= ShortPressThreshold)
		{
			AutoRunToCachedDestination(ControlledPawn);
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ClickNiagaraSystem, CachedDestination);
		}
		FollowTime = 0.f;
	}
	
}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputHeld)) return;

	if (InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB) && !bTargeting && !bShiftKeyDown)
	{	//点地
		FollowTime += GetWorld()->GetDeltaSeconds();
		if (CursorHit.bBlockingHit)
		{
			//点击地面的坐标
			CachedDestination = CursorHit.ImpactPoint;
		}
		if (APawn* ControlledPawn = GetPawn())
		{
			const FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
			ControlledPawn->AddMovementInput(WorldDirection);
		}
	}
	else if(InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_RMB))
	{
		//旋转镜头
		CameraRotatingHoldingSec += GetWorld()->GetDeltaSeconds();
		if (CameraRotatingHoldingSec >= CameraRotatingThresholdSeconds)
		{
			SetIsRotatingCamera(true);
		}
	}
	else if(bTargeting)
	{
		//有目标 分析目标的Actor
		if (ThisActor)
		{
			if (APickUpItem* PickItem = Cast<APickUpItem>(ThisActor))
			{
				OnPickUpItemClicked(PickItem);
			}
		}
		else
		{
			//点击高亮敌人 或者按住Shift点击anything
			if (GetASC())
			{
				GetASC()->AbilityInputTagHeld(InputTag);
			}
		}
	}
	else if(bShiftKeyDown)
	{
		if (GetASC())
		{
			GetASC()->AbilityInputTagHeld(InputTag);
		}
	}
}


UAuraAbilitySystemComponent* AAuraPlayerController::GetASC()
{
	if(AuraAbilitySystemComponent == nullptr)
	{
		AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	}
	return AuraAbilitySystemComponent;
}



void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(AuraContext);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	//other client的Subsystem是拿不到的
	//但是其实other client的BeginPlay根本不会调用 吧
	if (Subsystem)
	{
		Subsystem->AddMappingContext(AuraContext, 0);
	}

	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);
	AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Started, this, &AAuraPlayerController::ShiftPressed);
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Completed,this,&AAuraPlayerController::ShiftReleased);

	AuraInputComponent->BindAction(CtrlAction, ETriggerEvent::Started, this, &AAuraPlayerController::CtrlPressed);
	AuraInputComponent->BindAction(CtrlAction, ETriggerEvent::Completed, this, &AAuraPlayerController::CtrlReleased);

	AuraInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
	AuraInputComponent->BindAction(MouseMoveAction, ETriggerEvent::Triggered,this, &AAuraPlayerController::OnMouseXY);

	AuraInputComponent->BindAction(ToggleDebugAction, ETriggerEvent::Started, this, &AAuraPlayerController::ToggleDebugWidget);

}

void AAuraPlayerController::OnPickUpItemClicked(APickUpItem* ItemToPickUp)
{

	if (!ItemToPickUp) return;

	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn) return;

	const float Distance = (ItemToPickUp->GetActorLocation() - ControlledPawn->GetActorLocation()).Length();

	if (Distance > PickUpRange)
	{
		// 距离太远，移动过去
		TargetPickUpItem = ItemToPickUp;
		CachedDestination = ItemToPickUp->GetActorLocation();
		AutoRunToCachedDestination(ControlledPawn);
	}
	else
	{
		// 距离足够近，直接拾取
		if (AAuraCharacter* AuraCharacter = Cast<AAuraCharacter>(ControlledPawn))
		{
			AuraCharacter->OnPickUpItemBegin(ItemToPickUp);
		}
	}
}

void AAuraPlayerController::OnMouseXY(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();

	if (GetIsRotatingCamera())
	{
		if (APawn* ControlledPawn = GetPawn<APawn>())
		{
			if(AAuraCharacter* AuraCharacter = Cast<AAuraCharacter>(ControlledPawn))
			{
				AuraCharacter->OnRotatingCamera(InputAxisVector.X * RotationSpeed, InputAxisVector.Y * RotationSpeed);
			}
		}
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("InputAxisVector.Y:%f"), InputAxisVector.Y));
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("InputAxisVector.X:%f"), InputAxisVector.X));
	}
}
void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>(); 

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		if (AAuraCharacter* AuraCharacter = Cast<AAuraCharacter>(ControlledPawn))
		{
			AuraCharacter->AddMove(InputAxisVector.Y, InputAxisVector.X);
		}
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("InputAxisVector.Y:%f"), InputAxisVector.Y));
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("InputAxisVector.X:%f"), InputAxisVector.X));
	}
}

