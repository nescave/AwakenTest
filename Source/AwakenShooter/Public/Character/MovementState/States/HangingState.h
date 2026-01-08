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
	
	TOptional<FHitResult> HangingPoint;
	FVector EnterVelocity;
	bool bGrabbedLedge;
	float HangingTime;
	int32 WallRunCount;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "WallRun")
	int32 WallRunLimit;
	UPROPERTY(EditDefaultsOnly, Category = "WallRun", meta=(ClampMin=0.f, ClampMax=1.f))
	float UpWallRunLookWeight;
	UPROPERTY(EditDefaultsOnly, Category = "WallRun", meta=(ClampMin=0.f, ClampMax=1.f))
	float SidesWallRunLookWeight;
	
public:
	UHangingState();
	
	virtual void OnEnterState_Implementation() override;
	virtual void OnExitState_Implementation() override;
	virtual void OnStateTick_Implementation(float DeltaTime) override;
	
	virtual void HandleMove(const FInputActionValue& Value) override;
	virtual void HandleJump(const FInputActionValue& Value) override;
	virtual void HandleCrouch(const FInputActionValue& Value) override;
	virtual void HandleSprint(const FInputActionValue& Value) override;
	virtual void HandleWallRun(const FInputActionValue& Value) override;

	virtual FVector GetJumpDirection() override;
	bool GetHangingPoint(FHitResult& OutHangingPoint) const;
	void SetHangingPoint(const FHitResult& HitResult);
	
private:
	bool StartWallRun(bool bResetPreviousVelocity = false);	
	void StopWallRun();

	bool TryGetWallInDirection(FHitResult& OutHitResult, FVector Direction = FVector::ZeroVector);
	FVector GetWallRunDirection();
	void AdjustVelocityToDirection(const FVector& Direction, bool bDiscardDownVelocity = true);	
};
