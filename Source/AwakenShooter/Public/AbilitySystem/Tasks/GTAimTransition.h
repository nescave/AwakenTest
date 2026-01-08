// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTask.h"
#include "GTAimTransition.generated.h"

/**
 * 
 */
UCLASS()
class AWAKENSHOOTER_API UGTAimTransition : public UGameplayTask
{
	GENERATED_BODY()

	FVector TargetLocation;
	UPROPERTY()
	class AASCharacter* ControlledCharacter;
	float TaskDuration;
	float ElapsedTime;
	
public:
	UGTAimTransition(const FObjectInitializer& ObjectInitializer);
	static UGTAimTransition* Create(const TScriptInterface<IGameplayTaskOwnerInterface>& TaskOwner,
		const FVector& TargetLocation, float TaskDuration);

	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;
	virtual void ExternalCancel() override;
};
