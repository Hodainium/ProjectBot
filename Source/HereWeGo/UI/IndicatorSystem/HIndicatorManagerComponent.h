// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ControllerComponent.h"

#include "HIndicatorManagerComponent.generated.h"

class AController;
class UIndicatorDescriptor;
class UObject;
struct FFrame;

/**
 * @class UHIndicatorManagerComponent
 */
UCLASS(BlueprintType, Blueprintable)
class HEREWEGO_API UHIndicatorManagerComponent : public UControllerComponent
{
	GENERATED_BODY()

public:
	UHIndicatorManagerComponent(const FObjectInitializer& ObjectInitializer);

	static UHIndicatorManagerComponent* GetComponent(AController* Controller);

	UFUNCTION(BlueprintCallable, Category = Indicator)
	void AddIndicator(UIndicatorDescriptor* IndicatorDescriptor);

	UFUNCTION(BlueprintCallable, Category = Indicator)
	void RemoveIndicator(UIndicatorDescriptor* IndicatorDescriptor);

	DECLARE_EVENT_OneParam(UHIndicatorManagerComponent, FIndicatorEvent, UIndicatorDescriptor* Descriptor)
	FIndicatorEvent OnIndicatorAdded;
	FIndicatorEvent OnIndicatorRemoved;

	const TArray<UIndicatorDescriptor*>& GetIndicators() const { return Indicators; }

private:
	UPROPERTY()
	TArray<TObjectPtr<UIndicatorDescriptor>> Indicators;
};
