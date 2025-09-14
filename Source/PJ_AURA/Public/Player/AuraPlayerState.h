// ALL CODE FOR wangjunyang learning GAS

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "AuraPlayerState.generated.h"

class ULevelUpInfo;
class UAbilitySystemComponent;
class UAttributeSet;


DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStateChanged, int32 /*StateValue*/);

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

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<ULevelUpInfo> LevelUpInfo;

	FOnPlayerStateChanged OnExpChangedDelegate;
	FOnPlayerStateChanged OnLevelChangedDelegate;

	FORCEINLINE int32 GetPlayerLevel() const { return Level; }
	FORCEINLINE void SetPlayerLevel(int32 inLevel);
	void AddPlayerLevel(int32 inLevel);

	FORCEINLINE int32 GetPlayerExp() const { return Exp; }
	void SetPlayerExp(int32 inExp);
	void AddPlayerExp(int32 inExp);

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

private:
	UPROPERTY(VisibleAnywhere,ReplicatedUsing= OnRep_Level)
	int32 Level = 1;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_EXP)
	int32 Exp = 0;

	UFUNCTION()
	void OnRep_Level(int32 OldLevel);

	UFUNCTION()
	void OnRep_EXP(int32 OldLevel);
};
