// Fill out your copyright notice in the Description page of Project Settings.


#include "HCharacterBaseOld.h"

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

AHCharacterBaseOld::AHCharacterBaseOld(const FObjectInitializer& ObjectInitializer) :
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
void AHCharacterBaseOld::BeginPlay()
{
	Super::BeginPlay();

	//todo register stun tag event

	OnHealthChangedDelegate = AbilitySystemComponentRef->GetGameplayAttributeValueChangeDelegate(AttributeSetBaseRef->GetHealthAttribute()).AddUObject(this, &AHCharacterBaseOld::HealthChanged);

	AbilitySystemComponentRef->RegisterGameplayTagEvent(StunTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AHCharacterBaseOld::StunTagChanged);
}

// Called every frame
void AHCharacterBaseOld::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AHCharacterBaseOld::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AHCharacterBaseOld::InitializeASC()
{
	InitializeAttributes();
	//AddStartupEffects();
	//AddCharacterAbilities();
	AddStartupAbilitySets();
	SetTagRelationShipMapping();
}

void AHCharacterBaseOld::InitializeAttributes()
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

void AHCharacterBaseOld::AddStartupEffects()
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

void AHCharacterBaseOld::AddCharacterAbilities()
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

void AHCharacterBaseOld::AddStartupAbilitySets()
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

void AHCharacterBaseOld::SetTagRelationShipMapping()
{
	if (!AbilitySystemComponentRef.IsValid())
	{
		return;
	}

	AbilitySystemComponentRef->SetTagRelationshipMapping(TagRelationshipMapping);
}

void AHCharacterBaseOld::HealthChanged(const FOnAttributeChangeData& Data)
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

void AHCharacterBaseOld::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
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

UAbilitySystemComponent* AHCharacterBaseOld::GetAbilitySystemComponent() const
{
	return AbilitySystemComponentRef.Get();
}

UHAbilitySystemComponent* AHCharacterBaseOld::GetHAbilitySystemComp() const
{
	return AbilitySystemComponentRef.Get();
}

UHWeaponComponent* AHCharacterBaseOld::GetWeaponComponent() const
{
	return WeaponComponentRef.Get();
}

UHInventoryComponent* AHCharacterBaseOld::GetInventoryComponent() const
{
	return InventoryComponentRef.Get();
}

UHEquipmentComponent* AHCharacterBaseOld::GetEquipmentComponent() const
{
	return EquipmentComponentRef.Get();
}

UHItemSlotComponent* AHCharacterBaseOld::GetItemSlotComponent() const
{
	return ItemSlotComponentRef.Get();
}

int32 AHCharacterBaseOld::GetAbilityLevel(EHAbilityInputID AbilityID) const
{
	return 1;
}

int32 AHCharacterBaseOld::GetCharacterLevel() const
{
	if (!AttributeSetBaseRef.IsValid())
	{
		return 0.f;
	}

	return static_cast<int32>(AttributeSetBaseRef->GetLevel());
}

float AHCharacterBaseOld::GetMoveSpeed() const
{
	if(!AttributeSetBaseRef.IsValid())
	{
		return 0.f;
	}

	return AttributeSetBaseRef->GetMoveSpeed();
}

float AHCharacterBaseOld::GetBaseMoveSpeed() const
{
	if (!AttributeSetBaseRef.IsValid())
	{
		return 0.f;
	}

	return AttributeSetBaseRef->GetMoveSpeedAttribute().GetGameplayAttributeData(AttributeSetBaseRef.Get())->GetBaseValue();
}

float AHCharacterBaseOld::GetHealth() const
{
	if (!AttributeSetBaseRef.IsValid())
	{
		return 0.f;
	}

	return AttributeSetBaseRef->GetHealth();
}

float AHCharacterBaseOld::GetMaxHealth() const
{
	if (!AttributeSetBaseRef.IsValid())
	{
		return 0.f;
	}

	return AttributeSetBaseRef->GetMaxHealth();
}

float AHCharacterBaseOld::GetStamina() const
{
	if (!AttributeSetBaseRef.IsValid())
	{
		return 0.f;
	}

	return AttributeSetBaseRef->GetStamina();
}

float AHCharacterBaseOld::GetMaxStamina() const
{
	if (!AttributeSetBaseRef.IsValid())
	{
		return 0.f;
	}

	return AttributeSetBaseRef->GetMaxStamina();
}

float AHCharacterBaseOld::GetMana() const
{
	if (!AttributeSetBaseRef.IsValid())
	{
		return 0.f;
	}

	return AttributeSetBaseRef->GetMana();
}

float AHCharacterBaseOld::GetMaxMana() const
{
	if (!AttributeSetBaseRef.IsValid())
	{
		return 0.f;
	}

	return AttributeSetBaseRef->GetMaxMana();
}

void AHCharacterBaseOld::RemoveCharacterAbilities()
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

void AHCharacterBaseOld::DeathStarted()
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
void AHCharacterBaseOld::DeathFinished()
{
	//ragdoll
	USkeletalMeshComponent* SkeleMesh = GetMesh();
	SkeleMesh->SetAllBodiesSimulatePhysics(true);
	SkeleMesh->SetCollisionProfileName("Ragdoll");
	SkeleMesh->bBlendPhysics = true;

	DetachFromControllerPendingDestroy();
	SetLifeSpan(10.f);
}

bool AHCharacterBaseOld::IsAlive() const
{
	return AttributeSetBaseRef->GetHealth() > 0.f;
}

void AHCharacterBaseOld::UninitializeAbilitySystem()
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

void AHCharacterBaseOld::DisableMovementAndCapsuleCollision()
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


