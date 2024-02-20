// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HCommonActivatableWidget.h"

#include "Editor/WidgetCompilerLog.h"
#include "Input/CommonUIInputTypes.h"

UHCommonActivatableWidget::UHCommonActivatableWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

TOptional<FUIInputConfig> UHCommonActivatableWidget::GetDesiredInputConfig() const
{
	FUIInputConfig ConfigOverride;

	switch (InputMode)
	{
	case EHWidgetInputMode::Game:
		ConfigOverride = FUIInputConfig(ECommonInputMode::Game, GameMouseCaptureMode);
		break;
	case EHWidgetInputMode::GameAndMenu:
		ConfigOverride = FUIInputConfig(ECommonInputMode::All, GameMouseCaptureMode);
		break;
	case EHWidgetInputMode::Menu:
		ConfigOverride = FUIInputConfig(ECommonInputMode::Menu, EMouseCaptureMode::NoCapture);
		break;
	case EHWidgetInputMode::Default:
	default:
		// By default, no input change is desired, return an empty config
		return TOptional<FUIInputConfig>();
	}

	return ConfigOverride;
}

void UHCommonActivatableWidget::NativeDestruct()
{
	for (FUIActionBindingHandle Handle : BindingHandles)
	{
		if (Handle.IsValid())
		{
			Handle.Unregister();
		}
	}
	BindingHandles.Empty();

	Super::NativeDestruct();
}

void UHCommonActivatableWidget::RegisterBinding(FDataTableRowHandle InputAction, const FInputActionExecutedDelegate& Callback, FInputActionBindingHandle& BindingHandle)
{
	FBindUIActionArgs BindArgs(InputAction, FSimpleDelegate::CreateLambda([InputAction, Callback]()
	{
		Callback.ExecuteIfBound(InputAction.RowName);
	}));
	BindArgs.bDisplayInActionBar = true;

	BindingHandle.Handle = RegisterUIActionBinding(BindArgs);
	BindingHandles.Add(BindingHandle.Handle);
}

void UHCommonActivatableWidget::UnregisterBinding(FInputActionBindingHandle BindingHandle)
{
	if (BindingHandle.Handle.IsValid())
	{
		BindingHandle.Handle.Unregister();
		BindingHandles.Remove(BindingHandle.Handle);
	}
}

void UHCommonActivatableWidget::UnregisterAllBindings()
{
	for (FUIActionBindingHandle Handle : BindingHandles)
	{
		Handle.Unregister();
	}
	BindingHandles.Empty();
}
