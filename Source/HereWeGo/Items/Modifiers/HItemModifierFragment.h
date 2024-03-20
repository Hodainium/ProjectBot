// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "HItemModifierFragment.generated.h"

class UHItemModifierInstance;

// Represents a fragment of an item definition
UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class HEREWEGO_API UHItemModifierFragment : public UObject
{
	GENERATED_BODY()

public:
	virtual void OnInstanceCreated(UHItemModifierInstance* Instance) const {}

	virtual void OnInstanceRemoved(UHItemModifierInstance* Instance) const {}

	/** Text that describes the mod */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Text;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bDisplayInUI = false;
};

//UCLASS()
//class HEREWEGO_API UHItemModifierFragment_Test : public UHItemModifierFragment
//{
//	GENERATED_BODY()
//
//protected:
//	UPROPERTY(EditDefaultsOnly, Category = Equipment)
//	int32 InitialItemStats;
//
//public:
//	virtual void OnInstanceCreated(UHItemModifierInstance* Instance) const override;
//
//	int32 GetItemStatByTag(FGameplayTag Tag) const;
//};


