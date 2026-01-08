// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BotTaskBase.h"
#include "BotTask_IdleRoam.generated.h"

/**
 * 
 */
UCLASS()
class AWAKENSHOOTER_API UBotTask_IdleRoam : public UBotTaskBase
{
	GENERATED_BODY()

	FVector TargetLocation;
	float SearchRadius;
	
public:
	static UBotTask_IdleRoam* Create(TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner, float RadiusToSearch = 500.f);
	
	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;
};
