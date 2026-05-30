// ALL CODE FOR wangjunyang learning GAS


#include "Chararctor/AuraCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/AuraPlayerState.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "NiagaraComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"
#include "Actor/PickUpItem.h"
#include "Camera/CameraComponent.h"
#include "Game/AuraGameInstance.h"
#include "Game/AuraGameModeBase.h"
#include "Game/LoadScreenSaveGame.h"
#include "Player/AuraPlayerController.h"
#include "UI/HUD/AuraHUD.h"
#include "Inventory/InventoryComponent.h"
#include "Kismet/GameplayStatics.h"


AAuraCharacter::AAuraCharacter()
{
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	CharacterClass = ECharacterClass::Elementalist;

	NiagaraPlayComponent = CreateDefaultSubobject<UNiagaraComponent>("NiagaraPlayComponent");
	NiagaraPlayComponent->SetupAttachment(GetRootComponent());
	NiagaraPlayComponent->bAutoActivate = false;
}

void AAuraCharacter::BeginPlay()
{
	Super::BeginPlay();
	TArray<UCameraComponent*> CameraComps;
	GetComponents<UCameraComponent>(CameraComps);

	// 2. 遍历并匹配名称
	for (UCameraComponent* Comp : CameraComps)
	{
		if (Comp && Comp->GetName() == TEXT("TopDownCamera"))
		{
			TopDownCameraComponent = Comp; //TObjectPtr类型变量 = UObject原生指针 不会有问题 引用计数会正常加 因为TObjectPtr也依赖于垃圾回收机制
		}
	}
}

void AAuraCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	//Init ablity actor info for the Server
	InitAbilityActorInfo();
	//我选择在server执行 然后rep所有属性到client
	// todo load saved data here
	LoadProgress();

	Client_ProcessedBy();
}

void AAuraCharacter::LoadProgress()
{
	if (AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{

		ULoadScreenSaveGame* SaveData = AuraGameMode->RetrieveInGameSaveData();
		if (!SaveData)
		{
			return;
		}

		AuraGameMode->LoadWorldState(GetWorld(), SaveData);

		LoadInventory(SaveData);

		if (SaveData->bFirstTimeLoadIn)
		{
			//第一次加载 使用初始属性值
			InitializeDefaultAttributes();
			AddCharacterAbilities();
		}
		else
		{
			if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(GetAbilitySystemComponent()))
			{
				AuraASC->AddCharacterAbilitiesFromSavedData(SaveData);
			}
			if (AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>())
			{
				AuraPlayerState->SetPlayerLevel(SaveData->PlayerLevel);
				AuraPlayerState->SetPlayerExp(SaveData->PlayerXP);
				AuraPlayerState->SetAttributePoints(SaveData->AttributePoints);
				AuraPlayerState->SetSpellPoints(SaveData->SpellPoints);
			}
			UAuraAbilitySystemLibrary::InitalizeDefaultAttributesForSaveData(this, AbilitySystemComponent, SaveData);
		}
		//施加装备提供的属性加成GE
		ApplyEquipmentAttributeGE_OnInit();
	}
}

void AAuraCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	//Init ablity actor info for the Client
	InitAbilityActorInfo();
}

void AAuraCharacter::OnRep_Stunned()
{
	if(UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
	{
		const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
		if (bIsStunned)
		{
			AuraASC->AddLooseGameplayTag(GameplayTags.Player_Block_InputPressed);
			AuraASC->AddLooseGameplayTag(GameplayTags.Player_Block_InputReleased);
			AuraASC->AddLooseGameplayTag(GameplayTags.Player_Block_InputHeld);
			AuraASC->AddLooseGameplayTag(GameplayTags.Player_Block_CursorTrace);
			StunDebuffComponent->Activate();
		}
		else
		{
			AuraASC->RemoveLooseGameplayTag(GameplayTags.Player_Block_InputPressed);
			AuraASC->RemoveLooseGameplayTag(GameplayTags.Player_Block_InputReleased);
			AuraASC->RemoveLooseGameplayTag(GameplayTags.Player_Block_InputHeld);
			AuraASC->RemoveLooseGameplayTag(GameplayTags.Player_Block_CursorTrace);
			StunDebuffComponent->Deactivate();
		}
	}
}

void AAuraCharacter::OnRep_Burned()
{
	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
	{
		const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
		if (bIsBurned)
		{
			BurnDebuffComponent->Activate();
		}
		else
		{
			BurnDebuffComponent->Deactivate();
		}
	}
}



int32 AAuraCharacter::GetPlayerLevel_Implementation()
{
	const AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetPlayerLevel();
}

void AAuraCharacter::Die(const FVector& DeathImpulse)
{
	Super::Die(DeathImpulse);
	//创建一个委托，用于绑定委托回调
	FTimerDelegate DeathTimerDelegate;
	DeathTimerDelegate.BindLambda([this]()
		{
			if (const AAuraGameModeBase* RPGGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
			{
				RPGGameMode->PlayerDied(this);
			}
		});

	//通过定时器触发对应的委托广播
	GetWorldTimerManager().SetTimer(DeathTimer, DeathTimerDelegate, DeathTime, false);

	//防止相机在玩家角色死亡后跟随移动，将相机固定在世界坐标位置
	TopDownCameraComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
}

void AAuraCharacter::AddToExp_Implementation(int32 InExp)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->AddPlayerExp(InExp);
}

void AAuraCharacter::LevelUp_Implementation()
{
	MulticastLevelUpParticles();
}

void AAuraCharacter::MulticastLevelUpParticles_Implementation() const
{
	if (IsValid(NiagaraPlayComponent) && !HasAuthority())
	{
		const FVector CameraLocation = TopDownCameraComponent->GetComponentLocation();
		const FVector EffectLocation = NiagaraPlayComponent->GetComponentLocation();
		const FRotator ToCameraLocation = (CameraLocation - EffectLocation).Rotation();
		NiagaraPlayComponent->SetWorldRotation(ToCameraLocation);
		NiagaraPlayComponent->Activate(true);
	}
}


int32 AAuraCharacter::GetExp_Implementation()
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetPlayerExp();
}

int32 AAuraCharacter::FindLevelForExp_Implementation(int32 InExp)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	check(AuraPlayerState->LevelUpInfo);
	return  AuraPlayerState->LevelUpInfo->FindLevelForXp(InExp);
}

int32 AAuraCharacter::GetAttributePointsRewards_Implementation(int32 Level) const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	check(AuraPlayerState->LevelUpInfo);
	check(AuraPlayerState->LevelUpInfo->LevelUpInformation.Num() > Level);
	return AuraPlayerState->LevelUpInfo->LevelUpInformation[Level].AttributePointAward;
}

int32 AAuraCharacter::GetSpellPointsRewards_Implementation(int32 Level) const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	check(AuraPlayerState->LevelUpInfo);
	check(AuraPlayerState->LevelUpInfo->LevelUpInformation.Num() > Level);
	return AuraPlayerState->LevelUpInfo->LevelUpInformation[Level].SpellPointAward;
}

void AAuraCharacter::AddToAttributePoints_Implementation(int32 InAttributePoints)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->AddAttributePoints(InAttributePoints);
}

void AAuraCharacter::AddToPlayerLevel_Implementation(int32 InPlayLevel)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->AddPlayerLevel(InPlayLevel);

	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(GetAbilitySystemComponent()))
	{
		AuraASC->UpdateAbilityStatus(Execute_GetPlayerLevel(this));
	}
	

}

void AAuraCharacter::AddToSpellPoints_Implementation(int32 InSpellPoints)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->AddSpellPoints(InSpellPoints);
}

int32 AAuraCharacter::GetSpellPoints_Implementation() const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetSpellPoints();
}

int32 AAuraCharacter::GetAttributePoints_Implementation() const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetAttributePoints();
}

void AAuraCharacter::ShowMagicCircle_Implementation(UMaterialInterface* DecalMaterial)
{
	if (AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController()))
	{
		AuraPlayerController->ShowMagicCircle(DecalMaterial);
		AuraPlayerController->bShowMouseCursor = false;
	}
}

void AAuraCharacter::HideMagicCircle_Implementation()
{
	if (AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController()))
	{
		AuraPlayerController->HideMagicCircle();
		AuraPlayerController->bShowMouseCursor = true;
	}
}

void AAuraCharacter::SaveProgress_Implementation(const FName& CheckPointTag, const FString& DestinationMapAssetName )
{
	if (!HasAuthority())
	{
		return;
	}
	UAuraGameInstance* AuraGI = Cast<UAuraGameInstance>(GetGameInstance());
	check(AuraGI);
	if(AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		ULoadScreenSaveGame* SaveData = AuraGameMode->RetrieveInGameSaveData();
		if (!SaveData)
		{
			return;
		}
		//World信息
		AuraGameMode->SaveWorldState(GetWorld(), SaveData);
		//Player信息

		SaveInventory(SaveData);

		if (DestinationMapAssetName != FString(""))
		{
			SaveData->MapAssetName = DestinationMapAssetName;
			SaveData->MapName = AuraGameMode->GetMapNameFromMapAssetName(DestinationMapAssetName);
		}

		SaveData->PlayerStartTag = CheckPointTag;
		AuraGI->PlayerStartTag = CheckPointTag;

		SaveData->bFirstTimeLoadIn = false;

		if (AAuraPlayerState* AuraPlayerState = Cast<AAuraPlayerState>(GetPlayerState()))
		{
			SaveData->PlayerLevel = AuraPlayerState->GetPlayerLevel();
			SaveData->PlayerXP = AuraPlayerState->GetPlayerExp();
			SaveData->AttributePoints = AuraPlayerState->GetAttributePoints();
			SaveData->SpellPoints = AuraPlayerState->GetSpellPoints();
		}
		SaveData->Strength = UAuraAttributeSet::GetStrengthAttribute().GetNumericValue(GetAttributeSet());
		SaveData->Intelligence = UAuraAttributeSet::GetIntelligenceAttribute().GetNumericValue(GetAttributeSet());
		SaveData->Resilience = UAuraAttributeSet::GetResilienceAttribute().GetNumericValue(GetAttributeSet());
		SaveData->Vigor = UAuraAttributeSet::GetVigorAttribute().GetNumericValue(GetAttributeSet());

		
		FForEachAbility SaveAbilityDelegate;
		UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent);
		SaveData->SavedAbilities.Empty();
		SaveAbilityDelegate.BindLambda(
			[this, AuraASC,SaveData](const FGameplayAbilitySpec& AbilitySpec)
			{
				FSavedAbility SavedAbility;
				FGameplayTag Tag = AuraASC->GetAbilityTagFromSpec(AbilitySpec);

				SavedAbility.GameplayAbility = AbilitySpec.Ability->GetClass();
				SavedAbility.AbilityLevel = AbilitySpec.Level;
				SavedAbility.AbilityTag = Tag;
				SavedAbility.AbilitySlot = AuraASC->GetAbilitySlotFromTag(Tag);
				SavedAbility.AbilityStatus = AuraASC->GetAbilityStatusTagFromTag(Tag);
				if (UAbilityInfo* Info = UAuraAbilitySystemLibrary::GetAbilityInfo(this))
				{
					FAuraAbilityInfo AbilityInfo = Info->FindAbilityInfoForTag(Tag);
					SavedAbility.AbilityType = AbilityInfo.AbilityType;
				}

				SaveData->SavedAbilities.AddUnique(SavedAbility);
			}
		);
		AuraASC->ForEachAbility(SaveAbilityDelegate);
		
		AuraGameMode->SaveInGameProgressData(SaveData);
	}
}

void AAuraCharacter::PickUpItem_Implementation(UItem* PickUpItem)
{
	if (AAuraPlayerState* AuraPlayerState = Cast<AAuraPlayerState>(GetPlayerState()))
	{
		if (UInventoryComponent* InventoryComp = AuraPlayerState->GetInventoryComponent())
		{
			InventoryComp->FindEmptySlotAndAddItem(PickUpItem);
		}
	}
}

void AAuraCharacter::DropItem_Implementation(UItem* DropItem)
{
}


FVector AAuraCharacter::GetCaptureLocalPosition() const
{
	return HeadshotCaptureLocalPosition;
}

FRotator AAuraCharacter::GetCaptureLocalRotation() const
{
	return HeadshotCaptureLocalRotation;
}


void AAuraCharacter::OnPickUpItemBegin_Implementation(APickUpItem* ItemToPickUp)
{
	if (!ItemToPickUp)
	{
		return;
	}

	if (AAuraPlayerState* AuraPlayerState = Cast<AAuraPlayerState>(GetPlayerState()))
	{
		if (UInventoryComponent* InventoryComp = AuraPlayerState->GetInventoryComponent())
		{
			if (!InventoryComp->CanAddItem(ItemToPickUp->GetItem()))
			{
				if (GEngine)
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("背包空间不足，无法拾取"));
				}
				return;
			}
		}
	}

	ItemToPickUp->OnInteracted(this);
}

void AAuraCharacter::SaveInventory(ULoadScreenSaveGame* SaveData)
{
	if (!SaveData) return;
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->SaveInventory(SaveData);
}

void AAuraCharacter::LoadInventory(const ULoadScreenSaveGame* SaveData)
{
	if (!SaveData) return;
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->LoadInventory(SaveData);
}

void AAuraCharacter::ApplyEquipmentAttributeGE_OnInit()
{
	// 在玩家初始化成功后调用一次（Server）
	if (!HasAuthority()) return;

	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	if (!IsValid(AuraPlayerState)) return;

	OnEquipmentUpdate_ApplyEffectModifiers();

	UInventoryComponent* InventoryComponent = AuraPlayerState->GetInventoryComponent();
	if (!IsValid(InventoryComponent)) return;
	InventoryComponent->OnEquipmentUpdateSignature.AddUniqueDynamic(this,&ThisClass::OnEquipmentUpdate_ApplyEffectModifiers);
}

void AAuraCharacter::OnEquipmentUpdate_ApplyEffectModifiers()
{
	// Called when equipment/attributes from equipment change - server only
	if (!HasAuthority()) return;

	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	if (!IsValid(AuraPlayerState)) return;

	UInventoryComponent* InventoryComponent = AuraPlayerState->GetInventoryComponent();
	if (!IsValid(InventoryComponent)) return;

	TMap<FGameplayTag, float> AllEquipmentAddAttributes;
	InventoryComponent->GetAllEquipmentAddedAttributes(AllEquipmentAddAttributes);

	UAuraAbilitySystemComponent* ASC = Cast<UAuraAbilitySystemComponent>(AuraPlayerState->GetAbilitySystemComponent());
	if (!ASC) return;

	UCharacterClassInfo* ClassInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(this);
	if (!ClassInfo) return;


	//通过CDO 把所有的 Modifiers 读出来 在SetByCallerMap中设置为  0
	UGameplayEffect* GECDO = ClassInfo->SecondaryAttributes_Infinite_EquipmentProvided->GetDefaultObject<UGameplayEffect>();
	if (!GECDO)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get CDO for class: %s"), *ClassInfo->SecondaryAttributes_Infinite_EquipmentProvided->GetName());
		return;
	}

	//  访问 CDO 的 Modifiers 数组
	const TArray<FGameplayModifierInfo>& Modifiers = GECDO->Modifiers;
	TMap<FGameplayTag, float> SetByCallerMap;

	for (int32 i = 0; i < Modifiers.Num(); ++i)
	{
		const FGameplayModifierInfo& Modifier = Modifiers[i];

		if (Modifier.ModifierMagnitude.GetMagnitudeCalculationType() == EGameplayEffectMagnitudeCalculation::SetByCaller)
		{
			FGameplayTag AttributeTag = Modifier.ModifierMagnitude.GetSetByCallerFloat().DataTag;
			SetByCallerMap.Add(AttributeTag, 0.f);
		}

	}

	//装备中的属性加成 覆盖 SetByCallerMap 中的默认值
	for (const auto& Pair : AllEquipmentAddAttributes)
	{
		const FGameplayTag& SetByCallerTag = Pair.Key;
		if (SetByCallerMap.Contains(SetByCallerTag))
		{
			SetByCallerMap[SetByCallerTag] = Pair.Value;
		}
	}

	// 如果已存在已应用的 GE（保存的 ActiveHandle），直接更新 SetByCaller magnitudes
	if (EquipmentAttributeGEHandle.IsValid())
	{
		ASC->UpdateActiveGameplayEffectSetByCallerMagnitudes(EquipmentAttributeGEHandle, SetByCallerMap);
	}
	else
	{
		// 后备：若 ActiveHandle 丢失，重新应用带初始值的 Spec 并保存 Handle
		if (ClassInfo->SecondaryAttributes_Infinite_EquipmentProvided)
		{
			FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
			FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(ClassInfo->SecondaryAttributes_Infinite_EquipmentProvided, 1.f, EffectContext);
			if (SpecHandle.IsValid())
			{
				// 将所有 set-by-caller 数值注入到 Spec（初次应用时）
				for (const auto& KV : SetByCallerMap)
				{
					SpecHandle.Data.Get()->SetSetByCallerMagnitude(KV.Key, KV.Value);
				}
				FActiveGameplayEffectHandle ActiveHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
				EquipmentAttributeGEHandle = ActiveHandle;
			}
		}
	}
}

void AAuraCharacter::Client_ProcessedBy_Implementation()
{
	Client_OnProcessedBy();
}

void AAuraCharacter::InitAbilityActorInfo()
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(AuraPlayerState, this);
	Cast<UAuraAbilitySystemComponent>(AuraPlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet();
	AbilitySystemComponent = AuraPlayerState->GetAbilitySystemComponent();
	AttributeSet = AuraPlayerState->GetAttributeSet();
	OnAscRegistered.Broadcast(AbilitySystemComponent);

	AbilitySystemComponent->RegisterGameplayTagEvent(FAuraGameplayTags::Get().Debuff_Stun, EGameplayTagEventType::NewOrRemoved).AddUObject(
		this,
		&ThisClass::StunTagChanged
	);
	AbilitySystemComponent->RegisterGameplayTagEvent(FAuraGameplayTags::Get().Debuff_Burn, EGameplayTagEventType::NewOrRemoved).AddUObject(
		this,
		&ThisClass::BurnTagChanged
	);
	

	//在server上 是有所有AAuraCharacter的Controller的 在client上只有自己的 拿不到其他AAuraCharacter的Controller
	//所以这里不做check判断 
	if (AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController()))
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(AuraPlayerController->GetHUD()))
		{
			AuraHUD->InitOverlay(AuraPlayerController, AuraPlayerState, AbilitySystemComponent, AttributeSet);
		}
	}


}

