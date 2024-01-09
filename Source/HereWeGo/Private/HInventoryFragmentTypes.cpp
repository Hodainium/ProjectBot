// Fill out your copyright notice in the Description page of Project Settings.


#include "HInventoryFragmentTypes.h"
#include "HInventoryItemInstance.h"


UHInventoryFragment_PickupIcon::UHInventoryFragment_PickupIcon()
{
	PadColor = FLinearColor::Green;
}

void UHInventoryFragment_SetStats::OnInstanceCreated(UHInventoryItemInstance* Instance) const
{
	for (const auto& KVP : InitialItemStats)
	{
		Instance->AddStatTagStack(KVP.Key, KVP.Value);
	}
}

int32 UHInventoryFragment_SetStats::GetItemStatByTag(FGameplayTag Tag) const
{
	if (const int32* StatPtr = InitialItemStats.Find(Tag))
	{
		return *StatPtr;
	}

	return 0;
}
