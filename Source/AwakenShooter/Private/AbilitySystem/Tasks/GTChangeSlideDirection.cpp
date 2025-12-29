// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Tasks/GTChangeSlideDirection.h"

#include "Character/ASCharacter.h"
#include "Character/MovementState/States/SlidingState.h"
#include "GameFramework/CharacterMovementComponent.h"

UGTChangeSlideDirection::UGTChangeSlideDirection(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer),
	TargetDirection(),
	SlidingState(nullptr)
{
	bTickingTask = true;
}

UGTChangeSlideDirection* UGTChangeSlideDirection::Create(const TScriptInterface<IGameplayTaskOwnerInterface>& TaskOwner,
	const FVector& TargetDirection, USlidingState* SlidingState, float TransitionDuration)
{
	UGTChangeSlideDirection* Task = NewTask<UGTChangeSlideDirection>(TaskOwner);
	Task->TargetDirection = TargetDirection;
	Task->SlidingState = SlidingState;
	Task->TransitionDuration = FMath::Max(TransitionDuration, KINDA_SMALL_NUMBER);
	return Task;
}

void UGTChangeSlideDirection::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);
	ElapsedTime += DeltaTime;
	float LerpFactor = FMath::Min(ElapsedTime / TransitionDuration, 1.f);

	AASCharacter* Character = SlidingState->GetCharacter();
	FVector TargetVelocityDirection =
		FMath::Lerp(Character->GetCharacterMovement()->Velocity.GetSafeNormal(), TargetDirection, LerpFactor);

	float VelocityMagnitude = Character->GetCharacterMovement()->Velocity.Length();
	Character->GetCharacterMovement()->Velocity = TargetVelocityDirection * VelocityMagnitude;
	
	if (LerpFactor >= 1.f)
	{
		EndTask();
	}
}
