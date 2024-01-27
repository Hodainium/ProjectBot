// Fill out your copyright notice in the Description page of Project Settings.


#include "HCharacterBase.h"
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
#include "HAbilitySet.h"
#include "HAbilitySystemComponent.h"
#include "HAttributeSetBase.h"
#include "HCharacterMovementComponent.h"
#include "HEquipmentComponent.h"
#include "HGameplayAbility.h"
#include "HHealthComponent.h"
#include "HInventoryComponent.h"
#include "HItemSlotComponent.h"
#include "HPlayerController.h"
#include "HPlayerState.h"
#include "HVerbMessage.h"
#include "Blueprint/UserWidget.h"
#include "HWorldUserWidget.h"
#include "Components/CapsuleComponent.h"
#include "HWeaponComponent.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameFramework/PlayerState.h"
#include "HereWeGo/AttributeSets/HHealthSet.h"
#include "HereWeGo/Tags/H_Tags.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

FSharedRepMovement::FSharedRepMovement()
{
	RepMovement.LocationQuantizationLevel = EVectorQuantization::RoundTwoDecimals;
}

bool FSharedRepMovement::FillForCharacter(ACharacter* Character)
{
	if (USceneComponent* PawnRootComponent = Character->GetRootComponent())
	{
		UCharacterMovementComponent* CharacterMovement = Character->GetCharacterMovement();

		RepMovement.Location = FRepMovement::RebaseOntoZeroOrigin(PawnRootComponent->GetComponentLocation(), Character);
		RepMovement.Rotation = PawnRootComponent->GetComponentRotation();
		RepMovement.LinearVelocity = CharacterMovement->Velocity;
		RepMovementMode = CharacterMovement->PackNetworkMovementMode();
		bProxyIsJumpForceApplied = Character->bProxyIsJumpForceApplied || (Character->JumpForceTimeRemaining > 0.0f);
		bIsCrouched = Character->bIsCrouched;

		// Timestamp is sent as zero if unused
		if ((CharacterMovement->NetworkSmoothingMode == ENetworkSmoothingMode::Linear) || CharacterMovement->bNetworkAlwaysReplicateTransformUpdateTimestamp)
		{
			RepTimeStamp = CharacterMovement->GetServerLastTransformUpdateTimeStamp();
		}
		else
		{
			RepTimeStamp = 0.f;
		}

		return true;
	}
	return false;
}

bool FSharedRepMovement::Equals(const FSharedRepMovement& Other, ACharacter* Character) const
{
	if (RepMovement.Location != Other.RepMovement.Location)
	{
		return false;
	}

	if (RepMovement.Rotation != Other.RepMovement.Rotation)
	{
		return false;
	}

	if (RepMovement.LinearVelocity != Other.RepMovement.LinearVelocity)
	{
		return false;
	}

	if (RepMovementMode != Other.RepMovementMode)
	{
		return false;
	}

	if (bProxyIsJumpForceApplied != Other.bProxyIsJumpForceApplied)
	{
		return false;
	}

	if (bIsCrouched != Other.bIsCrouched)
	{
		return false;
	}

	return true;
}

bool FSharedRepMovement::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	bOutSuccess = true;
	RepMovement.NetSerialize(Ar, Map, bOutSuccess);
	Ar << RepMovementMode;
	Ar << bProxyIsJumpForceApplied;
	Ar << bIsCrouched;

	// Timestamp, if non-zero.
	uint8 bHasTimeStamp = (RepTimeStamp != 0.f);
	Ar.SerializeBits(&bHasTimeStamp, 1);
	if (bHasTimeStamp)
	{
		Ar << RepTimeStamp;
	}
	else
	{
		RepTimeStamp = 0.f;
	}

	return true;
}

AHCharacterBase::AHCharacterBase(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UHCharacterMovementComponent>(
		ACharacter::CharacterMovementComponentName))
{
	AbilitySystemComponent = CreateDefaultSubobject<UHAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSetBase = CreateDefaultSubobject<UHAttributeSetBase>(TEXT("AttributeSetBase"));

	HealthSet = CreateDefaultSubobject<UHHealthSet>(TEXT("HealthSet"));

	WeaponComponent = CreateDefaultSubobject<UHWeaponComponent>(TEXT("WeaponComponent"));
	WeaponComponent->RegisterWithAbilitySystem(AbilitySystemComponent);

	InventoryComponent = CreateDefaultSubobject<UHInventoryComponent>(TEXT("InventoryComponent"));

	EquipmentComponent = CreateDefaultSubobject<UHEquipmentComponent>(TEXT("EquipmentComponent"));

	ItemSlotComponent = CreateDefaultSubobject<UHItemSlotComponent>(TEXT("ItemSlotComponent"));

	HealthComponent = CreateDefaultSubobject<UHHealthComponent>(TEXT("HealthComponent"));
	HealthComponent->OnDeathStarted.AddDynamic(this, &ThisClass::OnDeathStarted);
	HealthComponent->OnDeathFinished.AddDynamic(this, &ThisClass::OnDeathFinished);

	DeathTag = FGameplayTag::RequestGameplayTag(FName("GAS.State.Death"));
	RemoveEffectOnDeathTag = FGameplayTag::RequestGameplayTag(FName("GAS.Effect.RemoveOnDeath"));
	AbilityPersistsDeathTag = FGameplayTag::RequestGameplayTag(FName("GAS.Ability.PersistDeath"));
	StunTag = FGameplayTag::RequestGameplayTag(FName("GAS.State.Debuff.Stun"));

	bReplicateUsingRegisteredSubObjectList = true;
}

AHPlayerController* AHCharacterBase::GetHPlayerController() const
{
	return CastChecked<AHPlayerController>(Controller, ECastCheckedType::NullAllowed);
}

AHPlayerState* AHCharacterBase::GetHPlayerState() const
{
	return CastChecked<AHPlayerState>(GetPlayerState(), ECastCheckedType::NullAllowed);
}

// Called when the game starts or when spawned
void AHCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	//todo register stun tag event

	UE_LOGFMT(LogHGame, Warning, "Begin play called onbase on actor {actor} at time {time}", GetName(), GetWorld()->TimeSeconds);

	if (AbilitySystemComponent)
	{
		InitializeAbilitySystem();
	}

	AbilitySystemComponent->RegisterGameplayTagEvent(StunTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AHCharacterBase::StunTagChanged);
}

void AHCharacterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AHCharacterBase::Reset()
{
	DisableMovementAndCollision();

	K2_OnReset();

	UninitAndDestroy();
}

void AHCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME_CONDITION(ThisClass, ReplicatedAcceleration, COND_SimulatedOnly);
	//DOREPLIFETIME(ThisClass, MyTeamID)
}

void AHCharacterBase::PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker)
{
	Super::PreReplication(ChangedPropertyTracker);

	

	if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
	{
		// Compress Acceleration: XY components as direction + magnitude, Z component as direct value
		const double MaxAccel = MovementComponent->MaxAcceleration;
		const FVector CurrentAccel = MovementComponent->GetCurrentAcceleration();
		double AccelXYRadians, AccelXYMagnitude;
		FMath::CartesianToPolar(CurrentAccel.X, CurrentAccel.Y, AccelXYMagnitude, AccelXYRadians);

		ReplicatedAcceleration.AccelXYRadians = FMath::FloorToInt((AccelXYRadians / TWO_PI) * 255.0);     // [0, 2PI] -> [0, 255]
		ReplicatedAcceleration.AccelXYMagnitude = FMath::FloorToInt((AccelXYMagnitude / MaxAccel) * 255.0);	// [0, MaxAccel] -> [0, 255]
		ReplicatedAcceleration.AccelZ = FMath::FloorToInt((CurrentAccel.Z / MaxAccel) * 127.0);   // [-MaxAccel, MaxAccel] -> [-127, 127]
	}
}

void AHCharacterBase::NotifyControllerChanged()
{
	const FGenericTeamId OldTeamId = GetGenericTeamId();

	Super::NotifyControllerChanged();

	// Update our team ID based on the controller
	if (HasAuthority() && (Controller != nullptr))
	{
		if (IHTeamAgentInterface* ControllerWithTeam = Cast<IHTeamAgentInterface>(Controller))
		{
			MyTeamID = ControllerWithTeam->GetGenericTeamId();
			ConditionalBroadcastTeamChanged(this, OldTeamId, MyTeamID);
		}
	}
}

void AHCharacterBase::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	if (GetController() == nullptr)
	{
		if (HasAuthority())
		{
			const FGenericTeamId OldTeamID = MyTeamID;
			MyTeamID = NewTeamID;
			ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
		}
		else
		{
			UE_LOG(LogHTeams, Error, TEXT("You can't set the team ID on a character (%s) except on the authority"), *GetPathNameSafe(this));
		}
	}
	else
	{
		UE_LOG(LogHTeams, Error, TEXT("You can't set the team ID on a possessed character (%s); it's driven by the associated controller"), *GetPathNameSafe(this));
	}
}

FGenericTeamId AHCharacterBase::GetGenericTeamId() const
{
	return MyTeamID;
}

FHOnTeamIndexChangedDelegate* AHCharacterBase::GetOnTeamIndexChangedDelegate()
{
	return &OnTeamChangedDelegate;
}

void AHCharacterBase::FastSharedReplication_Implementation(const FSharedRepMovement& SharedRepMovement)
{
	if (GetWorld()->IsPlayingReplay())
	{
		return;
	}

	// Timestamp is checked to reject old moves.
	if (GetLocalRole() == ROLE_SimulatedProxy)
	{
		// Timestamp
		ReplicatedServerLastTransformUpdateTimeStamp = SharedRepMovement.RepTimeStamp;

		// Movement mode
		if (ReplicatedMovementMode != SharedRepMovement.RepMovementMode)
		{
			ReplicatedMovementMode = SharedRepMovement.RepMovementMode;
			GetCharacterMovement()->bNetworkMovementModeChanged = true;
			GetCharacterMovement()->bNetworkUpdateReceived = true;
		}

		// Location, Rotation, Velocity, etc.
		FRepMovement& MutableRepMovement = GetReplicatedMovement_Mutable();
		MutableRepMovement = SharedRepMovement.RepMovement;

		// This also sets LastRepMovement
		OnRep_ReplicatedMovement();

		// Jump force
		bProxyIsJumpForceApplied = SharedRepMovement.bProxyIsJumpForceApplied;

		// Crouch
		if (bIsCrouched != SharedRepMovement.bIsCrouched)
		{
			bIsCrouched = SharedRepMovement.bIsCrouched;
			OnRep_IsCrouched();
		}
	}
}

void AHCharacterBase::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	if (const UHAbilitySystemComponent* HASC = GetHAbilitySystemComponent())
	{
		HASC->GetOwnedGameplayTags(TagContainer);
	}
}

bool AHCharacterBase::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	if (const UHAbilitySystemComponent* HASC = GetHAbilitySystemComponent())
	{
		return HASC->HasMatchingGameplayTag(TagToCheck);
	}

	return false;
}

bool AHCharacterBase::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const UHAbilitySystemComponent* HASC = GetHAbilitySystemComponent())
	{
		return HASC->HasAllMatchingGameplayTags(TagContainer);
	}

	return false;
}

bool AHCharacterBase::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const UHAbilitySystemComponent* HASC = GetHAbilitySystemComponent())
	{
		return HASC->HasAnyMatchingGameplayTags(TagContainer);
	}

	return false;
}

void AHCharacterBase::ToggleCrouch()
{
	const UHCharacterMovementComponent* HMoveComp = CastChecked<UHCharacterMovementComponent>(GetCharacterMovement());

	if (bIsCrouched || HMoveComp->bWantsToCrouch)
	{
		UnCrouch();
	}
	else if (HMoveComp->IsMovingOnGround())
	{
		Crouch();
	}
}

void AHCharacterBase::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

// Called to bind functionality to input
void AHCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AHCharacterBase::InitializeGameplayTags()
{
	// Clear tags that may be lingering on the ability system from the previous pawn.
	if (UHAbilitySystemComponent* HASC = GetHAbilitySystemComponent())
	{
		for (const TPair<uint8, FGameplayTag>& TagMapping : H_MovementMode_Tags::MovementModeTagMap)
		{
			if (TagMapping.Value.IsValid())
			{
				HASC->SetLooseGameplayTagCount(TagMapping.Value, 0);
			}
		}

		for (const TPair<uint8, FGameplayTag>& TagMapping : H_MovementMode_Tags::CustomMovementModeTagMap)
		{
			if (TagMapping.Value.IsValid())
			{
				HASC->SetLooseGameplayTagCount(TagMapping.Value, 0);
			}
		}

		UHCharacterMovementComponent* HMoveComp = CastChecked<UHCharacterMovementComponent>(GetCharacterMovement());
		SetMovementModeTag(HMoveComp->MovementMode, HMoveComp->CustomMovementMode, true);
	}
}

void AHCharacterBase::FellOutOfWorld(const UDamageType& dmgType)
{
	HealthComponent->DamageSelfDestruct(/*bFellOutOfWorld=*/ true);
}

void AHCharacterBase::InitializeAbilitySystem()
{
	InitializeAttributes();
	//AddStartupEffects();
	//AddCharacterAbilities();
	AddStartupAbilitySets();
	SetTagRelationShipMapping();

	//TODO healthcomp
	UHAbilitySystemComponent* HASC = GetHAbilitySystemComponent();
	check(HASC);

	HealthComponent->InitializeWithAbilitySystem(HASC);

	InitializeGameplayTags();
}

void AHCharacterBase::InitializeAttributes()
{
	if (!AbilitySystemComponent)
	{
		return;
	}

	if (!DefaultAttributes)
	{
		UE_LOG(LogTemp, Error, TEXT("Actor: %s does not have default attributes set! :("), *this->GetActorNameOrLabel());
	}

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributes, AttributeSetBase->GetLevel(), EffectContext);

	if (NewHandle.IsValid())
	{
		FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent.Get());
		AbilitySystemComponent->bAttributesInitialized = true;
	}
}

void AHCharacterBase::AddStartupEffects()
{
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent || AbilitySystemComponent->bStartUpEffectsApplied)
	{
		return;
	}

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	for (TSubclassOf<UGameplayEffect> GameplayEffect : StartupEffects)
	{
		FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffect, GetCharacterLevel(), EffectContext);
		if (NewHandle.IsValid())
		{
			FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent.Get());
		}
	}

	AbilitySystemComponent->bStartUpEffectsApplied = true;
}

void AHCharacterBase::AddCharacterAbilities()
{

	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent || AbilitySystemComponent->bCharacterAbilitiesGranted)
	{
		return;
	}

	for (TSubclassOf<UHGameplayAbility>& StartupAbility : CharacterAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(StartupAbility);
		AbilitySpec.SourceObject = this;
		AbilitySystemComponent->GiveAbility(AbilitySpec);

		////StartupAbility, GetAbilityLevel(StartupAbility.GetDefaultObject()->AbilityID), static_cast<int32>(StartupAbility.GetDefaultObject()->AbilityInputID
	}

	AbilitySystemComponent->bCharacterAbilitiesGranted = true;
}

void AHCharacterBase::AddStartupAbilitySets()
{
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent) // || AbilitySystemComponent->bStartUpEffectsApplied
	{
		return;
	}

	for (TObjectPtr<const UHAbilitySet> AbilitySet : StartupAbilitySetsToGrant)
	{
		if (AbilitySet)
		{
			AbilitySet->GiveToAbilitySystem(AbilitySystemComponent.Get(), nullptr);
		}
	}
}

void AHCharacterBase::SetTagRelationShipMapping()
{
	if (!AbilitySystemComponent)
	{
		return;
	}

	AbilitySystemComponent->SetTagRelationshipMapping(TagRelationshipMapping);
}

void AHCharacterBase::HealthChanged(const FOnAttributeChangeData& Data)
{
	float Health = Data.NewValue;

	UE_LOG(LogTemp, Warning, TEXT("Called health changed??"))

		if (!HealthBarWidgetInstance)
		{
			if (HealthBarWidgetClass)
			{
				APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);

				if (PC && PC->IsLocalController())
				{
					//Make sure that we arent showing healthbar to self
					if (PC != GetController())
					{
						HealthBarWidgetInstance = CreateWidget<UHWorldUserWidget>(PC, HealthBarWidgetClass);
						HealthBarWidgetInstance->AttachedActor = this;
						HealthBarWidgetInstance->AddToViewport();
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("The code thinks that we are trying to show healthbar to self is this right? in hcharbase"))
					}
				}
			}
		}
}

void AHCharacterBase::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (NewCount > 0)
	{
		FGameplayTagContainer AbilityTagsToCancel;
		AbilityTagsToCancel.AddTag(FGameplayTag::RequestGameplayTag(FName("GAS.Ability")));

		FGameplayTagContainer AbilityTagsToIgnore;
		AbilityTagsToCancel.AddTag(FGameplayTag::RequestGameplayTag(FName("GAS.Ability.NotCancelledByStun")));

		AbilitySystemComponent->CancelAbilities(&AbilityTagsToCancel, &AbilityTagsToIgnore);
	}
}

UAbilitySystemComponent* AHCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent.Get();
}

bool AHCharacterBase::UpdateSharedReplication()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		FSharedRepMovement SharedMovement;
		if (SharedMovement.FillForCharacter(this))
		{
			// Only call FastSharedReplication if data has changed since the last frame.
			// Skipping this call will cause replication to reuse the same bunch that we previously
			// produced, but not send it to clients that already received. (But a new client who has not received
			// it, will get it this frame)
			if (!SharedMovement.Equals(LastSharedReplication, this))
			{
				LastSharedReplication = SharedMovement;
				ReplicatedMovementMode = SharedMovement.RepMovementMode;

				FastSharedReplication(SharedMovement);
			}
			return true;
		}
	}

	// We cannot fastrep right now. Don't send anything.
	return false;
}

UHAbilitySystemComponent* AHCharacterBase::GetHAbilitySystemComponent() const
{
	return AbilitySystemComponent.Get();
}

void AHCharacterBase::OnControllerChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam)
{
	const FGenericTeamId MyOldTeamID = MyTeamID;
	MyTeamID = IntegerToGenericTeamId(NewTeam);
	ConditionalBroadcastTeamChanged(this, MyOldTeamID, MyTeamID);
}

void AHCharacterBase::OnRep_ReplicatedAcceleration()
{
	if (UHCharacterMovementComponent* HMovementComponent = Cast<UHCharacterMovementComponent>(GetCharacterMovement()))
	{
		// Decompress Acceleration
		const double MaxAccel = HMovementComponent->MaxAcceleration;
		const double AccelXYMagnitude = double(ReplicatedAcceleration.AccelXYMagnitude) * MaxAccel / 255.0; // [0, 255] -> [0, MaxAccel]
		const double AccelXYRadians = double(ReplicatedAcceleration.AccelXYRadians) * TWO_PI / 255.0;     // [0, 255] -> [0, 2PI]

		FVector UnpackedAcceleration(FVector::ZeroVector);
		FMath::PolarToCartesian(AccelXYMagnitude, AccelXYRadians, UnpackedAcceleration.X, UnpackedAcceleration.Y);
		UnpackedAcceleration.Z = double(ReplicatedAcceleration.AccelZ) * MaxAccel / 127.0; // [-127, 127] -> [-MaxAccel, MaxAccel]

		HMovementComponent->SetReplicatedAcceleration(UnpackedAcceleration);
	}
}

void AHCharacterBase::OnRep_MyTeamID(FGenericTeamId OldTeamID)
{
	ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
}

UHWeaponComponent* AHCharacterBase::GetWeaponComponent() const
{
	return WeaponComponent.Get();
}

UHInventoryComponent* AHCharacterBase::GetInventoryComponent() const
{
	return InventoryComponent.Get();
}

UHEquipmentComponent* AHCharacterBase::GetEquipmentComponent() const
{
	return EquipmentComponent.Get();
}

UHItemSlotComponent* AHCharacterBase::GetItemSlotComponent() const
{
	return ItemSlotComponent.Get();
}

int32 AHCharacterBase::GetAbilityLevel(EHAbilityInputID AbilityID) const
{
	return 1;
}

int32 AHCharacterBase::GetCharacterLevel() const
{
	if (!AttributeSetBase)
	{
		return 0.f;
	}

	return static_cast<int32>(AttributeSetBase->GetLevel());
}

float AHCharacterBase::GetMoveSpeed() const
{
	if (!AttributeSetBase)
	{
		return 0.f;
	}

	return AttributeSetBase->GetMoveSpeed();
}

float AHCharacterBase::GetBaseMoveSpeed() const
{
	if (!AttributeSetBase)
	{
		return 0.f;
	}

	return AttributeSetBase->GetMoveSpeedAttribute().GetGameplayAttributeData(AttributeSetBase.Get())->GetBaseValue();
}

float AHCharacterBase::GetHealth() const
{
	if (!AttributeSetBase)
	{
		return 0.f;
	}

	return AttributeSetBase->GetHealth();
}

float AHCharacterBase::GetMaxHealth() const
{
	if (!AttributeSetBase)
	{
		return 0.f;
	}

	return AttributeSetBase->GetMaxHealth();
}

float AHCharacterBase::GetStamina() const
{
	if (!AttributeSetBase)
	{
		return 0.f;
	}

	return AttributeSetBase->GetStamina();
}

float AHCharacterBase::GetMaxStamina() const
{
	if (!AttributeSetBase)
	{
		return 0.f;
	}

	return AttributeSetBase->GetMaxStamina();
}

float AHCharacterBase::GetMana() const
{
	if (!AttributeSetBase)
	{
		return 0.f;
	}

	return AttributeSetBase->GetMana();
}

float AHCharacterBase::GetMaxMana() const
{
	if (!AttributeSetBase)
	{
		return 0.f;
	}

	return AttributeSetBase->GetMaxMana();
}

void AHCharacterBase::RemoveCharacterAbilities()
{
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent || !AbilitySystemComponent->bCharacterAbilitiesGranted)
	{
		return;
	}

	TArray<FGameplayAbilitySpecHandle> AbilitiesToRemove;

	for (const FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities())
	{
		if ((Spec.SourceObject == this) && CharacterAbilities.Contains(Spec.Ability->GetClass()))
		{
			AbilitiesToRemove.Add(Spec.Handle);
		}
	}

	for (int32 i = 0; i < AbilitiesToRemove.Num(); i++)
	{
		AbilitySystemComponent->ClearAbility(AbilitiesToRemove[i]);
	}

	AbilitySystemComponent->bCharacterAbilitiesGranted = false;
}

void AHCharacterBase::HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser,
	const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue)
{
#if WITH_SERVER_CODE
	if (AbilitySystemComponent && DamageEffectSpec)
	{
		// Send the "GameplayEvent.Death" gameplay event through the owner's ability system.  This can be used to trigger a death gameplay ability.
		{
			FGameplayEventData Payload;
			Payload.EventTag = H_GameplayEvent_Tags::TAG_GAMEPLAYEVENT_DEATH;
			Payload.Instigator = DamageInstigator;
			Payload.Target = AbilitySystemComponent->GetAvatarActor();
			Payload.OptionalObject = DamageEffectSpec->Def;
			Payload.ContextHandle = DamageEffectSpec->GetEffectContext();
			Payload.InstigatorTags = *DamageEffectSpec->CapturedSourceTags.GetAggregatedTags();
			Payload.TargetTags = *DamageEffectSpec->CapturedTargetTags.GetAggregatedTags();
			Payload.EventMagnitude = DamageMagnitude;

			FScopedPredictionWindow NewScopedWindow(AbilitySystemComponent, true);
			AbilitySystemComponent->HandleGameplayEvent(Payload.EventTag, &Payload);
		}

		// Send a standardized verb message that other systems can observe
		{
			FHVerbMessage Message;
			Message.Verb = H_Message_Tags::TAG_ELIMINATION_MESSAGE;
			Message.Instigator = DamageInstigator;
			Message.InstigatorTags = *DamageEffectSpec->CapturedSourceTags.GetAggregatedTags();
			Message.Target = UHVerbMessageHelpers::GetPlayerStateFromObject(AbilitySystemComponent->GetAvatarActor());
			Message.TargetTags = *DamageEffectSpec->CapturedTargetTags.GetAggregatedTags();
			//@TODO: Fill out context tags, and any non-ability-system source/instigator tags
			//@TODO: Determine if it's an opposing team kill, self-own, team kill, etc...

			UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
			MessageSystem.BroadcastMessage(Message.Verb, Message);
		}

		//@TODO: assist messages (could compute from damage dealt elsewhere)?
	}

#endif // #if WITH_SERVER_CODE
}

void AHCharacterBase::DeathStarted()
{
	//OnCharacterDeath.Broadcast(this);

	DisableMovementAndCapsuleCollision();

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->CancelAllAbilities();

		FGameplayTagContainer EffectTagsToRemove;
		EffectTagsToRemove.AddTag(RemoveEffectOnDeathTag);
		AbilitySystemComponent->RemoveActiveEffectsWithTags(EffectTagsToRemove);

		AbilitySystemComponent->AddLooseGameplayTag(DeathTag);
	}

	//OnCharacterDeath.Broadcast(this);

	if (DeathMontage)
	{
		PlayAnimMontage(DeathMontage);
	}
	else
	{
		DeathFinished();
	}
}

//We should call this when deathanim is finished and we want to ragdoll. Should be called inside animnotify for now.
void AHCharacterBase::DeathFinished()
{
	//ragdoll
	USkeletalMeshComponent* SkeleMesh = GetMesh();
	SkeleMesh->SetAllBodiesSimulatePhysics(true);
	SkeleMesh->SetCollisionProfileName("Ragdoll");
	SkeleMesh->bBlendPhysics = true;

	DetachFromControllerPendingDestroy();
	SetLifeSpan(10.f);
}

bool AHCharacterBase::IsAlive() const
{
	return AttributeSetBase->GetHealth() > 0.f;
}

void AHCharacterBase::UninitializeAbilitySystem()
{
	if (!AbilitySystemComponent)
	{
		return;
	}

	// Uninitialize the ASC if we're still the avatar actor (otherwise another pawn already did it when they became the avatar actor)
	if (AbilitySystemComponent->GetAvatarActor() == GetOwner())
	{
		FGameplayTagContainer AbilityTypesToIgnore;
		AbilityTypesToIgnore.AddTag(H_Ability_Tags::TAG_ABILITY_BEHAVIOR_SURVIVESDEATH); 

		AbilitySystemComponent->CancelAbilities(nullptr, &AbilityTypesToIgnore);
		AbilitySystemComponent->ClearAbilityInput();
		AbilitySystemComponent->RemoveAllGameplayCues();

		if (AbilitySystemComponent->GetOwnerActor() != nullptr)
		{
			AbilitySystemComponent->SetAvatarActor(nullptr);
		}
		else
		{
			// If the ASC doesn't have a valid owner, we need to clear *all* actor info, not just the avatar pairing
			AbilitySystemComponent->ClearActorInfo();
		}

		HealthComponent->UninitializeFromAbilitySystem();
	}

	AbilitySystemComponent = nullptr;
}

void AHCharacterBase::OnDeathStarted(AActor* OwningActor)
{
	DisableMovementAndCollision();
}

void AHCharacterBase::OnDeathFinished(AActor* OwningActor)
{
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::DestroyDueToDeath);
}

void AHCharacterBase::DisableMovementAndCollision()
{
	if (Controller)
	{
		Controller->SetIgnoreMoveInput(true);
	}

	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore); 

	UHCharacterMovementComponent* HMoveComp = CastChecked<UHCharacterMovementComponent>(GetCharacterMovement());
	HMoveComp->StopMovementImmediately();
	HMoveComp->DisableMovement();
}

void AHCharacterBase::DestroyDueToDeath()
{
	K2_OnDeathFinished();

	UninitAndDestroy();
}

void AHCharacterBase::UninitAndDestroy()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		DetachFromControllerPendingDestroy();
		SetLifeSpan(0.1f);
	}

	UninitializeAbilitySystem();

	SetActorHiddenInGame(true);
}

void AHCharacterBase::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
}

void AHCharacterBase::SetMovementModeTag(EMovementMode MovementMode, uint8 CustomMovementMode, bool bTagEnabled)
{
}

void AHCharacterBase::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	if (UHAbilitySystemComponent* HASC = GetHAbilitySystemComponent())
	{
		HASC->SetLooseGameplayTagCount(H_Status_Tags::TAG_STATUS_CROUCHING, 1);
	}


	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

void AHCharacterBase::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	if (UHAbilitySystemComponent* HASC = GetHAbilitySystemComponent())
	{
		HASC->SetLooseGameplayTagCount(H_Status_Tags::TAG_STATUS_CROUCHING, 0);
	}

	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

bool AHCharacterBase::CanJumpInternal_Implementation() const
{
	// same as ACharacter's implementation but without the crouch check
	return JumpIsAllowedInternal();
}

void AHCharacterBase::PossessedBy(AController* NewController)
{
	const FGenericTeamId OldTeamID = MyTeamID;

	Super::PossessedBy(NewController);

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}

	// ASC MixedMode replication requires that the ASC Owner's Owner be the Controller.
	SetOwner(NewController);

	HandleControllerChanged();

	// Grab the current team ID and listen for future changes
	if (IHTeamAgentInterface* ControllerAsTeamProvider = Cast<IHTeamAgentInterface>(NewController))
	{
		MyTeamID = ControllerAsTeamProvider->GetGenericTeamId();
		ControllerAsTeamProvider->GetTeamChangedDelegateChecked().AddDynamic(this, &ThisClass::OnControllerChangedTeam);
	}
	ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
}

void AHCharacterBase::UnPossessed()
{
	AController* const OldController = Controller;

	// Stop listening for changes from the old controller
	const FGenericTeamId OldTeamID = MyTeamID;
	if (IHTeamAgentInterface* ControllerAsTeamProvider = Cast<IHTeamAgentInterface>(OldController))
	{
		ControllerAsTeamProvider->GetTeamChangedDelegateChecked().RemoveAll(this);
	}

	Super::UnPossessed();

	HandleControllerChanged();

	// Determine what the new team ID should be afterwards
	MyTeamID = DetermineNewTeamAfterPossessionEnds(OldTeamID);
	ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
}

void AHCharacterBase::HandleControllerChanged()
{
	//todo not sure if needed
	/*if (AbilitySystemComponent && (AbilitySystemComponent->GetAvatarActor() == GetPawnChecked<APawn>()))
	{
		ensure(AbilitySystemComponent->AbilityActorInfo->OwnerActor == AbilitySystemComponent->GetOwnerActor());
		if (AbilitySystemComponent->GetOwnerActor() == nullptr)
		{
			UninitializeAbilitySystem();
		}
		else
		{
			AbilitySystemComponent->RefreshAbilityActorInfo();
		}
	}

	CheckDefaultInitialization();*/
}

void AHCharacterBase::OnRep_Controller()
{
	Super::OnRep_Controller();

	HandleControllerChanged();
}

void AHCharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	//Do nothing
}

void AHCharacterBase::DisableMovementAndCapsuleCollision()
{
	if (Controller)
	{
		Controller->SetIgnoreMoveInput(true);
	}

	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);

	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);

	UHCharacterMovementComponent* MoveComp = CastChecked<UHCharacterMovementComponent>(GetCharacterMovement());
	MoveComp->StopMovementImmediately();
	MoveComp->DisableMovement();
}


