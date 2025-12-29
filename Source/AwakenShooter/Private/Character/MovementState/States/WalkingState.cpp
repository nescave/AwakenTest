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
		ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(FGameplayTags::Ability_Reload));
	}
}
