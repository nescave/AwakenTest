// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Bot/Tasks/BotTaskBase.h"
#include "BotTask_AimAtTarget.generated.h"

/**
 * 
 */
UCLASS()
class AWAKENSHOOTER_API UBotTask_AimAtTarget : public UBotTaskBase
{
	GENERATED_BODY()

	UPROPERTY()
	AActor* TargetActor;
	
public:
	UBotTask_AimAtTarget(const FObjectInitializer& ObjectInitializer);
	static UBotTask_AimAtTarget* Create(TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner, AActor* InTargetActor);

	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;
private:
	UPROPERTY()
	ACharacter* BotCharacter;

};
