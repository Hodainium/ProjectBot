// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueManager.h"
#include "HGameplayCueManager.generated.h"

/**
 * 
 */
UCLASS()
class HEREWEGO_API UHGameplayCueManager : public UGameplayCueManager
{
	GENERATED_BODY()

	virtual bool ShouldAsyncLoadRuntimeObjectLibraries() const override
	{
		return false;
	}
};
