// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "HCommonActivatableWidget.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FInputActionExecutedDelegate, FName, ActionName);

USTRUCT(BlueprintType)
struct FInputActionBindingHandle
{
	GENERATED_BODY()

public:
	FUIActionBindingHandle Handle;
};

/**
 * Input Modes that HActivatableWidgets can activate
 */
UENUM(BlueprintType)
enum class EHWidgetInputMode : uint8
{
	Default,
	GameAndMenu,
	Game,
	Menu
};

/**
 * Activatable Widget
 *
 * An activatable widget that optionally overrides the input mode when activated
 */
UCLASS(Abstract, Blueprintable, meta = (Category = "HUI", DisableNativeTick))
class HEREWEGO_API UHCommonActivatableWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	UHCommonActivatableWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UCommonActivatableWidget interface
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;
	//~End of UCommonActivatableWidget interface

protected:
	/** The desired input mode to use while this UI is activated, for example do you want key presses to still reach the game/player controller? */
	UPROPERTY(EditDefaultsOnly, Category = Input)
	EHWidgetInputMode InputMode = EHWidgetInputMode::Default;

	/** The desired mouse behavior when the game gets input. */
	UPROPERTY(EditDefaultsOnly, Category = Input)
	EMouseCaptureMode GameMouseCaptureMode = EMouseCaptureMode::CapturePermanently;

protected:
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintCallable, Category = ExtendedActivatableWidget)
	void RegisterBinding(FDataTableRowHandle InputAction, const FInputActionExecutedDelegate& Callback, FInputActionBindingHandle& BindingHandle);

	UFUNCTION(BlueprintCallable, Category = ExtendedActivatableWidget)
	void UnregisterBinding(FInputActionBindingHandle BindingHandle);

	UFUNCTION(BlueprintCallable, Category = ExtendedActivatableWidget)
	void UnregisterAllBindings();

private:
	TArray<FUIActionBindingHandle> BindingHandles;
};
