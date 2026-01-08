// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MovementState/States/WalkingState.h"

#include "AbilitySystem/GameplayTags.h"
#include "Character/ASCharacter.h"

UWalkingState::UWalkingState()
{
	StateID = EMovementState::Walking;
	NextState = EMovementState::Walking;
}

void UWalkingState::HandleReload(const FInputActionValue& Value)
{
	if (auto ASC = Character ? Character->GetAbilitySystemComponent() : nullptr)
	{
		ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(FASGameplayTags::Ability::Reload));
	}
}

void UWalkingState::HandleCrouch(const FInputActionValue& Value)
{
	if (Character->GetVelocity().SquaredLength() > 400000.f)
	{
		Character->TryActivateAbilityByTag(FASGameplayTags::Ability::Slide);		
	}
	else
	{
		Super::HandleCrouch(Value);
	}
}
