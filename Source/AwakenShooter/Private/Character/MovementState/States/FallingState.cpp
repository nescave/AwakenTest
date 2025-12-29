// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MovementState/States/FallingState.h"

#include "AbilitySystem/GameplayTags.h"
#include "Character/ASCharacter.h"
#include "Character/MovementState/MovementStateComponent.h"
#include "Character/MovementState/States/HangingState.h"
#include "GameFramework/CharacterMovementComponent.h"

UFallingState::UFallingState() :
	bCatchWall(true)
{
	StateID = EMovementState::Falling;
	NextState = EMovementState::Walking;
}

void UFallingState::HandleLook(const FInputActionValue& Value)
{
	Super::HandleLook(Value);
}

void UFallingState::HandleCrouch(const FInputActionValue& Value)
{
	bCatchWall = true;
}

void UFallingState::OnEnterState_Implementation()
{
	Super::OnEnterState_Implementation();
	SetTickEnabled(true);
	
	if (auto ASC = Character ? Character->GetAbilitySystemComponent() : nullptr)
	{
		// cancel reload ability
		FGameplayTagContainer CancelAbilitiesTags = FGameplayTagContainer(FASGameplayTags::Ability_Reload);		
		ASC->CancelAbilities(&CancelAbilitiesTags);
	}
}

void UFallingState::OnExitState_Implementation()
{
	Super::OnExitState_Implementation();

	bCatchWall = true;
}

void UFallingState::OnStateTick_Implementation(float DeltaTime)
{
	if (Character->GetCharacterMovement()->Velocity.Z < 0.f
		&& GetGroundDistance() > 100.f
		&& bCatchWall)
	{
		TryCatchWall();
	}
}

bool UFallingState::TryCatchWall()
{
	FHitResult HitResult;
	
	if (Character->TryFindWall(HitResult))
	{
		StateMachine->ChangeState(EMovementState::Hanging);
		if (UHangingState* HangingState = Cast<UHangingState>(StateMachine->GetCurrentState()))
		{ //CanChangeIntoState would be a much cleaner call that would fix tje issue of Hanging state that immediately ends on the same frame
			HangingState->SetHangingPoint(HitResult);
		}
		return true;
	}
	return false;
}


