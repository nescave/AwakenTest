// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/MovementState/MovementStateBase.h"
#include "HangingState.generated.h"

/**
 * 
 */
UCLASS()
class AWAKENSHOOTER_API UHangingState : public UMovementStateBase
{
	GENERATED_BODY()

	float CachedGravityScale;
	float CachedAirControl;

	bool bCanWallRun;
	bool bIsWallRunning;
	
	FHitResult HangingPoint;
	bool bCatchedLedge;
	float HangingTime;
	int32 WallRunCount;
	
public:
	UHangingState();
	virtual void HandleMove(const FInputActionValue& Value) override;
	virtual void HandleJump(const FInputActionValue& Value) override;
	virtual void HandleCrouch(const FInputActionValue& Value) override;
	virtual void HandleSprint(const FInputActionValue& Value) override {}
	virtual void HandleWallRun(const FInputActionValue& Value) override;
	
	virtual void OnEnterState_Implementation() override;
	virtual void OnExitState_Implementation() override;
	virtual void OnStateTick_Implementation(float DeltaTime) override;

	virtual FVector GetJumpDirection() override;
	const FHitResult& GetHangingPoint() const {return HangingPoint;}
	void SetHangingPoint(const FHitResult& HitResult, bool bSweepCharacter = false);
	
private:
	void StartWallRun();	
	void StopWallRun();

	bool TryGetWallInDirection(FHitResult& OutHitResult, FVector Direction = FVector::ZeroVector);
};
