// Fill out your copyright notice in the Description page of Project Settings.


#include "HPlayerCharacter.h"

#include <string>

#include "CommonUIExtensions.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "HInputConfig.h"
#include "HTagFunctionLibrary.h"
#include "HEnhancedInputComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "HCharacterMovementComponent.h"
#include "HInteractableGunPickup.h"
#include "HInteractionComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/CapsuleComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "HPlayerState.h"
#include "HAttributeSetBase.h"
#include "HAbilitySystemComponent.h"
#include "HCameraComponent.h"
#include "HCameraMode.h"
#include "HLocalPlayer.h"
#include "HLogChannels.h"
#include "HPlayerController.h"
#include "HWeaponComponent.h"
#include "HPawnData.h"
#include "HereWeGo/Tags/H_Tags.h"
#include "HereWeGo/UI/HHUDLayout.h"
#include "Input/HInputMappingContextAndPriority.h"
#include "Kismet/GameplayStatics.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AHPlayerCharacter::AHPlayerCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("SpringArmComp");
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->SetUsingAbsoluteRotation(true);

	CameraComp = CreateDefaultSubobject<UHCameraComponent>("CameraComp");
	CameraComp->SetRelativeLocation(FVector(-300.0f, 0.0f, 75.0f));
	//CameraComp->SetupAttachment(SpringArmComp);

	InteractionComp = CreateDefaultSubobject<UHInteractionComponent>("InteractionComp");

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;

	HMovementComp = Cast<UHCharacterMovementComponent>(ACharacter::GetMovementComponent());

	WantsToRun = false;
	WantsToAim = false;

	bJumpInputHeldRaw = false;
	bJumpAscending = false;

	IsInteractHeld = false;
	HasInteracted = false;

	WeaponEquipped = false;

	ADSCameraSocketOffset = FVector::Zero();

	GunDamage = 12.f;

	TimerDelegate_OnFireRateCooldown.BindUFunction(this, "OnShootCooldownReached");
}

//void AHPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//
//	DOREPLIFETIME(AHPlayerCharacter, PawnData);
//}

// Called when the game starts or when spawned
void AHPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	/*UE_LOG(LogTemp, Warning, TEXT("Begin"));
	if (const TObjectPtr<APlayerController> PlayerController = Cast<APlayerController>(GetController()))
	{
		if (TObjectPtr<UEnhancedInputLocalPlayerSubsystem> Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(TestContext, 1);
		}
	}*/

	SavedCameraTargetArmLength = SpringArmComp->TargetArmLength;
	SavedCameraSocketOffset = SpringArmComp->SocketOffset;

	LandedDelegate.AddDynamic(this, &AHPlayerCharacter::OnLandedEvent);

	OnReachedJumpApex.AddDynamic(this, &AHPlayerCharacter::OnJumpApexEvent);
}

void AHPlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

FVector AHPlayerCharacter::GetPawnViewLocation() const
{
	return CameraComp->GetComponentLocation();
}

// Called every frame
void AHPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//ProcessAiming();
}

UHCharacterMovementComponent* AHPlayerCharacter::GetHMovementComponent() const
{
	return HMovementComp;
}

#pragma region PlayerControllerFunctions

void AHPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	HandleControllerChanged();

}

void AHPlayerCharacter::UnPossessed()
{
	Super::UnPossessed();

	HandleControllerChanged();
}

void AHPlayerCharacter::OnRep_Controller()
{
	Super::OnRep_Controller();

	HandleControllerChanged();
}

void AHPlayerCharacter::HandleControllerChanged()
{
	AHPlayerState* PS = GetPlayerState<AHPlayerState>();

	if (PS) //This is if a playerstate is added or replicated
	{
		AbilitySystemComponentRef = Cast<UHAbilitySystemComponent>(PS->GetAbilitySystemComponent());

		AttributeSetBaseRef = PS->GetAttributeSetBase();

		WeaponComponentRef = PS->GetWeaponComponent();

		EquipmentComponentRef = PS->GetEquipmentComponent();
		InventoryComponentRef = PS->GetInventoryComponent();
		ItemSlotComponentRef = PS->GetItemSlotComponent();


		//todo bind to input

		AbilitySystemComponentRef->InitAbilityActorInfo(PS, this);

		InitializeASC();

		AHPlayerController* PC = Cast<AHPlayerController>(GetController());

		if (PC)
		{
			//PC->CreateHUD();

			// Add HUD Layout widget to the player's Game UI Layer
			if (PC->IsLocalPlayerController())
			{
				UE_LOG(LogTemp, Warning, TEXT("Pushing Game HUD [%s] to UI"), *GetNameSafe(HUDLayoutClass));
				HUDLayoutWidget = UCommonUIExtensions::PushContentToLayer_ForPlayer(PC->GetLocalPlayer(), H_CommonUI_Tags::TAG_UI_LAYER_GAME, HUDLayoutClass);
			}

			PC->RegisterASCRef(AbilitySystemComponentRef.Get());

			SetupPlayerInputComponent(PC->InputComponent);
		}

		if (IsLocallyControlled())
		{
			if (CameraComp)
			{
				CameraComp->DetermineCameraModeDelegate.BindUObject(this, &ThisClass::DetermineCameraMode);
			}
		}
		
	}
	else //This is on unpossessed
	{
		UE_LOG(LogHGame, Warning, TEXT("This should only occur when unpossessing. Unitting ASC and etc"));

		UninitializeAbilitySystem();

		// Remove any HUD we added to the player's UI
		if (HUDLayoutWidget.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("Cleaning up HUD Layout Widget"))
			UCommonUIExtensions::PopContentFromLayer(HUDLayoutWidget.Get());
			HUDLayoutWidget.Reset();
		}

		//// Remove any EnhancedInput IMCs
		//if (UEnhancedInputLocalPlayerSubsystem* Subsystem = getsubs)
		//{
		//	XCGS_LOG(TEXT("Clearing input mappings"));
		//	Subsystem->ClearAllMappings();
		//}
	}
}

#pragma endregion

#pragma region Jumping

void AHPlayerCharacter::ResetJumpState()
{
	bPressedJump = false;
	bWasJumping = false;
	JumpKeyHoldTime = 0.0f;
	JumpForceTimeRemaining = 0.0f;

	if (HMovementComp && !HMovementComp->IsFalling())
	{
		JumpCurrentCount = 0;
		JumpCurrentCountPreJump = 0;
		bJumpAscending = false;
	}
}

void AHPlayerCharacter::OnJumped_Implementation()
{
	bJumpAscending = true;
	UE_LOG(LogTemp, Warning, TEXT("We jumped"));
}

//void AHPlayerCharacter::SetPawnData(const UHPawnData* InPawnData)
//{
//	check(InPawnData);
//
//	if (GetLocalRole() != ROLE_Authority)
//	{
//		return;
//	}
//
//	if (PawnData)
//	{
//		UE_LOG(LogHGame, Error, TEXT("Trying to set PawnData [%s] on pawn [%s] that already has valid PawnData [%s]."), *GetNameSafe(InPawnData), *GetNameSafe(this), *GetNameSafe(PawnData));
//		return;
//	}
//
//	PawnData = InPawnData;
//
//	ForceNetUpdate();
//}

void AHPlayerCharacter::ToggleCouch()
{
	UE_LOG(LogHGame, Warning, TEXT("Should toggle crouch. Not yet implemented"));
}

void AHPlayerCharacter::OnJumpApexEvent()
{
	bJumpAscending = false;
}

void AHPlayerCharacter::OnLandedEvent(const FHitResult& Hit)
{
	bJumpAscending = false;
	HMovementComp->bOrientRotationToMovement = true;
}

#pragma endregion

#pragma region ShootingAndAiming

bool AHPlayerCharacter::CanShoot()
{
	return EquippedWeapon && !IsShootingCooldown;
}

bool AHPlayerCharacter::AttemptEquipInteractWeapon(AActor* FocusedGun)
{
	if(!WeaponEquipped)
	{
		EquippedWeapon = Cast<AHInteractableGunPickup>(FocusedGun);

		if (EquippedWeapon)
		{
			FocusedGun->SetActorEnableCollision(false);
			FocusedGun->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName(TEXT("RifleSocket")));
			WeaponEquipped = true;
			
			return true;
		}
		
	}

	return false;
}

void AHPlayerCharacter::StartAiming()
{
	//Here we can implement 
	HMovementComp->StartAimDownSights();

	SpringArmComp->TargetArmLength = ADSCameraArmLength;
	SpringArmComp->SocketOffset = ADSCameraSocketOffset;

}

void AHPlayerCharacter::StopAiming()
{
	HMovementComp->StopAimDownSights();

	SpringArmComp->TargetArmLength = SavedCameraTargetArmLength;
	SpringArmComp->SocketOffset = SavedCameraSocketOffset;
}

void AHPlayerCharacter::OnShootCooldownReached()
{
	IsShootingCooldown = false;
	if (WantsToShoot && CanShoot())
	{
		AttemptToShoot();
	}
}

bool AHPlayerCharacter::AttemptToShoot()
{
	//Should implement weapon shoot interface for equipped weapon
	//For now have simple line trace every shot
	UE_LOG(LogTemp, Warning, TEXT("Shooting"));
	{
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);
		Params.AddIgnoredActor(EquippedWeapon);

		FCollisionObjectQueryParams ObjectQueryParams;
		ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
		ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
		ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);

		FVector EyeLocation;
		FRotator EyeRotation;
		Owner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector End = EyeLocation + (EyeRotation.Vector() * EquippedWeapon->ShootTraceDistance);

		FHitResult HitResult;

		bool bBlockingHit = GetWorld()->LineTraceSingleByObjectType(HitResult, EyeLocation, End, ObjectQueryParams, Params); //Next we need to make shot come from gun barrel

		if(bBlockingHit)
		{
			End = HitResult.ImpactPoint;
		}

		FVector GunBarrelStartLocation = EquippedWeapon->GunMesh->GetSocketLocation(FName(TEXT("GunBarrel")));

		FColor LineColor = bBlockingHit ? FColor::Green : FColor::Red;

		if (bDebugShootDraw) //next we work on ads cam offset
		{
			DrawDebugSphere(GetWorld(), End, 10.f, 32, FColor::Magenta, false, 0.2f);
			DrawDebugLine(GetWorld(), GunBarrelStartLocation, End, LineColor, false, 0.2f);
		}

		if (AActor* HitActor = HitResult.GetActor())
		{
			UE_LOG(LogTemp, Warning, TEXT("HitActor named: %s"), *HitActor->GetName());

			AHCharacterBase* CharBase = Cast<AHCharacterBase>(HitActor);

			if(CharBase)
			{
				UAbilitySystemComponent* TargetASC = CharBase->GetAbilitySystemComponent();

				if(TargetASC)
				{
					ensure(DamageGameplayEffect);

					FGameplayEffectContextHandle DamageContextHandle = AbilitySystemComponentRef->MakeEffectContext();
					DamageContextHandle.AddHitResult(HitResult);

					FGameplayEffectSpecHandle DamageHandle = AbilitySystemComponentRef->MakeOutgoingSpec(DamageGameplayEffect, AttributeSetBaseRef->GetLevel(), DamageContextHandle);
					DamageHandle.Data.Get()->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Damage")), GunDamage);

					if(DamageHandle.IsValid())
					{
						FActiveGameplayEffectHandle AppliedEffect = AbilitySystemComponentRef->ApplyGameplayEffectSpecToTarget(*DamageHandle.Data.Get(), TargetASC);
					}
				}
			}
		}
		
	}

	IsShootingCooldown = true;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_FireRateDelay, TimerDelegate_OnFireRateCooldown, EquippedWeapon->FireRate, false);

	return true;
}

#pragma endregion

#pragma region InputFunctions

// Called to bind functionality to input
void AHPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (!PlayerInputComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player input not present"));
	}

	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	const UHLocalPlayer* LP = Cast<UHLocalPlayer>(PC->GetLocalPlayer());
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	Subsystem->ClearAllMappings();

	//UHInputConfig* Config = PawnData->InputConfig
	if (UHInputConfig* Config = InputConfig)
	{
		for (const FHInputMappingContextAndPriority& Mapping : DefaultInputMappings)
		{
			if (UInputMappingContext* IMC = Mapping.InputMapping)
			{
				if (Mapping.bRegisterWithSettings)
				{
					if (UEnhancedInputUserSettings* Settings = Subsystem->GetUserSettings())
					{
						UE_LOG(LogHGame, Warning, TEXT("Registering settings for input"));
						Settings->RegisterInputMappingContext(IMC);
					}

					FModifyContextOptions Options = {};
					Options.bIgnoreAllPressedKeysUntilRelease = false;
					// Actually add the config to the local player							
					Subsystem->AddMappingContext(IMC, Mapping.Priority, Options);
				}
			}
		}

		// The H Input Component has some additional functions to map Gameplay Tags to an Input Action.
				// If you want this functionality but still want to change your input component class, make it a subclass
				// of the UHInputComponent or modify this component accordingly.
		UHEnhancedInputComponent* HIC = Cast<UHEnhancedInputComponent>(InputComponent);

		if (ensureMsgf(HIC, TEXT("Unexpected Input Component class! The Gameplay Abilities will not be bound to their inputs. Change the input component to UHInputComponent or a subclass of it.")))
		{
			// Add the key mappings that may have been set by the player
			HIC->AddInputMappings(Config, Subsystem);

			// This is where we actually bind and input action to a gameplay tag, which means that Gameplay Ability Blueprints will
			// be triggered directly by these input actions Triggered events. 
			TArray<uint32> BindHandles;
			HIC->BindAbilityActions(Config, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);

			HIC->BindNativeAction(Config, TAG_InputAction_Ability_Interact, ETriggerEvent::Started, this, &ThisClass::Input_InteractHeld, /*bLogIfNotFound=*/ false);
			HIC->BindNativeAction(Config, TAG_InputAction_Ability_Interact, ETriggerEvent::Triggered, this, &ThisClass::Input_InteractTriggered, /*bLogIfNotFound=*/ false);
			HIC->BindNativeAction(Config, TAG_InputAction_Ability_Interact, ETriggerEvent::Completed, this, &ThisClass::Input_InteractReleased, /*bLogIfNotFound=*/ false);

			HIC->BindNativeAction(Config, TAG_InputAction_Movement, ETriggerEvent::Triggered, this, &ThisClass::Input_Movement, /*bLogIfNotFound=*/ false);
			HIC->BindNativeAction(Config, TAG_InputAction_LookMouse, ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse, /*bLogIfNotFound=*/ false);
			HIC->BindNativeAction(Config, TAG_InputAction_LookStick, ETriggerEvent::Triggered, this, &ThisClass::Input_LookStick, /*bLogIfNotFound=*/ false);
			HIC->BindNativeAction(Config, TAG_InputAction_Crouch, ETriggerEvent::Triggered, this, &ThisClass::Input_Crouch, /*bLogIfNotFound=*/ false);
			//HIC->BindNativeAction(Config, TAG_InputAction_Run, ETriggerEvent::Triggered, this, &ThisClass::Input_AutoRun, /*bLogIfNotFound=*/ false);
			//HIC->BindNativeAction(Config, TAG_InputAction_AutoRun, ETriggerEvent::Triggered, this, &ThisClass::Input_AutoRun, /*bLogIfNotFound=*/ false);
		}
	}

	//todo implement this somewhere. Input confirm SHOULD BE DONE
	AbilitySystemComponentRef->GenericConfirmInputID = static_cast<int32>(EHAbilityInputID::Attack1);
	AbilitySystemComponentRef->GenericCancelInputID = static_cast<int32>(EHAbilityInputID::Attack2);
	//AbilitySystemComponentRef->LocalInputConfirm();
}

void AHPlayerCharacter::SendLocalInputToASC(const EHAbilityInputID AbilityInputID, bool bIsPressed)
{
	if (!AbilitySystemComponentRef.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("No valid ASC found in hplayer::sendlocalinput"))
	}

	if(bIsPressed)
	{
		AbilitySystemComponentRef->AbilityLocalInputPressed(static_cast<int32>(AbilityInputID));
	}
	else
	{
		AbilitySystemComponentRef->AbilityLocalInputReleased(static_cast<int32>(AbilityInputID));
	}
	
}

void AHPlayerCharacter::Input_Movement(const FInputActionValue& InputValue)
{
	const FVector2D MovementInputVector = InputValue.Get<FVector2D>();

	FRotator InputRotation = Controller->GetControlRotation();
	InputRotation.Pitch = 0.f;
	InputRotation.Roll = 0.f;

	const FVector ForwardVector = FRotationMatrix(InputRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(ForwardVector, MovementInputVector.Y);


	const FVector RightVector = FRotationMatrix(InputRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(RightVector, MovementInputVector.X);
}

void AHPlayerCharacter::Input_LookMouse(const FInputActionValue& InputActionValue)
{
	const FVector2D LookVectorInput = InputActionValue.Get<FVector2D>();

	AddControllerPitchInput(LookVectorInput.Y);
	//UE_LOG(LogTemp, Warning, TEXT("Y Input: %f"), LookVectorInput.Y);
	AddControllerYawInput(LookVectorInput.X);
	InputLookAxisYaw = LookVectorInput.X;
}

void AHPlayerCharacter::Input_LookStick(const FInputActionValue& InputActionValue)
{
	const FVector2D LookVectorInput = InputActionValue.Get<FVector2D>();

	AddControllerPitchInput(LookVectorInput.Y);
	//UE_LOG(LogTemp, Warning, TEXT("Y Input: %f"), LookVectorInput.Y);
	AddControllerYawInput(LookVectorInput.X);
	InputLookAxisYaw = LookVectorInput.X;
}

void AHPlayerCharacter::Input_SprintStart(const FInputActionValue& InputActionValue)
{
	const bool IsPressed = InputActionValue.Get<bool>();
	//HMovementComp->StartSprinting();
	SendLocalInputToASC(EHAbilityInputID::Dash, true);
}

void AHPlayerCharacter::Input_SprintEnd(const FInputActionValue& InputActionValue)
{
	const bool IsPressed = InputActionValue.Get<bool>();
	//HMovementComp->StopSprinting();
	SendLocalInputToASC(EHAbilityInputID::Dash, false);
}

void AHPlayerCharacter::Input_Crouch(const FInputActionValue& InputActionValue)
{
	ToggleCouch();
}

void AHPlayerCharacter::Input_JumpStart()
{
	Jump();
	bJumpInputHeldRaw = true;

	HMovementComp->bNotifyApex = true;
	HMovementComp->bOrientRotationToMovement = false;
}

void AHPlayerCharacter::Input_JumpReleased()
{
	StopJumping();
	bJumpInputHeldRaw = false;
}

void AHPlayerCharacter::Input_Attack1Start()
{
	WantsToShoot = true;
	if (CanShoot())
	{
		AttemptToShoot();
	}
	SendLocalInputToASC(EHAbilityInputID::Attack1, true);
}

void AHPlayerCharacter::Input_Attack1Released()
{
	WantsToShoot = false;
	SendLocalInputToASC(EHAbilityInputID::Attack1, false);
}

void AHPlayerCharacter::Input_Attack2Start()
{
	WantsToAim = true;
	SendLocalInputToASC(EHAbilityInputID::Attack2, true);
}

void AHPlayerCharacter::Input_Attack2Released()
{
	WantsToAim = false;
	//StopAiming();
	SendLocalInputToASC(EHAbilityInputID::Attack2, false);
}

void AHPlayerCharacter::Input_InteractTriggered()
{
	if (!HasInteracted)
	{
		HasInteracted = InteractionComp->PrimaryInteract();
	}
}

void AHPlayerCharacter::Input_InteractHeld()
{
	UE_LOG(LogTemp, Error, TEXT("Trying to interact"))
	IsInteractHeld = true;
}

void AHPlayerCharacter::Input_InteractReleased()
{
	IsInteractHeld = false;
	HasInteracted = false;
}

void AHPlayerCharacter::Input_Ability1Start()
{
	SendLocalInputToASC(EHAbilityInputID::Ability1, true);
}

void AHPlayerCharacter::Input_Ability1Released()
{
	SendLocalInputToASC(EHAbilityInputID::Ability1, false);
}

void AHPlayerCharacter::Input_Ability2Start()
{
	SendLocalInputToASC(EHAbilityInputID::Ability2, true);
}

void AHPlayerCharacter::Input_Ability2Released()
{
	SendLocalInputToASC(EHAbilityInputID::Ability2, false);
}

void AHPlayerCharacter::Input_Ability3Start()
{
	SendLocalInputToASC(EHAbilityInputID::Ability3, true);
}

void AHPlayerCharacter::Input_Ability3Released()
{
	SendLocalInputToASC(EHAbilityInputID::Ability3, false);
}

void AHPlayerCharacter::Input_Ability4Start()
{
	SendLocalInputToASC(EHAbilityInputID::Ability4, true);
}

void AHPlayerCharacter::Input_Ability4Released()
{
	SendLocalInputToASC(EHAbilityInputID::Ability4, false);
}

void AHPlayerCharacter::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (UHAbilitySystemComponent* HASC = GetHAbilitySystemComp())
	{
		HASC->AbilityInputTagPressed(InputTag);
	}
}

void AHPlayerCharacter::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (UHAbilitySystemComponent* HASC = GetHAbilitySystemComp())
	{
		HASC->AbilityInputTagReleased(InputTag);
	}
}

#pragma endregion

TSubclassOf<UHCameraMode> AHPlayerCharacter::DetermineCameraMode() const
{
	if (AbilityCameraMode)
	{
		return AbilityCameraMode;
	}

	return DefaultCameraMode;
}

void AHPlayerCharacter::SetAbilityCameraMode(TSubclassOf<UHCameraMode> CameraMode, const FGameplayAbilitySpecHandle& OwningSpecHandle)
{
	if (CameraMode)
	{
		AbilityCameraMode = CameraMode;
		AbilityCameraModeOwningSpecHandle = OwningSpecHandle;
	}
}

void AHPlayerCharacter::ClearAbilityCameraMode(const FGameplayAbilitySpecHandle& OwningSpecHandle)
{
	if (AbilityCameraModeOwningSpecHandle == OwningSpecHandle)
	{
		AbilityCameraMode = nullptr;
		AbilityCameraModeOwningSpecHandle = FGameplayAbilitySpecHandle();
	}
}

