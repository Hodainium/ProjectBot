// Fill out your copyright notice in the Description page of Project Settings.


#include "HGA_FireGun.h"
#include "HAT_PlayMontageAndWaitForEvent.h"

UHGA_FireGun::UHGA_FireGun()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	FGameplayTag Ability1Tag = FGameplayTag::RequestGameplayTag(FName("GAS.State.Ability.Skill1"));
	AbilityTags.AddTag(Ability1Tag);
	ActivationOwnedTags.AddTag(Ability1Tag);

	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("GAS.State.Ability")));

	Damage = 15.f;
	Range = 8000.f;
}

void UHGA_FireGun::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}


}

void UHGA_FireGun::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
}

void UHGA_FireGun::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
}

void UHGA_FireGun::EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
}
