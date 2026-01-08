// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BotTaskBase.h"
#include "BotTask_IdleWait.generated.h"

/**
 * 
 */
UCLASS()
class AWAKENSHOOTER_API UBotTask_IdleWait : public UBotTaskBase
{
	GENERATED_BODY()

	float WaitTime;
	float ElapsedTime;
public:
	UBotTask_IdleWait(const FObjectInitializer& ObjectInitializer);
	static UBotTask_IdleWait* CreateTask(TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner);

protected:
	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;

};
