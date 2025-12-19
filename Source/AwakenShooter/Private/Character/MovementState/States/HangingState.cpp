// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MovementState/States/HangingState.h"

#include "InputActionValue.h"
#include "AbilitySystem/GameplayTags.h"
#include "Camera/CameraComponent.h"
#include "Character/ASCharacter.h"
#include "Character/MovementState/MovementStateComponent.h"
#include "Character/MovementState/States/FallingState.h"
#include "GameFramework/CharacterMovementComponent.h"

UHangingState::UHangingState() :
	CachedGravityScale(0.f),
	CachedAirControl(0.f),
	bCanWallRun(false),
	bIsWallRunning(false),
	bCatchedLedge(false),
	HangingTime(0.f)
{
	StateID = EMovementState::Hanging;
	NextState = EMovementState::Falling;
}

void UHangingState::HandleMove(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	bCanWallRun = MovementVector.Y > .5f;
	if (bIsWallRunning)
	{
		if (!bCanWallRun)
		{
			StopWallRun();
		}
		FHitResult OutHitResult;
		if (TryGetWallInDirection(OutHitResult))
		{
			if (OutHitResult.Normal.Dot(HangingPoint.Normal) > .75f)
			{
				FRotator VelocityAdjustingRotation = FRotationMatrix::MakeFromX(HangingPoint.Normal - OutHitResult.Normal).Rotator(); 
				Character->GetCharacterMovement()->Velocity = VelocityAdjustingRotation.RotateVector(Character->GetCharacterMovement()->Velocity);
			}
			else
			{
				StopWallRun();
			}
		}
		else
		{
			StopWallRun();
		}
	}
}

void UHangingState::HandleJump(const FInputActionValue& Value)
{
	const FVector& LookDirection = Character->GetCameraComponent()->GetForwardVector();
	if (LookDirection.Dot(HangingPoint.Normal) < -.1f
		&& bCatchedLedge)
	{
		FHitResult OutHit;
		
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(Character);
		if (GetWorld()->SweepSingleByChannel(
			OutHit,
			Character->GetCameraComponent()->GetComponentLocation()+FVector(0,0,100.f),
			HangingPoint.Location - HangingPoint.Normal * 25.f,
			FQuat::Identity,
			ECC_Visibility,
			FCollisionShape::MakeCapsule(20.f, 20.f),
			Params))
		{
			if (OutHit.Normal.Dot(FVector::UpVector) > .5f)
			{
				Character->TryActivateAbilityByTag(FGameplayTags::Ability_ClamberUp);
			}
		}
		return;
	}
	if (GetJumpDirection().IsNearlyZero())
		return;
	Super::HandleJump(Value);
}

void UHangingState::HandleCrouch(const FInputActionValue& Value)
{
	StateMachine->NextState();
	if (UFallingState* FallingState = Cast<UFallingState>(StateMachine->GetCurrentState()))
	{
		FallingState->SetCatchWall(false);
	}
}

void UHangingState::HandleWallRun(const FInputActionValue& Value)
{
	if (!bCanWallRun)
	{
		return;
	}

	if (!bIsWallRunning && WallRunCount < 1)
	{
		FVector LookDirection = Character->GetCameraComponent()->GetComponentRotation().Vector();
		if (LookDirection.Dot(Character->GetActorUpVector()) > .7f) // Wall run up
		{
			FHitResult HitResult;
			if (!TryGetWallInDirection(HitResult, Character->GetActorUpVector()))
			{
				return;
			}
			Character->GetCharacterMovement()->Velocity += Character->GetActorUpVector() * 500.f;
			StartWallRun();
		}

		FVector LeftVector = HangingPoint.Normal.Cross(Character->GetActorUpVector());
		
		if (LookDirection.Dot(LeftVector) > .7f) // Wall run left
		{
			FHitResult HitResult;
			if (!TryGetWallInDirection(HitResult, LeftVector))
			{
				return;
			}
			Character->GetCharacterMovement()->Velocity += LeftVector * 700.f;
			StartWallRun();
		}
		
		FVector RightVector = HangingPoint.Normal.Cross(-Character->GetActorUpVector());
		if (LookDirection.Dot(RightVector) > .7f) // Wall run right
		{
			FHitResult HitResult;
			if (!TryGetWallInDirection(HitResult, RightVector))
			{
				return;
			}
			Character->GetCharacterMovement()->Velocity += RightVector * 700.f;
			StartWallRun();
		}
	}
}

void UHangingState::OnEnterState_Implementation()
{
	Super::OnEnterState_Implementation();

	bIsWallRunning = false;
	
	UCharacterMovementComponent* CPC = Character->GetCharacterMovement();
	CPC->StopMovementImmediately();
	CPC->SetMovementMode(MOVE_Flying);
	CPC->Velocity = FVector::ZeroVector;

	HangingTime = 0.f;
	WallRunCount = 0;
	
	if (GetGroundDistance() < 100.f)
	{
		StateMachine->NextState();
	}
	SetTickEnabled(true);
}

void UHangingState::OnExitState_Implementation()
{
	Super::OnExitState_Implementation();
	
	UCharacterMovementComponent* CPC = Character->GetCharacterMovement();
	CPC->SetMovementMode(MOVE_Falling);
	// CPC->GravityScale = CachedGravityScale;
	// CPC->AirControl = CachedAirControl;
}

void UHangingState::OnStateTick_Implementation(float DeltaTime)
{
	if (bCatchedLedge)
	{
		HangingTime = 0.f;
	}
	else
	{
		HangingTime += DeltaTime;
	}

	if (HangingTime > 1.f)
	{
		Character->GetCharacterMovement()->Velocity -= Character->GetActorUpVector() * 2500.f * FMath::Pow(DeltaTime, 1.4f);
	}
	if (GetGroundDistance() < 100.f)
	{
		HandleCrouch(FInputActionValue());
	}
}

FVector UHangingState::GetJumpDirection()
{
	FVector LookDirection = Character->GetController()->GetControlRotation().Vector();

	if (HangingPoint.Normal.Dot(LookDirection) >= -.1f)
	{
		return (LookDirection + Character->GetActorUpVector()+ HangingPoint.Normal).GetSafeNormal();
	}
	return FVector::ZeroVector;
}

void UHangingState::SetHangingPoint(const FHitResult& HitResult, bool bSweepCharacter)
{
	HangingPoint = HitResult;
	// DrawDebugSphere(GetWorld(), HitResult.Location, 10.f, 7, FColor::Red, false, 5.f);
	DrawDebugDirectionalArrow(GetWorld(), HangingPoint.Location, HangingPoint.Location + HangingPoint.Normal * 10.f,
		10.f, FColor::Blue, false, 5.f, 0.f, 2.f);
	FHitResult OutHit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Character);
	const FVector Start = HangingPoint.Location + FVector::UpVector * 20.f+ HangingPoint.Normal * 10.f;
	DrawDebugLine(GetWorld(), Start, Start - HangingPoint.Normal * 35.f, FColor::Green, false, 5.f);
	if (GetWorld()->LineTraceSingleByChannel(OutHit,
		Start,
		Start - HangingPoint.Normal * 35.f,
		 ECC_Visibility,
		 Params))
	{
		bCatchedLedge = false;
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Ledge detected"));
	}else
	{
		bCatchedLedge = true;
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Ledge not detected"));
	}
	
	if (bSweepCharacter)
		Character->SetActorLocation(HitResult.Location + HitResult.Normal * 50.f - Character->GetCameraComponent()->GetRelativeLocation());
}

void UHangingState::StartWallRun()
{
	bIsWallRunning = true;
	WallRunCount++;
	// Character->GetCharacterMovement()->GravityScale = CachedGravityScale*.2f;
	// Character->GetCharacterMovement()->AirControl = CachedAirControl;
}

void UHangingState::StopWallRun()
{
	bIsWallRunning = false;
	// Character->GetCharacterMovement()->GravityScale = 0.f;
	// Character->GetCharacterMovement()->AirControl = 0.f;
	Character->GetCharacterMovement()->StopMovementImmediately();
	Character->GetCharacterMovement()->Velocity = FVector::ZeroVector;
	FHitResult OutHit;
	if (Character->TryFindWall(OutHit))
	{
		SetHangingPoint(OutHit, true);
		HangingTime = 0.f;
	}
}

bool UHangingState::TryGetWallInDirection(FHitResult& OutHitResult, FVector Direction)
{
	if (Direction.IsNearlyZero())
	{
		Direction = Character->GetMovementComponent()->Velocity.GetSafeNormal();
	}
	float CheckRange = 40.f;
	if (Direction.Dot(Character->GetActorUpVector()) > .7f)
	{
		CheckRange = 15.f;
	}
	const FVector Start = Character->GetCameraComponent()->GetComponentLocation() + Direction * CheckRange;
	const FVector End = Start - HangingPoint.Normal * 50.f;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Character);
			
	return GetWorld()->SweepSingleByChannel(
		OutHitResult, Start, End,
		FQuat::Identity, ECC_Visibility,
		FCollisionShape::MakeCapsule(10.f, 10.f), Params);
}

