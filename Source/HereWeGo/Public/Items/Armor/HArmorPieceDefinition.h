// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HEquipmentDefinition.h"
#include "UObject/NoExportTypes.h"
#include "HArmorPieceDefinition.generated.h"

class UHArmorInstance;
class UHEquipmentInstance;
class UHAbilitySet;

UENUM(BlueprintType)
enum EHArmorSlot
{
	UndefinedSlot,
	Head,
	Body,
	ArmL,
	HandL,
	ArmR,
	HandR,
	LegL,
	FootL,
	LegR,
	FootR
};

USTRUCT()
struct HEREWEGO_API FHArmorMeshToSpawn
{
	GENERATED_BODY()

	FHArmorMeshToSpawn()
	{}

	UPROPERTY(EditDefaultsOnly, Category = "ArmorSlots")
	TArray<TEnumAsByte<EHArmorSlot>> OccupyingArmorSlots;

	UPROPERTY(EditDefaultsOnly, Category = "Mesh")
	TObjectPtr<USkeletalMesh> SkeletalMesh;
};

/**
 * 
 */
UCLASS(Blueprintable, Const, Abstract, BlueprintType)
class HEREWEGO_API UHArmorPieceDefinition : public UObject
{
	GENERATED_BODY()

public:

	UHArmorPieceDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//What we actually spawn. Use for tracking world representation
	UPROPERTY(EditDefaultsOnly, Category = "ArmorPieceDefinition|Instance")
	TSubclassOf<UHArmorInstance> InstanceType;

	UPROPERTY(EditDefaultsOnly, Category = "ArmorPieceDefinition|ArmorToSpawn")
	TArray<FHArmorMeshToSpawn> ArmorPiecesToSpawn;

	UPROPERTY(EditDefaultsOnly, Category = "ArmorPieceDefinition|ActorToSpawn")
	TArray<FHItemActorToSpawn> ArmorActorsToSpawn;

	UPROPERTY(EditDefaultsOnly, Category = "ArmorPieceDefinition|AbilitySet")
	TArray<TObjectPtr<const UHAbilitySet>> AbilitySetsToGrant;
};
