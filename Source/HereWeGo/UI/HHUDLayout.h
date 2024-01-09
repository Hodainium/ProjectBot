// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/HCommonActivatableWidget.h"
#include "NativeGameplayTags.h"
#include "HHUDLayout.generated.h"

/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable, meta = (DisplayName = "HUD Layout", Category = HHUD))
class HEREWEGO_API UHHUDLayout : public UHCommonActivatableWidget
{
	GENERATED_BODY()

public:
	UHHUDLayout(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UUserWidget interface
	virtual void NativeOnInitialized() override;
	//~End of UUserWidget interface

protected:
	/** Callback called whenever the player gives the "Show Main Menu" input */
	void HandleMainMenuAction();

	/** Class to use as the Main Menu for this HUD Layout */
	UPROPERTY(EditDefaultsOnly)
	TSoftClassPtr<UHCommonActivatableWidget> MainMenuClass;
};
