// Fill out your copyright notice in the Description page of Project Settings.


#include "IndicatorLibrary.h"

#include "HIndicatorManagerComponent.h"

UIndicatorLibrary::UIndicatorLibrary()
{
}

UHIndicatorManagerComponent* UIndicatorLibrary::GetIndicatorManagerComponent(AController* Controller)
{
	return UHIndicatorManagerComponent::GetComponent(Controller);
}