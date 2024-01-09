// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "HHitCapsuleData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType, Category = "HitBoxData")
struct HEREWEGO_API FHHitCapsuleData
{
	GENERATED_BODY()

	//Radius of capsule or sphere
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HitBoxData|Size")
	float Radius;

	//If this is zero we will do sphere trace instead
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HitBoxData|Size")
	float CapsuleHalfHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitBoxData|Damage")
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitBoxData|Options")
	bool bCanHitMultipleTimes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitBoxData|Options")
	bool bCanPenetrate;
};
