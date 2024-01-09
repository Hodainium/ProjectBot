// Fill out your copyright notice in the Description page of Project Settings.


#include "AsyncTaskListenAttributeChanged.h"


UAsyncTaskListenAttributeChanged* UAsyncTaskListenAttributeChanged::ListenForAttributeChange(
	UAbilitySystemComponent* AbilitySystemComponent, FGameplayAttribute AttributeToListenTo)
{
	if (!IsValid(AbilitySystemComponent) || !AttributeToListenTo.IsValid())
	{
		return nullptr;
	}

	UAsyncTaskListenAttributeChanged* ListenTask = NewObject<UAsyncTaskListenAttributeChanged>();

	ListenTask->AbilitySystemComp = AbilitySystemComponent;
	ListenTask->AttributeToListenFor = AttributeToListenTo;

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeToListenTo).AddUObject(ListenTask, &UAsyncTaskListenAttributeChanged::HandleOnAttributeChanged);

	return ListenTask;
}

UAsyncTaskListenAttributeChanged* UAsyncTaskListenAttributeChanged::ListenForAttributesChange(
	UAbilitySystemComponent* AbilitySystemComponent, TArray<FGameplayAttribute> AttributesToListenTo)
{
	if (!IsValid(AbilitySystemComponent) || AttributesToListenTo.Num() < 1)
	{
		return nullptr;
	}

	UAsyncTaskListenAttributeChanged* ListenTask = NewObject<UAsyncTaskListenAttributeChanged>();

	ListenTask->AbilitySystemComp = AbilitySystemComponent;
	ListenTask->AttributesToListenFor = AttributesToListenTo;

	for(FGameplayAttribute Attribute : AttributesToListenTo)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(ListenTask, &UAsyncTaskListenAttributeChanged::HandleOnAttributeChanged);
	}

	return ListenTask;
}

void UAsyncTaskListenAttributeChanged::EndTask()
{
	if(IsValid(AbilitySystemComp))
	{
		AbilitySystemComp->GetGameplayAttributeValueChangeDelegate(AttributeToListenFor).RemoveAll(this);
	}

	SetReadyToDestroy();
	MarkAsGarbage();
}

void UAsyncTaskListenAttributeChanged::HandleOnAttributeChanged(const FOnAttributeChangeData& Data)
{
	OnAttributeChangedDelegate.Broadcast(Data.Attribute, Data.NewValue, Data.OldValue);
}


