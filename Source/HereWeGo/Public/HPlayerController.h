// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HCameraAssistInterface.h"
#include "GameFramework/PlayerController.h"
#include "CommonPlayerController.h"
#include "HPlayerController.generated.h"

class UHIndicatorManagerComponent;
class UHAbilitySystemComponent;
class AHPlayerState;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHOnPlayerStateChanged, APlayerState*, NewPlayerState);

/**
 * 
 */
UCLASS()
class HEREWEGO_API AHPlayerController : public ACommonPlayerController, public IHCameraAssistInterface
{
	GENERATED_BODY()

public:
	AHPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "H|PlayerController")
	AHPlayerState* GetHPlayerState() const;

	UFUNCTION(BlueprintCallable, Category = "H|PlayerController")
	UHAbilitySystemComponent* GetHAbilitySystemComponent() const;

	void CreateHUD();

	//~IHCameraAssistInterface interface
	virtual void OnCameraPenetratingTarget() override;
	//~End of IHCameraAssistInterface interface

	//~APlayerController interface
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
	//~End of APlayerController interface

	void RegisterASCRef(UHAbilitySystemComponent* Ref);

	virtual void InitPlayerState() override;
	virtual void CleanupPlayerState() override;
	virtual void OnRep_PlayerState() override;

protected:
	//~APlayerController interface
	virtual void UpdateHiddenComponents(const FVector& ViewLocation, TSet<FPrimitiveComponentId>& OutHiddenComponents) override;
	//~End of APlayerController interface

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> HUDWidgetClassType;

	UPROPERTY()
	TObjectPtr<UUserWidget> HUDWidgetInstance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UHIndicatorManagerComponent> IndicatorComponent;

	UPROPERTY()
	TWeakObjectPtr<UHAbilitySystemComponent> AbilitySystemComponentRef;

	UPROPERTY(BlueprintAssignable)
	FHOnPlayerStateChanged OnPlayerStateChangedDelegate;

	bool bHideViewTargetPawnNextFrame = false;

private:
	void BroadcastOnPlayerStateChanged();
};
