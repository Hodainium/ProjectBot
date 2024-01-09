// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "HInventoryInterface.h"
#include "HWeaponInterface.h"
#include "ModularPlayerState.h"
#include "HPlayerState.generated.h"

class UAbilitySystemComponent;
class UHAbilitySystemComponent;
class UHWeaponComponent;
class UHAttributeSetBase;
struct FGameplayEffectSpec;

/**
 * 
 */
UCLASS()
class HEREWEGO_API AHPlayerState : public AModularPlayerState, public IAbilitySystemInterface, public IHInventoryInterface, public IHWeaponInterface
{
	GENERATED_BODY()

public:
	AHPlayerState();

	UFUNCTION(BlueprintCallable, Category = "HGAS|PlayerState")
	UHAbilitySystemComponent* GetHAbilitySystemComp() const;

	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	virtual UHWeaponComponent* GetWeaponComponent() const override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual UHAttributeSetBase* GetAttributeSetBase() const;

	virtual UHInventoryComponent* GetInventoryComponent() const override;

	virtual UHEquipmentComponent* GetEquipmentComponent() const override;

	virtual UHItemSlotComponent* GetItemSlotComponent() const override;
	

	UFUNCTION(BlueprintCallable, Category = "HGAS|PlayerState")
	bool IsAlive() const;

	

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UHAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UHWeaponComponent> WeaponComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UHAttributeSetBase> AttributeSetBase;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UHInventoryComponent> InventoryComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UHEquipmentComponent> EquipmentComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UHItemSlotComponent> ItemSlotComponent;

protected:
	
	FDelegateHandle OnOutOfHealthDelegateHandle;

	FGameplayTag DeathTag;

	virtual void HandleOutOfHealth(AActor* InstigatorActor, AActor* CauserActor, const FGameplayEffectSpec& EffectSpec, float EffectMagnitude);

};
