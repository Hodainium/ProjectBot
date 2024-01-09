// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "IndicatorLibrary.generated.h"

class AController;
class UHIndicatorManagerComponent;
class UObject;
struct FFrame;

/**
 * 
 */
UCLASS()
class HEREWEGO_API UIndicatorLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UIndicatorLibrary();

	/**  */
	UFUNCTION(BlueprintCallable, Category = Indicator)
	static UHIndicatorManagerComponent* GetIndicatorManagerComponent(AController* Controller);
};
