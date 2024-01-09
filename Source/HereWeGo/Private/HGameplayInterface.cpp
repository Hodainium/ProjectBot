// Fill out your copyright notice in the Description page of Project Settings.


#include "HGameplayInterface.h"

// Add default functionality here for any IHGameplayInterface functions that are not pure virtual.

bool IHGameplayInterface::OnInteract_Implementation(APawn* InstigatorPawn)
{
	return false;
}

FText IHGameplayInterface::GetInteractText_Implementation(APawn* InstigatorPawn)
{
	return FText::GetEmpty();
}

