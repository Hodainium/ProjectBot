// Fill out your copyright notice in the Description page of Project Settings.


#include "HPawnData.h"

UHPawnData::UHPawnData(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PawnClass = nullptr;
	InputConfig = nullptr;
	DefaultCameraMode = nullptr;
}
