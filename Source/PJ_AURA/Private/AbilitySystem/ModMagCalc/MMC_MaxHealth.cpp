// ALL CODE FOR wangjunyang learning GAS


#include "AbilitySystem/ModMagCalc/MMC_MaxHealth.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "Interaction/CombatInterface.h"

UMMC_MaxHealth::UMMC_MaxHealth()
{
	VigorDef.AttributeToCapture = UAuraAttributeSet::GetVigorAttribute();
	VigorDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	VigorDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(VigorDef);
}

float UMMC_MaxHealth::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	//Gather tags from source and target
	const FGameplayTagContainer* sourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* targetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = sourceTags;
	EvaluateParameters.TargetTags = targetTags;

	float  Vigor = 0.f;
	GetCapturedAttributeMagnitude(VigorDef, Spec, EvaluateParameters, Vigor);
	Vigor = FMath::Max<float>(Vigor, 0.f);

	//ICombatInterface* CombatInterface =  Cast<ICombatInterface>(Spec.GetContext().GetSourceObject());
	//为什么不直接用上述cast 然后check返回值呢
	//使用上述cast 无法发挥GetPlayerLevel的BlueprintNativeEvent特性
	//使用cast只能检查C++端的GetSourceObject是否实现ICombatInterface
	//使用Implements检查可以检查蓝图端是否实现 UCombatInterface 这也就是为什么一个Interface在c++端全用的I开头 但是非要定义一个U开头的名字 就是为了反射
	//使用Execute_ 方法 优先调用蓝图端实现的GetPlayerLevel 否则才用C++端的 _implement
	int32 PlayerLevel = 0;
	if (Spec.GetContext().GetSourceObject()->Implements<UCombatInterface>())
	{
		PlayerLevel = ICombatInterface::Execute_GetPlayerLevel(Spec.GetContext().GetSourceObject());
	}
	

	return 80.f + 2.5f * Vigor + 10.f * PlayerLevel;
}
