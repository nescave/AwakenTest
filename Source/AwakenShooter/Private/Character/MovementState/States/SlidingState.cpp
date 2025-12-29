// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MovementState/States/SlidingState.h"

#include "InputActionValue.h"
#include "AbilitySystem/GameplayTags.h"
#include "AbilitySystem/Tasks/GTChangeSlideDirection.h"
#include "Character/ASCharacter.h"
#include "Character/MovementState/MovementStateComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "General/DebugCVars.h"

USlidingState::USlidingState() :
	CachedBreakingFrictionFactor(0.f),
	CachedGroundFriction(0.f),
	CachedDeceleration(0.f),
	VelocitySquaredNeededToExitSlide(250.f),
	MoveForwardPressed(false),
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
	Character->AddControllerPitchInput(-LookAxisVector.Y);
	// Super::HandleLook(Value);

	const FVector VelocityDirection = Character->GetCharacterMovement()->Velocity.GetSafeNormal();
	const FVector2D VelocityDirection2D = FVector2D(VelocityDirection.X, VelocityDirection.Y).GetSafeNormal();
	const FVector CharacterDirection = Character->GetController()->GetControlRotation().Vector().GetSafeNormal();
	const FVector2D CharacterDirection2D = FVector2D(CharacterDirection.X, CharacterDirection.Y).GetSafeNormal();

	float SignedAngle = FMath::RadiansToDegrees(
		FMath::Atan2(
			FVector::CrossProduct(VelocityDirection, CharacterDirection).Z,
			FVector2D::DotProduct(VelocityDirection2D, CharacterDirection2D)
		));
	if ((LookAxisVector.X > 0.f && SignedAngle < 80) ||
		(LookAxisVector.X < 0.f && SignedAngle > -80))
	{
		Character->AddControllerYawInput(LookAxisVector.X);
	}
}

void USlidingState::HandleMove(const FInputActionValue& Value)
{
	FVector InputVector = FVector(Value.Get<FVector2D>(), 0.f);
	float SlideFrictionFactor = SlideBaseDecelerationFactor;
	MoveForwardPressed = InputVector.Y >= .5f;
	if (MoveForwardPressed == false)
	{
		SlideFrictionFactor = SlideStopDecelerationFactor;
		NextState = EMovementState::Crouching;
	}
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
	if (MoveForwardPressed && NextState != EMovementState::Sprinting)
	{
		NextState = EMovementState::Sprinting;
		VelocitySquaredNeededToExitSlide *= 2.f;
	}
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

	if (auto ASC = Character ? Character->GetAbilitySystemComponent() : nullptr)
	{
		// cancel reload ability
		FGameplayTagContainer CancelAbilitiesTags = FGameplayTagContainer(FGameplayTags::Ability_Reload);		
		ASC->CancelAbilities(&CancelAbilitiesTags);
	}
}

void USlidingState::OnExitState_Implementation()
{
	Super::OnExitState_Implementation();
	Character->GetCharacterMovement()->bUseSeparateBrakingFriction = false;
	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->GetCharacterMovement()->BrakingFrictionFactor = CachedBreakingFrictionFactor;
	Character->GetCharacterMovement()->GroundFriction = CachedGroundFriction;
	Character->GetCharacterMovement()->BrakingDecelerationWalking = CachedDeceleration;
	Character->GetCharacterMovement()->MaxWalkSpeed =  Character->GetCachedInternalMovementSpeed();
	NextState = EMovementState::Crouching;
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

	if (FASCVars::ASDebugDraw && FASCVars::ASDrawSlideVelocity)
	{
		const FVector Start = Character->GetActorLocation() + Character->GetActorForwardVector() * 50.f;
		DrawDebugDirectionalArrow(GetWorld(), Start, Start + Character->GetCharacterMovement()->Velocity*.01f,
			15.f, FColor::Blue, false, 0.f, 0.f, 1.f);
	}
}
