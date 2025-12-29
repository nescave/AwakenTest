// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MovementStateBase.h"
#include "Components/ActorComponent.h"
#include "MovementStateComponent.generated.h"

class UMovementStateBase;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnMovementStateChanged,
	EMovementState, PreviousState,
	EMovementState, NewState
);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class AWAKENSHOOTER_API UMovementStateComponent : public UActorComponent
{
	GENERATED_BODY()
	
	bool bInitialized;
	EMovementState PreviousState;
protected:
	UPROPERTY(BlueprintAssignable)
	FOnMovementStateChanged OnMovementStateChanged;
	UPROPERTY()
	TMap<EMovementState, UMovementStateBase*> StateMap;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Movement State")
	TArray<TSubclassOf<UMovementStateBase>> PossibleMovementStates;

	UPROPERTY()
	TObjectPtr<UMovementStateBase> CurrentState;

public:
	UMovementStateComponent();

	void InitializeStates();
	/* State control */
	bool ChangeState(EMovementState NewState, bool bForce = false,  bool bTriggerEvent = false);
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void NextState();

	/* Input forwarding */
	void HandleLook(const FInputActionValue& Value);
	void HandleMove(const FInputActionValue& Value);
	void HandleJump(const FInputActionValue& Value);
	void HandleCrouch(const FInputActionValue& Value);
	void HandleSprint(const FInputActionValue& Value);
	void HandleWallRun(const FInputActionValue& Value);
	void HandleGunMain(const FInputActionValue& Value);
	void HandleGunSecondary(const FInputActionValue& Value);
	void HandleThrow(const FInputActionValue& Value);
	void HandleReload(const FInputActionValue& Value);

	UMovementStateBase* GetCurrentState() const { return CurrentState; }
	FOnMovementStateChanged& OnMovementStateChangedDelegate() { return OnMovementStateChanged; }
	EMovementState GetCurrentStateID() const;
	EMovementState GetPreviousStateID() const;
	bool IsInitialized() const { return bInitialized; }
};
