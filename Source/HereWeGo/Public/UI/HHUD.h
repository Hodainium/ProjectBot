// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "HHUD.generated.h"

/**
 * Modular HUD
 */
UCLASS(meta = (DisplayName = "H HUD", Category = "HUI"))
class HEREWEGO_API AHHUD : public AHUD
{
	GENERATED_BODY()

public:
	AHHUD(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	//~UObject interface
	virtual void PreInitializeComponents() override;
	//~End of UObject interface

	//~AActor interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End of AActor interface

	//~AHUD interface
	virtual void GetDebugActorList(TArray<AActor*>& InOutList) override;
	//~End of AHUD interface
};
