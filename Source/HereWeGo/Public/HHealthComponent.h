// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HHealthComponent.generated.h"

struct FHGameplayEffectSpec;
class UHGASComponent;
class UHHealthAttributeSet;
struct FHOnAttributeChangeData;
class UHHealthComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHHealthComp_DeathEvent, AActor*, OwningActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FHHealthComp_AttributeChanged, UHHealthComponent*, HealthComp, float, OldValue, float, NewValue, AActor*, Instigator);

UENUM()
enum EHDeathState
{
	NotDead,
	DeathStarted,
	DeathFinished
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HEREWEGO_API UHHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHHealthComponent();

	UFUNCTION(BlueprintPure, Category = "HGAS|Health")
	static UHHealthComponent* FindHealthComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UHHealthComponent>() : nullptr); }

	/*UFUNCTION(BlueprintCallable, Category="HGAS|Health")
	void RegisterWithGASComp(UHGASComponent* InGASComp);*/

	UFUNCTION(BlueprintCallable, Category = "HGAS|Health")
	void UnregisterWithGASComp();

	UFUNCTION(BlueprintCallable, Category = "HGAS|Health")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "HGAS|Health")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "HGAS|Health")
	float GetHealthNormalized() const;

	UFUNCTION(BlueprintCallable, Category = "HGAS|Health")
	EHDeathState GetDeathState() const { return DeathState;}

	UFUNCTION(BlueprintCallable, Category = "HGAS|Health")
	bool IsDeadOrDying() const { return (DeathState > EHDeathState::NotDead); }

	virtual void StartDeath();

	virtual void FinishDeath();

	virtual void DamageSelfDestruct(bool bFellOutOfWorld = false);

public:

	UPROPERTY(BlueprintAssignable)
	FHHealthComp_AttributeChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FHHealthComp_AttributeChanged OnMaxHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FHHealthComp_DeathEvent OnDeathStarted;

	UPROPERTY(BlueprintAssignable)
	FHHealthComp_DeathEvent OnDeathFinished;

protected:

	virtual void OnUnregister() override;

	void ClearGameplayTags();

	virtual void HandleHealthChanged(const FHOnAttributeChangeData& ChangeData);
	virtual void HandleMaxHealthChanged(const FHOnAttributeChangeData& ChangeData);
	virtual void HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser, const FHGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude);

protected:
	/*UPROPERTY()
	TObjectPtr<UHGASComponent> LinkedGASComp;*/

	/*UPROPERTY()
	TObjectPtr<const UHHealthAttributeSet> HealthSet;*/

	UPROPERTY()
	TEnumAsByte<EHDeathState> DeathState;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
