// Fill out your copyright notice in the Description page of Project Settings.


#include "HDamageExecCalculation.h"
#include "HAbilitySystemComponent.h"
#include "HAttributeSetBase.h"
#include "HGameplayEffectContext.h"
#include "HereWeGo/AttributeSets/HHealthSet.h"


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
#if WITH_SERVER_CODE
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FHGameplayEffectContext* TypedContext = FHGameplayEffectContext::ExtractEffectContext(Spec.GetContext());
	check(TypedContext);

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float BaseDamage = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DamageDef, EvaluateParameters, BaseDamage);

	const AActor* EffectCauser = TypedContext->GetEffectCauser();
	const FHitResult* HitActorResult = TypedContext->GetHitResult();

	AActor* HitActor = nullptr;
	FVector ImpactLocation = FVector::ZeroVector;
	FVector ImpactNormal = FVector::ZeroVector;
	FVector StartTrace = FVector::ZeroVector;
	FVector EndTrace = FVector::ZeroVector;

	// Calculation of hit actor, surface, zone, and distance all rely on whether the calculation has a hit result or not.
	// Effects just being added directly w/o having been targeted will always come in without a hit result, which must default
	// to some fallback information.
	if (HitActorResult)
	{
		const FHitResult& CurHitResult = *HitActorResult;
		HitActor = CurHitResult.HitObjectHandle.FetchActor();
		if (HitActor)
		{
			ImpactLocation = CurHitResult.ImpactPoint;
			ImpactNormal = CurHitResult.ImpactNormal;
			StartTrace = CurHitResult.TraceStart;
			EndTrace = CurHitResult.TraceEnd;
		}
	}

	// Handle case of no hit result or hit result not actually returning an actor
	UAbilitySystemComponent* TargetAbilitySystemComponent = ExecutionParams.GetTargetAbilitySystemComponent();
	if (!HitActor)
	{
		HitActor = TargetAbilitySystemComponent ? TargetAbilitySystemComponent->GetAvatarActor_Direct() : nullptr;
		if (HitActor)
		{
			ImpactLocation = HitActor->GetActorLocation();
		}
	}

	// Apply rules for team damage/self damage/etc...
	float DamageInteractionAllowedMultiplier = 1.0f; //0.0f
	/*if (HitActor)
	{
		UHTeamSubsystem* TeamSubsystem = HitActor->GetWorld()->GetSubsystem<UHTeamSubsystem>();
		if (ensure(TeamSubsystem))
		{
			DamageInteractionAllowedMultiplier = TeamSubsystem->CanCauseDamage(EffectCauser, HitActor) ? 1.0 : 0.0;
		}
	}*/

	// Determine distance
	double Distance = WORLD_MAX;

	if (TypedContext->HasOrigin())
	{
		Distance = FVector::Dist(TypedContext->GetOrigin(), ImpactLocation);
	}
	else if (EffectCauser)
	{
		Distance = FVector::Dist(EffectCauser->GetActorLocation(), ImpactLocation);
	}
	else
	{
		ensureMsgf(false, TEXT("Damage Calculation cannot deduce a source location for damage coming from %s; Falling back to WORLD_MAX dist!"), *GetPathNameSafe(Spec.Def));
	}

	// Apply ability source modifiers
	float PhysicalMaterialAttenuation = 1.0f;
	float DistanceAttenuation = 1.0f;
	/*if (const IHAbilitySourceInterface* AbilitySource = TypedContext->GetAbilitySource())
	{
		if (const UPhysicalMaterial* PhysMat = TypedContext->GetPhysicalMaterial())
		{
			PhysicalMaterialAttenuation = AbilitySource->GetPhysicalMaterialAttenuation(PhysMat, SourceTags, TargetTags);
		}

		DistanceAttenuation = AbilitySource->GetDistanceAttenuation(Distance, SourceTags, TargetTags);
	}
	DistanceAttenuation = FMath::Max(DistanceAttenuation, 0.0f);*/

	// Clamping is done when damage is converted to -health
	const float DamageDone = FMath::Max(BaseDamage * DistanceAttenuation * PhysicalMaterialAttenuation * DamageInteractionAllowedMultiplier, 0.0f);

	if (DamageDone > 0.0f)
	{
		// Apply a damage modifier, this gets turned into - health on the target
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UHHealthSet::GetDamageAttribute(), EGameplayModOp::Additive, DamageDone));
	}
#endif // #if WITH_SERVER_CODE
}

//UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
//UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
//
//const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
//
//FAggregatorEvaluateParameters EvalParams;
//EvalParams.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
//EvalParams.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
//
//float Armor = 0.f;
//ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvalParams, Armor);
//
//float Damage = 0.f; //Capture optional damage attribute from ge for calcmodifier under execcalc
//ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DamageDef, EvalParams, Damage);
////Add setbycaller if it exists. Remember ror
//Damage += FMath::Max<float>(Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Damage")), false, -1.f), 0.f);
//
////We can multiply thi shere if we want like isaac
//float FlatDamage = Damage;
//
//float ArmorReducedDamage = (FlatDamage) * (100.f / (100.f + Armor));
//
//if (ArmorReducedDamage > 0.f)
//{
//	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().DamageProperty, EGameplayModOp::Additive, ArmorReducedDamage));
//}
