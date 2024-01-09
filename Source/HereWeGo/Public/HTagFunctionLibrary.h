// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HTagFunctionLibrary.generated.h"

struct FGameplayTag;

/**
 * 
 */
UCLASS()
class HEREWEGO_API UHTagFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	
	static FGameplayTag GetTagFromString(const char* TagName);
};
