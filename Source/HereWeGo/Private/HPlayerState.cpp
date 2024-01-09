// Fill out your copyright notice in the Description page of Project Settings.

#include "HPlayerState.h"
#include "HAbilitySystemComponent.h"
#include "HAttributeSetBase.h"
#include "HEquipmentComponent.h"
#include "HInventoryComponent.h"
#include "HItemSlotComponent.h"
#include "HPlayerCharacter.h"
#include "HWeaponComponent.h"


AHPlayerState::AHPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UHAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSetBase = CreateDefaultSubobject<UHAttributeSetBase>(TEXT("AttributeSetBase"));

	WeaponComponent = CreateDefaultSubobject<UHWeaponComponent>(TEXT("WeaponComponent"));

	WeaponComponent->RegisterWithAbilitySystem(AbilitySystemComponent);

	InventoryComponent = CreateDefaultSubobject<UHInventoryComponent>(TEXT("InventoryComponent"));
	InventoryComponent->SetIsReplicated(true);

	EquipmentComponent = CreateDefaultSubobject<UHEquipmentComponent>(TEXT("EquipmentComponent"));
	InventoryComponent->SetIsReplicated(true);

	ItemSlotComponent = CreateDefaultSubobject<UHItemSlotComponent>(TEXT("ItemSlotComponent"));
	InventoryComponent->SetIsReplicated(true);


	NetUpdateFrequency = 100.f; //todo lower eventually but maybe not lyra uses 100

	DeathTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
}

UHAbilitySystemComponent* AHPlayerState::GetHAbilitySystemComp() const
{
	return AbilitySystemComponent;
}

UHWeaponComponent* AHPlayerState::GetWeaponComponent() const
{
	return WeaponComponent;
}

UAbilitySystemComponent* AHPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UHAttributeSetBase* AHPlayerState::GetAttributeSetBase() const
{
	return AttributeSetBase;
}

UHInventoryComponent* AHPlayerState::GetInventoryComponent() const
{
	return InventoryComponent;
}

UHEquipmentComponent* AHPlayerState::GetEquipmentComponent() const
{
	return EquipmentComponent;
}

UHItemSlotComponent* AHPlayerState::GetItemSlotComponent() const
{
	return ItemSlotComponent;
}

bool AHPlayerState::IsAlive() const
{
	return AttributeSetBase->GetHealth() > 0.f;
}

void AHPlayerState::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		OnOutOfHealthDelegateHandle = AttributeSetBase->OnOutOfHealth.AddUObject(this, &AHPlayerState::HandleOutOfHealth);
	}
}

void AHPlayerState::HandleOutOfHealth(AActor* InstigatorActor, AActor* CauserActor, const FGameplayEffectSpec& EffectSpec,
                                      float EffectMagnitude)
{
	AHPlayerCharacter* PlayerPawn = Cast<AHPlayerCharacter>(GetPawn());

	if(PlayerPawn)
	{
		//First make sure we are actually dead and then make sure we aren't already dead. Tag is added to asc in death function in charbase
		if(!IsAlive() && !AbilitySystemComponent->HasMatchingGameplayTag(DeathTag)) 
		{
			PlayerPawn->DeathStarted();
		}
	}
}
