// ALL CODE FOR wangjunyang learning GAS

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"


class ULoadScreenSaveGame;
DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer& /* AssetTags*/);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FAbilityStatusChanged, const FGameplayTag& /* AbilityTag*/, const FGameplayTag& /* StatusTag*/,int32 /*AbilityLevel*/);
DECLARE_MULTICAST_DELEGATE_FiveParams(FAbilityEquipped, const FGameplayTag& /* AbilityTag*/, const FGameplayTag& /* StatusTag*/, const FGameplayTag& /* Slot*/, const FGameplayTag& /* PrevSlot*/,int32);

DECLARE_MULTICAST_DELEGATE(FAbilitiesGiven);

DECLARE_DELEGATE_OneParam(FForEachAbility, const FGameplayAbilitySpec&);

DECLARE_MULTICAST_DELEGATE_OneParam(FDeactivatePassiveAbility, const FGameplayTag& /* AbilityTag*/);


DECLARE_MULTICAST_DELEGATE_TwoParams(FActivatePassiveEffect, const FGameplayTag& /* AbilityTag*/, bool /*bActivate*/);
/**
 * 
 */
UCLASS()
class PJ_AURA_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	void AbilityActorInfoSet();

	FEffectAssetTags EffectAssetTags;
	FAbilitiesGiven AbilitiesGivenDelegate;
	FAbilityStatusChanged AbilityStatusChanged;
	FAbilityEquipped AbilityEquipped;
	FDeactivatePassiveAbility DeactivatePassiveAbility;
	FActivatePassiveEffect ActivatePassiveEffect;

	bool bStartupAbilitiesGiven = false;

	UFUNCTION(Client,Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent,
		const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)const;

	UFUNCTION(Client, Reliable)
	void ClientUpdateAbilityStatus(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 AbilityLevel);

	void AddCharacterAbilitiesFromSavedData(ULoadScreenSaveGame* SaveData);
	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);
	void AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);

	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagHeld(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

	void ForEachAbility(const FForEachAbility& Delegate);

	static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetAbilityInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetAbilityStatusTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	FGameplayTag GetAbilityStatusTagFromTag(const FGameplayTag& AbilityTag);
	FGameplayTag GetAbilitySlotFromTag(const FGameplayTag& AbilityTag);
	bool SlotIsEmpty(const FGameplayTag& Slot);
	static bool AbilityHasSlot(const FGameplayAbilitySpec& AbilitySpec, const FGameplayTag& Slot);
	static bool AbilityHasAnySlot(const FGameplayAbilitySpec& AbilitySpec);
	static void AssignSlotToAbility(FGameplayAbilitySpec& AbilitySpec, const FGameplayTag& Slot);

	UFUNCTION(NetMulticast,Reliable)
	void MulticastActivatePassiveEffect(const FGameplayTag& AbilityTag, bool bActivate);

	FGameplayAbilitySpec* GetSpecWithSlot(const FGameplayTag& slot);

	FGameplayAbilitySpec* GetSpecFromAbilityTags(const FGameplayTag& AbilityTag);

	bool IsPassiveAbility(const FGameplayAbilitySpec& AbilitySpec)const;

	void UpgradeAttribute(const FGameplayTag& AttributeTag);

	UFUNCTION(Server,Reliable)
	void ServerUpgradeAttribute(const FGameplayTag& AttributeTag);

	void UpdateAbilityStatus(int32 Level);

	UFUNCTION(Server,Reliable)
	void ServerSpendSpellPoint(const FGameplayTag& AbilityTag);

	UFUNCTION(Server, Reliable)
	void ServerEquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& Slot);

	UFUNCTION(Client, Reliable)
	void ClientEquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PrevSlot,int32 AbilityLevel);

	static void ClearSlot(FGameplayAbilitySpec* AbilitySpec);
	void ClearAbilitiesOfSlot(const FGameplayTag& Slot);
	static bool AbilityHasSlot(FGameplayAbilitySpec* AbilitySpec, const FGameplayTag& Slot);

protected:
	virtual void OnRep_ActivateAbilities() override;
};




