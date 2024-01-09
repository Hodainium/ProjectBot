// Fill out your copyright notice in the Description page of Project Settings.


#include "HEnhancedInputComponent.h"

#include "EnhancedInputSubsystems.h"

UHEnhancedInputComponent::UHEnhancedInputComponent(const FObjectInitializer& ObjectInitializer)
{
}

void UHEnhancedInputComponent::AddInputMappings(const UHInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);

	// Here you can handle any custom logic to add something from your input config if required
}

void UHEnhancedInputComponent::RemoveInputMappings(const UHInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);

	// Here you can handle any custom logic to remove input mappings that you may have added above
}

void UHEnhancedInputComponent::RemoveBinds(TArray<uint32>& BindHandles)
{
	for (uint32 Handle : BindHandles)
	{
		RemoveBindingByHandle(Handle);
	}
	BindHandles.Reset();
}
