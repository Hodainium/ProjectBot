// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HGameplayTagStackContainer.h"
#include "HTeamInfoBase.h"
#include "HTeamPrivateInfo.generated.h"

namespace EEndPlayReason { enum Type : int; }

class UHTeamCreationComponent;
class UHTeamSubsystem;
class UObject;
struct FFrame;

UCLASS()
class AHTeamPrivateInfo : public AHTeamInfoBase
{
	GENERATED_BODY()

public:
	AHTeamPrivateInfo(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};