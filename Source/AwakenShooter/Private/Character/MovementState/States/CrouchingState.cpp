// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MovementState/States/CrouchingState.h"

#include "AbilitySystem/GameplayTags.h"
#include "Character/ASCharacter.h"
#include "Character/MovementState/MovementStateComponent.h"

UCrouchingState::UCrouchingState()
{
	StateID = EMovementState::Crouching;
	NextState = EMovementState::Walking;
	TargetCameraHeight = 40.f;
	TargetCapsuleHalfHeight = 30.f;
}

void UCrouchingState::OnEnterState_Implementation()
{
	Super::OnEnterState_Implementation();
}

void UCrouchingState::OnExitState_Implementation()
{
	Super::OnExitState_Implementation();
}

void UCrouchingState::HandleCrouch(const FInputActionValue& Value)
{
	Super::HandleCrouch(Value);
	StateMachine->NextState();
}

void UCrouchingState::HandleReload(const FInputActionValue& Value)
{
	if (auto ASC = Character ? Character->GetAbilitySystemComponent() : nullptr)
	{
		ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(FASGameplayTags::Ability_Reload));
	}
}
