// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "HInventoryInterface.h"
#include "HWeaponInterface.h"
#include "ModularCharacter.h"
#include "GameFramework/Character.h"
#include "HereWeGo/HereWeGo.h"
#include "HCharacterBase.generated.h"

struct FGameplayEffectSpec;
struct FOnAttributeChangeData;
class UHAbilitySet;
class UHAbilityTagRelationshipMapping;
class UHInventoryComponent;
class UHWeaponComponent;
class UHWorldUserWidget;
class UHGameplayAbility;
class UHAttributeSetBase;
class UGameplayEffect;
class UHAbilitySystemComponent;

//Basis for which all characters inherit from. Even if nonplayable. Actors like barrels should be seperate class
UCLASS()
class HEREWEGO_API AHCharacterBase : public AModularCharacter, public IAbilitySystemInterface, public IHInventoryInterface, public IHWeaponInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AHCharacterBase(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintCallable, Category = "HGAS|Character")
	UHAbilitySystemComponent* GetHAbilitySystemComp() const;

	//UFUNCTION(BlueprintCallable, Category = "Character|Weapon")

	//Weapon interface/////////////////
	virtual UHWeaponComponent* GetWeaponComponent() const override;

	//////////////////////////////////

	//Inventory interface///////////////

	virtual UHInventoryComponent* GetInventoryComponent() const override;

	virtual UHEquipmentComponent* GetEquipmentComponent() const override;

	virtual UHItemSlotComponent* GetItemSlotComponent() const override;

	//////////////////////////////////

	UFUNCTION(BlueprintCallable, Category = "HGAS|Character|Abilities")
	virtual int32 GetAbilityLevel(EHAbilityInputID AbilityID) const;

	//todo add rest of accessors
	UFUNCTION(BlueprintCallable, Category = "HGAS|Character|Attributes")
	int32 GetCharacterLevel() const;

	UFUNCTION(BlueprintCallable, Category = "HGAS|Character|Attributes")
	float GetMoveSpeed() const;

	UFUNCTION(BlueprintCallable, Category = "HGAS|Character|Attributes")
	float GetBaseMoveSpeed() const;

	UFUNCTION(BlueprintCallable, Category = "HGAS|Character|Attributes")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "HGAS|Character|Attributes")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "HGAS|Character|Attributes")
	float GetStamina() const;

	UFUNCTION(BlueprintCallable, Category = "HGAS|Character|Attributes")
	float GetMaxStamina() const;

	UFUNCTION(BlueprintCallable, Category = "HGAS|Character|Attributes")
	float GetMana() const;

	UFUNCTION(BlueprintCallable, Category = "HGAS|Character|Attributes")
	float GetMaxMana() const;

	virtual void RemoveCharacterAbilities();

	virtual void HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);

	UFUNCTION()
	virtual void DeathStarted();

	UFUNCTION()
	virtual void DeathFinished();

	virtual bool IsAlive() const;

	/*UPROPERTY(BlueprintAssignable, Category = "HGAS|Character")
	FOnCharacterDeathDelegate OnCharacterDeath;

	UPROPERTY(BlueprintAssignable, Category = "HGAS|Character")
	FOnCharacterBaseHitReactDelegate OnCharacterBaseHit;*/

	// What mapping of ability tags to use for actions taking by this pawn
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HGAS|Abilities")
	TObjectPtr<UHAbilityTagRelationshipMapping> TagRelationshipMapping;

	

	virtual void PossessedBy(AController* NewController) override;

protected:

	/** Should be called by the owning pawn to Set up sets I guess? */
	virtual void InitializeAbilitySystem();

	/** Should be called by the owning pawn to remove itself as the avatar of the ability system. */
	void UninitializeAbilitySystem();

	virtual void InitializeAttributes();

	virtual void AddStartupEffects();

	virtual void AddCharacterAbilities();

	virtual void AddStartupAbilitySets();

	virtual void SetTagRelationShipMapping();

	virtual void HealthChanged(const FOnAttributeChangeData& Data);

	virtual void StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	TObjectPtr<UHAbilitySystemComponent> AbilitySystemComponent;
	TObjectPtr<UHAttributeSetBase> AttributeSetBase;

	TObjectPtr<UHWeaponComponent> WeaponComponent;

	TObjectPtr<UHInventoryComponent> InventoryComponent;

	TObjectPtr<UHEquipmentComponent> EquipmentComponent;

	TObjectPtr<UHItemSlotComponent> ItemSlotComponent;

	FDelegateHandle OnHealthChangedDelegate;

	UPROPERTY(EditDefaultsOnly, Category = "HCharacterBase|UI")
	TSubclassOf<UHWorldUserWidget> HealthBarWidgetClass;

	UPROPERTY()
	TObjectPtr<UHWorldUserWidget> HealthBarWidgetInstance;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "HGAS|Attributes")
	TSubclassOf<UGameplayEffect> DefaultAttributes;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "HGAS|Abilities")
	TArray<TSubclassOf<UHGameplayAbility>> CharacterAbilities;

	// Gameplay ability sets to grant when this is equipped
	UPROPERTY(EditDefaultsOnly, Category = "HGAS|Abilities")
	TArray<TObjectPtr<const UHAbilitySet>> StartupAbilitySetsToGrant;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "HGAS|Abilities")
	TArray<TSubclassOf<UGameplayEffect>> StartupEffects;

	void DisableMovementAndCapsuleCollision();

	// Begins the death sequence for the character (disables collision, disables movement, etc...)
	UFUNCTION()
	virtual void OnDeathStarted(AActor* OwningActor);

	// Ends the death sequence for the character (detaches controller, destroys pawn, etc...)
	UFUNCTION()
	virtual void OnDeathFinished(AActor* OwningActor);

	void DisableMovementAndCollision();
	void DestroyDueToDeath();
	void UninitAndDestroy();

	// Called when the death sequence for the character has completed
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnDeathFinished"))
	void K2_OnDeathFinished();

	FGameplayTag DeathTag;
	FGameplayTag RemoveEffectOnDeathTag;
	FGameplayTag AbilityPersistsDeathTag;
	FGameplayTag StunTag;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HGAS|Animation")
	UAnimMontage* DeathMontage;

	//TODO Need to implement

	///** Delegate fired when our pawn becomes the ability system's avatar actor */
	//FSimpleMulticastDelegate OnAbilitySystemInitialized;

	///** Delegate fired when our pawn is removed as the ability system's avatar actor */
	//FSimpleMulticastDelegate OnAbilitySystemUninitialized;

	virtual void OnAbilitySystemInitialized();

	virtual void OnAbilitySystemUninitialized();

};
