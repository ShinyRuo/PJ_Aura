#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/PlayerInterface.h"
#include "Interaction/HighlightInterface.h"
#include "PickUpItem.generated.h"

class UItem;
class USphereComponent;
class UStaticMeshComponent;
class USkeletalMeshComponent;
class UWidgetComponent;
class UTimelineComponent;

UCLASS()
class PJ_AURA_API APickUpItem : public AActor, public IHighlightInterface
{
	GENERATED_BODY()

	friend class UItemManager;
	
public:	
	APickUpItem();

	//~ Begin AActor interface
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override; // 重写 EndPlay
	virtual void Tick(float DeltaTime) override;
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	//~ End AActor interface

	//~ Begin IHighlightInterface
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;
	//~ End IHighlightInterface

	//~ Begin IPlayerInterface
	virtual void OnInteracted(AActor* InteractingActor);
	//~ End IPlayerInterface

	/** [Server] 设置此拾取物代表的物品数据 */
	UFUNCTION(BlueprintCallable, Category = "PickUpItem")
	void SetItemName(const FName& InItemName);

	UFUNCTION(BlueprintCallable, Category = "PickUpItem")
	void SetItemQuantity(const int32 InQuantity);

	/** [Server] 物品被丢弃时的逻辑 */
	UFUNCTION(BlueprintCallable, Category = "PickUpItem")
	void OnItemDropped(const FVector& TargetLocation);

	/** 由 ItemManager 调用，用于激活和设置物品 */
	void ActivateItem(int64 NewItemID, const FName& InItemName, const int32 InQuantity, const FVector& WorldLocation);

	/** 停用物品，准备回收到对象池 */
	void DeactivateItem();

	UItem* GetItem() const
	{
		return ItemData;
	}

	/** 当 ItemData 属性被复制时调用的函数 */
	UFUNCTION()
	void OnRep_Item();
protected:
	/** 当 ItemID 属性被复制时调用的函数 (可选，但有助于调试) */
	UFUNCTION()
	void OnRep_ItemID();

	/** 根据 ItemData 数据更新 Mesh */
	void UpdateMesh();

	/** 绑定组件的事件，如悬停事件 */
	void BindEvents();

	/** 当鼠标悬停在碰撞体上时调用 */
	UFUNCTION()
	void OnCursorOver(UPrimitiveComponent* TouchedComponent);

	/** 当鼠标离开碰撞体时调用 */
	UFUNCTION()
	void OnCursorEndOver(UPrimitiveComponent* TouchedComponent);

	/** 丢弃动画的 Timeline 更新函数 */
	UFUNCTION()
	void DropTimelineUpdate(float Value);

	/** 丢弃动画的 Timeline 结束函数 */
	UFUNCTION()
	void DropTimelineFinished();

	/** 拾取动画的 Timeline 更新函数 */
	UFUNCTION()
	void PickUpTimelineUpdate(float Value);

	/** 拾取动画的 Timeline 结束函数 */
	UFUNCTION()
	void PickUpTimelineFinished();

	/** [Multicast] 播放丢弃动画和音效 */
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayDropEffects(const FVector& FinalLocation);

	/** [Multicast] 播放拾取动画和音效 */
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayPickUpEffects(AActor* InteractingActor);


	/** 准备销毁此Actor (现在改为通知管理器回收) */
	void StartRecycle();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USceneComponent> SceneRoot;

	/** 用于检测鼠标交互和触发拾取的球形碰撞体 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> CollisionSphere;

	/** 用于显示物品的静态网格体组件 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;

	/** 用于显示物品的骨骼网格体组件 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComponent;

	/** 用于在物品上方显示信息的 Widget 组件 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UWidgetComponent> InfoWidgetComponent;

	/** 用于处理丢弃动画的 Timeline 组件 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UTimelineComponent> DropTimeline;

	/** 用于处理拾取动画的 Timeline 组件 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UTimelineComponent> PickUpTimeline;

	/**
	 * 代表该拾取物的物品数据实例。
	 */
	UPROPERTY(ReplicatedUsing = OnRep_Item, BlueprintReadOnly, Instanced,Category = "PickUpItem")
	TObjectPtr<UItem> ItemData;

	/** 由 ItemManager 分配的唯一ID */

	UPROPERTY(ReplicatedUsing = OnRep_ItemID, VisibleAnywhere, BlueprintReadOnly, Category = "PickUpItem")
	int64 ItemID;


	/** 丢弃动画的浮动曲线 */
	UPROPERTY(EditDefaultsOnly, Category = "PickUpItem|Effects")
	TObjectPtr<UCurveFloat> DropBounceCurve;

	/** 拾取动画的浮动曲线 */
	UPROPERTY(EditDefaultsOnly, Category = "PickUpItem|Effects")
	TObjectPtr<UCurveFloat> PickUpCurve;

	/** 物品在地面上的生命周期计时器 */
	FTimerHandle LifeSpanTimer;



private:
	// 丢弃动画的起始和结束位置
	FVector DropStartLocation;
	FVector DropEndLocation;

	// 拾取动画的起始位置
	FVector PickUpStartLocation;


	// 拾取动画的目标
	UPROPERTY()
	TWeakObjectPtr<AActor> PickUpTargetActor;
};