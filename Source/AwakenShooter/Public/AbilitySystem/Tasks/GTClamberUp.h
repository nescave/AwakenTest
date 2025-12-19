// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTask.h"
#include "GTClamberUp.generated.h"

/**
 * 
 */
UCLASS()
class AWAKENSHOOTER_API UGTClamberUp : public UGameplayTask
{
	GENERATED_BODY()

	FVector TargetLocation;
	UPROPERTY()
	class AASCharacter* ControlledCharacter;
	float TaskDuration;
	float ElapsedTime;
public:
	UGTClamberUp(const FObjectInitializer& ObjectInitializer);
	static UGTClamberUp* Create(const TScriptInterface<IGameplayTaskOwnerInterface>& TaskOwner,
		const FVector& TargetLocation, AASCharacter* Character, float TaskDuration);

protected:
	virtual void Activate() override;
	virtual void OnDestroy(bool AbilityEndedNormally) override;
	virtual void TickTask(float DeltaTime) override;
};
