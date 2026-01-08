// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/MovementState/MovementStateBase.h"
#include "FallingState.generated.h"

/**
 * 
 */
UCLASS()
class AWAKENSHOOTER_API UFallingState : public UMovementStateBase
{
	GENERATED_BODY()

	bool bCatchWall = false;
	protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SlideProperties", meta=(AllowPrivateAccess))
	TSubclassOf<UGameplayEffect> FallDamageEffect;

public:
	UFallingState();
	virtual void Initialize(UMovementStateComponent* InStateMachine, AASCharacter* InCharacter) override;
	virtual void HandleLook(const FInputActionValue& Value) override;
	virtual void HandleCrouch(const FInputActionValue& Value) override;
	virtual void HandleSprint(const FInputActionValue& Value) override;

	virtual void OnEnterState_Implementation() override;
	virtual void OnExitState_Implementation() override;
	virtual void OnStateTick_Implementation(float DeltaTime) override;

	void SetCatchWall(bool bInCatchWall) {bCatchWall = bInCatchWall;}

protected:
	UFUNCTION()
	void OnFallHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	bool TryCatchWall();
	UPROPERTY()
	TArray<AActor*> ThisSlideHits;
};
