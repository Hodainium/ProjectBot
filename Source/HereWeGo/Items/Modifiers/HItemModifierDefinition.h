// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "HItemModifierDefinition.generated.h"

//Like fragments but 100% different. These define the base of mnodifiers we can recieve. Maybe should have their own fragments. 

class UHItemModifierInstance;
class UHItemModifierFragment;
// DefaultToInstanced, EditInlineNew, Abstract

UCLASS()
class HEREWEGO_API UHItemModifierDefinition : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Fragments")
	TArray<TObjectPtr<UHItemModifierFragment>> Fragments;
};

UCLASS()
class HEREWEGO_API UHItemModifierInstance : public UObject
{
	GENERATED_BODY()

	virtual void ApplyMod(UHItemModifierInstance* Instance) const {}

	virtual void RemoveMod(UHItemModifierInstance* Instance) const {}
};

