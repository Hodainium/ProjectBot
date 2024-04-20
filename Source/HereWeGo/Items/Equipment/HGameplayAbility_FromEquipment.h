// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HGameplayAbility.h"
#include "HGameplayAbility_FromEquipment.generated.h"

class UHInventoryItemInstance;
class UHEquipmentInstance;
/**
 *
 */
UCLASS()
class HEREWEGO_API UHGameplayAbility_FromEquipment : public UHGameplayAbility
{
	GENERATED_BODY()

public:

	UHGameplayAbility_FromEquipment(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "H|Ability")
	UHEquipmentInstance* GetAssociatedEquipment() const;

	UFUNCTION(BlueprintCallable, Category = "H|Ability")
	UHInventoryItemInstance* GetAssociatedItem() const;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
};
