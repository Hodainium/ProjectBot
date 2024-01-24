// Fill out your copyright notice in the Description page of Project Settings.


#include "HBaseGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemGlobals.h"
#include "HAbilityCost.h"
#include "AbilitySystemLog.h"
#include "HAbilityGlobalMessages.h"
#include "HAbilitySourceInterface.h"
#include "HAbilitySystemComponent.h"
#include "HCameraMode.h"
#include "HCharacterBaseOld.h"
#include "HGameplayEffectContext.h"
#include "HLogChannels.h"
#include "HPhysicalMaterialWithTags.h"
#include "HPlayerCharacter.h"
#include "HPlayerController.h"
#include "NativeGameplayTags.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameFramework/PlayerState.h"
//
//#define ENSURE_ABILITY_IS_INSTANTIATED_OR_RETURN(FunctionName, ReturnValue)																				\
//{																																						\
//	if (!ensure(IsInstantiated()))																														\
//	{																																					\
//		ABILITY_LOG(Error, TEXT("%s: " #FunctionName " cannot be called on a non-instanced ability. Check the instancing policy."), *GetPathName());	\
//		return ReturnValue;																																\
//	}																																					\
//}
//
//UE_DEFINE_GAMEPLAY_TAG(TAG_ABILITY_SIMPLE_FAILURE_MESSAGE, "Ability.UserFacingSimpleActivateFail.Message");
//UE_DEFINE_GAMEPLAY_TAG(TAG_ABILITY_PLAY_MONTAGE_FAILURE_MESSAGE, "Ability.PlayMontageOnActivateFail.Message");
//
//UE_DEFINE_GAMEPLAY_TAG(TAG_ABILITY_ACTIVATE_FAIL_ACTIVATION_GROUP, "Ability.ActivateFail.ActivationGroup");
//UE_DEFINE_GAMEPLAY_TAG(TAG_ABILITY_ACTIVATE_FAIL_DEATH, "Ability.ActivateFail.Death");
//
//UE_DEFINE_GAMEPLAY_TAG(TAG_STATE_DEATH, "State.Death");
//
//UHBaseGameplayAbility::UHBaseGameplayAbility(const FObjectInitializer& ObjectInitializer)
//	: Super(ObjectInitializer)
//{
//	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;
//	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
//	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
//	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;
//
//	ActivationPolicy = EHAbilityActivationPolicy::OnInputTriggered;
//	ActivationGroup = EHAbilityActivationGroup::Independent;
//
//	bLogCancelation = false;
//
//	ActiveCameraMode = nullptr;
//}
//
//UHAbilitySystemComponent* UHBaseGameplayAbility::GetHAbilitySystemComponentFromActorInfo() const
//{
//	return (CurrentActorInfo ? Cast<UHAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent.Get()) : nullptr);
//}
//
//AHPlayerController* UHBaseGameplayAbility::GetHPlayerControllerFromActorInfo() const
//{
//	return (CurrentActorInfo ? Cast<AHPlayerController>(CurrentActorInfo->PlayerController.Get()) : nullptr);
//}
//
//AController* UHBaseGameplayAbility::GetControllerFromActorInfo() const
//{
//	if (CurrentActorInfo)
//	{
//		if (AController* PC = CurrentActorInfo->PlayerController.Get())
//		{
//			return PC;
//		}
//
//		// Look for a player controller or pawn in the owner chain.
//		AActor* TestActor = CurrentActorInfo->OwnerActor.Get();
//		while (TestActor)
//		{
//			if (AController* C = Cast<AController>(TestActor))
//			{
//				return C;
//			}
//
//			if (APawn* Pawn = Cast<APawn>(TestActor))
//			{
//				return Pawn->GetController();
//			}
//
//			TestActor = TestActor->GetOwner();
//		}
//	}
//
//	return nullptr;
//}
//
//AHCharacterBaseOld* UHBaseGameplayAbility::GetHCharacterBaseFromActorInfo() const
//{
//	return (CurrentActorInfo ? Cast<AHCharacterBaseOld>(CurrentActorInfo->AvatarActor.Get()) : nullptr);
//}
//
//AHPlayerCharacter* UHBaseGameplayAbility::GetHPlayerCharacterFromActorInfo() const
//{
//	return (CurrentActorInfo ? Cast<AHPlayerCharacter>(CurrentActorInfo->AvatarActor.Get()) : nullptr);
//}
//
//void UHBaseGameplayAbility::NativeOnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const
//{
//	bool bSimpleFailureFound = false;
//	for (FGameplayTag Reason : FailedReason)
//	{
//		if (!bSimpleFailureFound)
//		{
//			if (const FText* pUserFacingMessage = FailureTagToUserFacingMessages.Find(Reason))
//			{
//				FHAbilitySimpleFailureMessage Message;
//				Message.PlayerController = GetActorInfo().PlayerController.Get();
//				Message.FailureTags = FailedReason;
//				Message.UserFacingReason = *pUserFacingMessage;
//
//				UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
//				MessageSystem.BroadcastMessage(TAG_ABILITY_SIMPLE_FAILURE_MESSAGE, Message);
//				bSimpleFailureFound = true;
//			}
//		}
//
//		if (UAnimMontage* pMontage = FailureTagToAnimMontage.FindRef(Reason))
//		{
//			FHAbilityMontageFailureMessage Message;
//			Message.PlayerController = GetActorInfo().PlayerController.Get();
//			Message.FailureTags = FailedReason;
//			Message.FailureMontage = pMontage;
//
//			UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
//			MessageSystem.BroadcastMessage(TAG_ABILITY_PLAY_MONTAGE_FAILURE_MESSAGE, Message);
//		}
//	}
//}
//
//bool UHBaseGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
//{
//	if (!ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid())
//	{
//		return false;
//	}
//
//	UHAbilitySystemComponent* HASC = CastChecked<UHAbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get());
//
//	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
//	{
//		return false;
//	}
//
//	//@TODO Possibly remove after setting up tag relationships
//	if (HASC->IsActivationGroupBlocked(ActivationGroup))
//	{
//		if (OptionalRelevantTags)
//		{
//			OptionalRelevantTags->AddTag(TAG_ABILITY_ACTIVATE_FAIL_ACTIVATION_GROUP);
//		}
//		return false;
//	}
//
//	return true;
//}
//
//void UHBaseGameplayAbility::SetCanBeCanceled(bool bCanBeCanceled)
//{
//	// The ability can not block canceling if it's replaceable.
//	if (!bCanBeCanceled && (ActivationGroup == EHAbilityActivationGroup::Exclusive_Replaceable))
//	{
//		UE_LOG(LogHAbilitySystem, Error, TEXT("SetCanBeCanceled: Ability [%s] can not block canceling because its activation group is replaceable."), *GetName());
//		return;
//	}
//
//	Super::SetCanBeCanceled(bCanBeCanceled);
//}
//
//void UHBaseGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
//{
//	Super::OnGiveAbility(ActorInfo, Spec);
//
//	K2_OnAbilityAdded();
//
//	TryActivateAbilityOnSpawn(ActorInfo, Spec);
//}
//
//void UHBaseGameplayAbility::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
//{
//	K2_OnAbilityRemoved();
//
//	Super::OnRemoveAbility(ActorInfo, Spec);
//}
//
//void UHBaseGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
//{
//	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
//}
//
//void UHBaseGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
//{
//#if !UE_BUILD_SHIPPING
//	if (bWasCancelled && bLogCancelation)
//	{
//		UE_LOG(LogHAbilitySystem, Warning, TEXT("========  (%s) canceled with EndAbility (locally controlled? %i) ========"), *GetName(), IsLocallyControlled());
//
//		if (APlayerState* PS = Cast<APlayerState>(GetOwningActorFromActorInfo()))
//		{
//			UE_LOG(LogHAbilitySystem, Log, TEXT("Player Name: %s"), *PS->GetPlayerName());
//		}
//
//		PrintScriptCallstack();
//
//		const SIZE_T StackTraceSize = 65535;
//		ANSICHAR* StackTrace = (ANSICHAR*)FMemory::SystemMalloc(StackTraceSize);
//		if (StackTrace != nullptr)
//		{
//			StackTrace[0] = 0;
//			// Walk the stack and dump it to the allocated memory.
//			FPlatformStackWalk::StackWalkAndDump(StackTrace, StackTraceSize, 1);
//			UE_LOG(LogHAbilitySystem, Log, TEXT("Call Stack:\n%s"), ANSI_TO_TCHAR(StackTrace));
//			FMemory::SystemFree(StackTrace);
//		}
//	}
//#endif // !UE_BUILD_SHIPPING
//
//
//	ClearCameraMode();
//
//	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
//}
//
//bool UHBaseGameplayAbility::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags) const
//{
//	if (!Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags) || !ActorInfo)
//	{
//		return false;
//	}
//
//	// Verify we can afford any additional costs
//	for (TObjectPtr<UHAbilityCost> AdditionalCost : AdditionalCosts)
//	{
//		if (AdditionalCost != nullptr)
//		{
//			if (!AdditionalCost->CheckCost(this, Handle, ActorInfo, /*inout*/ OptionalRelevantTags))
//			{
//				return false;
//			}
//		}
//	}
//
//	return true;
//}
//
//void UHBaseGameplayAbility::ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
//{
//	Super::ApplyCost(Handle, ActorInfo, ActivationInfo);
//
//	check(ActorInfo);
//
//	// Used to determine if the ability actually hit a target (as some costs are only spent on successful attempts)
//	auto DetermineIfAbilityHitTarget = [&]()
//	{
//		if (ActorInfo->IsNetAuthority())
//		{
//			if (UHAbilitySystemComponent* ASC = Cast<UHAbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get()))
//			{
//				FGameplayAbilityTargetDataHandle TargetData;
//				ASC->GetAbilityTargetData(Handle, ActivationInfo, TargetData);
//				for (int32 TargetDataIdx = 0; TargetDataIdx < TargetData.Data.Num(); ++TargetDataIdx)
//				{
//					if (UAbilitySystemBlueprintLibrary::TargetDataHasHitResult(TargetData, TargetDataIdx))
//					{
//						return true;
//					}
//				}
//			}
//		}
//
//		return false;
//	};
//
//	// Pay any additional costs
//	bool bAbilityHitTarget = false;
//	bool bHasDeterminedIfAbilityHitTarget = false;
//	for (TObjectPtr<UHAbilityCost> AdditionalCost : AdditionalCosts)
//	{
//		if (AdditionalCost != nullptr)
//		{
//			if (AdditionalCost->ShouldOnlyApplyCostOnHit())
//			{
//				if (!bHasDeterminedIfAbilityHitTarget)
//				{
//					bAbilityHitTarget = DetermineIfAbilityHitTarget();
//					bHasDeterminedIfAbilityHitTarget = true;
//				}
//
//				if (!bAbilityHitTarget)
//				{
//					continue;
//				}
//			}
//
//			AdditionalCost->ApplyCost(this, Handle, ActorInfo, ActivationInfo);
//		}
//	}
//}
//
//FGameplayEffectContextHandle UHBaseGameplayAbility::MakeEffectContext(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo) const
//{
//	FGameplayEffectContextHandle ContextHandle = Super::MakeEffectContext(Handle, ActorInfo);
//
//	FHGameplayEffectContext* EffectContext = FHGameplayEffectContext::ExtractEffectContext(ContextHandle);
//	check(EffectContext);
//
//	check(ActorInfo);
//
//	AActor* EffectCauser = nullptr;
//	const IHAbilitySourceInterface* AbilitySource = nullptr;
//	float SourceLevel = 0.0f;
//	GetAbilitySource(Handle, ActorInfo, /*out*/ SourceLevel, /*out*/ AbilitySource, /*out*/ EffectCauser);
//
//	UObject* SourceObject = GetSourceObject(Handle, ActorInfo);
//
//	AActor* Instigator = ActorInfo ? ActorInfo->OwnerActor.Get() : nullptr;
//
//	EffectContext->SetAbilitySource(AbilitySource, SourceLevel);
//	EffectContext->AddInstigator(Instigator, EffectCauser);
//	EffectContext->AddSourceObject(SourceObject);
//
//	return ContextHandle;
//}
//
//void UHBaseGameplayAbility::ApplyAbilityTagsToGameplayEffectSpec(FGameplayEffectSpec& Spec, FGameplayAbilitySpec* AbilitySpec) const
//{
//	Super::ApplyAbilityTagsToGameplayEffectSpec(Spec, AbilitySpec);
//
//	if (const FHitResult* HitResult = Spec.GetContext().GetHitResult())
//	{
//		if (const UHPhysicalMaterialWithTags* PhysMatWithTags = Cast<const UHPhysicalMaterialWithTags>(HitResult->PhysMaterial.Get()))
//		{
//			Spec.CapturedTargetTags.GetSpecTags().AppendTags(PhysMatWithTags->Tags);
//		}
//	}
//}
//
//bool UHBaseGameplayAbility::DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
//{
//	// Specialized version to handle death exclusion and AbilityTags expansion via ASC
//
//	bool bBlocked = false;
//	bool bMissing = false;
//
//	UAbilitySystemGlobals& AbilitySystemGlobals = UAbilitySystemGlobals::Get();
//	const FGameplayTag& BlockedTag = AbilitySystemGlobals.ActivateFailTagsBlockedTag;
//	const FGameplayTag& MissingTag = AbilitySystemGlobals.ActivateFailTagsMissingTag;
//
//	// Check if any of this ability's tags are currently blocked
//	if (AbilitySystemComponent.AreAbilityTagsBlocked(AbilityTags))
//	{
//		bBlocked = true;
//	}
//
//	const UHAbilitySystemComponent* HASC = Cast<UHAbilitySystemComponent>(&AbilitySystemComponent);
//	static FGameplayTagContainer AllRequiredTags;
//	static FGameplayTagContainer AllBlockedTags;
//
//	AllRequiredTags = ActivationRequiredTags;
//	AllBlockedTags = ActivationBlockedTags;
//
//	// Expand our ability tags to add additional required/blocked tags
//	if (HASC)
//	{
//		HASC->GetAdditionalActivationTagRequirements(AbilityTags, AllRequiredTags, AllBlockedTags);
//	}
//
//	// Check to see the required/blocked tags for this ability
//	if (AllBlockedTags.Num() || AllRequiredTags.Num())
//	{
//		static FGameplayTagContainer AbilitySystemComponentTags;
//
//		AbilitySystemComponentTags.Reset();
//		AbilitySystemComponent.GetOwnedGameplayTags(AbilitySystemComponentTags);
//
//		if (AbilitySystemComponentTags.HasAny(AllBlockedTags))
//		{
//			if (OptionalRelevantTags && AbilitySystemComponentTags.HasTag(TAG_STATE_DEATH))
//			{
//				// If player is dead and was rejected due to blocking tags, give that feedback
//				OptionalRelevantTags->AddTag(TAG_ABILITY_ACTIVATE_FAIL_DEATH);
//			}
//
//			bBlocked = true;
//		}
//
//		if (!AbilitySystemComponentTags.HasAll(AllRequiredTags))
//		{
//			bMissing = true;
//		}
//	}
//
//	if (SourceTags != nullptr)
//	{
//		if (SourceBlockedTags.Num() || SourceRequiredTags.Num())
//		{
//			if (SourceTags->HasAny(SourceBlockedTags))
//			{
//				bBlocked = true;
//			}
//
//			if (!SourceTags->HasAll(SourceRequiredTags))
//			{
//				bMissing = true;
//			}
//		}
//	}
//
//	if (TargetTags != nullptr)
//	{
//		if (TargetBlockedTags.Num() || TargetRequiredTags.Num())
//		{
//			if (TargetTags->HasAny(TargetBlockedTags))
//			{
//				bBlocked = true;
//			}
//
//			if (!TargetTags->HasAll(TargetRequiredTags))
//			{
//				bMissing = true;
//			}
//		}
//	}
//
//	if (bBlocked)
//	{
//		if (OptionalRelevantTags && BlockedTag.IsValid())
//		{
//			OptionalRelevantTags->AddTag(BlockedTag);
//		}
//		return false;
//	}
//	if (bMissing)
//	{
//		if (OptionalRelevantTags && MissingTag.IsValid())
//		{
//			OptionalRelevantTags->AddTag(MissingTag);
//		}
//		return false;
//	}
//
//	return true;
//}
//
//void UHBaseGameplayAbility::OnPawnAvatarSet()
//{
//	K2_OnPawnAvatarSet();
//}
//
//void UHBaseGameplayAbility::GetAbilitySource(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, float& OutSourceLevel, const IHAbilitySourceInterface*& OutAbilitySource, AActor*& OutEffectCauser) const
//{
//	OutSourceLevel = 0.0f;
//	OutAbilitySource = nullptr;
//	OutEffectCauser = nullptr;
//
//	OutEffectCauser = ActorInfo->AvatarActor.Get();
//
//	// If we were added by something that's an ability info source, use it
//	UObject* SourceObject = GetSourceObject(Handle, ActorInfo);
//
//	OutAbilitySource = Cast<IHAbilitySourceInterface>(SourceObject);
//}
//
//void UHBaseGameplayAbility::TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const
//{
//	const bool bIsPredicting = (Spec.ActivationInfo.ActivationMode == EGameplayAbilityActivationMode::Predicting);
//
//	// Try to activate if activation policy is on spawn.
//	if (ActorInfo && !Spec.IsActive() && !bIsPredicting && (ActivationPolicy == EHAbilityActivationPolicy::OnSpawn))
//	{
//		UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
//		const AActor* AvatarActor = ActorInfo->AvatarActor.Get();
//
//		// If avatar actor is torn off or about to die, don't try to activate until we get the new one.
//		if (ASC && AvatarActor && !AvatarActor->GetTearOff() && (AvatarActor->GetLifeSpan() <= 0.0f))
//		{
//			const bool bIsLocalExecution = (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::LocalPredicted) || (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::LocalOnly);
//			const bool bIsServerExecution = (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::ServerOnly) || (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::ServerInitiated);
//
//			const bool bClientShouldActivate = ActorInfo->IsLocallyControlled() && bIsLocalExecution;
//			const bool bServerShouldActivate = ActorInfo->IsNetAuthority() && bIsServerExecution;
//
//			if (bClientShouldActivate || bServerShouldActivate)
//			{
//				ASC->TryActivateAbility(Spec.Handle);
//			}
//		}
//	}
//}
//
//bool UHBaseGameplayAbility::CanChangeActivationGroup(EHAbilityActivationGroup NewGroup) const
//{
//	if (!IsInstantiated() || !IsActive())
//	{
//		return false;
//	}
//
//	if (ActivationGroup == NewGroup)
//	{
//		return true;
//	}
//
//	UHAbilitySystemComponent* HASC = GetHAbilitySystemComponentFromActorInfo();
//	check(HASC);
//
//	if ((ActivationGroup != EHAbilityActivationGroup::Exclusive_Blocking) && HASC->IsActivationGroupBlocked(NewGroup))
//	{
//		// This ability can't change groups if it's blocked (unless it is the one doing the blocking).
//		return false;
//	}
//
//	if ((NewGroup == EHAbilityActivationGroup::Exclusive_Replaceable) && !CanBeCanceled())
//	{
//		// This ability can't become replaceable if it can't be canceled.
//		return false;
//	}
//
//	return true;
//}
//
//bool UHBaseGameplayAbility::ChangeActivationGroup(EHAbilityActivationGroup NewGroup)
//{
//	ENSURE_ABILITY_IS_INSTANTIATED_OR_RETURN(ChangeActivationGroup, false);
//
//	if (!CanChangeActivationGroup(NewGroup))
//	{
//		return false;
//	}
//
//	if (ActivationGroup != NewGroup)
//	{
//		UHAbilitySystemComponent* HASC = GetHAbilitySystemComponentFromActorInfo();
//		check(HASC);
//
//		HASC->RemoveAbilityFromActivationGroup(ActivationGroup, this);
//		HASC->AddAbilityToActivationGroup(NewGroup, this);
//
//		ActivationGroup = NewGroup;
//	}
//
//	return true;
//}
//
//void UHBaseGameplayAbility::SetCameraMode(TSubclassOf<UHCameraMode> CameraMode)
//{
//	ENSURE_ABILITY_IS_INSTANTIATED_OR_RETURN(SetCameraMode, );
//
//	if(AHPlayerCharacter* Char = GetHPlayerCharacterFromActorInfo())
//	{
//		Char->SetAbilityCameraMode(CameraMode, CurrentSpecHandle);
//		ActiveCameraMode = CameraMode;
//	}
//}
//
//void UHBaseGameplayAbility::ClearCameraMode()
//{
//	ENSURE_ABILITY_IS_INSTANTIATED_OR_RETURN(ClearCameraMode, );
//
//	if (ActiveCameraMode)
//	{
//		if (AHPlayerCharacter* Char = GetHPlayerCharacterFromActorInfo())
//		{
//			Char->ClearAbilityCameraMode(CurrentSpecHandle);
//		}
//
//		ActiveCameraMode = nullptr;
//	}
//}