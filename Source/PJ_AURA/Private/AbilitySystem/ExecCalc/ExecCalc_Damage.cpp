// ALL CODE FOR  learning GAS


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "AuraAbilityTypes.h"
#include "AuraAssetManager.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Interaction/CombatInterface.h"

struct AuraDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Resistance_Fire);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Resistance_Lighting);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Resistance_Arcane);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Resistance_Physical);

	TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagsToCaptureDefs;

	AuraDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false); //这里定义了是捕获target还是source
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Resistance_Fire, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Resistance_Lighting, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Resistance_Arcane, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Resistance_Physical, Target, false);
		//在AuraAssetManager初始化成功之后调用   因为InitTagsToCaptureDefs里面会用到初始化后的tags
		UAuraAssetManager::OnAuraAssetManagerManagerCreatedDelegate.AddRaw(this, &AuraDamageStatics::InitTagsToCaptureDefs);
	}

	void InitTagsToCaptureDefs()
	{
		const FAuraGameplayTags Tags = FAuraGameplayTags::Get();
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_Armor, ArmorDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_ArmorPenetration, ArmorPenetrationDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_BlockChance, BlockChanceDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitChance, CriticalHitChanceDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitDamage, CriticalHitDamageDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitResistance, CriticalHitResistanceDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_Resistance_Fire, Resistance_FireDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_Resistance_Lighting, Resistance_LightingDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_Resistance_Arcane, Resistance_ArcaneDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_Resistance_Physical, Resistance_PhysicalDef);
	}
};

#define DEFINE_GETATTR(side,attrName)\
float side##attrName = 0.f;\
ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().attrName##Def, EvaluateParameters, side##attrName);\
side##attrName = FMath::Max<float>(side##attrName, 0);

static const AuraDamageStatics& DamageStatics()
{
	static AuraDamageStatics DStatics;
	return DStatics;
}

UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().Resistance_FireDef);
	RelevantAttributesToCapture.Add(DamageStatics().Resistance_LightingDef);
	RelevantAttributesToCapture.Add(DamageStatics().Resistance_ArcaneDef);
	RelevantAttributesToCapture.Add(DamageStatics().Resistance_PhysicalDef);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                              FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;
	ICombatInterface* SourceCombatInterface = Cast<ICombatInterface>(SourceAvatar);
	ICombatInterface* TargetCombatInterface = Cast<ICombatInterface>(TargetAvatar);

	const FGameplayEffectSpec Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* sourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* targetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = sourceTags;
	EvaluateParameters.TargetTags = targetTags;
	/*SourceTags 和 TargetTags
		类型：const FGameplayTagContainer* （游戏标签容器的指针）
		作用：分别指向 “效果施加者” 和 “效果接收者” 的标签容器。
		用途：在评估时，可用于校验修饰符是否与源 / 目标的标签匹配（例如，只对带有 “Fire” 标签的目标应用火焰伤害加成）。*/

	float  Armor = 0.f;
	//Get Damage Set by Caller Magnitude
	float Damage = 0;
	for (const auto& Pair : FAuraGameplayTags::Get().DamageTypesToResistance)
	{
		const auto [DamageTypeTag, ResistanceTag] = Pair; //structured binding
		checkf(DamageStatics().TagsToCaptureDefs.Contains(ResistanceTag), TEXT("TagsToCaptureDefs doesn't contain Tag:[%s] in ExecCalc_Damage"), *ResistanceTag.ToString());
		const FGameplayEffectAttributeCaptureDefinition CaptureDef = DamageStatics().TagsToCaptureDefs[ResistanceTag];

		float DamageTypeValue = Spec.GetSetByCallerMagnitude(DamageTypeTag,false);
		float Resistance = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CaptureDef, EvaluateParameters, Resistance);
		Resistance = FMath::Clamp(Resistance,0.f, 100.f);

		DamageTypeValue *= (100.f - Resistance) / 100.f;

		Damage += DamageTypeValue;
	}

	FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();

	//Capture BlockChance On Target , and determine if there was a successful Block
	//If Block, halve the damage.
	DEFINE_GETATTR(Target, BlockChance);

	const bool bBlocked = FMath::RandRange(1, 100) < TargetBlockChance;
	if (bBlocked)
	{
		Damage /= 2.f;
	}
	UAuraAbilitySystemLibrary::SetIsBlockedHit(EffectContextHandle, bBlocked);

	//ArmorPenetration ignores a percentage of the Target's Armor
	DEFINE_GETATTR(Target, Armor);
	DEFINE_GETATTR(Source, ArmorPenetration);
	//使用护甲穿透和穿透计算系数表
	UCharacterClassInfo*  CharacterClassInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(SourceAvatar);
	if (!CharacterClassInfo || !CharacterClassInfo->DamageCalculationCoefficients|| !SourceCombatInterface|| !TargetCombatInterface)
		return;

	FRealCurve* ArmorPenetrationCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("ArmorPenetration"), FString());
	FRealCurve* EffectiveArmorCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("EffectiveArmor"), FString());
	if (ArmorPenetrationCurve && EffectiveArmorCurve)
	{
		const float ArmorPenetrationCoefficient = ArmorPenetrationCurve->Eval(SourceCombatInterface->GetPlayerLevel());
		const float EffectiveArmorCoefficient = EffectiveArmorCurve->Eval(TargetCombatInterface->GetPlayerLevel());
		//等级越高 系数越贬值 避免数值爆炸吧
		const float EffectiveArmor = TargetArmor * (100 - SourceArmorPenetration * ArmorPenetrationCoefficient) / 100.f;
		Damage *= (100 - EffectiveArmor * EffectiveArmorCoefficient) / 100.f;
	}

	DEFINE_GETATTR(Source, CriticalHitChance);
	DEFINE_GETATTR(Source, CriticalHitDamage);
	DEFINE_GETATTR(Target, CriticalHitResistance);

	if (FRealCurve* CriticalHitResistanceCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("CriticalHitResistance"), FString()))
	{
		const float CriticalHitResistanceCurveCoefficient = CriticalHitResistanceCurve->Eval(SourceCombatInterface->GetPlayerLevel());
		const float EffectiveCriticalHitChance = SourceCriticalHitChance - TargetCriticalHitResistance * CriticalHitResistanceCurveCoefficient;
		const bool bCriticalHit = FMath::RandRange(1, 100) < EffectiveCriticalHitChance;
		Damage = bCriticalHit ? 2.f * Damage + SourceCriticalHitDamage : Damage;
		UAuraAbilitySystemLibrary::SetIsCriticalHit(EffectContextHandle, bCriticalHit);
	}


	//给Target的incomingDamage 属性 加上值 
	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(),EGameplayModOp::Additive, Damage);

	OutExecutionOutput.AddOutputModifier(EvaluatedData);

}
