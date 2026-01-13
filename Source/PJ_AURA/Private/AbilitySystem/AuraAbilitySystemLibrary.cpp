// ALL CODE FOR wangjunyang learning GAS


#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilityTypes.h"
#include "AuraGameplayTags.h"
#include "Actor/PickUpItem.h"
#include "Components/SlateWrapperTypes.h"
#include "Game/AuraGameModeBase.h"
#include "Game/ItemManager.h"
#include "Game/LoadScreenSaveGame.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"
#include "UI/Widget/AuraUserWidget.h"
#include "UI/WidgetController/AuraWidgetController.h"


bool UAuraAbilitySystemLibrary::MakeWidgetControllerParams(const UObject* WorldContextObject, FWidgetContorllerParams& OutParam)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PC->GetHUD()))
		{
			AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			OutParam.AbilitySystemComponent = ASC;
			OutParam.PlayerController = PC;
			OutParam.PlayerState = PS;
			OutParam.AttributeSet = AS;
			return true;
		}
	}
	return false;
}

UOverlayWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	FWidgetContorllerParams WidgetControllerParams;
	if(MakeWidgetControllerParams(WorldContextObject, WidgetControllerParams))
	{
		if(AAuraHUD* AuraHUD = Cast<AAuraHUD>(WidgetControllerParams.PlayerController->GetHUD()))
		{
			return AuraHUD->GetOverlayWidgetController(WidgetControllerParams);
		}
	}
	return nullptr;
}

UAttributeMenuWidgetController* UAuraAbilitySystemLibrary::GetAttributeMenuWidgetController(const UObject* WorldContextObject)
{
	FWidgetContorllerParams WidgetControllerParams;
	if (MakeWidgetControllerParams(WorldContextObject, WidgetControllerParams))
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(WidgetControllerParams.PlayerController->GetHUD()))
		{
			return AuraHUD->GetAttributeMenuWidgetController(WidgetControllerParams);
		}
	}
	return nullptr;
}

USpellMenuWidgetController* UAuraAbilitySystemLibrary::GetSpellMenuWidgetController(const UObject* WorldContextObject)
{
	FWidgetContorllerParams WidgetControllerParams;
	if (MakeWidgetControllerParams(WorldContextObject, WidgetControllerParams))
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(WidgetControllerParams.PlayerController->GetHUD()))
		{
			return AuraHUD->GetSpellMenuWidgetController(WidgetControllerParams);
		}
	}
	return nullptr;
}

UInventoryWidgetController* UAuraAbilitySystemLibrary::GetInventoryWidgetController(const UObject* WorldContextObject)
{

	FWidgetContorllerParams WidgetControllerParams;
	if(MakeWidgetControllerParams(WorldContextObject, WidgetControllerParams))
	{
		if(AAuraHUD* AuraHUD = Cast<AAuraHUD>(WidgetControllerParams.PlayerController->GetHUD()))
		{
			return AuraHUD->GetInventoryWidgetController(WidgetControllerParams);
		}
	}
	return nullptr;
}

UAuraUserWidget* UAuraAbilitySystemLibrary::ShowAuraWidget(const UObject* WorldContextObject, const FName& WidgetName,
	TSubclassOf<UAuraUserWidget> WidgetClass)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PC->GetHUD()))
		{
			UAuraUserWidget* AuraWidget = AuraHUD->GetOrCreateWidget(WidgetName, WidgetClass);
			AuraWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			return AuraWidget;
		}
	}
	return nullptr;
}

void UAuraAbilitySystemLibrary::HideAuraWidget(const UObject* WorldContextObject, const FName& WidgetName)
{
	
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
    {
        if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PC->GetHUD()))
        {
            UAuraUserWidget* Widget = AuraHUD->GetWidgetByName(WidgetName);
            if (Widget)
            {
                // 设置 Widget 的可见性为隐藏
                Widget->SetVisibility(ESlateVisibility::Hidden);
            }
        }
    }
}

void UAuraAbilitySystemLibrary::InitalizeDefaultAttributes(const UObject* WorldContextObject,ECharacterClass CharacterClass, float Level,UAbilitySystemComponent* ASC)
{
	UCharacterClassInfo* ClassInfo = GetCharacterClassInfo(WorldContextObject);
	check(ClassInfo);
	check(ASC);

	AActor* AvatarActor = ASC->GetAvatarActor();
	FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
	EffectContext.AddSourceObject(AvatarActor);

	FCharacterClassDefaultInfo ClassDefaultInfo = ClassInfo->GetClassDefaultInfo(CharacterClass);

	const FGameplayEffectSpecHandle PrimaryAttributeSpecHandle = ASC->MakeOutgoingSpec(ClassDefaultInfo.PrimaryAttributes, Level, EffectContext);
	ASC->ApplyGameplayEffectSpecToSelf(*PrimaryAttributeSpecHandle.Data.Get());

	const FGameplayEffectSpecHandle SecondaryAttributeSpecHandle = ASC->MakeOutgoingSpec(ClassInfo->SecondaryAttributes, Level, EffectContext);
	ASC->ApplyGameplayEffectSpecToSelf(*SecondaryAttributeSpecHandle.Data.Get());

	const FGameplayEffectSpecHandle VitalAttributeSpecHandle = ASC->MakeOutgoingSpec(ClassInfo->VitalAttributes, Level, EffectContext);
	ASC->ApplyGameplayEffectSpecToSelf(*VitalAttributeSpecHandle.Data.Get());

}

void UAuraAbilitySystemLibrary::InitalizeDefaultAttributesForSaveData(const UObject* WorldContextObject,
	 UAbilitySystemComponent* ASC, ULoadScreenSaveGame* SaveGame)
{
	UCharacterClassInfo* ClassInfo = GetCharacterClassInfo(WorldContextObject);
	check(ClassInfo);
	check(ASC);
	const FAuraGameplayTags& AuraTags = FAuraGameplayTags::Get();
	const AActor* SourceAvatarActor = ASC->GetAvatarActor();
	FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
	EffectContext.AddSourceObject(SourceAvatarActor);

	const FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(ClassInfo->PrimaryAttributes_SetByCaller, 1.f, EffectContext);

	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, AuraTags.Attributes_Primary_Strength, SaveGame->Strength);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, AuraTags.Attributes_Primary_Intelligence, SaveGame->Intelligence);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, AuraTags.Attributes_Primary_Resilience, SaveGame->Resilience);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, AuraTags.Attributes_Primary_Vigor, SaveGame->Vigor);

	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);

	const FGameplayEffectSpecHandle SecondaryAttributeSpecHandle = ASC->MakeOutgoingSpec(ClassInfo->SecondaryAttributes_Infinite, 1.f, EffectContext);
	ASC->ApplyGameplayEffectSpecToSelf(*SecondaryAttributeSpecHandle.Data.Get());

	const FGameplayEffectSpecHandle VitalAttributeSpecHandle = ASC->MakeOutgoingSpec(ClassInfo->VitalAttributes, 1.f, EffectContext);
	ASC->ApplyGameplayEffectSpecToSelf(*VitalAttributeSpecHandle.Data.Get());
}


void UAuraAbilitySystemLibrary::GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, ECharacterClass CharacterClass)
{
	UCharacterClassInfo* ClassInfo = GetCharacterClassInfo(WorldContextObject);
	check(ClassInfo);
	check(ASC);
	for (auto AbilityClass : ClassInfo->CommonAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		ASC->GiveAbility(AbilitySpec);
	}
	const FCharacterClassDefaultInfo& DefaultInfo = ClassInfo->GetClassDefaultInfo(CharacterClass);
	int32 Level = 1;
	if (ASC->GetAvatarActor() && ASC->GetAvatarActor()->Implements<UCombatInterface>())
	{
		Level = ICombatInterface::Execute_GetPlayerLevel(ASC->GetAvatarActor());
	}
	for (auto AbilityClass : DefaultInfo.DefaultAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, Level);
		ASC->GiveAbility(AbilitySpec);
	}
}

UCharacterClassInfo* UAuraAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	const AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (!AuraGameMode) return nullptr;
	return AuraGameMode->CharacterClassInfo;
}

UAbilityInfo* UAuraAbilitySystemLibrary::GetAbilityInfo(const UObject* WorldContextObject)
{
	const AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (!AuraGameMode) return nullptr;
	return AuraGameMode->AbilityInfo;
}




bool UAuraAbilitySystemLibrary::IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return EffectContext->IsBlockedHit();
	}
	return false;
}

bool UAuraAbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return EffectContext->IsCriticalHit();
	}
	return false;
}

bool UAuraAbilitySystemLibrary::IsSuccessfulDebuff(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return EffectContext->IsSuccessfulDebuff();
	}
	return false;
}

float UAuraAbilitySystemLibrary::GetDebuffDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return EffectContext->GetDebuffDamage();
	}
	return 0.f; 
}

float UAuraAbilitySystemLibrary::GetDebuffDuration(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return EffectContext->GetDebuffDuration();
	}
	return 0.f; 
}

FVector UAuraAbilitySystemLibrary::GetDeathImpulse(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return EffectContext->GetDeathImpulse();
	}
	return FVector::ZeroVector;
}

FVector UAuraAbilitySystemLibrary::GetKnockbackForce(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return EffectContext->GetKnockbackForce();
	}
	return FVector::ZeroVector;
}

float UAuraAbilitySystemLibrary::GetDebuffFrequency(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return EffectContext->GetDebuffFrequency();
	}
	return 0.f; 
}

TSharedPtr<FGameplayTag> UAuraAbilitySystemLibrary::GetDamageType(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return EffectContext->GetDamageType();
	}
	return nullptr; 
}

FGameplayTag UAuraAbilitySystemLibrary::GetDamageTypeValue(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* EffectContext = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		const TSharedPtr<FGameplayTag>& DamageTypePtr = EffectContext->GetDamageType();
		if (DamageTypePtr.IsValid())
		{
			return *DamageTypePtr;
		}
	}
	return FGameplayTag(); 
}

void UAuraAbilitySystemLibrary::SetIsCriticalHit(FGameplayEffectContextHandle& EffectContextHandle, bool bValue)
{
	if (FAuraGameplayEffectContext* EffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return EffectContext->SetIsCriticalHit(bValue);
	}
}
void UAuraAbilitySystemLibrary::SetSuccessfulDebuff(FGameplayEffectContextHandle& EffectContextHandle, bool bValue)
{
	if (FAuraGameplayEffectContext* EffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		EffectContext->SetIsSuccessfulDebuff(bValue);
	}
}
void UAuraAbilitySystemLibrary::SetDebuffDamage(FGameplayEffectContextHandle& EffectContextHandle, float InDamage)
{
	if (FAuraGameplayEffectContext* EffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get())) 
	{ EffectContext->SetDebuffDamage(InDamage); }
}
void UAuraAbilitySystemLibrary::SetDebuffDuration(FGameplayEffectContextHandle& EffectContextHandle, float InDuration)
{
	if (FAuraGameplayEffectContext* EffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		EffectContext->SetDebuffDuration(InDuration);
	}
}
void UAuraAbilitySystemLibrary::SetDebuffFrequency(FGameplayEffectContextHandle& EffectContextHandle, float InFrequency)
{
	if (FAuraGameplayEffectContext* EffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		EffectContext->SetDebuffFrequency(InFrequency);
	}
}

void UAuraAbilitySystemLibrary::SetDeathImpulse(FGameplayEffectContextHandle& EffectContextHandle,
	const FVector& InDeathImpulse)
{
	if (FAuraGameplayEffectContext* EffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		EffectContext->SetDeathImpulse(InDeathImpulse);
	}
}

void UAuraAbilitySystemLibrary::SetKnockbackForce(FGameplayEffectContextHandle& EffectContextHandle,
	const FVector& InKnockbackForce)
{
	if (FAuraGameplayEffectContext* EffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		EffectContext->SetKnockbackForce(InKnockbackForce);
	}
}

void UAuraAbilitySystemLibrary::SetDamageType(FGameplayEffectContextHandle& EffectContextHandle, const TSharedPtr<FGameplayTag>& InDamageType)
{
	if (FAuraGameplayEffectContext* EffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		EffectContext->SetDamageType(InDamageType);
	}
}
void UAuraAbilitySystemLibrary::SetDamageTypeByValue(FGameplayEffectContextHandle& EffectContextHandle, const FGameplayTag& InDamageType)
{
	if (FAuraGameplayEffectContext* EffectContext = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		EffectContext->SetDamageType(MakeShareable(new FGameplayTag(InDamageType)));
	}
}

void UAuraAbilitySystemLibrary::SetIsRadialDamageEffectParams(FDamageEffectParams& DamageEffectParams,
	bool isRadial, float InnerRadius, float OuterRadiius, FVector Origin)
{
	DamageEffectParams.bIsRadialDamage = isRadial;
	DamageEffectParams.RadialDamageInnerRadius = InnerRadius;
	DamageEffectParams.RadialDamageOuterRadius = OuterRadiius;
	DamageEffectParams.RadialDamageOrigin = Origin;
}

void UAuraAbilitySystemLibrary::SetKnockbackDirection(FDamageEffectParams& DamageEffectParams, FVector KnockbackDirection, float Magnitude)
{
	if (Magnitude != 0.f)
	{
		DamageEffectParams.KnockbackForceMagnitude = Magnitude;
	}
	DamageEffectParams.KnockbackForce = KnockbackDirection.GetSafeNormal() * DamageEffectParams.KnockbackForceMagnitude;
}

void UAuraAbilitySystemLibrary::SetImpulseDirection(FDamageEffectParams& DamageEffectParams, FVector ImpulseDirection, float Magnitude)
{
	if (Magnitude != 0.f)
	{
		DamageEffectParams.DeathImpulseMagnitude = Magnitude;
	}
	DamageEffectParams.DeathImpulse = ImpulseDirection.GetSafeNormal() * DamageEffectParams.DeathImpulseMagnitude;
}

void UAuraAbilitySystemLibrary::SetEffectParamsTargetASC(FDamageEffectParams& DamageEffectParams,
	 UAbilitySystemComponent* InASC)
{
	DamageEffectParams.TargetAbilitySystemComponent = InASC;
}

void UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(const UObject* WorldContextObject,
                                                           TArray<AActor*>& OutOverlappingActors, const TArray<AActor*>& ActorsToIgnore, float Radius,
                                                           const FVector& SphereOrigin)
{
	FCollisionQueryParams SphereParams;
	SphereParams.AddIgnoredActors(ActorsToIgnore);

	TArray<FOverlapResult> Overlaps;
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		World->OverlapMultiByObjectType(Overlaps, SphereOrigin, FQuat::Identity, FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects), FCollisionShape::MakeSphere(Radius), SphereParams);
		for (FOverlapResult& Overlap : Overlaps)
		{
			if (const bool ImplementsCombatInterface = Overlap.GetActor()->Implements<UCombatInterface>())
			{
				if (const bool IsAlive = !ICombatInterface::Execute_IsDead(Overlap.GetActor()))
				{
					OutOverlappingActors.AddUnique(ICombatInterface::Execute_GetAvatar(Overlap.GetActor()));
				}
			}
		}
	}
}

void UAuraAbilitySystemLibrary::GetClosestTargets(int32 MaxTarget, const TArray<AActor*>& Actors,
	TArray<AActor*>& OutClosestTargets, const FVector& Origin)
{
	if (Actors.Num() <= MaxTarget)
	{
		OutClosestTargets = Actors;
		return;
	}
	// 创建一个临时数组，用于保存 Actor 和它到 Origin 的距离
	struct FActorDistance
	{
		AActor* Actor;
		float Distance;
		FActorDistance(AActor* InActor, float InDistance)
			: Actor(InActor), Distance(InDistance) {
		}
	};
	TArray<FActorDistance> ActorDistances;
	// 遍历所有传入的 Actors，计算距离
	for (AActor* Actor : Actors)
	{
		if (!Actor) // 跳过空 Actor
			continue;
		float Distance = FVector::Dist(Actor->GetActorLocation(), Origin);
		ActorDistances.Add(FActorDistance(Actor, Distance));
	}
	// 按距离从小到大排序
	ActorDistances.Sort([](const FActorDistance& A, const FActorDistance& B) {
		return A.Distance < B.Distance;
		});
	// 取前 MaxTarget 个（或者全部，如果不足）
	int32 NumToTake = FMath::Min(MaxTarget, ActorDistances.Num());
	for (int32 i = 0; i < NumToTake; ++i)
	{
		OutClosestTargets.Add(ActorDistances[i].Actor);
	}
}

bool UAuraAbilitySystemLibrary::IsFriend(const AActor* FirstActor, const AActor* SecondActor)
{
	if (FirstActor && SecondActor)
	{
		if (FirstActor->ActorHasTag(FName("Player")) && SecondActor->ActorHasTag(FName("Player")))
		{
			return true;
		}
		if (FirstActor->ActorHasTag(FName("Enemy")) && SecondActor->ActorHasTag(FName("Enemy")))
		{
			return true;
		}

		return false;
	}
	return true;
}

int32 UAuraAbilitySystemLibrary::GetExpRewardForClassAndLevel(const UObject* WorldContextObject,
	ECharacterClass CharacterClass, int32 Level)
{
	UCharacterClassInfo* ClassInfo = GetCharacterClassInfo(WorldContextObject);
	check(ClassInfo);
	const FCharacterClassDefaultInfo Info = ClassInfo->GetClassDefaultInfo(CharacterClass);
	const float Exp = Info.ExpReward.GetValueAtLevel(Level);
	return static_cast<int32>(Exp);
}

void UAuraAbilitySystemLibrary::SpawnAItemOnTheFloor(const UObject* WorldContextObject, const FVector& WorldLocation,
	const FName& ItemName, const int32 ItemCount)
{
	if (UItemManager* ItemManager = UItemManager::Get(WorldContextObject))
	{
		if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
		{
			const FVector RaisedLocation = FVector(WorldLocation.X, WorldLocation.Y, WorldLocation.Z + 500.f);
			const FVector LowestLocation = FVector(WorldLocation.X, WorldLocation.Y, WorldLocation.Z - 500.f);
			FVector ChosenSpawnLocation = WorldLocation;
			FHitResult HitResult;
			World->LineTraceSingleByChannel(HitResult, ChosenSpawnLocation + FVector(0.f, 0.f, 400.f), ChosenSpawnLocation - FVector(0.f, 0.f, 400.f), ECollisionChannel::ECC_Visibility);
			if (HitResult.bBlockingHit)
			{
				ChosenSpawnLocation = HitResult.ImpactPoint;
			}
			ItemManager->SpawnItemOnTheFloor(ItemName, ItemCount, ChosenSpawnLocation);
		}
	}
}

bool UAuraAbilitySystemLibrary::IsScreenSpacePositionOverAnyWidget(const UObject* WorldContextObject,
	const FVector2D& ScreenPosition)
{

	if (FSlateApplication::IsInitialized())
	{
		const TArray<TSharedRef<SWindow>> Windows = FSlateApplication::Get().GetInteractiveTopLevelWindows();
		for (const TSharedRef<SWindow>& Window : Windows)
		{
			const FWidgetPath WidgetPath = FSlateApplication::Get().LocateWindowUnderMouse(ScreenPosition, Windows, false);
			if (WidgetPath.IsValid())
			{
				// 遍历路径中的所有Widget
				for (int32 i = WidgetPath.Widgets.Num() - 1; i >= 0; --i)
				{
					const FArrangedWidget& ArrangedWidget = WidgetPath.Widgets[i];
					const TSharedRef<SWidget> Widget = ArrangedWidget.Widget;

					// 检查Widget是否是用户创建的UI（而不是视口本身或装饰器）
					// SViewport 通常是游戏世界的渲染区域，我们希望忽略它
					if (Widget->GetTypeAsString() != "SViewport" && Widget->GetVisibility().IsHitTestVisible())
					{
						return true;
					}
				}
			}
		}
	}
	return false;
}


FGameplayEffectContextHandle UAuraAbilitySystemLibrary::ApplyDamageEffect(const FDamageEffectParams& EffectParam)
{
	const FAuraGameplayTags& AuraTags = FAuraGameplayTags::Get();
	const AActor* SourceAvatarActor = EffectParam.SourceAbilitySystemComponent->GetAvatarActor();
	FGameplayEffectContextHandle EffectContext = EffectParam.SourceAbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(SourceAvatarActor);

	//effect keep 数据的方式1
	SetDeathImpulse(EffectContext, EffectParam.DeathImpulse);
	SetKnockbackForce(EffectContext, EffectParam.KnockbackForce);

	SetIsRadialDamage(EffectContext, EffectParam.bIsRadialDamage);
	SetRadialDamageOrigin(EffectContext, EffectParam.RadialDamageOrigin);
	SetRadialDamageInnerRadius(EffectContext, EffectParam.RadialDamageInnerRadius);
	SetRadialDamageOuterRadius(EffectContext, EffectParam.RadialDamageOuterRadius);

	const FGameplayEffectSpecHandle SpecHandle = EffectParam.SourceAbilitySystemComponent->MakeOutgoingSpec(EffectParam.DamageGameplayEffectClass, EffectParam.AbilityLevel, EffectContext);
	//effect keep 数据的方式2
	// 这里通过tag->value的方式 keep了 debuff 相关信息
	// 不是应该用 FAuraGameplayEffectContext保存吗  不然费那么大力气加了干啥？
	// 这里 steven 选择用tag把 chance和BaseDamage 外加另外3个可以在context携带的数据 DebuffDamage DebuffDuration DebuffFrequency 通过tag的方式
	// 带到 execdamage阶段 在那里计算chance 判定是否需要施加debuff 再在context上设置是否会产生debuff
	// 再到AttributeSet postExec阶段 make一个动态debuff（这个debuff不能被网络复制 如果设置复制会报错） 对目标施加
	// 感觉steven是偷的某个项目的逻辑强行这么写 现有功能的话 这么写差点把我绕晕了
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, EffectParam.DamageType, EffectParam.BaseDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, AuraTags.Debuff_Chance, EffectParam.DebuffChance);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, AuraTags.Debuff_Damage, EffectParam.DebuffDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, AuraTags.Debuff_Duration, EffectParam.DebuffDuration);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, AuraTags.Debuff_Frequency, EffectParam.DebuffFrequency);

	EffectParam.TargetAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);

	return EffectContext;
}

TArray<FRotator> UAuraAbilitySystemLibrary::EvenlySpacedRotators(const FVector& Forward, const FVector& Axis, float Spread, int32 NumRotators)
{
	TArray<FRotator> Rotators;

	const FVector LeftOfSpread = Forward.RotateAngleAxis(-Spread / 2.f, Axis);
	const float DeltaSpread = Spread / (NumRotators - 1);
	if (NumRotators > 1)
	{
		for (int32 i = 0; i < NumRotators; ++i)
		{
			const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
			Rotators.Add(Direction.Rotation());
		}
	}
	else
	{
		Rotators.Add(Forward.Rotation());
	}
	return  Rotators;
}

TArray<FVector> UAuraAbilitySystemLibrary::EvenlyRotatedVectors(const FVector& Forward, const FVector& Axis, float Spread, int32 NumVectors)
{
	TArray<FVector> Vectors;

	const FVector LeftOfSpread = Forward.RotateAngleAxis(-Spread / 2.f, Axis);
	const float DeltaSpread = Spread / (NumVectors - 1);
	if (NumVectors > 1)
	{
		for (int32 i = 0; i < NumVectors; ++i)
		{
			const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
			Vectors.Add(Direction);
		}
	}
	else
	{
		Vectors.Add(Forward);
	}
	return  Vectors;
}


void UAuraAbilitySystemLibrary::SetIsBlockedHit(FGameplayEffectContextHandle& EffectContextHandle, bool bValue)
{
	if ( FAuraGameplayEffectContext* EffectContext = static_cast< FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return EffectContext->SetIsBlockedHit(bValue);
	}
}
bool UAuraAbilitySystemLibrary::GetIsRadialDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* Context = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return Context->GetIsRadialDamage();
	}
	return false;
}

void UAuraAbilitySystemLibrary::SetIsRadialDamage(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsRadialDamage)
{
	if (FAuraGameplayEffectContext* Context = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		Context->SetIsRadialDamage(bInIsRadialDamage);
	}
}

float UAuraAbilitySystemLibrary::GetRadialDamageInnerRadius(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* Context = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return Context->GetRadialDamageInnerRadius();
	}
	return 0.f;
}

void UAuraAbilitySystemLibrary::SetRadialDamageInnerRadius(FGameplayEffectContextHandle& EffectContextHandle, float InInnerRadius)
{
	if (FAuraGameplayEffectContext* Context = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		Context->SetRadialDamageInnerRadius(InInnerRadius);
	}
}

float UAuraAbilitySystemLibrary::GetRadialDamageOuterRadius(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* Context = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return Context->GetRadialDamageOuterRadius();
	}
	return 0.f;
}

void UAuraAbilitySystemLibrary::SetRadialDamageOuterRadius(FGameplayEffectContextHandle& EffectContextHandle, float InOuterRadius)
{
	if (FAuraGameplayEffectContext* Context = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		Context->SetRadialDamageOuterRadius(InOuterRadius);
	}
}

FVector UAuraAbilitySystemLibrary::GetRadialDamageOrigin(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* Context = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return Context->GetRadialDamageOrigin();
	}
	return FVector::ZeroVector;
}

void UAuraAbilitySystemLibrary::SetRadialDamageOrigin(FGameplayEffectContextHandle& EffectContextHandle, const FVector& InOrigin)
{
	if (FAuraGameplayEffectContext* Context = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		Context->SetRadialDamageOrigin(InOrigin);
	}
}