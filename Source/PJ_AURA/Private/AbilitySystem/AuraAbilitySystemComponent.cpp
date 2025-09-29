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

void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
    if(!InputTag.IsValid()) return;
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
    for (auto& AbiliySpec : GetActivatableAbilities())
    {
        if (AbiliySpec.DynamicAbilityTags.HasTagExact(InputTag))
        {
            AbilitySpecInputReleased(AbiliySpec);
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

FGameplayTag UAuraAbilitySystemComponent::GetAbilityStatusTagFromTag(const FGameplayTag& AbilityTag)
{
	if (const FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTags(AbilityTag))
	{
        return GetAbilityStatusTagFromSpec(*AbilitySpec);
	}
    return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetAbilityInputTagFromTag(const FGameplayTag& AbilityTag)
{
    if (const FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTags(AbilityTag))
    {
        return GetAbilityInputTagFromSpec(*AbilitySpec);
    }
    return FGameplayTag();
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
            //remove this input from any abilities 
            ClearAbilitiesOfSlot(Slot);
            //clear this ability slot
            ClearSlot(AbilitySpec);
            //assign this ability to this slot
            AbilitySpec->DynamicAbilityTags.AddTag(Slot);
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
    MarkAbilitySpecDirty(*AbilitySpec);
}

void UAuraAbilitySystemComponent::ClearAbilitiesOfSlot(const FGameplayTag& Slot)
{
    FScopedAbilityListLock ActiveScopeLock(*this);
    for ( FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
    {
	    if (AbilityHasSlot(&AbilitySpec,Slot))
	    {
            ClearSlot(&AbilitySpec);
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

 