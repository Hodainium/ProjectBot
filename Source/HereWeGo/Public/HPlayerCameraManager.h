// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "HPlayerCameraManager.generated.h"

#define H_CAMERA_DEFAULT_FOV		(80.0f)
#define H_CAMERA_DEFAULT_PITCH_MIN	(-89.0f)
#define H_CAMERA_DEFAULT_PITCH_MAX	(89.0f)

class UHUICameraManagerComponent;
/**
 * 
 */
UCLASS(notplaceable)
class HEREWEGO_API AHPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public:

	AHPlayerCameraManager(const FObjectInitializer& ObjectInitializer);

	UHUICameraManagerComponent* GetUICameraComponent() const;

protected:

	virtual void UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime) override;

	virtual void DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos) override;

private:
	/** The UI Camera Component, controls the camera when UI is doing something important that gameplay doesn't get priority over. */
	UPROPERTY(Transient)
	TObjectPtr<UHUICameraManagerComponent> UICamera;
};
