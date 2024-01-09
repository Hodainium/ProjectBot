// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "HItemDefinition.h"
#include "Styling/SlateBrush.h"
#include "HInventoryFragmentTypes.generated.h"

class UHArmorPieceDefinition;
class UHReticleWidgetBase;
/**
 * 
 */
class UHEquipmentDefinition;
class USkeletalMesh;
class UObject;

UCLASS()
class HEREWEGO_API UHInventoryFragment_EquippableItem : public UHInventoryItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "EquipmentDef")
	TObjectPtr<UHEquipmentDefinition> EquipmentDefinition;
};

UCLASS()
class HEREWEGO_API UHInventoryFragment_ArmorItem : public UHInventoryItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "ArmorDef")
	TSubclassOf<UHArmorPieceDefinition> ArmorDefinition;
};

UCLASS()
class HEREWEGO_API UHInventoryFragment_PickupIcon : public UHInventoryItemFragment
{
	GENERATED_BODY()

public:
	UHInventoryFragment_PickupIcon();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance)
	TObjectPtr<USkeletalMesh> SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance)
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance)
	FLinearColor PadColor;
};

UCLASS()
class HEREWEGO_API UInventoryFragment_QuickBarIcon : public UHInventoryItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance)
	FSlateBrush Brush;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance)
	FSlateBrush AmmoBrush;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance)
	FText DisplayNameWhenEquipped;
};

UCLASS()
class HEREWEGO_API UHInventoryFragment_SetStats : public UHInventoryItemFragment
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category = Equipment)
	TMap<FGameplayTag, int32> InitialItemStats;

public:
	virtual void OnInstanceCreated(UHInventoryItemInstance* Instance) const override;

	int32 GetItemStatByTag(FGameplayTag Tag) const;
};

UCLASS()
class UHInventoryFragment_ReticleConfig : public UHInventoryItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Reticle)
	TArray<TSubclassOf<UHReticleWidgetBase>> ReticleWidgets;
};
