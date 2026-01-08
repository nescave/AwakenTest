// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Bot/Tasks/BotTask_IdleWait.h"

UBotTask_IdleWait::UBotTask_IdleWait(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer),
	WaitTime(2.f),
	ElapsedTime(0.f)
{}

UBotTask_IdleWait* UBotTask_IdleWait::CreateTask(TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner)
{
	return NewBotTask<UBotTask_IdleWait>(TaskOwner);
}

void UBotTask_IdleWait::Activate()
{
	WaitTime = FMath::RandRange(2.f, 8.f);	
}

void UBotTask_IdleWait::TickTask(float DeltaTime)
{
	ElapsedTime += DeltaTime;

	if (ElapsedTime >= WaitTime)
	{
		EndTask();
	}
}
