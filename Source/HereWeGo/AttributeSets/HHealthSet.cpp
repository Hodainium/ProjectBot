// Fill out your copyright notice in the Description page of Project Settings.


#include "HHealthSet.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
#include "HAbilitySystemComponent.h"
#include "HVerbMessage.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "HereWeGo/Tags/H_Tags.h"
#include "Net/UnrealNetwork.h"


UHHealthSet::UHHealthSet()
	: Health(100.0f)
	, MaxHealth(100.0f)
{
	bOutOfHealth = false;
	bOutOfShield = false;
	MaxHealthBeforeAttributeChange = 0.0f;
	HealthBeforeAttributeChange = 0.0f;
}

void UHHealthSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UHHealthSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHHealthSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHHealthSet, Shield, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHHealthSet, MaxShield, COND_None, REPNOTIFY_Always);
}

void UHHealthSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHHealthSet, Health, OldValue);

	// Call the change callback, but without an instigator
	// This could be changed to an explicit RPC in the future
	// These events on the client should not be changing attributes

	const float CurrentHealth = GetHealth();
	const float EstimatedMagnitude = CurrentHealth - OldValue.GetCurrentValue();

	OnHealthChanged.Broadcast(nullptr, nullptr, nullptr, EstimatedMagnitude, OldValue.GetCurrentValue(), CurrentHealth);

	if (!bOutOfHealth && CurrentHealth <= 0.0f)
	{
		OnOutOfHealth.Broadcast(nullptr, nullptr, nullptr, EstimatedMagnitude, OldValue.GetCurrentValue(), CurrentHealth);
	}

	bOutOfHealth = (CurrentHealth <= 0.0f);
}

void UHHealthSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHHealthSet, MaxHealth, OldValue);

	// Call the change callback, but without an instigator
	// This could be changed to an explicit RPC in the future
	OnMaxHealthChanged.Broadcast(nullptr, nullptr, nullptr, GetMaxHealth() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetMaxHealth());
}

void UHHealthSet::OnRep_Shield(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHHealthSet, Shield, OldValue);

	// Call the change callback, but without an instigator
	// This could be changed to an explicit RPC in the future
	// These events on the client should not be changing attributes

	const float CurrentShield = GetShield();
	const float EstimatedMagnitude = CurrentShield - OldValue.GetCurrentValue();

	OnShieldChanged.Broadcast(nullptr, nullptr, nullptr, EstimatedMagnitude, OldValue.GetCurrentValue(), CurrentShield);

	if (!bOutOfShield && CurrentShield <= 0.0f)
	{
		OnOutOfShield.Broadcast(nullptr, nullptr, nullptr, EstimatedMagnitude, OldValue.GetCurrentValue(), CurrentShield);
	}

	bOutOfShield = (CurrentShield <= 0.0f);
}

void UHHealthSet::OnRep_MaxShield(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHHealthSet, MaxShield, OldValue);

	// Call the change callback, but without an instigator
	// This could be changed to an explicit RPC in the future
	OnMaxShieldChanged.Broadcast(nullptr, nullptr, nullptr, GetMaxShield() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetMaxShield());
}

bool UHHealthSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}

	// Handle modifying incoming normal damage
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		if (Data.EvaluatedData.Magnitude > 0.0f)
		{
			const bool bIsDamageFromSelfDestruct = Data.EffectSpec.GetDynamicAssetTags().HasTagExact(H_Damage_Tags::TAG_DAMAGE_SELFDESTRUCT);

			if (Data.Target.HasMatchingGameplayTag(H_Damage_Tags::TAG_DAMAGE_IMMUNITY) && !bIsDamageFromSelfDestruct)
			{
				// Do not take away any health.
				Data.EvaluatedData.Magnitude = 0.0f;
				return false;
			}

#if !UE_BUILD_SHIPPING
			// Check GodMode cheat, unlimited health is checked below
			if (Data.Target.HasMatchingGameplayTag(H_Cheat_Tags::TAG_CHEAT_GODMODE) && !bIsDamageFromSelfDestruct)
			{
				// Do not take away any health.
				Data.EvaluatedData.Magnitude = 0.0f;
				return false;
			}
#endif // #if !UE_BUILD_SHIPPING
		}
	}

	// Save the current health
	HealthBeforeAttributeChange = GetHealth();
	MaxHealthBeforeAttributeChange = GetMaxHealth();
	ShieldBeforeAttributeChange = GetShield();
	MaxShieldBeforeAttributeChange = GetMaxShield();

	return true;
}

void UHHealthSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	const bool bIsDamageFromSelfDestruct = Data.EffectSpec.GetDynamicAssetTags().HasTagExact(H_Damage_Tags::TAG_DAMAGE_SELFDESTRUCT);
	float MinimumHealth = 0.0f;

#if !UE_BUILD_SHIPPING
	// Godmode and unlimited health stop death unless it's a self destruct
	if (!bIsDamageFromSelfDestruct &&
		(Data.Target.HasMatchingGameplayTag(H_Cheat_Tags::TAG_CHEAT_GODMODE) || Data.Target.HasMatchingGameplayTag(H_Cheat_Tags::TAG_CHEAT_UNLIMITEDHEALTH)))
	{
		MinimumHealth = 1.0f;
	}
#endif // #if !UE_BUILD_SHIPPING

	const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
	AActor* Instigator = EffectContext.GetOriginalInstigator();
	AActor* Causer = EffectContext.GetEffectCauser();

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		// Send a standardized verb message that other systems can observe
		if (Data.EvaluatedData.Magnitude > 0.0f)
		{
			FHVerbMessage Message;
			Message.Verb = H_Message_Tags::TAG_DAMAGE_MESSAGE;
			Message.Instigator = Data.EffectSpec.GetEffectContext().GetEffectCauser();
			Message.InstigatorTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();
			Message.Target = GetOwningActor();
			Message.TargetTags = *Data.EffectSpec.CapturedTargetTags.GetAggregatedTags();
			//@TODO: Fill out context tags, and any non-ability-system source/instigator tags
			//@TODO: Determine if it's an opposing team kill, self-own, team kill, etc...
			Message.Magnitude = Data.EvaluatedData.Magnitude;

			UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
			MessageSystem.BroadcastMessage(Message.Verb, Message);
		}

		float CurrentShield = GetShield();
		float DamageToDo = GetDamage();

		float ShieldDamageRemainder = 0;

		

		if(CurrentShield >= DamageToDo)
		{
			// Check if shields are greater than damage, if so, just do damage to shields
			SetShield(FMath::Clamp(CurrentShield - DamageToDo, 0.f, GetMaxShield()));
		}
		else if(CurrentShield <= 0)
		{
			// If current shield is already 0 then just do damage to health
			SetHealth(FMath::Clamp(GetHealth() - GetDamage(), MinimumHealth, GetMaxHealth()));
		}
		else
		{
			// Otherwise, we find remaining damage after dealing damage to shield
			// Then remove health according to remaining damage
			ShieldDamageRemainder = DamageToDo - CurrentShield;
			SetShield(0.f);
			SetHealth(FMath::Clamp(GetHealth() - ShieldDamageRemainder, MinimumHealth, GetMaxHealth()));
		}

		//Set damage back to 0
		SetDamage(0.0f);
	}
	else if (Data.EvaluatedData.Attribute == GetHealingAttribute())
	{
		// Convert into +Health and then clamo
		SetHealth(FMath::Clamp(GetHealth() + GetHealing(), MinimumHealth, GetMaxHealth()));
		SetHealing(0.0f);
	}
	else if (Data.EvaluatedData.Attribute == GetShieldAttribute())
	{
		// Clamp and fall into out of shield handling below
		SetShield(FMath::Clamp(GetShield(), 0.f, GetMaxShield()));
	}
	else if (Data.EvaluatedData.Attribute == GetMaxShieldAttribute())
	{
		// TODO clamp current shield?

		// Notify on any requested max shield changes
		OnMaxShieldChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, MaxShieldBeforeAttributeChange, GetMaxShield());
	}
	else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		// Clamp and fall into out of health handling below
		SetHealth(FMath::Clamp(GetHealth(), MinimumHealth, GetMaxHealth()));
	}
	else if (Data.EvaluatedData.Attribute == GetMaxHealthAttribute())
	{
		// TODO clamp current health?

		// Notify on any requested max health changes
		OnMaxHealthChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, MaxHealthBeforeAttributeChange, GetMaxHealth());
	}

	// If shield has actually changed activate callbacks
	if (GetShield() != ShieldBeforeAttributeChange)
	{
		OnShieldChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, ShieldBeforeAttributeChange, GetShield());
	}

	if ((GetShield() <= 0.0f) && !bOutOfShield)
	{
		OnOutOfShield.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, ShieldBeforeAttributeChange, GetShield());
	}

	// If health has actually changed activate callbacks
	if (GetHealth() != HealthBeforeAttributeChange)
	{
		OnHealthChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, HealthBeforeAttributeChange, GetHealth());
	}

	if ((GetHealth() <= 0.0f) && !bOutOfHealth)
	{
		OnOutOfHealth.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, HealthBeforeAttributeChange, GetHealth());
	}

	// Check shield again in case an event above changed it.
	bOutOfShield = (GetShield() <= 0.0f);

	// Check health again in case an event above changed it.
	bOutOfHealth = (GetHealth() <= 0.0f);
}

void UHHealthSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}

void UHHealthSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}

void UHHealthSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetMaxShieldAttribute())
	{
		// Make sure current health is not greater than the new max health.
		if (GetShield() > NewValue)
		{
			UHAbilitySystemComponent* HASC = GetHAbilitySystemComponent();
			check(HASC);

			HASC->ApplyModToAttribute(GetShieldAttribute(), EGameplayModOp::Override, NewValue);
		}
	}

	if (Attribute == GetMaxHealthAttribute())
	{
		// Make sure current health is not greater than the new max health.
		if (GetHealth() > NewValue)
		{
			UHAbilitySystemComponent* HASC = GetHAbilitySystemComponent();
			check(HASC);

			HASC->ApplyModToAttribute(GetHealthAttribute(), EGameplayModOp::Override, NewValue);
		}
	}

	if (bOutOfShield && (GetShield() > 0.0f))
	{
		bOutOfShield = false;
	}

	if (bOutOfHealth && (GetHealth() > 0.0f))
	{
		bOutOfHealth = false;
	}
}

void UHHealthSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetShieldAttribute())
	{
		// Do not allow shield to go negative or above max shield.
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxShield());
	}
	else if (Attribute == GetHealthAttribute())
	{
		// Do not allow health to go negative or above max health.
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	else if (Attribute == GetMaxShieldAttribute())
	{
		// Do not allow max shield to drop below 1.
		NewValue = FMath::Max(NewValue, 1.0f);
	}
	else if (Attribute == GetMaxHealthAttribute())
	{
		// Do not allow max health to drop below 1.
		NewValue = FMath::Max(NewValue, 1.0f);
	}
}
