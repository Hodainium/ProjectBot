// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HCommonActivatableWidget.h"

#include "Editor/WidgetCompilerLog.h"

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
