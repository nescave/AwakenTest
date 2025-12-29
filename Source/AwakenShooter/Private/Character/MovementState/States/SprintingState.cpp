// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MovementState/States/SprintingState.h"

#include "InputActionValue.h"
#include "AbilitySystem/GameplayTags.h"
#include "Character/ASCharacter.h"
#include "Character/MovementState/MovementStateComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

USprintingState::USprintingState()
{
	StateID = EMovementState::Sprinting;
	NextState = EMovementState::Walking;
	TargetCameraHeight = 30.f;
}

void USprintingState::HandleMove(const FInputActionValue& Value)
{
	Super::HandleMove(Value);
	FVector2D MovementVector = Value.Get<FVector2D>();
	if (MovementVector.Y <= 0.f)
	{
		StateMachine->NextState();
	}
}

void USprintingState::HandleSprint(const FInputActionValue& Value)
{
	StateMachine->NextState();
}

void USprintingState::HandleCrouch(const FInputActionValue& Value)
{
	Character->TryActivateAbilityByTag(FGameplayTags::Ability_Slide);
	StateMachine->ChangeState(EMovementState::Sliding);
}

void USprintingState::OnEnterState_Implementation()
{
	Super::OnEnterState_Implementation();
	if (auto ASC = Character ? Character->GetAbilitySystemComponent() : nullptr)
	{
		// cancel reload ability
		FGameplayTagContainer CancelAbilitiesTags = FGameplayTagContainer(FGameplayTags::Ability_Reload);		
		ASC->CancelAbilities(&CancelAbilitiesTags);
	}
}

void USprintingState::OnExitState_Implementation()
{
	Super::OnExitState_Implementation();
}

void USprintingState::OnStateTick_Implementation(float DeltaTime)
{
	Super::OnStateTick_Implementation(DeltaTime);
}
