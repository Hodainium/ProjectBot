// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "HAimSensitivityData.generated.h"

enum class EHGamepadSensitivity : uint8;

/**
 * 
 */
UCLASS()
class HEREWEGO_API UHAimSensitivityData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UHAimSensitivityData(const FObjectInitializer& ObjectInitializer);

	const float SensitivtyEnumToFloat(const EHGamepadSensitivity InSensitivity) const;

protected:
	/** Map of SensitivityMap settings to their corresponding float */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EHGamepadSensitivity, float> SensitivityMap;
};
