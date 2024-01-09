// Fill out your copyright notice in the Description page of Project Settings.


#include "HAttributeSetBase.h"
#include "GameplayEffectExtension.h"
#include "HVerbMessage.h"
#include "NativeGameplayTags.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Net/UnrealNetwork.h"

UE_DEFINE_GAMEPLAY_TAG(TAG_Message_Damage, "Message.Damage");

UHAttributeSetBase::UHAttributeSetBase()
{
	bOutOfHealth = false;
}

void UHAttributeSetBase::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	//ClampMagnitude(Attribute, NewValue);

	if (Attribute == GetMaxHealthAttribute())
	{
		AdjustAttributeForMaxChange(Health, MaxHealth, GetHealthAttribute(), NewValue);
	}
	else if (Attribute == GetMaxStaminaAttribute())
	{
		AdjustAttributeForMaxChange(Stamina, MaxStamina, GetStaminaAttribute(), NewValue);
	}
	else if (Attribute == GetMaxManaAttribute())
	{
		AdjustAttributeForMaxChange(Mana, MaxMana, GetManaAttribute(), NewValue);
	}
	else if (Attribute == GetMoveSpeedAttribute())
	{
		NewValue = FMath::Clamp<float>(NewValue, 150.f, 1000.f);
	}
}

void UHAttributeSetBase::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	//ClampMagnitude(Attribute, NewValue);
}

void UHAttributeSetBase::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetMaxHealthAttribute())
	{
		if(GetHealth() > NewValue)
		{
			SetHealth(GetMaxHealth());
		}
	}

	if (bOutOfHealth && GetHealth() > 0.f)
	{
		bOutOfHealth = false;
	}
}

void UHAttributeSetBase::AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttributeData,
                                                     const FGameplayAttributeData& MaxAttributeData, const FGameplayAttribute& AffectedAttribute, float& NewMaxValue)
{
	UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
	const float CurrentMaxValue = MaxAttributeData.GetCurrentValue();

	if(!FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue) && ASC)
	{
		const float CurrentValue = AffectedAttributeData.GetCurrentValue();
		float NewDelta = (CurrentMaxValue > 0.f) ? (CurrentValue * NewMaxValue / CurrentMaxValue) - CurrentValue : NewMaxValue;

		ASC->ApplyModToAttribute(AffectedAttribute, EGameplayModOp::Additive, NewDelta);
	}
}

void UHAttributeSetBase::ClampMagnitude(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, MaxHealth.GetCurrentValue());
	}
	else if (Attribute == GetMaxHealthAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.f);
	}
	else if (Attribute == GetStaminaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, MaxStamina.GetCurrentValue());
	}
	else if (Attribute == GetMaxStaminaAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.f);
	}
	else if (Attribute == GetMoveSpeedAttribute())
	{
		//clamp movespeed
	}
}

void UHAttributeSetBase::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAttributeSetBase, Health, OldHealth);
}

void UHAttributeSetBase::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAttributeSetBase, MaxHealth, OldMaxHealth);
}

void UHAttributeSetBase::OnRep_HealthRegenRate(const FGameplayAttributeData& OldHealthRegenRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAttributeSetBase, HealthRegenRate, OldHealthRegenRate);
}

void UHAttributeSetBase::OnRep_Stamina(const FGameplayAttributeData& OldStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAttributeSetBase, Stamina, OldStamina);
}

void UHAttributeSetBase::OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAttributeSetBase, MaxStamina, OldMaxStamina);
}

void UHAttributeSetBase::OnRep_StaminaRegenRate(const FGameplayAttributeData& OldStaminaRegenRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAttributeSetBase, StaminaRegenRate, OldStaminaRegenRate);
}

void UHAttributeSetBase::OnRep_Mana(const FGameplayAttributeData& OldMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAttributeSetBase, Mana, OldMana)
}

void UHAttributeSetBase::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAttributeSetBase, MaxMana, OldMaxMana)
}

void UHAttributeSetBase::OnRep_ManaRegenRate(const FGameplayAttributeData& OldManaRegenRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAttributeSetBase, ManaRegenRate, OldManaRegenRate)
}

void UHAttributeSetBase::OnRep_Armor(const FGameplayAttributeData& OldArmor)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAttributeSetBase, Armor, OldArmor)
}

void UHAttributeSetBase::OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAttributeSetBase, MoveSpeed, OldMoveSpeed);
}

void UHAttributeSetBase::OnRep_Level(const FGameplayAttributeData& OldLevel)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHAttributeSetBase, Level, OldLevel);
}

//Here is where we cancel the gameplayeffect execute with a bool. Use for godmode and stuff like that
bool UHAttributeSetBase::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) 
{
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}

	if(Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		//block the effect if we have godmode tags and such todo
	}

	return true;
}

void UHAttributeSetBase::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	float MinHealth = 0.f;
	float MinMana = 0.f;
	float MinStamina = 0.f;
	

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		if (Data.EvaluatedData.Magnitude > 0.0f)
		{
			FHVerbMessage Message;
			Message.Verb = TAG_Message_Damage;
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

		SetHealth(FMath::Clamp(GetHealth() - GetDamage(), MinHealth, GetMaxHealth()));
		SetDamage(0.f);
	}
	else if (Data.EvaluatedData.Attribute == GetHealingAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth() + GetHealing(), MinHealth, GetMaxHealth()));
		SetHealing(0.f);
	}
	else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), MinHealth, GetMaxHealth()));
	}
	else if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), MinMana, GetMaxMana()));
	}
	else if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		SetStamina(FMath::Clamp(GetStamina(), MinStamina, GetMaxStamina()));
	}

	if ((GetHealth() <= 0.f) && !bOutOfHealth && (GetMaxHealth() > 0.f))
	{
		if (OnOutOfHealth.IsBound())
		{
			UE_LOG(LogTemp, Warning, TEXT("%s : died!"), *GetOwningActor()->GetActorNameOrLabel())

			//Broadcast on out of health
			const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();

			AActor* Instigator = EffectContext.GetInstigator();
			AActor* Causer = EffectContext.GetEffectCauser();
			const FGameplayEffectSpec& EffectSpec = Data.EffectSpec;
			float EffectMagnitude = Data.EvaluatedData.Magnitude;

			OnOutOfHealth.Broadcast(Instigator, Causer, EffectSpec, EffectMagnitude);
		}
	}

	ClampMagnitude(Data.EvaluatedData.Attribute, Data.EvaluatedData.Magnitude);

	bOutOfHealth = (GetHealth() <= 0.f);
}

UWorld* UHAttributeSetBase::GetWorld() const
{
	const UObject* Outer = GetOuter();
	check(Outer);

	return Outer->GetWorld();
}

void UHAttributeSetBase::GetHAbilitySystemComponent()
{
	UE_LOG(LogTemp, Error, TEXT("UHAttributeSetBase::GetHAbilitySystemComponent()---- is Not implemented yet"));
}

void UHAttributeSetBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UHAttributeSetBase, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAttributeSetBase, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAttributeSetBase, HealthRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAttributeSetBase, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAttributeSetBase, MaxStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAttributeSetBase, StaminaRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAttributeSetBase, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAttributeSetBase, MaxMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAttributeSetBase, ManaRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAttributeSetBase, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAttributeSetBase, MoveSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHAttributeSetBase, Level, COND_None, REPNOTIFY_Always);
}
