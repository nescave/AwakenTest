// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/MovementState/MovementStateBase.h"
#include "DeathState.generated.h"

/**
 * 
 */
UCLASS()
class AWAKENSHOOTER_API UDeathState : public UMovementStateBase
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditDefaultsOnly, Category="Death")
	TArray<UMaterialInterface*> DecayMaterials;
	UPROPERTY(EditDefaultsOnly, Category="Death")
	float DeathDecayDuration;
	
public:
	UDeathState();

	virtual void OnEnterState_Implementation() override;
	
	virtual void HandleMove(const FInputActionValue& Value) override {}
	virtual void HandleLook(const FInputActionValue& Value) override {}
	virtual void HandleJump(const FInputActionValue& Value) override {}
	virtual void HandleReload(const FInputActionValue& Value) override {}
	virtual void HandleThrow(const FInputActionValue& Value) override {}
	virtual void HandleCrouch(const FInputActionValue& Value) override {}
	virtual void HandleSprint(const FInputActionValue& Value) override {}
	virtual void HandleGunMain(const FInputActionValue& Value) override {}
	virtual void HandleGunSecondary(const FInputActionValue& Value) override {}
};
