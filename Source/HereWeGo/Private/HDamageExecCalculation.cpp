// Fill out your copyright notice in the Description page of Project Settings.


#include "HDamageExecCalculation.h"
#include "HAbilitySystemComponent.h"
#include "HAttributeSetBase.h"


struct FHDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Damage);

	FHDamageStatics()
	{
		//Snapshot happens at time of gespec creation

		//We can capture source attributes like attack power if we want to

		//Capture optional damage attribute from ge for calcmodifier under execcalc
		DEFINE_ATTRIBUTE_CAPTUREDEF(UHAttributeSetBase, Damage, Source, true);

		//Capture target's armor dont snapshot
		DEFINE_ATTRIBUTE_CAPTUREDEF(UHAttributeSetBase, Armor, Target, false);
	}
};

static const FHDamageStatics& DamageStatics()
{
	static FHDamageStatics DmgStatics;
	return DmgStatics;
}

UHDamageExecCalculation::UHDamageExecCalculation()
{
	RelevantAttributesToCapture.Add(DamageStatics().DamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
}

void UHDamageExecCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                                     FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	FAggregatorEvaluateParameters EvalParams;
	EvalParams.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvalParams.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	float Armor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvalParams, Armor);

	float Damage = 0.f; //Capture optional damage attribute from ge for calcmodifier under execcalc
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DamageDef, EvalParams, Damage);
	//Add setbycaller if it exists. Remember ror
	Damage += FMath::Max<float>(Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Damage")), false, -1.f), 0.f);

	//We can multiply thi shere if we want like isaac
	float FlatDamage = Damage;

	float ArmorReducedDamage = (FlatDamage) * (100.f / (100.f + Armor));

	if (ArmorReducedDamage > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().DamageProperty, EGameplayModOp::Additive, ArmorReducedDamage));
	}

	//UHAbilitySystemComponent* HTargetASC = Cast<UHAbilitySystemComponent>(TargetASC);
	//if(HTargetASC)
	//{
	//	UHAbilitySystemComponent* HSourceASC = Cast<UHAbilitySystemComponent>(SourceASC);
	//	TargetASC->ReceiveDamage //We can broadcast here but not implemented yet
	//}
}
