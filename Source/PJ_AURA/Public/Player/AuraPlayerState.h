// ALL CODE FOR wangjunyang learning GAS

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "AuraPlayerState.generated.h"

class ULoadScreenSaveGame;
class UInventoryComponent;
class ULevelUpInfo;
class UAbilitySystemComponent;
class UAttributeSet;


DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStateChanged, int32 /*StateValue*/);

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnLevelChanged, int32 /*StateValue*/,bool /*bLevelUp*/);

/**
 * 
 */
UCLASS()
class PJ_AURA_API AAuraPlayerState : public APlayerState,public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	AAuraPlayerState();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const{ return AttributeSet; }
	UInventoryComponent* GetInventoryComponent() const{return InventoryComponent;}

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<ULevelUpInfo> LevelUpInfo;

	FOnPlayerStateChanged OnExpChangedDelegate;
	FOnLevelChanged OnLevelChangedDelegate;
	FOnPlayerStateChanged OnAttributePointsChangedDelegate;
	FOnPlayerStateChanged OnSpellPointsChangedDelegate;

	FORCEINLINE int32 GetPlayerLevel() const { return Level; }
	FORCEINLINE void SetPlayerLevel(int32 inLevel);
	void AddPlayerLevel(int32 inLevel);

	FORCEINLINE int32 GetPlayerExp() const { return Exp; }
	void SetPlayerExp(int32 inExp);
	void AddPlayerExp(int32 inExp);

	FORCEINLINE int32 GetAttributePoints() const { return AttributePoints; }
	void SetAttributePoints(int32 InValue);
	void AddAttributePoints(int32 InValue);

	FORCEINLINE int32 GetSpellPoints() const { return SpellPoints; }
	void SetSpellPoints(int32 InValue);
	void AddSpellPoints(int32 InValue);

	void SaveInventory(ULoadScreenSaveGame* SaveData) const;
	void LoadInventory(const ULoadScreenSaveGame* SaveData) const;

	void RequestLevelUpForDebug();

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UInventoryComponent> InventoryComponent;

private:
	UPROPERTY(VisibleAnywhere,ReplicatedUsing= OnRep_Level)
	int32 Level = 1;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_EXP)
	int32 Exp = 0;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_AttributePoints)
	int32 AttributePoints = 5;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_SpellPoints)
	int32 SpellPoints = 5;

	UFUNCTION()
	void OnRep_Level(int32 OldLevel)const;

	UFUNCTION()
	void OnRep_EXP(int32 OldLevel)const;

	UFUNCTION()
	void OnRep_AttributePoints(int32 OldLevel)const;

	UFUNCTION()
	void OnRep_SpellPoints(int32 OldLevel)const;
};
