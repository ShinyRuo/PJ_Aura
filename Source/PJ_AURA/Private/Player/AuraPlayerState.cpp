// ALL CODE FOR wangjunyang learning GAS


#include "Player/AuraPlayerState.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Net/UnrealNetwork.h"

AAuraPlayerState::AAuraPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");

	NetUpdateFrequency = 100.f;
}

void AAuraPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AAuraPlayerState, Level);
	DOREPLIFETIME(AAuraPlayerState, Exp);

}

UAbilitySystemComponent* AAuraPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}



void AAuraPlayerState::SetPlayerLevel(int32 inLevel)
{
	Level = inLevel;
	OnLevelChangedDelegate.Broadcast(Level);
}

void AAuraPlayerState::AddPlayerLevel(int32 inLevel)
{
	Level += inLevel;
	OnLevelChangedDelegate.Broadcast(Level);
}
void AAuraPlayerState::OnRep_Level(int32 OldLevel)
{
	OnLevelChangedDelegate.Broadcast(Level);
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


void AAuraPlayerState::OnRep_EXP(int32 OldLevel)
{
	OnExpChangedDelegate.Broadcast(Exp);
}
