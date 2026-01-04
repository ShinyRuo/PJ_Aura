// ALL CODE FOR  learning GAS


#include "AbilitySystem/Passive/PassiveNiagaraComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Interaction/CombatInterface.h"

UPassiveNiagaraComponent::UPassiveNiagaraComponent()
{
	bAutoActivate = false;
}

void UPassiveNiagaraComponent::BeginPlay()
{
	Super::BeginPlay();

	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner())))
	{
		AuraASC->ActivatePassiveEffect.AddUObject(this, &UPassiveNiagaraComponent::OnPassiveActivate);
	}
	else if(ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner()))
	{
		CombatInterface->GetOnASCRegisteredDelegate().AddWeakLambda(this,
			[this](UAbilitySystemComponent* InASC)
			{
				if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(InASC))
				{
					AuraASC->ActivatePassiveEffect.AddUObject(this, &UPassiveNiagaraComponent::OnPassiveActivate);
				}
			}
		);
	}

}

void UPassiveNiagaraComponent::OnPassiveActivate(const FGameplayTag& AbilityTag, bool bActivate)
{
	if (AbilityTag.MatchesTagExact(PassiveSpellTag))
	{
		if (bActivate)
		{
			Activate();
		}
		else
		{
			Deactivate();
		}
	}
	
	

}
