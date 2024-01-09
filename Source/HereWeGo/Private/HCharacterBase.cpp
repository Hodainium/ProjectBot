// Fill out your copyright notice in the Description page of Project Settings.


#include "HCharacterBase.h"

#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
#include "HAbilitySet.h"
#include "HAbilitySystemComponent.h"
#include "HAttributeSetBase.h"
#include "HCharacterMovementComponent.h"
#include "HGameplayAbility.h"
#include "HInventoryComponent.h"
#include "Blueprint/UserWidget.h"
#include "HWorldUserWidget.h"
#include "Components/CapsuleComponent.h"
#include "HWeaponComponent.h"
#include "Kismet/GameplayStatics.h"

AHCharacterBase::AHCharacterBase(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UHCharacterMovementComponent>(
		ACharacter::CharacterMovementComponentName))
{
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

	OnHealthChangedDelegate = AbilitySystemComponentRef->GetGameplayAttributeValueChangeDelegate(AttributeSetBaseRef->GetHealthAttribute()).AddUObject(this, &AHCharacterBase::HealthChanged);

	AbilitySystemComponentRef->RegisterGameplayTagEvent(StunTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AHCharacterBase::StunTagChanged);
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

void AHCharacterBase::InitializeASC()
{
	InitializeAttributes();
	//AddStartupEffects();
	//AddCharacterAbilities();
	AddStartupAbilitySets();
	SetTagRelationShipMapping();
}

void AHCharacterBase::InitializeAttributes()
{
	if (!AbilitySystemComponentRef.IsValid())
	{
		return;
	}

	if (!DefaultAttributes)
	{
		UE_LOG(LogTemp, Error, TEXT("Actor: %s does not have default attributes set! :("), *this->GetActorNameOrLabel());
	}

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponentRef->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	FGameplayEffectSpecHandle NewHandle = AbilitySystemComponentRef->MakeOutgoingSpec(DefaultAttributes, AttributeSetBaseRef->GetLevel(), EffectContext);

	if (NewHandle.IsValid())
	{
		FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponentRef->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponentRef.Get());
		AbilitySystemComponentRef->bAttributesInitialized = true;
	}
}

void AHCharacterBase::AddStartupEffects()
{
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponentRef.IsValid() || AbilitySystemComponentRef->bStartUpEffectsApplied)
	{
		return;
	}

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponentRef->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	for(TSubclassOf<UGameplayEffect> GameplayEffect : StartupEffects)
	{
		FGameplayEffectSpecHandle NewHandle = AbilitySystemComponentRef->MakeOutgoingSpec(GameplayEffect, GetCharacterLevel(), EffectContext);
		if(NewHandle.IsValid())
		{
			FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponentRef->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponentRef.Get());
		}
	}

	AbilitySystemComponentRef->bStartUpEffectsApplied = true;
}

void AHCharacterBase::AddCharacterAbilities()
{

	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponentRef.IsValid() || AbilitySystemComponentRef->bCharacterAbilitiesGranted)
	{
		return;
	}

	for (TSubclassOf<UHGameplayAbility>& StartupAbility : CharacterAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(StartupAbility);
		AbilitySpec.SourceObject = this;
		AbilitySystemComponentRef->GiveAbility(AbilitySpec);

		////StartupAbility, GetAbilityLevel(StartupAbility.GetDefaultObject()->AbilityID), static_cast<int32>(StartupAbility.GetDefaultObject()->AbilityInputID
	}

	AbilitySystemComponentRef->bCharacterAbilitiesGranted = true;
}

void AHCharacterBase::AddStartupAbilitySets()
{
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponentRef.IsValid()) // || AbilitySystemComponentRef->bStartUpEffectsApplied
	{
		return;
	}

	for (TObjectPtr<const UHAbilitySet> AbilitySet : StartupAbilitySetsToGrant)
	{
		if(AbilitySet)
		{
			AbilitySet->GiveToAbilitySystem(AbilitySystemComponentRef.Get(), nullptr);
		}
	}
}

void AHCharacterBase::SetTagRelationShipMapping()
{
	if (!AbilitySystemComponentRef.IsValid())
	{
		return;
	}

	AbilitySystemComponentRef->SetTagRelationshipMapping(TagRelationshipMapping);
}

void AHCharacterBase::HealthChanged(const FOnAttributeChangeData& Data)
{
	float Health = Data.NewValue;

	UE_LOG(LogTemp, Warning, TEXT("Called health changed??"))

	if(!HealthBarWidgetInstance)
	{
		if (HealthBarWidgetClass)
		{
			APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);

			if(PC && PC->IsLocalController())
			{
				//Make sure that we arent showing healthbar to self
				if(PC != GetController())
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

		AbilitySystemComponentRef->CancelAbilities(&AbilityTagsToCancel, &AbilityTagsToIgnore);
	}
}

UAbilitySystemComponent* AHCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponentRef.Get();
}

UHAbilitySystemComponent* AHCharacterBase::GetHAbilitySystemComp() const
{
	return AbilitySystemComponentRef.Get();
}

UHWeaponComponent* AHCharacterBase::GetWeaponComponent() const
{
	return WeaponComponentRef.Get();
}

UHInventoryComponent* AHCharacterBase::GetInventoryComponent() const
{
	return InventoryComponentRef.Get();
}

UHEquipmentComponent* AHCharacterBase::GetEquipmentComponent() const
{
	return EquipmentComponentRef.Get();
}

UHItemSlotComponent* AHCharacterBase::GetItemSlotComponent() const
{
	return ItemSlotComponentRef.Get();
}

int32 AHCharacterBase::GetAbilityLevel(EHAbilityInputID AbilityID) const
{
	return 1;
}

int32 AHCharacterBase::GetCharacterLevel() const
{
	if (!AttributeSetBaseRef.IsValid())
	{
		return 0.f;
	}

	return static_cast<int32>(AttributeSetBaseRef->GetLevel());
}

float AHCharacterBase::GetMoveSpeed() const
{
	if(!AttributeSetBaseRef.IsValid())
	{
		return 0.f;
	}

	return AttributeSetBaseRef->GetMoveSpeed();
}

float AHCharacterBase::GetBaseMoveSpeed() const
{
	if (!AttributeSetBaseRef.IsValid())
	{
		return 0.f;
	}

	return AttributeSetBaseRef->GetMoveSpeedAttribute().GetGameplayAttributeData(AttributeSetBaseRef.Get())->GetBaseValue();
}

float AHCharacterBase::GetHealth() const
{
	if (!AttributeSetBaseRef.IsValid())
	{
		return 0.f;
	}

	return AttributeSetBaseRef->GetHealth();
}

float AHCharacterBase::GetMaxHealth() const
{
	if (!AttributeSetBaseRef.IsValid())
	{
		return 0.f;
	}

	return AttributeSetBaseRef->GetMaxHealth();
}

float AHCharacterBase::GetStamina() const
{
	if (!AttributeSetBaseRef.IsValid())
	{
		return 0.f;
	}

	return AttributeSetBaseRef->GetStamina();
}

float AHCharacterBase::GetMaxStamina() const
{
	if (!AttributeSetBaseRef.IsValid())
	{
		return 0.f;
	}

	return AttributeSetBaseRef->GetMaxStamina();
}

float AHCharacterBase::GetMana() const
{
	if (!AttributeSetBaseRef.IsValid())
	{
		return 0.f;
	}

	return AttributeSetBaseRef->GetMana();
}

float AHCharacterBase::GetMaxMana() const
{
	if (!AttributeSetBaseRef.IsValid())
	{
		return 0.f;
	}

	return AttributeSetBaseRef->GetMaxMana();
}

void AHCharacterBase::RemoveCharacterAbilities()
{
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponentRef.IsValid() || !AbilitySystemComponentRef->bCharacterAbilitiesGranted)
	{
		return;
	}

	TArray<FGameplayAbilitySpecHandle> AbilitiesToRemove;

	for (const FGameplayAbilitySpec& Spec : AbilitySystemComponentRef->GetActivatableAbilities())
	{
		if ((Spec.SourceObject == this) && CharacterAbilities.Contains(Spec.Ability->GetClass()))
		{
			AbilitiesToRemove.Add(Spec.Handle);
		}
	}

	for (int32 i = 0; i < AbilitiesToRemove.Num(); i++)
	{
		AbilitySystemComponentRef->ClearAbility(AbilitiesToRemove[i]);
	}

	AbilitySystemComponentRef->bCharacterAbilitiesGranted = false;
}

void AHCharacterBase::DeathStarted()
{
	OnCharacterDeath.Broadcast(this);

	DisableMovementAndCapsuleCollision();

	if(AbilitySystemComponentRef.IsValid())
	{
		AbilitySystemComponentRef->CancelAllAbilities();

		FGameplayTagContainer EffectTagsToRemove;
		EffectTagsToRemove.AddTag(RemoveEffectOnDeathTag);
		AbilitySystemComponentRef->RemoveActiveEffectsWithTags(EffectTagsToRemove);

		AbilitySystemComponentRef->AddLooseGameplayTag(DeathTag);
	}

	OnCharacterDeath.Broadcast(this);

	if(DeathMontage)
	{
		PlayAnimMontage(DeathMontage);
	}
	else
	{
		DeathFinished();
	}

	//// Send the "GameplayEvent.Death" gameplay event through the owner's ability system.  This can be used to trigger a death gameplay ability.
	//{
	//	FGameplayEventData Payload;
	//	Payload.EventTag = FLyraGameplayTags::Get().GameplayEvent_Death;
	//	Payload.Instigator = DamageInstigator;
	//	Payload.Target = AbilitySystemComponent->GetAvatarActor();
	//	Payload.OptionalObject = DamageEffectSpec.Def;
	//	Payload.ContextHandle = DamageEffectSpec.GetEffectContext();
	//	Payload.InstigatorTags = *DamageEffectSpec.CapturedSourceTags.GetAggregatedTags();
	//	Payload.TargetTags = *DamageEffectSpec.CapturedTargetTags.GetAggregatedTags();
	//	Payload.EventMagnitude = DamageMagnitude;

	//	FScopedPredictionWindow NewScopedWindow(AbilitySystemComponent, true);
	//	AbilitySystemComponent->HandleGameplayEvent(Payload.EventTag, &Payload);
	//}

	//// Send a standardized verb message that other systems can observe
	//{
	//	FLyraVerbMessage Message;
	//	Message.Verb = TAG_Lyra_Elimination_Message;
	//	Message.Instigator = DamageInstigator;
	//	Message.InstigatorTags = *DamageEffectSpec.CapturedSourceTags.GetAggregatedTags();
	//	Message.Target = ULyraVerbMessageHelpers::GetPlayerStateFromObject(AbilitySystemComponent->GetAvatarActor());
	//	Message.TargetTags = *DamageEffectSpec.CapturedTargetTags.GetAggregatedTags();
	//	//@TODO: Fill out context tags, and any non-ability-system source/instigator tags
	//	//@TODO: Determine if it's an opposing team kill, self-own, team kill, etc...

	//	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
	//	MessageSystem.BroadcastMessage(Message.Verb, Message);
	//}
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
	return AttributeSetBaseRef->GetHealth() > 0.f;
}

void AHCharacterBase::UninitializeAbilitySystem()
{
	if (!AbilitySystemComponentRef.IsValid())
	{
		return;
	}

	if (AbilitySystemComponentRef->GetAvatarActor() == GetOwner())
	{
		/*FGameplayTagContainer AbilityTypesToIgnore;
		AbilityTypesToIgnore.AddTag(AbilityPersistsDeathTag);*/
		//AbilitySystemComponentRef->CancelAbilities(nullptr, &AbilityTypesToIgnore);

		AbilitySystemComponentRef->CancelAbilities();
		AbilitySystemComponentRef->RemoveAllGameplayCues();

		if (AbilitySystemComponentRef->GetOwnerActor() != nullptr)
		{
			AbilitySystemComponentRef->SetAvatarActor(nullptr);
		}
		else
		{
			AbilitySystemComponentRef->ClearActorInfo();
		}
	}

	AbilitySystemComponentRef = nullptr;
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


