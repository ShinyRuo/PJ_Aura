// ALL CODE FOR wangjunyang learning GAS


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
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

void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
    for(const TSubclassOf<UGameplayAbility> AbilityClass : StartupAbilities)
    {
    	FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
        if (const UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec.Ability))
        {
            AbilitySpec.DynamicAbilityTags.AddTag(AuraAbility->StartUpInputTag);//instant 一个ability 并把CDO的StartUpInputTag赋值给instantability的DynamicAbilityTags
            GiveAbility(AbilitySpec);
        }
    }
    bStartupAbilitiesGiven = true;
    AbilitiesGivenDelegate.Broadcast(this);
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
        AbilitiesGivenDelegate.Broadcast(this);
	}
}

 