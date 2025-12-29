// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/MovementState/MovementStateBase.h"
#include "SlidingState.generated.h"

/**
 * 
 */
UCLASS()
class AWAKENSHOOTER_API USlidingState : public UMovementStateBase
{
	GENERATED_BODY()

	float CachedBreakingFrictionFactor; // used for dragging current velocity
	float CachedGroundFriction; // used for dragging input acceleration
	float CachedDeceleration;
	FVector CachedVelocityDirection;
	float VelocitySquaredNeededToExitSlide;

	bool MoveForwardPressed;
	float LastRightInput;

	UPROPERTY()
	class UGTChangeSlideDirection* SlideDirectionTask;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SlideProperties", meta=(AllowPrivateAccess))
	float SlideBaseDecelerationFactor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SlideProperties", meta=(AllowPrivateAccess))
	float SlideStopDecelerationFactor;

public:
	USlidingState();

	virtual void HandleLook(const FInputActionValue& Value) override;
	virtual void HandleMove(const FInputActionValue& Value) override;
	virtual void HandleSprint(const FInputActionValue& Value) override;
	virtual void HandleCrouch(const FInputActionValue& Value) override {}

	virtual void OnEnterState_Implementation() override;
	virtual void OnExitState_Implementation() override;
	virtual void OnStateTick_Implementation(float DeltaTime) override;

private:
	float LockedLookDuration;
	bool bLookRotationIsLocked;
};
