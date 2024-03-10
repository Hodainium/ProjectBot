// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "HCharacterBaseOld.h"
#include "GameFramework/Character.h"
#include "InputMappingContext.h"
#include "HereWeGo/HereWeGo.h"
#include "HereWeGo/Actors/Characters/HCharacterBase.h"
#include "Input/HInputMappingContextAndPriority.h"
#include "HPlayerCharacter.generated.h"

class UCommonActivatableWidget;
class UHHUDLayout;
class UHPawnData;
class UHCameraComponent;
struct FGameplayAbilitySpecHandle;
class UHCameraMode;
class UHCharacterMovementComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UHInputConfig;
struct FInputActionValue;
class UHInteractionComponent;
class AHInteractableGunPickup;
class UHAbilitySystemComponent;
class UHAttributeSetBase;
class UGameplayEffect;
class UGameplayAbility;

UCLASS()
class HEREWEGO_API AHPlayerCharacter : public AHCharacterBase
{
	GENERATED_BODY()

	public:

	// Sets default values for this character's properties
	AHPlayerCharacter(const FObjectInitializer& ObjectInitializer);

	//virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void ResetJumpState() override;
	virtual void OnJumped_Implementation() override;

	//const UHPawnData* GetPawnData() const { return PawnData; }

	///** Sets the current pawn data */
	//void SetPawnData(const UHPawnData* InPawnData);

	void ToggleCouch();

	UFUNCTION(BlueprintCallable, Category="Player|ADS")
	void StartAiming();

	UFUNCTION(BlueprintCallable, Category = "Player|ADS")
	void StopAiming();

	bool CanShoot();

	void Input_Movement(const FInputActionValue& InputValue);
	void Input_LookMouse(const FInputActionValue& InputActionValue);
	void Input_LookStick(const FInputActionValue& InputActionValue);
	void Input_SprintStart(const FInputActionValue& InputActionValue);
	void Input_SprintEnd(const FInputActionValue& InputActionValue);
	void Input_Crouch(const FInputActionValue& InputActionValue);
	void Input_JumpStart();
	void Input_JumpReleased();
	void Input_InteractTriggered();
	void Input_InteractHeld();
	void Input_InteractReleased();
	void Input_Ability1Start();
	void Input_Ability1Released();
	void Input_Ability2Start();
	void Input_Ability2Released();
	void Input_Ability3Start();
	void Input_Ability3Released();
	void Input_Ability4Start();
	void Input_Ability4Released();

	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);

	bool AttemptToShoot();

	UFUNCTION()
	void OnJumpApexEvent();

	UFUNCTION()
	void OnLandedEvent(const FHitResult& Hit);

	UFUNCTION()
	bool AttemptEquipInteractWeapon(AActor* FocusedGun);

	UFUNCTION()
	void OnShootCooldownReached();

	/** Overrides the camera from an active gameplay ability */
	void SetAbilityCameraMode(TSubclassOf<UHCameraMode> CameraMode, const FGameplayAbilitySpecHandle& OwningSpecHandle);

	/** Clears the camera override if it is set */
	void ClearAbilityCameraMode(const FGameplayAbilitySpecHandle& OwningSpecHandle);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable, Category = "HPlayer|MovementComponent")
	UHCharacterMovementComponent* GetHMovementComponent() const;

	virtual FVector GetPawnViewLocation() const override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void UnPossessed() override;

	virtual void OnRep_Controller() override;

	virtual void HandleControllerChanged() override;

	void SendLocalInputToASC(const EHAbilityInputID AbilityInputID, bool bIsPressed);

	TSubclassOf<UHCameraMode> DetermineCameraMode() const;

	/*UFUNCTION()
	void OnRep_PawnData();*/

public:

#pragma region Jumping

	bool bJumpAscending;
	bool bJumpInputHeldRaw;

#pragma endregion

protected:
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UHCharacterMovementComponent> HMovementComp;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UHInteractionComponent> InteractionComp;

	UPROPERTY(EditAnywhere, Category = "HPlayer|Input")
	TArray<FHInputMappingContextAndPriority> DefaultInputMappings;

	UPROPERTY(EditAnywhere, Category = "HPlayer|Input")
	TObjectPtr<UHInputConfig> InputConfig;

	// Default camera mode used by player controlled pawns.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	TSubclassOf<UHCameraMode> DefaultCameraMode;

	/** Camera mode set by an ability. */
	UPROPERTY()
	TSubclassOf<UHCameraMode> AbilityCameraMode;

	/** Spec handle for the last ability to set a camera mode. */
	FGameplayAbilitySpecHandle AbilityCameraModeOwningSpecHandle;

	///** Pawn data used to create the pawn. Specified from a spawn function or on a placed instance. */
	//UPROPERTY(EditInstanceOnly, ReplicatedUsing = OnRep_PawnData, Category = "H|Pawn")
	//TObjectPtr<const UHPawnData> PawnData;

	UPROPERTY(EditInstanceOnly, Category = "H|Pawn")
	TObjectPtr<const UHPawnData> PawnData;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UHCameraComponent> CameraComp;

	//TODO: Remove asap 1/27/24
#pragma region Camera

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArmComp;

	float SavedCameraTargetArmLength;
	FVector SavedCameraSocketOffset;

	UPROPERTY(EditAnywhere, Category = "HPlayer|ADSCamera")
	float ADSCameraArmLength;

	UPROPERTY(EditAnywhere, Category = "HPlayer|ADSCamera")
	FVector ADSCameraSocketOffset;

#pragma endregion

#pragma region Running

	UPROPERTY(BlueprintReadOnly)
	bool WantsToRun;

#pragma endregion

#pragma region ShootingandADS

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<AHInteractableGunPickup> EquippedWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HPlayer|Shooting")
	TSubclassOf<UGameplayEffect> DamageGameplayEffect;

	/*UPROPERTY(BlueprintReadWrite, Meta = (ExposeOnSpawn = true))
	FGameplayEffectSpecHandle DamageEffectSpecHandle;*/

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HPlayer|Shooting")
	float GunDamage;

	UPROPERTY(VisibleAnywhere)
	bool WantsToShoot;

	UPROPERTY(VisibleAnywhere)
	bool IsShootingCooldown;

	UPROPERTY(BlueprintReadWrite)
	bool WantsToAim;

	UPROPERTY(VisibleAnywhere)
	bool WeaponEquipped;

	UPROPERTY(EditAnywhere)
	bool bDebugShootDraw;

	FTimerHandle TimerHandle_FireRateDelay;
	FTimerDelegate TimerDelegate_OnFireRateCooldown;

#pragma endregion

#pragma region Looking

	UPROPERTY(BlueprintReadOnly)
	float InputLookAxisYaw;

#pragma endregion

#pragma region Interaction

	UPROPERTY(VisibleAnywhere)
	bool IsInteractHeld;

	UPROPERTY(VisibleAnywhere)
	bool HasInteracted;

#pragma endregion
};
