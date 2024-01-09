// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HGameplayAbility.h"
#include "HGA_MeleeAttack.generated.h"

/**
 * 
 */
UCLASS()
class HEREWEGO_API UHGA_MeleeAttack : public UHGameplayAbility
{
	GENERATED_BODY()

public:
	UHGA_MeleeAttack();

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UAnimMontage* MeleeAttackMontage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<TObjectPtr<UGameplayEffect>> EffectsToApplyOnHit;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UGameplayEffect> DamageGE;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UPROPERTY()
	float Damage;

	UPROPERTY()
	TArray<TObjectPtr<AActor>> ConfirmedHitActorsToIgnore;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION()
	virtual void OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	virtual void OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	virtual void EventReceived(FGameplayTag EventTag, FGameplayEventData EventData);

	void OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& InData, FGameplayTag ApplicationTag);

	UFUNCTION(BlueprintNativeEvent)
	void OnMeleeWeaponTargetDataReady(const FGameplayAbilityTargetDataHandle& TargetData);

	UFUNCTION()
	virtual void OnMeleeHitReceived(TArray<FHitResult>& HitResults, UPrimitiveComponent* HitMesh);
};
