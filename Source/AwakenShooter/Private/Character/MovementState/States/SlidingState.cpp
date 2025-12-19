// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MovementState/States/SlidingState.h"

#include "InputActionValue.h"
#include "AbilitySystem/Tasks/GTChangeSlideDirection.h"
#include "Character/ASCharacter.h"
#include "Character/MovementState/MovementStateComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

USlidingState::USlidingState() :
	CachedBreakingFrictionFactor(0.f),
	CachedGroundFriction(0.f),
	CachedDeceleration(0.f),
	VelocitySquaredNeededToExitSlide(250.f),
	LastRightInput(0.f),
	SlideBaseDecelerationFactor(.1f),
	SlideStopDecelerationFactor(1.f)
{
	StateID = EMovementState::Sliding;
	NextState = EMovementState::Crouching;
	bTickEnabled = true;
}

void USlidingState::HandleLook(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	FRotator LookDeltaRotation = FRotator(-LookAxisVector.Y, LookAxisVector.X, 0.f);
	FRotator CurrentLookRotation = Character->GetController()->GetControlRotation();
	
	FRotator NewLookRotation = CurrentLookRotation + LookDeltaRotation;
	NewLookRotation.Normalize();
	
	FVector NewLookDirection = NewLookRotation.Vector();
	
	if (NewLookDirection.Dot(CachedVelocityDirection) < .25f)
	{
		if (LockedLookDuration <= 0.f)
		{
			bLookRotationIsLocked = true;
		}
		return; // reject direction
	}
	if (LockedLookDuration > 0.f)
	{
		LockedLookDuration = 0.f;
		bLookRotationIsLocked = false;
	}
	Super::HandleLook(Value);
}

void USlidingState::HandleMove(const FInputActionValue& Value)
{
	FVector InputVector = FVector(Value.Get<FVector2D>(), 0.f);
	float SlideFrictionFactor = FMath::Lerp(SlideStopDecelerationFactor, SlideBaseDecelerationFactor, FMath::Clamp(InputVector.Y, 0.f, 1.f));
	Character->GetCharacterMovement()->BrakingDecelerationWalking = CachedDeceleration * SlideFrictionFactor;

	if (FMath::IsNearlyEqual(InputVector.X, LastRightInput)
		|| CachedVelocityDirection.IsNearlyZero())
	{
		return;
	}
	LastRightInput = InputVector.X;

	if (SlideDirectionTask)
		SlideDirectionTask->ExternalCancel();

	FVector NewVelocityDirection;
	if (!FMath::IsNearlyZero(InputVector.X))
	{
		FVector RightVector = Character->GetActorUpVector().Cross(CachedVelocityDirection);
		NewVelocityDirection = ((RightVector * InputVector.X + CachedVelocityDirection)*.5f).GetSafeNormal();
	}
	else
	{
		NewVelocityDirection = CachedVelocityDirection;
	}
	
	SlideDirectionTask = UGTChangeSlideDirection::Create(
		Character->GetGameplayTasksComponent(),
		NewVelocityDirection,
		this,
		1.f );
	
	SlideDirectionTask->ReadyForActivation();
}

void USlidingState::HandleSprint(const FInputActionValue& Value)
{
	NextState = EMovementState::Sprinting;
	VelocitySquaredNeededToExitSlide *= 2.f;
}

void USlidingState::OnEnterState_Implementation()
{
	Super::OnEnterState_Implementation();
	
	Character->GetCharacterMovement()->bUseSeparateBrakingFriction = true;
	CachedBreakingFrictionFactor = Character->GetCharacterMovement()->BrakingFrictionFactor;
	CachedGroundFriction = Character->GetCharacterMovement()->GroundFriction;
	CachedDeceleration = Character->GetCharacterMovement()->BrakingDecelerationWalking;
	CachedVelocityDirection = Character->GetCharacterMovement()->Velocity.GetSafeNormal();
	VelocitySquaredNeededToExitSlide = FMath::Square(Character->GetCachedInternalMovementSpeed()*.5f);

	
	Character->GetCharacterMovement()->BrakingFrictionFactor = 1.f;
	Character->GetCharacterMovement()->GroundFriction = 0.1f;
	Character->GetCharacterMovement()->BrakingDecelerationWalking = CachedDeceleration * SlideBaseDecelerationFactor;
	Character->GetCharacterMovement()->MaxWalkSpeed = Character->GetCachedInternalMovementSpeed()*.25f;
}

void USlidingState::OnExitState_Implementation()
{
	Super::OnExitState_Implementation();
	Character->GetCharacterMovement()->bUseSeparateBrakingFriction = false;
	Character->GetCharacterMovement()->BrakingFrictionFactor = CachedBreakingFrictionFactor;
	Character->GetCharacterMovement()->GroundFriction = CachedGroundFriction;
	Character->GetCharacterMovement()->BrakingDecelerationWalking = CachedDeceleration;
	Character->GetCharacterMovement()->MaxWalkSpeed =  Character->GetCachedInternalMovementSpeed();

	if (SlideDirectionTask)
		SlideDirectionTask->ExternalCancel();
}

void USlidingState::OnStateTick_Implementation(float DeltaTime)
{
	Super::OnStateTick_Implementation(DeltaTime);
	
	if (Character->GetCharacterMovement()->Velocity.SizeSquared() < FMath::Square(Character->GetCachedInternalMovementSpeed()*.5f))
	{
		StateMachine->NextState();
	}
	if (bLookRotationIsLocked)
	{
		UpdateBlockedRotation(DeltaTime);
	}
}

void USlidingState::UpdateBlockedRotation(float DeltaTime)
{
	LockedLookDuration += DeltaTime;
	float LerpFactor = FMath::Clamp(LockedLookDuration / 1.f, 0.f, 1.f);
	const FQuat FromQuat = Character->GetController()->GetControlRotation().Quaternion();
	const FQuat ToQuat   = CachedVelocityDirection.ToOrientationRotator().Quaternion();

	const FVector ResultDirection = FQuat::Slerp(FromQuat, ToQuat, LerpFactor).Vector();
	const FRotator Result = FRotationMatrix::MakeFromXZ(ResultDirection, FVector::UpVector).Rotator();

	Character->GetController()->SetControlRotation(Result);
}
