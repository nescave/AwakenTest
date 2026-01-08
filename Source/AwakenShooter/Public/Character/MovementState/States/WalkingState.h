// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/MovementState/MovementStateBase.h"
#include "WalkingState.generated.h"

/**
 * 
 */
UCLASS()
class AWAKENSHOOTER_API UWalkingState : public UMovementStateBase
{
	GENERATED_BODY()
	
public:
	UWalkingState();

	virtual void HandleReload(const FInputActionValue& Value) override;
	virtual void HandleCrouch(const FInputActionValue& Value) override;
};
