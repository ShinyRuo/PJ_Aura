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

void AAuraCharacter::SaveProgress_Implementation(const FName& CheckPointTag)
{
	if (!HasAuthority())
	{
		return;
	}
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

		SaveData->PlayerStartTag = CheckPointTag;
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

