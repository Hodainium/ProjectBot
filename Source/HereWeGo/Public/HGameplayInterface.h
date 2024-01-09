// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HGameplayInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UHGameplayInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class HEREWEGO_API IHGameplayInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FText GetInteractText(APawn* InstigatorPawn);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool OnInteract(APawn* InstigatorPawn);

	virtual bool OnInteract_Implementation(APawn* InstigatorPawn);
	virtual FText GetInteractText_Implementation(APawn* InstigatorPawn);
};
