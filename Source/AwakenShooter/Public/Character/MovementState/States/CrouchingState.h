// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "Character/MovementState/MovementStateBase.h"
#include "CrouchingState.generated.h"

/**
 * 
 */
UCLASS()
class AWAKENSHOOTER_API UCrouchingState : public UMovementStateBase
{
	GENERATED_BODY()

public:
	UCrouchingState();
	
	virtual void OnEnterState_Implementation() override;
	virtual void OnExitState_Implementation() override;

	virtual void HandleCrouch(const FInputActionValue& Value) override;
	virtual void HandleReload(const FInputActionValue& Value) override;
};
