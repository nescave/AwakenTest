// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MovementState/MovementStateComponent.h"

#include "AwakenShooter.h"
#include "Character/ASCharacter.h"
#include "Character/MovementState/MovementStateBase.h"


UMovementStateComponent::UMovementStateComponent() :
	bInitialized(false)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

}

void UMovementStateComponent::InitializeStates()
{
	StateMap.Empty();
	AASCharacter* Character = Cast<AASCharacter>(GetOwner());
	for (const auto& StateClass : PossibleMovementStates)
	{
		if (StateClass)
		{
			if (UMovementStateBase* NewState = NewObject<UMovementStateBase>(this, StateClass))
			{
				NewState->Initialize(this, Character);
				StateMap.Add(NewState->GetStateID(), NewState);
			}
		}
	}
	PrimaryComponentTick.SetTickFunctionEnable(ChangeState(EMovementState::Walking));
	bInitialized = true;
}

bool UMovementStateComponent::ChangeState(EMovementState NewState, bool bForce, bool bTriggerEvent)
{
	if (CurrentState)
	{
		if ( (GetCurrentStateID() == NewState || CurrentState->IsLocked()) && !bForce)
		{
			UE_LOG(LogAwakenShooter, Warning, TEXT("Movement state wasn't changed but it is still in a valid state"));
			PrimaryComponentTick.SetTickFunctionEnable(true);
			return true;
		}
		PreviousState = GetCurrentStateID();
	}

	if (PreviousState != EMovementState::NONE)
	{
		CurrentState->OnExitState();
	}
	
	CurrentState = StateMap.FindRef(NewState);
	if (CurrentState)
	{
		CurrentState->OnEnterState();
	}
	else
	{
		const FString StateName = StaticEnum<EMovementState>()->GetNameByValue(static_cast<int64>(NewState)).ToString();
		UE_LOG(LogAwakenShooter, Error, TEXT("Failed to find movement state %s"), *StateName);
		PrimaryComponentTick.SetTickFunctionEnable(false);
		return false;
	}
	
	if (bTriggerEvent)
	{
		OnMovementStateChanged.Broadcast(PreviousState, NewState);
	}
	PrimaryComponentTick.SetTickFunctionEnable(true);
	return true;
}

// Called every frame
void UMovementStateComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                            FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!CurrentState || !CurrentState->HasTickEnabled())
		return;
	
	CurrentState->OnStateTick(DeltaTime);
}

void UMovementStateComponent::NextState()
{
	if (!CurrentState)
	{
		UE_LOG(LogAwakenShooter, Warning, TEXT("Called NextState() when movement is in NONE state"));
		return;
	}
	ChangeState(CurrentState->GetNextState());
}

void UMovementStateComponent::HandleLook(const FInputActionValue& Value)
{
	if (!CurrentState)
		return;
	CurrentState->HandleLook(Value);
}

void UMovementStateComponent::HandleMove(const FInputActionValue& Value)
{
	if (!CurrentState)
		return;
	CurrentState->HandleMove(Value);
}

void UMovementStateComponent::HandleJump(const FInputActionValue& Value)
{
	if (!CurrentState)
		return;
	CurrentState->HandleJump(Value);
}

void UMovementStateComponent::HandleCrouch(const FInputActionValue& Value)
{
	if (!CurrentState)
		return;
	CurrentState->HandleCrouch(Value);
}

void UMovementStateComponent::HandleSprint(const FInputActionValue& Value)
{
	if (!CurrentState)
		return;
	CurrentState->HandleSprint(Value);
}

void UMovementStateComponent::HandleWallRun(const FInputActionValue& Value)
{
	if (!CurrentState)
		return;
	CurrentState->HandleWallRun(Value);
}

void UMovementStateComponent::HandleGunMain(const FInputActionValue& Value)
{
	if (!CurrentState)
		return;
	CurrentState->HandleGunMain(Value);
}

void UMovementStateComponent::HandleGunSecondary(const FInputActionValue& Value)
{
	if (!CurrentState)
		return;
	CurrentState->HandleGunSecondary(Value);
}

void UMovementStateComponent::HandleThrow(const FInputActionValue& Value)
{
	if (!CurrentState)
		return;
	CurrentState->HandleThrow(Value);
}

void UMovementStateComponent::HandleReload(const FInputActionValue& Value)
{
	if (!CurrentState)
		return;
	CurrentState->HandleReload(Value);
}

EMovementState UMovementStateComponent::GetCurrentStateID() const
{
	if (!CurrentState)
		return EMovementState::NONE;
	return CurrentState->GetStateID();
}

EMovementState UMovementStateComponent::GetPreviousStateID() const
{
	return PreviousState;
}

