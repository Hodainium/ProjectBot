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
#include "HInventoryComponent.h"
#include "HItemSlotComponent.h"
#include "HVerbMessage.h"
#include "Blueprint/UserWidget.h"
#include "HWorldUserWidget.h"
#include "Components/CapsuleComponent.h"
#include "HWeaponComponent.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameFramework/PlayerState.h"
#include "HereWeGo/Tags/H_Tags.h"
#include "Kismet/GameplayStatics.h"

AHCharacterBase::AHCharacterBase(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UHCharacterMovementComponent>(
		ACharacter::CharacterMovementComponentName))
{
	AbilitySystemComponent = CreateDefaultSubobject<UHAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSetBase = CreateDefaultSubobject<UHAttributeSetBase>(TEXT("AttributeSetBase"));

	WeaponComponent = CreateDefaultSubobject<UHWeaponComponent>(TEXT("WeaponComponent"));
	WeaponComponent->RegisterWithAbilitySystem(AbilitySystemComponent);

	InventoryComponent = CreateDefaultSubobject<UHInventoryComponent>(TEXT("InventoryComponent"));

	EquipmentComponent = CreateDefaultSubobject<UHEquipmentComponent>(TEXT("EquipmentComponent"));

	ItemSlotComponent = CreateDefaultSubobject<UHItemSlotComponent>(TEXT("ItemSlotComponent"));

	DeathTag = FGameplayTag::RequestGameplayTag(FName("GAS.State.Death"));
	RemoveEffectOnDeathTag = FGameplayTag::RequestGameplayTag(FName("GAS.Effect.RemoveOnDeath"));
	AbilityPersistsDeathTag = FGameplayTag::RequestGameplayTag(FName("GAS.Ability.PersistDeath"));
	StunTag = FGameplayTag::RequestGameplayTag(FName("GAS.State.Debuff.Stun"));

	bReplicateUsingRegisteredSubObjectList = true;
}

// Called when the game starts or when spawned
void AHCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	//todo register stun tag event

	if (AbilitySystemComponent)
	{
		InitializeAbilitySystem();
	}

	OnHealthChangedDelegate = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthAttribute()).AddUObject(this, &AHCharacterBase::HealthChanged);

	AbilitySystemComponent->RegisterGameplayTagEvent(StunTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AHCharacterBase::StunTagChanged);
}

// Called every frame
void AHCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AHCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AHCharacterBase::InitializeAbilitySystem()
{
	InitializeAttributes();
	//AddStartupEffects();
	//AddCharacterAbilities();
	AddStartupAbilitySets();
	SetTagRelationShipMapping();

	OnAbilitySystemInitialized();
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

UHAbilitySystemComponent* AHCharacterBase::GetHAbilitySystemComp() const
{
	return AbilitySystemComponent.Get();
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
		AbilityTypesToIgnore.AddTag(H_GameplayEvent_Tags::TAG_ABILITY_BEHAVIOR_SURVIVESDEATH); 

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

		OnAbilitySystemUninitialized();
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

void AHCharacterBase::OnAbilitySystemInitialized()
{
	//TODO healthcomp
	/*ULyraAbilitySystemComponent* LyraASC = GetLyraAbilitySystemComponent();
	check(LyraASC);

	HealthComponent->InitializeWithAbilitySystem(LyraASC);

	InitializeGameplayTags();*/
}

void AHCharacterBase::OnAbilitySystemUninitialized()
{
	//Todo healthcomp
	//HealthComponent->UninitializeFromAbilitySystem();
}

void AHCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}

	// ASC MixedMode replication requires that the ASC Owner's Owner be the Controller.
	SetOwner(NewController);
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


