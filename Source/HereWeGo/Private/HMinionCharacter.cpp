// Fill out your copyright notice in the Description page of Project Settings.


#include "HMinionCharacter.h"

#include "HAbilitySystemComponent.h"
#include "HAttributeSetBase.h"
#include "HEquipmentComponent.h"
#include "HInventoryComponent.h"
#include "HItemSlotComponent.h"
#include "HWeaponComponent.h"
#include "Components/CapsuleComponent.h"

AHMinionCharacter::AHMinionCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	AbilitySystemComponentHardRef = CreateDefaultSubobject<UHAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponentHardRef->SetIsReplicated(true);
	AbilitySystemComponentHardRef->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	AbilitySystemComponentRef = AbilitySystemComponentHardRef;

	AttributeSetBaseHardRef = CreateDefaultSubobject<UHAttributeSetBase>(TEXT("AttributeSetBase"));
	AttributeSetBaseRef = AttributeSetBaseHardRef;

	WeaponComponentHardRef = CreateDefaultSubobject<UHWeaponComponent>(TEXT("WeaponComponent"));
	WeaponComponentRef = WeaponComponentHardRef;
	WeaponComponentHardRef->RegisterWithAbilitySystem(AbilitySystemComponentHardRef);

	InventoryComponentHardRef = CreateDefaultSubobject<UHInventoryComponent>(TEXT("InventoryComponent"));
	InventoryComponentRef = InventoryComponentHardRef;

	EquipmentComponentHardRef = CreateDefaultSubobject<UHEquipmentComponent>(TEXT("EquipmentComponent"));
	EquipmentComponentRef = EquipmentComponentHardRef;

	ItemSlotComponentHardRef = CreateDefaultSubobject<UHItemSlotComponent>(TEXT("ItemSlotComponent"));
	ItemSlotComponentRef = ItemSlotComponentHardRef;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
}

void AHMinionCharacter::BeginPlay()
{
	if(AbilitySystemComponentRef.IsValid())
	{
		AbilitySystemComponentRef->InitAbilityActorInfo(this, this);
		InitializeASC();
	}

	//We need to call this after because it will call healthchanged when initting atts
	Super::BeginPlay();
}

