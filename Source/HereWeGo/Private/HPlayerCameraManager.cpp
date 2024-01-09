// Fill out your copyright notice in the Description page of Project Settings.


#include "HPlayerCameraManager.h"

#include "HCameraComponent.h"
#include "HUICameraManagerComponent.h"
#include "Engine/Canvas.h"

static FName UICameraComponentName(TEXT("UICamera"));

AHPlayerCameraManager::AHPlayerCameraManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DefaultFOV = H_CAMERA_DEFAULT_FOV;
	ViewPitchMin = H_CAMERA_DEFAULT_PITCH_MIN;
	ViewPitchMax = H_CAMERA_DEFAULT_PITCH_MAX;

	UICamera = CreateDefaultSubobject<UHUICameraManagerComponent>(UICameraComponentName);
}

UHUICameraManagerComponent* AHPlayerCameraManager::GetUICameraComponent() const
{
	return UICamera;
}

void AHPlayerCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
	// If the UI Camera is looking at something, let it have priority.
	if (UICamera->NeedsToUpdateViewTarget())
	{
		Super::UpdateViewTarget(OutVT, DeltaTime);
		UICamera->UpdateViewTarget(OutVT, DeltaTime);
		return;
	}

	Super::UpdateViewTarget(OutVT, DeltaTime);
}

void AHPlayerCameraManager::DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos)
{
	check(Canvas);

	FDisplayDebugManager& DisplayDebugManager = Canvas->DisplayDebugManager;

	DisplayDebugManager.SetFont(GEngine->GetSmallFont());
	DisplayDebugManager.SetDrawColor(FColor::Yellow);
	DisplayDebugManager.DrawString(FString::Printf(TEXT("HPlayerCameraManager: %s"), *GetNameSafe(this)));

	Super::DisplayDebug(Canvas, DebugDisplay, YL, YPos);

	const APawn* Pawn = (PCOwner ? PCOwner->GetPawn() : nullptr);

	if (const UHCameraComponent* CameraComponent = UHCameraComponent::FindCameraComponent(Pawn))
	{
		CameraComponent->DrawDebug(Canvas);
	}
}