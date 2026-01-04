// ALL CODE FOR wangjunyang learning GAS


#include "Player/AuraPlayerState.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Inventory/InventoryComponent.h"
#include "Net/UnrealNetwork.h"

AAuraPlayerState::AAuraPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>("InventoryComponent");
	InventoryComponent->SetIsReplicated(false);

	NetUpdateFrequency = 100.f;
}

void AAuraPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AAuraPlayerState, Level);
	DOREPLIFETIME(AAuraPlayerState, Exp);
	DOREPLIFETIME(AAuraPlayerState, AttributePoints);
	DOREPLIFETIME(AAuraPlayerState, SpellPoints);

}

UAbilitySystemComponent* AAuraPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}



void AAuraPlayerState::SetPlayerLevel(int32 inLevel)
{
	Level = inLevel;
	OnLevelChangedDelegate.Broadcast(Level,false);
}


void AAuraPlayerState::AddPlayerLevel(int32 inLevel)
{
	Level += inLevel;
	OnLevelChangedDelegate.Broadcast(Level, true);
}
void AAuraPlayerState::OnRep_Level(int32 OldLevel)const
{
	OnLevelChangedDelegate.Broadcast(Level,true); //todo rep如何知道是否触发升级特效
}



void AAuraPlayerState::SetPlayerExp(int32 inExp)
{
	Exp = inExp;
	OnExpChangedDelegate.Broadcast(Exp);
}

void AAuraPlayerState::AddPlayerExp(int32 inExp)
{
	Exp += inExp;
	OnExpChangedDelegate.Broadcast(Exp);
}

void AAuraPlayerState::SetAttributePoints(int32 InValue)
{
	AttributePoints = InValue;
	OnAttributePointsChangedDelegate.Broadcast(AttributePoints);
}

void AAuraPlayerState::AddAttributePoints(int32 InValue)
{
	AttributePoints += InValue;
	OnAttributePointsChangedDelegate.Broadcast(AttributePoints);
}

void AAuraPlayerState::SetSpellPoints(int32 InValue)
{
	SpellPoints = InValue;
	OnSpellPointsChangedDelegate.Broadcast(SpellPoints);
}

void AAuraPlayerState::AddSpellPoints(int32 InValue)
{
	SpellPoints += InValue;
	OnSpellPointsChangedDelegate.Broadcast(SpellPoints);
}


void AAuraPlayerState::OnRep_EXP(int32 OldLevel)const
{
	OnExpChangedDelegate.Broadcast(Exp);
}

void AAuraPlayerState::OnRep_AttributePoints(int32 OldLevel)const
{
	OnAttributePointsChangedDelegate.Broadcast(AttributePoints);
}

void AAuraPlayerState::OnRep_SpellPoints(int32 OldLevel)const
{
	OnSpellPointsChangedDelegate.Broadcast(SpellPoints);
}
