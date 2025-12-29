// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MovementState/States/HangingState.h"

#include "InputActionValue.h"
#include "AbilitySystem/GameplayTags.h"
#include "Camera/CameraComponent.h"
#include "Character/ASCharacter.h"
#include "Character/MovementState/MovementStateComponent.h"
#include "Character/MovementState/States/FallingState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "General/DebugCVars.h"


UHangingState::UHangingState() :
	CachedGravityScale(0.f),
	CachedAirControl(0.f),
	bCanWallRun(false),
	bIsWallRunning(false),
	bGrabbedLedge(false),
	HangingTime(0.f)
{
	StateID = EMovementState::Hanging;
	NextState = EMovementState::Falling;
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

	if (auto ASC = Character ? Character->GetAbilitySystemComponent() : nullptr)
	{
		// cancel reload ability
		FGameplayTagContainer CancelAbilitiesTags = FGameplayTagContainer(FGameplayTags::Ability_Reload);		
		ASC->CancelAbilities(&CancelAbilitiesTags);
	}
}

void UHangingState::OnExitState_Implementation()
{
	Super::OnExitState_Implementation();
	
	UCharacterMovementComponent* CPC = Character->GetCharacterMovement();
	CPC->SetMovementMode(MOVE_Falling);
}

void UHangingState::OnStateTick_Implementation(float DeltaTime)
{
	if (bGrabbedLedge)
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
			float HDotR = OutHitResult.Normal.Dot(HangingPoint.Normal);
			if (HDotR >= .75f && HDotR < .99f)
			{
				FRotator DeltaRotation = (OutHitResult.Normal.Rotation() - HangingPoint.Normal.Rotation()).GetNormalized();
				DeltaRotation.Roll = 0.f;

				if (FASCVars::ASDebugDraw && FASCVars::ASDrawWallRunDirection)
					DrawDebugDirectionalArrow(GetWorld(), OutHitResult.Location,
						OutHitResult.Location + Character->GetCharacterMovement()->Velocity *.1f, 1.f,
						FColor::Red, false, 5.f, 0.f, 1.f);
					
				Character->GetCharacterMovement()->Velocity = DeltaRotation.RotateVector(Character->GetCharacterMovement()->Velocity) *1.1f;
				SetHangingPoint(OutHitResult, false);

				if (FASCVars::ASDebugDraw && FASCVars::ASDrawWallRunDirection)
					DrawDebugDirectionalArrow(GetWorld(), OutHitResult.Location,
						OutHitResult.Location + Character->GetCharacterMovement()->Velocity *.1f, 1.f,
						FColor::Green, false, 5.f, 0.f, 1.f);
			}
			else if (HDotR < .75f)
			{
				StopWallRun();
			}
		}
		else
		{
			StopWallRun();
		}
	}
	else
	{
		if (!(FASCVars::ASDebugDraw && FASCVars::ASDrawWallRunDirection))
		{
			return;
		}
		FVector LookDirection = Character->GetCameraComponent()->GetComponentRotation().Vector();
		if (LookDirection.Dot(Character->GetActorUpVector()) > .7f) // Wall run up
		{
			FHitResult HitResult;
			if (!TryGetWallInDirection(HitResult, Character->GetActorUpVector()))
			{
				return;
			}
			DrawDebugDirectionalArrow(GetWorld(), HitResult.Location, HitResult.Location + Character->GetActorUpVector() * 30.f,
				1.f, FColor::Blue, false, 0.f, 0.f, 1.f);
		}

		FVector LeftVector = HangingPoint.Normal.Cross(Character->GetActorUpVector());
		
		if (LookDirection.Dot(LeftVector) > .7f) // Wall run left
		{
			FHitResult HitResult;
			if (!TryGetWallInDirection(HitResult, LeftVector))
			{
				return;
			}
			DrawDebugDirectionalArrow(GetWorld(), HitResult.Location, HitResult.Location + LeftVector * 30.f,
			1.f, FColor::Blue, false, 0.f, 0.f, 1.f);
		}
		
		FVector RightVector = HangingPoint.Normal.Cross(-Character->GetActorUpVector());
		if (LookDirection.Dot(RightVector) > .7f) // Wall run right
		{
			FHitResult HitResult;
			if (!TryGetWallInDirection(HitResult, RightVector))
			{
				return;
			}
			DrawDebugDirectionalArrow(GetWorld(), HitResult.Location, HitResult.Location + RightVector * 30.f,
			1.f, FColor::Blue, false, 0.f, 0.f, 1.f);
		}
	}
}

void UHangingState::HandleJump(const FInputActionValue& Value)
{
	const FVector& LookDirection = Character->GetCameraComponent()->GetForwardVector();
	if (LookDirection.Dot(HangingPoint.Normal) < -.1f
		&& bGrabbedLedge)
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
	if (FASCVars::ASDebugDraw && FASCVars::ASDrawWallCatchTests)
	{
		DrawDebugDirectionalArrow(GetWorld(), HangingPoint.Location, HangingPoint.Location + HangingPoint.Normal * 10.f,
			10.f, FColor::Blue, false, FASCVars::ASDebugDrawDuration, 0.f, 1.f);
	}
	FHitResult OutHit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Character);
	const FVector Start = HangingPoint.Location + FVector::UpVector * 25.f+ HangingPoint.Normal * 10.f;
	if (GetWorld()->LineTraceSingleByChannel(OutHit,
		Start,
		Start - HangingPoint.Normal * 35.f,
		 ECC_Visibility,
		 Params))
	{
		bGrabbedLedge = false;
	}else
	{
		bGrabbedLedge = true;
		if (FASCVars::ASDebugDraw && FASCVars::ASDrawWallCatchTests)
			DrawDebugLine(GetWorld(), Start, Start - HangingPoint.Normal * 35.f, FColor::Green, false,
				FASCVars::ASDebugDrawDuration, 0.f, 1.f);
	}
	
	if (bSweepCharacter)
		Character->SetActorLocation(HitResult.Location + HitResult.Normal * 50.f - Character->GetCameraComponent()->GetRelativeLocation());
}

void UHangingState::StartWallRun()
{
	bIsWallRunning = true;
	WallRunCount++;
}

void UHangingState::StopWallRun()
{
	bIsWallRunning = false;
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
	const FVector End = Start - HangingPoint.Normal * 70.f;
	
	if (FASCVars::ASDebugDraw && FASCVars::ASDrawDetailWallRun)
		DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 10.f, 0.f, 1.f);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Character);
			
	return GetWorld()->SweepSingleByChannel(
		OutHitResult, Start, End,
		FQuat::Identity, ECC_Visibility,
		FCollisionShape::MakeCapsule(10.f, 25.f), Params);
}

