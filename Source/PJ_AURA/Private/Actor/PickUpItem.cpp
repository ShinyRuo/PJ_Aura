#include "Actor/PickUpItem.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/TimelineComponent.h"
#include "Engine/ActorChannel.h"
#include "Game/ItemManager.h"
#include "Net/UnrealNetwork.h"
#include "Inventory/Item.h"
#include "Inventory/InventoryComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PJ_AURA/PJ_AURA.h"
#include "Player/AuraPlayerController.h"
#include "Sound/SoundCue.h"

APickUpItem::APickUpItem()
{
	PrimaryActorTick.bCanEverTick = true; // Tick is needed for timeline
	bReplicates = true;
	SetReplicatingMovement(true);

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionSphere->SetupAttachment(GetRootComponent());
	CollisionSphere->SetSphereRadius(100.f);
	CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionSphere->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComponent->SetupAttachment(CollisionSphere);
	//StaticMeshComponent->SetSimulatePhysics(true); // 1. 开启物理模拟以接受重力
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 2. 设置碰撞模式为物理和查询
	//StaticMeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore); // 3. 默认忽略所有通道
	//StaticMeshComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block); // 4. 只与地面(WorldStatic)碰撞
	//StaticMeshComponent->SetRenderCustomDepth(false);
	//StaticMeshComponent->SetCustomDepthStencilValue(0);

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMeshComponent->SetupAttachment(CollisionSphere);
	//SkeletalMeshComponent->SetSimulatePhysics(true); // 1. 开启物理模拟以接受重力
	SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 2. 设置碰撞模式为物理和查询
	//SkeletalMeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore); // 3. 默认忽略所有通道
	//SkeletalMeshComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block); // 4. 只与地面(WorldStatic)碰撞
	//SkeletalMeshComponent->SetRenderCustomDepth(false);
	//SkeletalMeshComponent->SetCustomDepthStencilValue(0);

	InfoWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("InfoWidget"));
	InfoWidgetComponent->SetupAttachment(GetRootComponent());
	InfoWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	InfoWidgetComponent->SetDrawAtDesiredSize(true);
	InfoWidgetComponent->SetVisibility(false);

	DropTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DropTimeline"));
	PickUpTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("PickUpTimeline"));
}

void APickUpItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APickUpItem, ItemData);
	DOREPLIFETIME(APickUpItem, ItemID);
}

void APickUpItem::BeginPlay()
{
	Super::BeginPlay();
	
	BindEvents();
	UpdateMesh();

	if (DropBounceCurve)
	{
		FOnTimelineFloat TimelineUpdate;
		TimelineUpdate.BindUFunction(this, FName("DropTimelineUpdate"));
		DropTimeline->AddInterpFloat(DropBounceCurve, TimelineUpdate);

		FOnTimelineEvent TimelineFinished;
		TimelineFinished.BindUFunction(this, FName("DropTimelineFinished"));
		DropTimeline->SetTimelineFinishedFunc(TimelineFinished);
	}
	if (PickUpCurve)
	{
		FOnTimelineFloat TimelineUpdate;
		TimelineUpdate.BindUFunction(this, FName("PickUpTimelineUpdate"));
		PickUpTimeline->AddInterpFloat(PickUpCurve, TimelineUpdate);

		FOnTimelineEvent TimelineFinished;
		TimelineFinished.BindUFunction(this, FName("PickUpTimelineFinished"));
	/*	PickUpTimeline->SetTimelineFinishedFunc(TimelineFinished);*/

		const float TimelineLength = PickUpTimeline->GetTimelineLength();
		PickUpTimeline->AddEvent(TimelineLength-0.1f , TimelineFinished);


	}
}

void APickUpItem::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 在Actor销毁前，通知管理器回收自己
	
	Super::EndPlay(EndPlayReason);
}

void APickUpItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool APickUpItem::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bWroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	// 确保 ItemData 指针不为空
	if (ItemData != nullptr)
	{
		// 明确告诉Actor通道去复制ItemData子对象内部的属性
		bWroteSomething |= Channel->ReplicateSubobject(ItemData, *Bunch, *RepFlags);
	}

	return bWroteSomething;
}

void APickUpItem::HighlightActor()
{
	StaticMeshComponent->SetRenderCustomDepth(true);
	StaticMeshComponent->SetCustomDepthStencilValue(CUSTOM_DEPTH_GREEN);
	SkeletalMeshComponent->SetRenderCustomDepth(true);
	SkeletalMeshComponent->SetCustomDepthStencilValue(CUSTOM_DEPTH_GREEN);
	InfoWidgetComponent->SetVisibility(true);
}

void APickUpItem::UnHighlightActor()
{
	StaticMeshComponent->SetRenderCustomDepth(false);
	SkeletalMeshComponent->SetRenderCustomDepth(false);
	InfoWidgetComponent->SetVisibility(false);
}

void APickUpItem::OnInteracted(AActor* InteractingActor)
{
	if (HasAuthority() && InteractingActor)
	{
		Multicast_PlayPickUpEffects(InteractingActor);
	}
}

void APickUpItem::SetItemName(const FName& InItemName)
{
	if (HasAuthority())
	{
		ItemData->ItemID = InItemName;
		OnRep_Item(); // 在服务器上直接调用以立即生效
	}
}

void APickUpItem::SetItemQuantity(const int32 InQuantity)
{
	if (HasAuthority())
	{
		ItemData->Quantity = InQuantity;
		OnRep_Item(); // 在服务器上直接调用以立即生效
	}
}

void APickUpItem::OnItemDropped(const FVector& TargetLocation)
{
	if (HasAuthority())
	{
		Multicast_PlayDropEffects(TargetLocation);
	}
}

void APickUpItem::OnRep_Item()
{
	UpdateMesh();
	// TODO: 根据 ItemData 数据更新 InfoWidgetComponent 的内容

		// 仅在客户端打印日志
	if (GetLocalRole() < ROLE_Authority)
	{
		FString RoleString = UEnum::GetValueAsString(GetLocalRole());
		FString MeshName = TEXT("None");
		if (StaticMeshComponent && StaticMeshComponent->GetStaticMesh())
		{
			MeshName = StaticMeshComponent->GetStaticMesh()->GetName();
		}

		UE_LOG(LogTemp, Warning, TEXT("[Client %s] APickUpItem::OnRep_Item - Actor: %s, ItemID: %lld, StaticMesh: %s, IsActorHidden: %s"),
			*RoleString,
			*GetName(),
			ItemID,
			*MeshName,
			IsHidden() ? TEXT("true") : TEXT("false"));
	}
}

void APickUpItem::OnRep_ItemID()
{
	// 当客户端接收到新的 ItemID 时，可以判断物品是否被激活/停用
	if (ItemID == -1)
	{
		// ID被重置，意味着物品被回收了，在客户端上隐藏它
		SetActorHiddenInGame(true);
		//SetActorEnableCollision(false);
	}
	else
	{
		// 获得了有效ID，确保它是可见的
		SetActorHiddenInGame(false);
		//SetActorEnableCollision(true);
	}


	// 仅在客户端打印日志
	if (GetLocalRole() < ROLE_Authority)
	{
		FString RoleString = UEnum::GetValueAsString(GetLocalRole());
		FString MeshName = TEXT("None");
		if (StaticMeshComponent && StaticMeshComponent->GetStaticMesh())
		{
			MeshName = StaticMeshComponent->GetStaticMesh()->GetName();
		}

		UE_LOG(LogTemp, Warning, TEXT("[Client %s] APickUpItem::OnRep_ItemID - Actor: %s, ItemID: %lld, StaticMesh: %s, IsActorHidden: %s"),
			*RoleString,
			*GetName(),
			ItemID,
			*MeshName,
			IsHidden() ? TEXT("true") : TEXT("false"));
	}
}

void APickUpItem::UpdateMesh()
{
	if (!ItemData) return;

	if (UStaticMesh* Mesh = ItemData->GetStaticMesh())
	{
		StaticMeshComponent->SetStaticMesh(Mesh);
		StaticMeshComponent->SetVisibility(true);
		SkeletalMeshComponent->SetVisibility(false);
	}
	else if (USkeletalMesh* SkelMesh = ItemData->GetSkeletalMesh())
	{
		SkeletalMeshComponent->SetSkeletalMesh(SkelMesh);
		SkeletalMeshComponent->SetVisibility(true);
		StaticMeshComponent->SetVisibility(false);
	}
}

void APickUpItem::BindEvents()
{
	CollisionSphere->OnBeginCursorOver.AddDynamic(this, &APickUpItem::OnCursorOver);
	CollisionSphere->OnEndCursorOver.AddDynamic(this, &APickUpItem::OnCursorEndOver);
}

void APickUpItem::OnCursorOver(UPrimitiveComponent* TouchedComponent)
{
	//HighlightActor();
}

void APickUpItem::OnCursorEndOver(UPrimitiveComponent* TouchedComponent)
{
	//UnHighlightActor();
}

void APickUpItem::DropTimelineUpdate(float Value)
{
	const FVector NewLocation = FMath::Lerp(DropStartLocation, DropEndLocation, Value);
	SetActorLocation(NewLocation);


}

void APickUpItem::DropTimelineFinished()
{
	StaticMeshComponent->SetSimulatePhysics(true);
	SkeletalMeshComponent->SetSimulatePhysics(true);
	// 动画结束后，开启物理模拟或使其固定
	// 也可以在这里启动地面生命周期计时器
	if (HasAuthority() && ItemData && ItemData->GetLifeTimeOnGround() > 0.f)
	{
		GetWorldTimerManager().SetTimer(LifeSpanTimer, this, &APickUpItem::StartRecycle, ItemData->GetLifeTimeOnGround(), false);
	}
}

//传入的时Curve根据时间插值
void APickUpItem::PickUpTimelineUpdate(float Value)
{
	if (PickUpTargetActor.IsValid())//todo item的位置有点问题 server和client不同步
	{
		// --- 修改：使用缓存的起始位置 ---
		const FVector EndLocation = PickUpTargetActor->GetActorLocation();
		const FVector NewLocation = FMath::Lerp(PickUpStartLocation, EndLocation, Value);
		// --------------------------------
		SetActorLocationAndRotation(NewLocation, GetActorRotation(), false, nullptr, ETeleportType::TeleportPhysics);
	}
}

void APickUpItem::PickUpTimelineFinished()
{
	// 在所有客户端和服务器上立即隐藏Actor，消除视觉延迟
	SetActorHiddenInGame(true);

	if (HasAuthority())
	{
		if (PickUpTargetActor.IsValid())
		{
			if (UInventoryComponent* Inventory = PickUpTargetActor->FindComponentByClass<UInventoryComponent>())
			{
				Inventory->FindEmptySlotAndAddItem(ItemData);
			}
		}

		StartRecycle();
	}
}

void APickUpItem::Multicast_PlayDropEffects_Implementation(const FVector& FinalLocation)
{
	DropStartLocation = GetActorLocation();
	DropEndLocation = FinalLocation;

	//StaticMeshComponent->SetSimulatePhysics(false);
	//SkeletalMeshComponent->SetSimulatePhysics(false);
	//SetReplicatingMovement(false);

	if (ItemData && ItemData->GetDropSound())
	{
		UGameplayStatics::PlaySoundAtLocation(this, ItemData->GetDropSound(), DropStartLocation);
	}

	if (DropTimeline)
	{
		DropTimeline->PlayFromStart();
	}
}

void APickUpItem::Multicast_PlayPickUpEffects_Implementation(AActor* InteractingActor)
{
	PickUpTargetActor = InteractingActor;
	//CollisionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//StaticMeshComponent->SetSimulatePhysics(false);
	//SkeletalMeshComponent->SetSimulatePhysics(false);


	//// --- 新增：临时关闭位置同步，允许客户端自由移动 ---
	//SetReplicatingMovement(false);

	PickUpStartLocation = GetActorLocation();

	if (ItemData && ItemData->GetPickUpSound())
	{
		UGameplayStatics::PlaySoundAtLocation(this, ItemData->GetPickUpSound(), GetActorLocation());
	}

	if (PickUpTimeline)
	{
		PickUpTimeline->PlayFromStart();
	}
}


void APickUpItem::ActivateItem(int64 NewItemID, const FName& InItemName, const int32 InQuantity, const FVector& WorldLocation)
{
	// 确保只在服务器上执行
	if (!HasAuthority()) return;

	if (!ItemData)
	{
		ItemData = NewObject<UItem>(this);
	}

	ItemID = NewItemID;
	SetItemName(InItemName);
	SetItemQuantity(InQuantity);
	SetActorLocation(WorldLocation);
	SetActorHiddenInGame(false);
	//SetActorEnableCollision(true);
	SetActorTickEnabled(true);


	//SetReplicatingMovement(true);
	//StaticMeshComponent->SetSimulatePhysics(true);
	//SkeletalMeshComponent->SetSimulatePhysics(true);

	// 如果有生命周期，在这里启动计时器
	if (ItemData && ItemData->GetLifeTimeOnGround() > 0.f)
	{
		GetWorldTimerManager().SetTimer(LifeSpanTimer, this, &APickUpItem::StartRecycle, ItemData->GetLifeTimeOnGround(), false);
	}
}

void APickUpItem::DeactivateItem()
{
	// 确保只在服务器上执行
	if (!HasAuthority()) return;
	ItemID = -1;
	SetActorHiddenInGame(true);
	//StaticMeshComponent->SetSimulatePhysics(false);
	//SkeletalMeshComponent->SetSimulatePhysics(false);
	//SetActorEnableCollision(false);
	SetActorTickEnabled(false);
	GetWorldTimerManager().ClearAllTimersForObject(this);
}

void APickUpItem::StartRecycle()
{
	// 不再直接调用 Destroy()，而是让管理器来处理
	if (HasAuthority() && UItemManager::Get(this))
	{
		UItemManager::Get(this)->ReleaseItem(this);
	}
	else
	{
		// 如果没有管理器，则按旧方式销毁
		Destroy();
	}
}