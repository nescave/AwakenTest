// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayTagContainer.h"
#include "Character/MovementState/MovementStateBase.h"
#include "SprintingState.generated.h"

/**
 * 
 */
UCLASS()
class AWAKENSHOOTER_API USprintingState : public UMovementStateBase
{
	GENERATED_BODY()

public:
	USprintingState();

	virtual void HandleMove(const FInputActionValue& Value) override;
	virtual void HandleSprint(const FInputActionValue& Value) override;
	virtual void HandleCrouch(const FInputActionValue& Value) override;
	
	virtual void OnEnterState_Implementation() override;
	virtual void OnExitState_Implementation() override;
	virtual void OnStateTick_Implementation(float DeltaTime) override;
};
