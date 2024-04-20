// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HAbilitySystemComponent.h"
#include "HEquipmentInstance.h"
#include "UObject/NoExportTypes.h"
#include "HAnimationTypes.h"
#include "HWeaponInstance.generated.h"

/**
 * Type of equipment applied to pawn
 */
UCLASS()
class HEREWEGO_API UHWeaponInstance : public UHEquipmentInstance
{
	GENERATED_BODY()

public:
	UHWeaponInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//Equipment interface
	virtual void OnEquipped() override;
	virtual void OnUnequipped() override;
	////////////

	UFUNCTION(BlueprintCallable)
	void UpdateFiringTime();

	//Returns how long it has been since weapon was fired or equipped
	UFUNCTION(BlueprintPure)
	float GetTimeSinceLastInteractedWith() const;

	UFUNCTION(BlueprintCallable)
	UHAbilitySystemComponent* GetASCFromOwningPawn();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Animation")
	FHAnimLayerSelectionSet EquippedAnimSet;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Animation")
	FHAnimLayerSelectionSet UnequippedAnimSet;

	UFUNCTION(BlueprintCallable, BlueprintPure=false, Category="Animation")
	TSubclassOf<UAnimInstance> PickBestAnimLayer(bool bEquipped, const FGameplayTagContainer& CosmeticTags) const;

	

private:
	double TimeLastEquipped = 0.0;
	double TimeLastFired = 0.0;
};
