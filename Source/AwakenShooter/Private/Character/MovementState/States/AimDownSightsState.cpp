// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MovementState/States/AimDownSightsState.h"

#include "InputActionValue.h"
#include "AbilitySystem/Tasks/GTAimTransition.h"
#include "Character/ASCharacter.h"
#include "Character/MovementState/MovementStateComponent.h"
#include "Items/Gun.h"

UAimDownSightsState::UAimDownSightsState() :
	ADSOffset(FVector::ZeroVector)
{
	StateID = EMovementState::ADS;
	NextState = EMovementState::Walking;
}

void UAimDownSightsState::OnEnterState_Implementation()
{
	Super::OnEnterState_Implementation();
	CachedSkeletalMeshLocation = Character->GetFirstPersonMesh()->GetRelativeLocation();
	UGameplayTask* AimTask = UGTAimTransition::Create(Character->GetGameplayTasksComponent(), ADSOffset, .4f);
	UE_LOG(LogTemp, Warning, TEXT("ADS GunForward: %s"), *Character->GetEquippedGun()->GetActorRotation().ToString());
	UE_LOG(LogTemp, Warning, TEXT("ADS CharacterForward: %s"), *Character->GetActorRotation().ToString());
	
	AimTask->ReadyForActivation();
}

void UAimDownSightsState::OnExitState_Implementation()
{
	Super::OnExitState_Implementation();

	UGameplayTask* AimTask = UGTAimTransition::Create(Character->GetGameplayTasksComponent(), CachedSkeletalMeshLocation, .4f);
	AimTask->ReadyForActivation();
}

void UAimDownSightsState::HandleADS(const FInputActionValue& Value)
{
	float InputValue = Value.Get<float>();
	if (InputValue < .5f)
	{
		StateMachine->NextState();
	}
}
