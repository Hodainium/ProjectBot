// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonGameInstance.h"
#include "HGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class HEREWEGO_API UHGameInstance : public UCommonGameInstance
{
	GENERATED_BODY()

	UHGameInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
