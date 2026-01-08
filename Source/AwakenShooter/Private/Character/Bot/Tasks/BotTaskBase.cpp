// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Bot/Tasks/BotTaskBase.h"

UBotTaskBase::UBotTaskBase(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer),
	bExternalCancelRequested(false)
{
	bTickingTask = true;
}

void UBotTaskBase::OnDestroy(bool bInOwnerFinished)
{
	if (!bExternalCancelRequested)
	{
		OnBotTaskEnded.ExecuteIfBound();
	}
	
	Super::OnDestroy(bInOwnerFinished);
}

void UBotTaskBase::ExternalCancel()
{
	bExternalCancelRequested = true;
	EndTask();
	Super::ExternalCancel();
}
