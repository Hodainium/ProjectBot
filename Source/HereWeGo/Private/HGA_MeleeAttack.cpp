// Fill out your copyright notice in the Description page of Project Settings.


#include "HGA_MeleeAttack.h"

#include "AbilitySystemComponent.h"
#include "HAT_PlayMontageAndWaitForEvent.h"

#include "HWeaponComponent.h"

UHGA_MeleeAttack::UHGA_MeleeAttack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	FGameplayTag Ability1Tag = FGameplayTag::RequestGameplayTag(FName("GAS.State.Ability.Skill1"));
	AbilityTags.AddTag(Ability1Tag);
	ActivationOwnedTags.AddTag(Ability1Tag);

	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("GAS.State.Ability")));

	Damage = 15.f;
}

void UHGA_MeleeAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if(!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}

	UHAT_PlayMontageAndWaitForEvent* Task = UHAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this, NAME_None, 
		MeleeAttackMontage, FGameplayTagContainer());

	Task->OnBlendOut.AddDynamic(this, &UHGA_MeleeAttack::OnCompleted);
	Task->OnCompleted.AddDynamic(this, &UHGA_MeleeAttack::OnCompleted);
	Task->OnInterrupted.AddDynamic(this, &UHGA_MeleeAttack::OnCancelled);
	Task->OnCancelled.AddDynamic(this, &UHGA_MeleeAttack::OnCancelled);
	Task->OnEventReceived.AddDynamic(this, &UHGA_MeleeAttack::EventReceived);

	

	//Calls task once delegates are set up
	Task->ReadyForActivation();
}

void UHGA_MeleeAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	const AHCharacterBase* CharBase = Cast<AHCharacterBase>(GetAvatarActorFromActorInfo());

	if (CharBase && CharBase->IsLocallyControlled())
	{
		UHWeaponComponent* WeaponComp = CharBase->GetWeaponComponent();

		WeaponComp->StopMeleeTrace();
		WeaponComp->OnMeleeHitFound.RemoveDynamic(this, &UHGA_MeleeAttack::OnMeleeHitReceived);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UHGA_MeleeAttack::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UHGA_MeleeAttack::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UHGA_MeleeAttack::EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	if(EventTag == FGameplayTag::RequestGameplayTag(FName("Event.Montage.Melee.StartCollision")))
	{
		const AHCharacterBase* CharBase = Cast<AHCharacterBase>(GetAvatarActorFromActorInfo());

		if (CharBase && CharBase->IsLocallyControlled())
		{
			UHWeaponComponent* WeaponComp = CharBase->GetWeaponComponent();

			WeaponComp->StartMeleeTrace();
			WeaponComp->OnMeleeHitFound.AddUniqueDynamic(this, &UHGA_MeleeAttack::OnMeleeHitReceived);
		}
	}
	else if(EventTag == FGameplayTag::RequestGameplayTag(FName("Event.Montage.Melee.EndCollision")))
	{
		const AHCharacterBase* CharBase = Cast<AHCharacterBase>(GetAvatarActorFromActorInfo());

		if (CharBase && CharBase->IsLocallyControlled())
		{
			UHWeaponComponent* WeaponComp = CharBase->GetWeaponComponent();

			WeaponComp->StopMeleeTrace();
			WeaponComp->OnMeleeHitFound.RemoveDynamic(this, &UHGA_MeleeAttack::OnMeleeHitReceived);
		}
	}
	
}

void UHGA_MeleeAttack::OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& InData,
	FGameplayTag ApplicationTag)
{
	UAbilitySystemComponent* OwningASC = CurrentActorInfo->AbilitySystemComponent.Get();
	check(OwningASC);

	if (const FGameplayAbilitySpec* AbilitySpec = OwningASC->FindAbilitySpecFromHandle(CurrentSpecHandle))
	{
		FScopedPredictionWindow	ScopedPrediction(OwningASC);

		// Take ownership of the target data to make sure no callbacks into game code invalidate it out from under us
		FGameplayAbilityTargetDataHandle LocalTargetDataHandle(MoveTemp(const_cast<FGameplayAbilityTargetDataHandle&>(InData)));

		const bool bShouldNotifyServer = CurrentActorInfo->IsLocallyControlled() && !CurrentActorInfo->IsNetAuthority();
		if (bShouldNotifyServer)
		{
			OwningASC->CallServerSetReplicatedTargetData(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey(), LocalTargetDataHandle, ApplicationTag, OwningASC->ScopedPredictionKey);
		}

		const bool bIsTargetDataValid = true;

#if WITH_SERVER_CODE

		if (AController* Controller = GetControllerFromActorInfo())
		{
			if (Controller->GetLocalRole() == ROLE_Authority)
			{
				// Confirm hit markers
				if (UHWeaponComponent* WeaponComp = Controller->GetComponentByClass<UHWeaponComponent>())
				{
					TArray<uint8> HitReplaces;
					for (uint8 i = 0; (i < LocalTargetDataHandle.Num()) && (i < 255); i++)
					{
						if (FGameplayAbilityTargetData_SingleTargetHit* SingleTargetHit = static_cast<FGameplayAbilityTargetData_SingleTargetHit*>(LocalTargetDataHandle.Get(i)))
						{
							if (SingleTargetHit->bHitReplaced)
							{
								HitReplaces.Add(i);
							}
						}
					}

					WeaponComp->ClientConfirmTargetData(LocalTargetDataHandle.UniqueId, bIsTargetDataValid, HitReplaces);
				}
				//If it is an NPC no need to have hitmarkers. Thus not ignore if not on controller

			}
		}
		
#endif //WITH_SERVER_CODE


		// See if we still have ammo
		if (bIsTargetDataValid && CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo))
		{
			// Let the blueprint do stuff like apply effects to the targets
			OnMeleeWeaponTargetDataReady(LocalTargetDataHandle);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Weapon ability %s failed to commit (bIsTargetDataValid=%d)"), *GetPathName(), bIsTargetDataValid ? 1 : 0);
			K2_EndAbility();
		}
	}

	// We've processed the data
	OwningASC->ConsumeClientReplicatedTargetData(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey());
}

void UHGA_MeleeAttack::OnMeleeWeaponTargetDataReady_Implementation(const FGameplayAbilityTargetDataHandle& TargetData)
{
	UE_LOG(LogTemp, Error, TEXT("We aren't doing anything with the target data this ain't right!"));
}

void UHGA_MeleeAttack::OnMeleeHitReceived(TArray<FHitResult>& HitResults, UPrimitiveComponent* HitMesh)
{
	UAbilitySystemComponent* OwningASC = CurrentActorInfo->AbilitySystemComponent.Get();
	check(OwningASC);

	AController* Controller = GetControllerFromActorInfo();
	check(Controller);

	UHWeaponComponent* WeaponComp = Controller->FindComponentByClass<UHWeaponComponent>();
	check(WeaponComp);

	FScopedPredictionWindow ScopedPrediction(OwningASC, CurrentActivationInfo.GetActivationPredictionKey());

	FGameplayAbilityTargetDataHandle TargetData;

	for (FHitResult Hit : HitResults)
	{
		AActor* HitActor = Hit.GetActor();

		if(!ConfirmedHitActorsToIgnore.Contains(HitActor))
		{
			FGameplayAbilityTargetData_SingleTargetHit* NewTargetData = new FGameplayAbilityTargetData_SingleTargetHit();
			NewTargetData->HitResult = Hit;

			TargetData.Add(NewTargetData);
		}
	}

	//Only add hitmarkers if its on a player controller
	if(WeaponComp)
	{
		WeaponComp->AddUnconfirmedServerSideHitMarkers(TargetData, HitResults);
	}
	

	
}
