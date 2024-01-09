// Fill out your copyright notice in the Description page of Project Settings.

#include "HGameModeBase.h"

#include "HPlayerCharacter.h"
#include "HPlayerState.h"
#include "HPlayerController.h"
#include "UI/HHUD.h"


AHGameModeBase::AHGameModeBase()
{
	PlayerControllerClass = AHPlayerController::StaticClass();
	PlayerStateClass = AHPlayerState::StaticClass();
	DefaultPawnClass = AHPlayerCharacter::StaticClass();
	HUDClass = AHHUD::StaticClass();
}
