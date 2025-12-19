// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MovementState/States/WalkingState.h"

UWalkingState::UWalkingState()
{
	StateID = EMovementState::Walking;
	NextState = EMovementState::Walking;
}
