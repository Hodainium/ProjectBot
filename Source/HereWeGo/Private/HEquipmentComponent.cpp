// Fill out your copyright notice in the Description page of Project Settings.


#include "HEquipmentComponent.h"

#include "AbilitySystemGlobals.h"
#include "HAbilitySystemComponent.h"
#include "HEquipmentInstance.h"
#include "HEquipmentDefinition.h"
#include "HItemDefinition.h"
#include "HInventoryItemInstance.h"
#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"

class FLifetimeProperty;
struct FReplicationFlags;

//////////////////////////////////////////////////////////////////////
// FHAppliedEquipmentEntry

FString FHAppliedEquipmentEntry::GetDebugString() const
{
	return FString::Printf(TEXT("%s of %s"), *GetNameSafe(Instance), *GetNameSafe(EquipmentDefinition.Get()));
}

//////////////////////////////////////////////////////////////////////
// FHEquipmentList

void FHEquipmentList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (int32 Index : RemovedIndices)
	{
		const FHAppliedEquipmentEntry& Entry = Entries[Index];
		if (Entry.Instance != nullptr)
		{
			Entry.Instance->OnUnequipped();
		}
	}
}

void FHEquipmentList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (int32 Index : AddedIndices)
	{
		const FHAppliedEquipmentEntry& Entry = Entries[Index];
		if (Entry.Instance != nullptr)
		{
			Entry.Instance->OnEquipped();
		}
	}
}

void FHEquipmentList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
}

UHAbilitySystemComponent* FHEquipmentList::GetAbilitySystemComponent() const
{
	check(OwnerComponent);
	AActor* OwningActor = OwnerComponent->GetOwner();
	return Cast<UHAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor));
}

UHEquipmentInstance* FHEquipmentList::AddEntry(UHEquipmentDefinition* EquipmentDefinition)
{
	UHEquipmentInstance* Result = nullptr;

	check(EquipmentDefinition != nullptr);
	check(OwnerComponent);
	check(OwnerComponent->GetOwner()->HasAuthority());

	TSubclassOf<UHEquipmentInstance> InstanceType = EquipmentDefinition->InstanceType;

	if (InstanceType == nullptr)
	{
		InstanceType = UHEquipmentInstance::StaticClass();
	}

	FHAppliedEquipmentEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.EquipmentDefinition = EquipmentDefinition;
	NewEntry.Instance = NewObject<UHEquipmentInstance>(OwnerComponent->GetOwner(), InstanceType);  //@TODO: Using the actor instead of component as the outer due to UE-127172
	Result = NewEntry.Instance;

	if (UHAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		for (TObjectPtr<const UHAbilitySet> AbilitySet : EquipmentDefinition->AbilitySetsToGrant)
		{
			AbilitySet->GiveToAbilitySystem(ASC, /*inout*/ &NewEntry.GrantedHandles, Result);
		}
	}
	else
	{
		//@TODO: Warning logging?
	}

	Result->SpawnEquipmentActors(EquipmentDefinition->ActorsToSpawn);


	MarkItemDirty(NewEntry);

	return Result;
}

void FHEquipmentList::RemoveEntry(UHEquipmentInstance* Instance)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FHAppliedEquipmentEntry& Entry = *EntryIt;
		if (Entry.Instance == Instance)
		{
			if (UHAbilitySystemComponent* ASC = GetAbilitySystemComponent())
			{
				Entry.GrantedHandles.TakeFromAbilitySystem(ASC);
			}

			Instance->DestroyEquipmentActors();


			EntryIt.RemoveCurrent();
			MarkArrayDirty();
		}
	}
}

//////////////////////////////////////////////////////////////////////
// UHEquipmentManagerComponent

UHEquipmentComponent::UHEquipmentComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, EquipmentList(this)
{
	SetIsReplicatedByDefault(true);
	bWantsInitializeComponent = true;
}

void UHEquipmentComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, EquipmentList);
}

UHEquipmentInstance* UHEquipmentComponent::EquipItem(UHEquipmentDefinition* EquipmentClass, UObject* InInstigator)
{
	UHEquipmentInstance* Result = nullptr;

	if (EquipmentClass != nullptr)
	{
		Result = EquipmentList.AddEntry(EquipmentClass);
		if (Result != nullptr)
		{
			Result->SetInstigator(InInstigator);
			Result->OnEquipped();
			
			if (IsUsingRegisteredSubObjectList() && IsReadyForReplication())
			{
				AddReplicatedSubObject(Result);
			}
		}
	}
	return Result;
}

void UHEquipmentComponent::UnequipItem(UHEquipmentInstance* EquipmentInstance)
{
	if (EquipmentInstance != nullptr)
	{
		if (IsUsingRegisteredSubObjectList())
		{
			RemoveReplicatedSubObject(EquipmentInstance);
		}

		EquipmentInstance->OnUnequipped();
		EquipmentList.RemoveEntry(EquipmentInstance);
	}
}

bool UHEquipmentComponent::ReplicateSubobjects(UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (FHAppliedEquipmentEntry& Entry : EquipmentList.Entries)
	{
		UHEquipmentInstance* Instance = Entry.Instance;

		if (IsValid(Instance))
		{
			WroteSomething |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags);
		}
	}

	return WroteSomething;
}

void UHEquipmentComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void UHEquipmentComponent::UninitializeComponent()
{
	TArray<UHEquipmentInstance*> AllEquipmentInstances;

	// gathering all instances before removal to avoid side effects affecting the equipment list iterator	
	for (const FHAppliedEquipmentEntry& Entry : EquipmentList.Entries)
	{
		AllEquipmentInstances.Add(Entry.Instance);
	}

	for (UHEquipmentInstance* EquipInstance : AllEquipmentInstances)
	{
		UnequipItem(EquipInstance);
	}

	Super::UninitializeComponent();
}

void UHEquipmentComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	// Register existing HEquipmentInstances
	if (IsUsingRegisteredSubObjectList())
	{
		for (const FHAppliedEquipmentEntry& Entry : EquipmentList.Entries)
		{
			UHEquipmentInstance* Instance = Entry.Instance;

			if (IsValid(Instance))
			{
				AddReplicatedSubObject(Instance);
			}
		}
	}
}

UHEquipmentInstance* UHEquipmentComponent::GetFirstInstanceOfType(TSubclassOf<UHEquipmentInstance> InstanceType)
{
	for (FHAppliedEquipmentEntry& Entry : EquipmentList.Entries)
	{
		if (UHEquipmentInstance* Instance = Entry.Instance)
		{
			if (Instance->IsA(InstanceType))
			{
				return Instance;
			}
		}
	}

	return nullptr;
}

TArray<UHEquipmentInstance*> UHEquipmentComponent::GetEquipmentInstancesOfType(TSubclassOf<UHEquipmentInstance> InstanceType) const
{
	TArray<UHEquipmentInstance*> Results;
	for (const FHAppliedEquipmentEntry& Entry : EquipmentList.Entries)
	{
		if (UHEquipmentInstance* Instance = Entry.Instance)
		{
			if (Instance->IsA(InstanceType))
			{
				Results.Add(Instance);
			}
		}
	}
	return Results;
}

