// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "HCameraComponent.generated.h"

class UHCameraModeStack;
struct FGameplayTag;
class UHCameraMode;

DECLARE_DELEGATE_RetVal(TSubclassOf<UHCameraMode>, FHCameraModeDelegate);

/**
 * The base camera component class used by this project.
 */
UCLASS()
class HEREWEGO_API UHCameraComponent : public UCameraComponent
{
	GENERATED_BODY()

public:

	UHCameraComponent(const FObjectInitializer& ObjectInitializer);

	// Returns the camera component if one exists on the specified actor.
	UFUNCTION(BlueprintPure, Category = "H|Camera")
	static UHCameraComponent* FindCameraComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UHCameraComponent>() : nullptr); }

	// Returns the target actor that the camera is looking at.
	virtual AActor* GetTargetActor() const { return GetOwner(); }

	// Delegate used to query for the best camera mode.
	FHCameraModeDelegate DetermineCameraModeDelegate;

	// Add an offset to the field of view.  The offset is only for one frame, it gets cleared once it is applied.
	void AddFieldOfViewOffset(float FovOffset) { FieldOfViewOffset += FovOffset; }

	virtual void DrawDebug(UCanvas* Canvas) const;

	// Gets the tag associated with the top layer and the blend weight of it
	void GetBlendInfo(float& OutWeightOfTopLayer, FGameplayTag& OutTagOfTopLayer) const;

protected:

	virtual void OnRegister() override;
	virtual void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView) override;

	virtual void UpdateCameraModes();

protected:

	// Stack used to blend the camera modes.
	UPROPERTY()
	TObjectPtr<UHCameraModeStack> CameraModeStack;

	// Offset applied to the field of view.  The offset is only for one frame, it gets cleared once it is applied.
	float FieldOfViewOffset;

};
