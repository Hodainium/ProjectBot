// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "HEquipmentDefinition.generated.h"

class UHEquipmentInstance;
class UHAbilitySet;

/**
 * 
 */
USTRUCT()
struct HEREWEGO_API FHItemActorToSpawn
{
	GENERATED_BODY()

	FHItemActorToSpawn()
	{}

	UPROPERTY(EditAnywhere, Category = "Equipment")
	TSubclassOf<AActor> ActorToSpawn;

	UPROPERTY(EditAnywhere, Category = "Equipment")
	FName AttachSocket;

	UPROPERTY(EditAnywhere, Category = "Equipment")
	FTransform AttachTransform;
};

//Need to change to equipmentdef
UCLASS(Const, BlueprintType)
class HEREWEGO_API UHEquipmentDefinition : public UDataAsset
{
	GENERATED_BODY()

public:

	//What we actually spawn. Use for tracking world representation
	UPROPERTY(EditDefaultsOnly, Category="EquipmentDefinition|Instance")
	TSubclassOf<UHEquipmentInstance> InstanceType;

	UPROPERTY(EditDefaultsOnly, Category="EquipmentDefinition|Abilities")
	TArray<TObjectPtr<const UHAbilitySet>> AbilitySetsToGrant;

	UPROPERTY(EditDefaultsOnly, Category = "EquipmentDefinition|ActorsToSpawn")
	TArray<FHItemActorToSpawn> ActorsToSpawn;
};
