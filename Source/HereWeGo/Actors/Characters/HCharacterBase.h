// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayCueInterface.h"
#include "GameplayTagAssetInterface.h"
#include "GameplayTagContainer.h"
#include "HInventoryInterface.h"
#include "HWeaponInterface.h"
#include "ModularCharacter.h"
#include "GameFramework/Character.h"
#include "HereWeGo/HereWeGo.h"
#include "HereWeGo/Interfaces/HTeamAgentInterface.h"
#include "HCharacterBase.generated.h"

class AHPlayerState;
class AHPlayerController;
class UHHealthComponent;
class UHHealthSet;
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

/**
 * FHReplicatedAcceleration: Compressed representation of acceleration
 */
USTRUCT()
struct FHReplicatedAcceleration
{
	GENERATED_BODY()

	UPROPERTY()
	uint8 AccelXYRadians = 0;	// Direction of XY accel component, quantized to represent [0, 2*pi]

	UPROPERTY()
	uint8 AccelXYMagnitude = 0;	//Accel rate of XY component, quantized to represent [0, MaxAcceleration]

	UPROPERTY()
	int8 AccelZ = 0;	// Raw Z accel rate component, quantized to represent [-MaxAcceleration, MaxAcceleration]
};

/** The type we use to send FastShared movement updates. */
USTRUCT()
struct FSharedRepMovement
{
	GENERATED_BODY()

	FSharedRepMovement();

	bool FillForCharacter(ACharacter* Character);
	bool Equals(const FSharedRepMovement& Other, ACharacter* Character) const;

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

	UPROPERTY(Transient)
	FRepMovement RepMovement;

	UPROPERTY(Transient)
	float RepTimeStamp = 0.0f;

	UPROPERTY(Transient)
	uint8 RepMovementMode = 0;

	UPROPERTY(Transient)
	bool bProxyIsJumpForceApplied = false;

	UPROPERTY(Transient)
	bool bIsCrouched = false;
};

template<>
struct TStructOpsTypeTraits<FSharedRepMovement> : public TStructOpsTypeTraitsBase2<FSharedRepMovement>
{
	enum
	{
		WithNetSerializer = true,
		WithNetSharedSerialization = true,
	};
};

//Basis for which all characters inherit from. Even if nonplayable. Actors like barrels should be seperate class
UCLASS()
class HEREWEGO_API AHCharacterBase : public AModularCharacter, public IAbilitySystemInterface, public IGameplayCueInterface, public IGameplayTagAssetInterface,
	public IHTeamAgentInterface, public IHInventoryInterface, public IHWeaponInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AHCharacterBase(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "H|Character")
	AHPlayerController* GetHPlayerController() const;

	UFUNCTION(BlueprintCallable, Category = "H|Character")
	AHPlayerState* GetHPlayerState() const;

	UFUNCTION(BlueprintCallable, Category = "HGAS|Character")
	UHAbilitySystemComponent* GetHAbilitySystemComponent() const;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;

	void ToggleCrouch();

	//~AActor interface
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Reset() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker) override;
	virtual void PostInitializeComponents() override;
	//~End of AActor interface

	//~APawn interface
	virtual void NotifyControllerChanged() override;
	//~End of APawn interface

	//~IHTeamAgentInterface interface
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	virtual FHOnTeamIndexChangedDelegate* GetOnTeamIndexChangedDelegate() override;
	//~End of IHTeamAgentInterface interface

	/** RPCs that is called on frames when default property replication is skipped. This replicates a single movement update to everyone. */
	UFUNCTION(NetMulticast, unreliable)
	void FastSharedReplication(const FSharedRepMovement& SharedRepMovement);

	// Last FSharedRepMovement we sent, to avoid sending repeatedly.
	FSharedRepMovement LastSharedReplication;

	virtual bool UpdateSharedReplication();

protected:
	virtual void InitializeAbilitySystem();
	void UninitializeAbilitySystem();

	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;
	virtual void HandleControllerChanged();

	virtual void OnRep_Controller() override;
	virtual void OnRep_PlayerState() override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void InitializeGameplayTags();

	virtual void FellOutOfWorld(const class UDamageType& dmgType) override;

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

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;
	void SetMovementModeTag(EMovementMode MovementMode, uint8 CustomMovementMode, bool bTagEnabled);

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	virtual bool CanJumpInternal_Implementation() const;

private:

	UPROPERTY(Transient, ReplicatedUsing = OnRep_ReplicatedAcceleration)
	FHReplicatedAcceleration ReplicatedAcceleration;

	UPROPERTY(ReplicatedUsing = OnRep_MyTeamID)
	FGenericTeamId MyTeamID;

	UPROPERTY()
	FHOnTeamIndexChangedDelegate OnTeamChangedDelegate;

protected:
	// Called to determine what happens to the team ID when possession ends
	virtual FGenericTeamId DetermineNewTeamAfterPossessionEnds(FGenericTeamId OldTeamID) const
	{
		// This could be changed to return, e.g., OldTeamID if you want to keep it assigned afterwards, or return an ID for some neutral faction, or etc...
		return FGenericTeamId::NoTeam;
	}
private:
	UFUNCTION()
	void OnControllerChangedTeam(UObject* TeamAgent, int32 OldTeam, int32 NewTeam);

	UFUNCTION()
	void OnRep_ReplicatedAcceleration();

	UFUNCTION()
	void OnRep_MyTeamID(FGenericTeamId OldTeamID);
public:

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

	

	

protected:

	virtual void InitializeAttributes();

	virtual void AddStartupEffects();

	virtual void AddCharacterAbilities();

	virtual void AddStartupAbilitySets();

	virtual void SetTagRelationShipMapping();

	virtual void HealthChanged(const FOnAttributeChangeData& Data);

	virtual void StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	TObjectPtr<UHAbilitySystemComponent> AbilitySystemComponent;
	TObjectPtr<UHAttributeSetBase> AttributeSetBase;
	TObjectPtr<UHHealthSet> HealthSet;

	TObjectPtr<UHWeaponComponent> WeaponComponent;

	TObjectPtr<UHInventoryComponent> InventoryComponent;

	TObjectPtr<UHEquipmentComponent> EquipmentComponent;

	TObjectPtr<UHItemSlotComponent> ItemSlotComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "H|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UHHealthComponent> HealthComponent;

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

	

};
