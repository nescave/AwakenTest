// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/MovementState/MovementStateBase.h"
#include "AimDownSightsState.generated.h"

/**
 * 
 */
UCLASS()
class AWAKENSHOOTER_API UAimDownSightsState : public UMovementStateBase
{
	GENERATED_BODY()

	FVector CachedSkeletalMeshLocation;
protected:
	UPROPERTY(EditDefaultsOnly, Category="ADS")
	FVector ADSOffset;
public:
	UAimDownSightsState();
	virtual void OnEnterState_Implementation() override;
	virtual void OnExitState_Implementation() override;

	virtual void HandleADS(const FInputActionValue& Value) override;
};
