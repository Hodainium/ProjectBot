// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "HPawnData.generated.h"

class UHInputConfig;
class UHCameraMode;
class UHAbilityTagRelationshipMapping;
class UHAbilitySet;
/**
 * 
 */
UCLASS()
class HEREWEGO_API UHPawnData : public UDataAsset
{
	GENERATED_BODY()

public:

	UHPawnData(const FObjectInitializer& ObjectInitializer);

public:

	// Class to instantiate for this pawn (should usually derive from AHPawn or AHCharacter).
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "H|Pawn")
	TSubclassOf<APawn> PawnClass;

	// Ability sets to grant to this pawn's ability system.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "H|Abilities")
	TArray<TObjectPtr<UHAbilitySet>> AbilitySets;

	// What mapping of ability tags to use for actions taking by this pawn
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "H|Abilities")
	TObjectPtr<UHAbilityTagRelationshipMapping> TagRelationshipMapping;

	// Input configuration used by player controlled pawns to create input mappings and bind input actions.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "H|Input")
	TObjectPtr<UHInputConfig> InputConfig;

	// Default camera mode used by player controlled pawns.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "H|Camera")
	TSubclassOf<UHCameraMode> DefaultCameraMode;
};
