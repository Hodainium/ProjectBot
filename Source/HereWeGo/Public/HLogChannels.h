// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

HEREWEGO_API DECLARE_LOG_CATEGORY_EXTERN(LogHAbilitySystem, Log, All);
HEREWEGO_API DECLARE_LOG_CATEGORY_EXTERN(LogHGame, Log, All);
HEREWEGO_API DECLARE_LOG_CATEGORY_EXTERN(LogHTeams, Log, All);

HEREWEGO_API FString GetClientServerContextString(UObject* ContextObject = nullptr);
