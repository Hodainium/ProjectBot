// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HItemDefinition.h"
#include "HereWeGo/HAssetManager.h"
#include "HArmorItemDefinition.generated.h"

class UHArmorPieceDefinition;
/**
 * 
 */
UCLASS()
class HEREWEGO_API UHArmorItemDefinition : public UHItemDefinition
{
	GENERATED_BODY()

public:
	/** Constructor */
	UHArmorItemDefinition()
	{
		ItemType = EHItemType::Armor;
	}

	UPROPERTY(EditAnywhere, Category = "ArmorDef")
	TSubclassOf<UHArmorPieceDefinition> ArmorDefinition;
};
