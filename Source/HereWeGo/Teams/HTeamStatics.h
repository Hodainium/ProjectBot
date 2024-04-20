// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HTeamStatics.generated.h"

class UHTeamDisplayAsset;
/**
 * 
 */
UCLASS()
class HEREWEGO_API UHTeamStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	// Returns the team this object belongs to, or INDEX_NONE if it is not part of a team
	UFUNCTION(BlueprintCallable, Category = Teams, meta = (Keywords = "GetTeamFromObject", DefaultToSelf = "Agent", AdvancedDisplay = "bLogIfNotSet"))
	static void FindTeamFromObject(const UObject* Agent, bool& bIsPartOfTeam, int32& TeamId, UHTeamDisplayAsset*& DisplayAsset, bool bLogIfNotSet = false);

	UFUNCTION(BlueprintCallable, Category = Teams, meta = (WorldContext = "WorldContextObject"))
	static UHTeamDisplayAsset* GetTeamDisplayAsset(const UObject* WorldContextObject, int32 TeamId);

	UFUNCTION(BlueprintCallable, Category = Teams)
	static float GetTeamScalarWithFallback(UHTeamDisplayAsset* DisplayAsset, FName ParameterName, float DefaultValue);

	UFUNCTION(BlueprintCallable, Category = Teams)
	static FLinearColor GetTeamColorWithFallback(UHTeamDisplayAsset* DisplayAsset, FName ParameterName, FLinearColor DefaultValue);

	UFUNCTION(BlueprintCallable, Category = Teams)
	static UTexture* GetTeamTextureWithFallback(UHTeamDisplayAsset* DisplayAsset, FName ParameterName, UTexture* DefaultValue);
};

