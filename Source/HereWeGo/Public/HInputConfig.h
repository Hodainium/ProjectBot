// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"
#include "Engine/DataAsset.h"
#include "UObject/NameTypes.h"
#include "HInputConfig.generated.h"

class UInputAction;
struct FGameplayTag;
class FName;

UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_InputAction_Movement);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_InputAction_LookMouse);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_InputAction_LookStick);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_InputAction_Run);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_InputAction_AutoRun);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_InputAction_Jump);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_InputAction_Crouch);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_InputAction_Equipment_1);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_InputAction_Equipment_2);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_InputAction_Equipment_3);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_InputAction_Equipment_4);

UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_InputAction_Ability_Attack_1);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_InputAction_Ability_Attack_2);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_InputAction_Ability_Interact);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_InputAction_Ability_1);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_InputAction_Ability_2);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_InputAction_Ability_3);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_InputAction_Ability_4);

UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_InputAction_Inventory);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_InputAction_Pause);

/**
 * 
 */

USTRUCT(BlueprintType)
struct FHInputAction
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;

};

UCLASS()
class HEREWEGO_API UHInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:

	UHInputConfig(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "Pawn")
	const UInputAction* FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;

	UFUNCTION(BlueprintCallable, Category = "Pawn")
	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;

public:
	// List of input actions used by the owner.  These input actions are mapped to a gameplay tag and must be manually bound.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FHInputAction> NativeInputActions;

	// List of input actions used by the owner.  These input actions are mapped to a gameplay tag and are automatically bound to abilities with matching input tags.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FHInputAction> AbilityInputActions;
};
