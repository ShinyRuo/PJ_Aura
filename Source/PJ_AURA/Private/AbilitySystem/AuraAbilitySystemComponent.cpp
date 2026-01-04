// ALL CODE FOR wangjunyang learning GAS


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Interaction/PlayerInterface.h"
#include "PJ_AURA/AuraLogChannels.h"

void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
    OnGameplayEffectAppliedDelegateToSelf.AddUObject(this,&UAuraAbilitySystemComponent::ClientEffectApplied);
}

void UAuraAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent,
	const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle) const
{
    //GEngine->AddOnScreenDebugMessage(1, 8.f, FColor::Blue, FString("Effect Applied"));
    FGameplayTagContainer TagContainer;
    EffectSpec.GetAllAssetTags(TagContainer);
    EffectAssetTags.Broadcast(TagContainer);
}

void UAuraAbilitySystemComponent::ClientUpdateAbilityStatus_Implementation(const FGameplayTag& AbilityTag,
	const FGameplayTag& StatusTag, int32 AbilityLevel)
{
    AbilityStatusChanged.Broadcast(AbilityTag, StatusTag, AbilityLevel);
}

void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
    for(const TSubclassOf<UGameplayAbility> AbilityClass : StartupAbilities)
    {
    	FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
        if (const UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec.Ability))
        {
            AbilitySpec.DynamicAbilityTags.AddTag(AuraAbility->StartUpInputTag);//instant 一个ability 并把CDO的StartUpInputTag赋值给instantability的DynamicAbilityTags
            AbilitySpec.DynamicAbilityTags.AddTag(FAuraGameplayTags::Get().Abilities_Status_Equipped);
        	GiveAbility(AbilitySpec);
        }
    }
    bStartupAbilitiesGiven = true;
    AbilitiesGivenDelegate.Broadcast();
}

void UAuraAbilitySystemComponent::AddCharacterPassiveAbilities(
	const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
    for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupAbilities)
    {
        FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
    	GiveAbilityAndActivateOnce(AbilitySpec);
    }
}

void UAuraAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
    if (!InputTag.IsValid()) return;
    FScopedAbilityListLock ActiveScopeLock(*this);

    for (auto& AbiliySpec : GetActivatableAbilities())
    {
        if (AbiliySpec.DynamicAbilityTags.HasTagExact(InputTag))
        {
            AbilitySpecInputPressed(AbiliySpec);
            if (AbiliySpec.IsActive())
            {
                InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, AbiliySpec.Handle, AbiliySpec.ActivationInfo.GetActivationPredictionKey());
            }
        }
    }
}

void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
    if(!InputTag.IsValid()) return;
    FScopedAbilityListLock ActiveScopeLock(*this);

    for (auto& AbiliySpec : GetActivatableAbilities())
    {
	    if (AbiliySpec.DynamicAbilityTags.HasTagExact(InputTag))
	    {
            AbilitySpecInputPressed(AbiliySpec);
		    if (!AbiliySpec.IsActive())
		    {
                TryActivateAbility(AbiliySpec.Handle);
		    }
	    }
    }
}

void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
    if (!InputTag.IsValid()) return;
    FScopedAbilityListLock ActiveScopeLock(*this);

    for (auto& AbiliySpec : GetActivatableAbilities())
    {
        if (AbiliySpec.DynamicAbilityTags.HasTagExact(InputTag) && AbiliySpec.IsActive())
        {
            AbilitySpecInputReleased(AbiliySpec);
            InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, AbiliySpec.Handle, AbiliySpec.ActivationInfo.GetActivationPredictionKey());
        }
    }
}

void UAuraAbilitySystemComponent::ForEachAbility(const FForEachAbility& Delegate)
{
    //即使在单线程环境下 列表也有在遍历时被修改的风险
    //防止在遍历期间对能力列表进行结构性修改 所以这里得加锁
    FScopedAbilityListLock ActiveScopeLock(*this);
    for (const FGameplayAbilitySpec& AbilitySpec: GetActivatableAbilities())
    {
	    if (!Delegate.ExecuteIfBound(AbilitySpec))
	    {
            UE_LOG(LogAura, Error, TEXT("Failed to execute delegate in %hs"), __FUNCTION__);
	    }
    }
}

FGameplayTag UAuraAbilitySystemComponent::GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (AbilitySpec.Ability)
	{
        for (FGameplayTag Tag : AbilitySpec.Ability.Get()->AbilityTags)
        {
	        if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities"))))
	        {
                return Tag;
	        }
        }
	}
    return FGameplayTag();
}

 FGameplayTag UAuraAbilitySystemComponent::GetAbilityStatusTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
     for (FGameplayTag Tag : AbilitySpec.DynamicAbilityTags)
     {
         if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities.Status"))))
         {
             return Tag;
         }
     }
     return FGameplayTag();
}

FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetSpecFromAbilityTags(const FGameplayTag& AbilityTag)
{
    FScopedAbilityListLock AbilityListLock(*this);
    for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
    {
	    for (FGameplayTag Tag : AbilitySpec.Ability.Get()->AbilityTags)
	    {
		    if (Tag.MatchesTag(AbilityTag))
		    {
                return &AbilitySpec;
		    }
	    }
    }
    return nullptr;
}

bool UAuraAbilitySystemComponent::IsPassiveAbility(const FGameplayAbilitySpec& AbilitySpec) const
{
    UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
    const FGameplayTag AbilityTag = GetAbilityTagFromSpec(AbilitySpec);
    if (AbilityInfo)
    {
        const FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
        const FGameplayTag AbilityType = Info.AbilityType;
        return AbilityType.MatchesTagExact(FAuraGameplayTags::Get().Abilities_Type_Passive);
    }
    return false;
}

FGameplayTag UAuraAbilitySystemComponent::GetAbilityStatusTagFromTag(const FGameplayTag& AbilityTag)
{
	if (const FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTags(AbilityTag))
	{
        return GetAbilityStatusTagFromSpec(*AbilitySpec);
	}
    return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetAbilitySlotFromTag(const FGameplayTag& AbilityTag)
{
    if (const FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTags(AbilityTag))
    {
        return GetAbilityInputTagFromSpec(*AbilitySpec);
    }
    return FGameplayTag();
}

bool UAuraAbilitySystemComponent::SlotIsEmpty(const FGameplayTag& Slot)
{
    FScopedAbilityListLock ActiveScopeLock(*this);
    for (auto& AbiliySpec : GetActivatableAbilities())
    {
         if(AbilityHasSlot(AbiliySpec, Slot))
         {
	         return false;
         }
    }
    return true;
}

bool UAuraAbilitySystemComponent::AbilityHasSlot(const FGameplayAbilitySpec& AbilitySpec, const FGameplayTag& Slot)
{
    return AbilitySpec.DynamicAbilityTags.HasTagExact(Slot);
}

bool UAuraAbilitySystemComponent::AbilityHasAnySlot(const FGameplayAbilitySpec& AbilitySpec)
{
    return AbilitySpec.DynamicAbilityTags.HasTag(FGameplayTag::RequestGameplayTag(FName("InputTag")));
}

void UAuraAbilitySystemComponent::AssignSlotToAbility(FGameplayAbilitySpec& AbilitySpec, const FGameplayTag& Slot)
{
    ClearSlot(&AbilitySpec);
    AbilitySpec.DynamicAbilityTags.AddTag(Slot);
}

void UAuraAbilitySystemComponent::MulticastActivatePassiveEffect_Implementation(const FGameplayTag& AbilityTag,
	bool bActivate)
{
    ActivatePassiveEffect.Broadcast(AbilityTag, bActivate);
}

FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetSpecWithSlot(const FGameplayTag& slot)
{
    FScopedAbilityListLock AbilityListLock(*this);
    for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
    {
        if (AbilitySpec.DynamicAbilityTags.HasTagExact(slot))
        {
            return &AbilitySpec;
        }
    }
    return nullptr;
}

FGameplayTag UAuraAbilitySystemComponent::GetAbilityInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
    for (FGameplayTag Tag : AbilitySpec.DynamicAbilityTags)
    {
	    if(Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("InputTag"))))
	    {
            return Tag;
	    }
    }
    return FGameplayTag();
}

void UAuraAbilitySystemComponent::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		if (IPlayerInterface::Execute_GetAttributePoints(GetAvatarActor()) > 0)
		{
            ServerUpgradeAttribute(AttributeTag);
		}
	}
}

void UAuraAbilitySystemComponent::ServerUpgradeAttribute_Implementation(const FGameplayTag& AttributeTag)
{
    FGameplayEventData Payload;
    Payload.EventTag = AttributeTag;
    Payload.EventMagnitude = 1.f;
    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActor(), AttributeTag, Payload);
    if (GetAvatarActor()->Implements<UPlayerInterface>())
    {
        IPlayerInterface::Execute_AddToAttributePoints(GetAvatarActor(), -1);
    }
}


void UAuraAbilitySystemComponent::UpdateAbilityStatus(int32 Level)
{
    UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
    for (const FAuraAbilityInfo& Info : AbilityInfo->AbilityInformation)
    {
        if (Level < Info.LevelRequirement) continue;
        if (!Info.AbilityTag.IsValid()) continue;
		    
	    if (GetSpecFromAbilityTags(Info.AbilityTag) == nullptr)
	    {
            FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Info.Ability,1);
            AbilitySpec.DynamicAbilityTags.AddTag(FAuraGameplayTags::Get().Abilities_Status_Eligible);
            GiveAbility(AbilitySpec);
            MarkAbilitySpecDirty(AbilitySpec);//force rep
            ClientUpdateAbilityStatus(Info.AbilityTag, FAuraGameplayTags::Get().Abilities_Status_Eligible,1);
	    }
    }
}

void UAuraAbilitySystemComponent::ServerSpendSpellPoint_Implementation(const FGameplayTag& AbilityTag)
{
   if( FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTags(AbilityTag))
   {
   		if(GetAvatarActor()->Implements<UPlayerInterface>())
   		{
	        IPlayerInterface::Execute_AddToSpellPoints(GetAvatarActor(),-1);
   		}
       const FAuraGameplayTags& AuraTags = FAuraGameplayTags::Get();
       FGameplayTag Status = GetAbilityStatusTagFromSpec(*AbilitySpec);
       if (Status.MatchesTagExact(AuraTags.Abilities_Status_Eligible))
       {
           AbilitySpec->DynamicAbilityTags.RemoveTag(AuraTags.Abilities_Status_Eligible);
           AbilitySpec->DynamicAbilityTags.AddTag(AuraTags.Abilities_Status_Unlocked);
           Status = AuraTags.Abilities_Status_Unlocked;
       }
       else if(Status.MatchesTagExact(AuraTags.Abilities_Status_Equipped) || Status.MatchesTagExact(AuraTags.Abilities_Status_Unlocked))
       {
           AbilitySpec->Level += 1;
       }
       ClientUpdateAbilityStatus(AbilityTag, Status, AbilitySpec->Level);
       MarkAbilitySpecDirty(*AbilitySpec);//force rep
   }
}


void UAuraAbilitySystemComponent::ServerEquipAbility_Implementation(const FGameplayTag& AbilityTag,
	const FGameplayTag& Slot)
{
	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTags(AbilityTag))
	{
        const FGameplayTag& PrevSlot = GetAbilityInputTagFromSpec(*AbilitySpec);
        const FGameplayTag& Status = GetAbilityStatusTagFromSpec(*AbilitySpec);

        const bool bStatusValid = Status == FAuraGameplayTags::Get().Abilities_Status_Equipped || Status == FAuraGameplayTags::Get().Abilities_Status_Unlocked;

        if (bStatusValid)
        {
            //Handle activation/deactivation for passive ability

            if (!SlotIsEmpty(Slot)) // There is an ability in this slot , Deactivate and clear this slot
            {
                FGameplayAbilitySpec* SpecWithSlot = GetSpecWithSlot(Slot);
                if (SpecWithSlot)
                {
                    // 向slot上安装slot上相同的技能 可以提前return
                    // 按理说可以不调用ClientEquipAbility 但是 客户端有个取消聚焦闪烁动画的功能 在ClientEquipAbility后面 
	                if (AbilityTag.MatchesTagExact(GetAbilityTagFromSpec(*SpecWithSlot)))
	                {
                        ClientEquipAbility(AbilityTag, FAuraGameplayTags::Get().Abilities_Status_Equipped, Slot, PrevSlot, AbilitySpec->Level);
						return; 
	                }
                    //是否是被动技能
                    if (IsPassiveAbility(*SpecWithSlot))
                    {
                        MulticastActivatePassiveEffect(GetAbilityTagFromSpec(*SpecWithSlot), false);
                        DeactivatePassiveAbility.Broadcast(GetAbilityTagFromSpec(*SpecWithSlot));
                    }
                    // 清理spec的slot 这里的slot并没有抽象出一个容器 而是 某个abilityspec的dynamictags中 含有某个inputtag 那么它就在这个slot中
                    ClearSlot(SpecWithSlot);
                }
            }
            //这里想要知道装上去的如果是被动技能 那它需不需要激活
            if (!AbilityHasAnySlot(*AbilitySpec))
            {
                if (IsPassiveAbility(*AbilitySpec))
                {
                    TryActivateAbility(AbilitySpec->Handle);
                    MulticastActivatePassiveEffect(AbilityTag, true);
                }
            }
            //assign this ability to this slot
            AssignSlotToAbility(*AbilitySpec, Slot);

            if(Status.MatchesTagExact(FAuraGameplayTags::Get().Abilities_Status_Unlocked))
            {
                //replace status tag
                AbilitySpec->DynamicAbilityTags.RemoveTag(FAuraGameplayTags::Get().Abilities_Status_Unlocked);
                AbilitySpec->DynamicAbilityTags.AddTag(FAuraGameplayTags::Get().Abilities_Status_Equipped);
            }

            MarkAbilitySpecDirty(*AbilitySpec);
            ClientEquipAbility(AbilityTag, FAuraGameplayTags::Get().Abilities_Status_Equipped, Slot,PrevSlot, AbilitySpec->Level);
        }
	}
}


void UAuraAbilitySystemComponent::ClientEquipAbility_Implementation(const FGameplayTag& AbilityTag,
	const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PrevSlot, int32 AbilityLevel)
{
    AbilityEquipped.Broadcast(AbilityTag, Status, Slot, PrevSlot, AbilityLevel);
}

void UAuraAbilitySystemComponent::ClearSlot(FGameplayAbilitySpec* AbilitySpec)
{
    const FGameplayTag Slot = GetAbilityInputTagFromSpec(*AbilitySpec);
    AbilitySpec->DynamicAbilityTags.RemoveTag(Slot);
}

void UAuraAbilitySystemComponent::ClearAbilitiesOfSlot(const FGameplayTag& Slot)
{
    FScopedAbilityListLock ActiveScopeLock(*this);
    for ( FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
    {
	    if (AbilityHasSlot(&AbilitySpec,Slot))
	    {
            ClearSlot(&AbilitySpec);
            MarkAbilitySpecDirty(AbilitySpec);
	    }
    }
}

bool UAuraAbilitySystemComponent::AbilityHasSlot(FGameplayAbilitySpec* AbilitySpec, const FGameplayTag& Slot)
{
    for (FGameplayTag Tag : AbilitySpec->DynamicAbilityTags)
    {
	    if (Tag.MatchesTagExact(Slot))
	    {
		    return true;
	    }
    }
    return false;
}

void UAuraAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();

    //StartupAbilities
    //Server -> AddCharacterAbilities
    //一帧内 所有的StartupAbilities被加入ActivatableAbilities 标记为dirty
    //下一帧就同步给client
    //client执行OnRep_ActivateAbilities 这里让它只执行一次 因为UI只需要设置一次StartupAbilities
	if (!bStartupAbilitiesGiven)
	{
        bStartupAbilitiesGiven = true;
        AbilitiesGivenDelegate.Broadcast();
	}
}

 