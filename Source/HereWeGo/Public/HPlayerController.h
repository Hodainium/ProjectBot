// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HCameraAssistInterface.h"
#include "GameFramework/PlayerController.h"
#include "CommonPlayerController.h"
#include "HPlayerCharacter.h"
#include "HPlayerController.generated.h"

class UHIndicatorManagerComponent;
class UHAbilitySystemComponent;
class AHPlayerState;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHOnPlayerStateChanged, APlayerState*, NewPlayerState);

/**
 * 
 */
UCLASS()
class HEREWEGO_API AHPlayerController : public ACommonPlayerController, public IHCameraAssistInterface, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AHPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "H|PlayerController")
	AHPlayerState* GetHPlayerState() const;

	UFUNCTION(BlueprintCallable, Category = "H|PlayerCharacter")
	AHCharacterBase* GetHCharacterBase() const;

	UFUNCTION(BlueprintCallable, Category = "H|Player")
	UHAbilitySystemComponent* GetHAbilitySystemComponent() const;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void BeginPlay() override;

	UFUNCTION()
	void HandlePossessedPawnChanged(APawn* OldPawnBroadcasted, APawn* NewPawnBroadcasted);

	void CreateHUD();

	void RemoveHUD();

	//~IHCameraAssistInterface interface
	virtual void OnCameraPenetratingTarget() override;
	//~End of IHCameraAssistInterface interface

	//~APlayerController interface
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
	virtual void PlayerTick(float DeltaTime) override;
	//~End of APlayerController interface

	virtual void InitPlayerState() override;
	virtual void CleanupPlayerState() override;
	virtual void OnRep_PlayerState() override;

protected:
	//~APlayerController interface
	virtual void UpdateHiddenComponents(const FVector& ViewLocation, TSet<FPrimitiveComponentId>& OutHiddenComponents) override;
	//~End of APlayerController interface

	/** The HUD Layout widget to use (must be derived from HHUD Layout) */
	UPROPERTY(EditDefaultsOnly, DisplayName = "HUD Layout Class")
	TSubclassOf<UHHUDLayout> HUDLayoutClass;

	/** Used to keep track of the widget that was created to be our HUD */
	UPROPERTY(Transient, VisibleInstanceOnly)
	TWeakObjectPtr<UCommonActivatableWidget> HUDLayoutWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UHIndicatorManagerComponent> IndicatorComponent;

	UPROPERTY(BlueprintAssignable)
	FHOnPlayerStateChanged OnPlayerStateChangedDelegate;

	bool bHideViewTargetPawnNextFrame = false;

private:
	void BroadcastOnPlayerStateChanged();
};
