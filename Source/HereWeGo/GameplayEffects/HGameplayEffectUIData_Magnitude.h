// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectUIData.h"
#include "ScalableFloat.h"
#include "HGameplayEffectUIData_Magnitude.generated.h"


/**
 * 
 */
UCLASS()
class HEREWEGO_API UHGameplayEffectUIData_Magnitude : public UGameplayEffectUIData
{
	GENERATED_BODY()

public:

	UHGameplayEffectUIData_Magnitude();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Data)
	FScalableFloat DisplayMagnitude;
};

