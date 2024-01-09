// Fill out your copyright notice in the Description page of Project Settings.


#include "HHealthComponent.h"

// Sets default values for this component's properties
UHHealthComponent::UHHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	/*LinkedGASComp = nullptr;
	HealthSet = nullptr;*/
	DeathState = EHDeathState::NotDead;
}

//void UHHealthComponent::RegisterWithGASComp(UHGASComponent* InGASComp)
//{
//	AActor* Owner = GetOwner();
//	check(Owner);
//
//	if(LinkedGASComp)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("Trying to register but already linked gas comp in healthcomp"))
//		return;
//	}
//
//	LinkedGASComp = InGASComp;
//
//	if(!LinkedGASComp)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("Failed to register with gascomp in healthcomp"))
//		return;
//	}
//
//	HealthSet = LinkedGASComp->GetSet<UHHealthAttributeSet>();
//	if (!HealthSet)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("Cannot link to healthset from gascomp in healthcomp"))
//		return;
//	}
//
//	LinkedGASComp->GetAttributeValueChangeDelegate(UHHealthAttributeSet::HealthTag).AddUObject(this, &ThisClass::HandleHealthChanged);
//	LinkedGASComp->GetAttributeValueChangeDelegate(UHHealthAttributeSet::HealthMaxTag).AddUObject(this, &ThisClass::HandleMaxHealthChanged);
//	HealthSet->OnOutOfHealth.AddUObject(this, &ThisClass::HandleOutOfHealth);
//
//	LinkedGASComp->SetAttributeBaseValue(UHHealthAttributeSet::HealthTag, HealthSet->GetAttributeCurrentValueByTag(UHHealthAttributeSet::HealthMaxTag));
//
//	ClearGameplayTags();
//
//	float CurrentHealth = HealthSet->GetAttributeCurrentValueByTag(UHHealthAttributeSet::HealthTag);
//
//	OnHealthChanged.Broadcast(this, CurrentHealth, CurrentHealth, nullptr);
//	OnMaxHealthChanged.Broadcast(this, CurrentHealth, CurrentHealth, nullptr);
//	
//}

void UHHealthComponent::UnregisterWithGASComp()
{
	ClearGameplayTags();

	/*if(HealthSet)
	{
		HealthSet->OnOutOfHealth.RemoveAll(this);
	}

	HealthSet = nullptr;*/
	//LinkedGASComp = nullptr;
}

void UHHealthComponent::OnUnregister()
{
	UnregisterWithGASComp();

	Super::OnUnregister();
}

void UHHealthComponent::ClearGameplayTags()
{
	//if(LinkedGASComp)
	//{
	//	//LinkedGASComp->ActiveTagCountMap.UpdateTagCount()

	//	//Remove death related tags here, need to make static inline detah atg somewhere
	//	//For both dying and dead tags needed
	//	//todo
	//}
}

static AActor* GetInstigatorFromAttChangeData(const FHOnAttributeChangeData& ChangeData)
{
	/*if (ChangeData.ModCallbackData != nullptr)
	{
		return ChangeData.ModCallbackData->EffectSpec->EffectContext.SourceGASComp->GetOwningActor();
	}*/

	return nullptr;
}

void UHHealthComponent::HandleHealthChanged(const FHOnAttributeChangeData& ChangeData)
{
	//OnHealthChanged.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue, GetInstigatorFromAttChangeData(ChangeData));
}

void UHHealthComponent::HandleMaxHealthChanged(const FHOnAttributeChangeData& ChangeData)
{
	//OnMaxHealthChanged.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue, GetInstigatorFromAttChangeData(ChangeData));
}

void UHHealthComponent::HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser,
	const FHGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude)
{
	//Does nothing for now
}

float UHHealthComponent::GetHealth() const
{
	//return (HealthSet ? HealthSet->GetAttributeCurrentValueByTag(UHHealthAttributeSet::HealthTag) : 0.f);
	return 0.f;
}

float UHHealthComponent::GetMaxHealth() const
{
	//return (HealthSet ? HealthSet->GetAttributeCurrentValueByTag(UHHealthAttributeSet::HealthMaxTag) : 0.f);
	return 0.f;
}

float UHHealthComponent::GetHealthNormalized() const
{
	/*if(HealthSet)
	{
		const float Health = HealthSet->GetAttributeCurrentValueByTag(UHHealthAttributeSet::HealthTag);
		const float MaxHealth = HealthSet->GetAttributeCurrentValueByTag(UHHealthAttributeSet::HealthMaxTag);

		return ((MaxHealth > 0.f) ? (Health / MaxHealth) : 0.f);
	}*/
	return 0.f;
}

void UHHealthComponent::StartDeath()
{
	if (DeathState != EHDeathState::NotDead)
	{
		return;
	}

	DeathState = EHDeathState::DeathStarted;

	//if(LinkedGASComp)
	//{
	//	//todo add death tags here for dying
	//}

	AActor* Owner = GetOwner();
	check(Owner);

	OnDeathStarted.Broadcast(Owner);
}

void UHHealthComponent::FinishDeath()
{
	if (DeathState != EHDeathState::DeathStarted)
	{
		return;
	}

	DeathState = EHDeathState::DeathFinished;

	//if (LinkedGASComp)
	//{
	//	//todo add death tags here for dead
	//}

	AActor* Owner = GetOwner();
	check(Owner);

	OnDeathFinished.Broadcast(Owner);
}

void UHHealthComponent::DamageSelfDestruct(bool bFellOutOfWorld)
{
	//if((DeathState == EHDeathState::NotDead) && LinkedGASComp)
	//{
	//	//todo need to implement  custom ge inm gamemode for killing player via kill bind
	//}
}


// Called when the game starts




// Called every frame
void UHHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

