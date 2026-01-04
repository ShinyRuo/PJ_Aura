// ALL CODE FOR wangjunyang learning GAS

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UI/Widget/Debug/DebugWidget.h"
#include "AuraPlayerController.generated.h"

class IHighlightInterface;
class AMagicCircle;
class UNiagaraSystem;
class UDamageTextComponent;
class UAuraAbilitySystemComponent;
struct FGameplayTag;
class UAuraInputConfig;
class UInputMappingContext;
class UInputAction;
struct  FInputActionValue;
class IEnemyInterface;
class USplineComponent;
class APickUpItem;
/**
 * 
 */
UCLASS()
class PJ_AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AAuraPlayerController();
	void PickingItem();
	virtual void PlayerTick(float DeltaTime) override;

	UFUNCTION(Client,Reliable)
	void ShowDamageNumber(float DamageAmount,ACharacter* TargetCharacter, bool bBlockedHit, bool bCriticalHit);

	UFUNCTION(BlueprintCallable)
	void ShowMagicCircle(UMaterialInterface* DecalMaterial = nullptr);

	UFUNCTION(BlueprintCallable)
	void HideMagicCircle();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	/**
	 * 当点击一个可拾取的物品时调用。
	 * @param ItemToPickUp 被点击的物品。
	 */
	void OnPickUpItemClicked(APickUpItem* ItemToPickUp);

	// 用于跟踪玩家是否正在向一个物品移动以进行拾取
	UPROPERTY()
	TWeakObjectPtr<APickUpItem> TargetPickUpItem;

	// 拾取物品的最大距离
	UPROPERTY(EditDefaultsOnly, Category="Config")
	float PickUpRange = 150.f;

private:
	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputMappingContext> AuraContext;

	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> ShiftAction;
	void ShiftPressed() { bShiftKeyDown = true; }
	void ShiftReleased() { bShiftKeyDown = false; }
	bool bShiftKeyDown = false;
	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> CtrlAction;
	void CtrlPressed() { bCtrlKeyDown = true; }
	void CtrlReleased() { bCtrlKeyDown = false; }
	bool bCtrlKeyDown = false;
	UPROPERTY(EditAnywhere, Category = "input")
	TObjectPtr<UInputAction> MouseMoveAction;
	UPROPERTY(EditDefaultsOnly)
	float CameraRotatingThresholdSeconds = 0.5;
	float CameraRotatingHoldingSec = 0.f;
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	bool bRotatingCamera = false;
	UPROPERTY(EditAnywhere, Category = "Camera", meta = (ClampMin = 0.1, ClampMax = 5.0))
	float RotationSpeed = 2.0f; // 旋转灵敏度

	void SetIsRotatingCamera(bool Value) { bRotatingCamera = Value; }
	bool GetIsRotatingCamera()const { return bRotatingCamera; }

	void Move(const FInputActionValue& InputActionValue);

	void CursorTrace();
	FHitResult CursorHit;
	IHighlightInterface* LastActor;
	IHighlightInterface* ThisActor;

	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);
	void OnMouseXY(const FInputActionValue& InputActionValue);

	UPROPERTY(EditAnywhere , Category = "Input")
	TObjectPtr<UInputAction> ToggleDebugAction;


	UPROPERTY(EditDefaultsOnly,Category="Input")
	TObjectPtr<UAuraInputConfig> InputConfig;

	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;
	UAuraAbilitySystemComponent* GetASC();

	FVector CachedDestination = FVector::Zero();
	float FollowTime = 0.f;
	float ShortPressThreshold = 0.8f;
	bool bAutoRunning = false;
	bool bTargeting = false;
	UPROPERTY(EditDefaultsOnly)
	float AutoRunAcceptanceRadius = 50.f;

	//寻路点的样条 平滑曲线用
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> Spline;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNiagaraSystem> ClickNiagaraSystem;

	void AutoRun();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageTextComponent> DamageTextComponentClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AMagicCircle> MagicCircleClass;

	UPROPERTY()
	TObjectPtr<AMagicCircle> MagicCircle;

	void UpdateMagicCircleLocation();

	/* debug window */
private:
	/** 切换调试窗口的可见性 */
	void ToggleDebugWidget();

	UPROPERTY(EditDefaultsOnly, Category = "Debug")
	TSubclassOf<UDebugWidget> DebugWidgetClass;

	UPROPERTY()
	TWeakObjectPtr<UDebugWidget> DebugWidgetInstance;


	void AutoRunToCachedDestination(const APawn* ControlledPawn);


};
