// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MovementState/States/HangingState.h"

#include "InputActionValue.h"
#include "AbilitySystem/GameplayTags.h"
#include "Camera/CameraComponent.h"
#include "Character/ASPlayerCharacter.h"
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
	HangingTime(0.f),
	WallRunLimit(1),
	UpWallRunLookWeight(.8f),
	SidesWallRunLookWeight(.35f)
{
	StateID = EMovementState::Hanging;
	NextState = EMovementState::Falling;
}


void UHangingState::OnEnterState_Implementation()
{
	Super::OnEnterState_Implementation();

	
	HangingTime = 0.f;
	WallRunCount = 0;
	bIsWallRunning = false;
	
	UCharacterMovementComponent* CMC = Character->GetCharacterMovement();

	CMC->SetMovementMode(MOVE_Flying);
	if (HangingPoint.IsSet())
	{
		if (!Character->GetAbilitySystemComponent()->HasMatchingGameplayTag(FASGameplayTags::MovementState::Sprinting))
		{
			CMC->StopMovementImmediately();
			CMC->Velocity = FVector::ZeroVector;
		}
		else
		{
			float VelocityMag = CMC->Velocity.Size();
			if (VelocityMag > 1000.f)
			{
				EnterVelocity = CMC->Velocity * 600.f / VelocityMag;		
			}
			else
			{
				EnterVelocity = CMC->Velocity;
			}
			if (!StartWallRun())
			{
				CMC->StopMovementImmediately();
				CMC->Velocity = FVector::ZeroVector;
			}
		}
	}
	else
	{
		CMC->StopMovementImmediately();
		CMC->Velocity = FVector::ZeroVector;
	}

	EnterVelocity = FVector::ZeroVector;
	
	SetTickEnabled(true);
}

void UHangingState::OnExitState_Implementation()
{
	Super::OnExitState_Implementation();
	
	UCharacterMovementComponent* CPC = Character->GetCharacterMovement();
	EnterVelocity = FVector::ZeroVector;
	HangingPoint.Reset();
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
		Character->GetCharacterMovement()->Velocity -= Character->GetActorUpVector() * 2000.f * FMath::Pow(DeltaTime, 1.4f);
	}
	if (Character->GetGroundDistance() < 200.f)
	{
		StateMachine->NextState();
	}
}

void UHangingState::HandleMove(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	bCanWallRun = MovementVector.Y > .5f;

	if (!bCanWallRun)
	{
		StopWallRun();
	}
	if (bIsWallRunning)
	{
		FHitResult OutHitResult;
		if (TryGetWallInDirection(OutHitResult))
		{
			float HDotR = OutHitResult.Normal.Dot(HangingPoint.GetValue().Normal);
			SetHangingPoint(OutHitResult);
			if (HDotR >= .55f && HDotR < .99f)
			{
				const FVector WallNormal = OutHitResult.Normal;
				const FVector CharacterVelocityDirection = Character->GetVelocity().GetSafeNormal();
				const FVector WallRunHelperVec = CharacterVelocityDirection.Cross(WallNormal).GetSafeNormal();
				const FVector WallRunProperDirection = WallNormal.Cross(WallRunHelperVec).GetSafeNormal();

				if (FASCVars::ASDebugDraw && FASCVars::ASDrawDetailWallRun)
					DrawDebugDirectionalArrow(GetWorld(), Character->GetActorLocation(),
						Character->GetActorLocation() + WallRunProperDirection * 25.f, 3.f, FColor::Green,
						false, 10.f, 0.f, 1.f);
			
				Character->GetCharacterMovement()->Velocity = WallRunProperDirection * Character->GetVelocity().Size();
			}
		}
		else
		{
			StopWallRun();
		}
	}
	else
	{
		const FVector WallRunDir = GetWallRunDirection();
		if (FASCVars::ASDebugDraw && FASCVars::ASDrawWallRunDirection)
		{
			DrawDebugDirectionalArrow(GetWorld(), HangingPoint.GetValue().Location + WallRunDir * 30.f, HangingPoint.GetValue().Location + WallRunDir * 50.f,
			1.f, FColor::Blue, false, 0.f, 0.f, 1.f);
		}
	}

}

void UHangingState::HandleJump(const FInputActionValue& Value)
{
	const FVector& LookDirection = Character->GetCameraComponent()->GetForwardVector();
	if (LookDirection.Dot(HangingPoint.GetValue().Normal) < -.1f
		&& bGrabbedLedge)
	{
		FHitResult OutHit;
		
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(Character);
		if (GetWorld()->SweepSingleByChannel(
			OutHit,
			Character->GetCameraComponent()->GetComponentLocation()+FVector(0,0,100.f),
			HangingPoint.GetValue().Location - HangingPoint.GetValue().Normal * 25.f,
			FQuat::Identity,
			ECC_Visibility,
			FCollisionShape::MakeCapsule(20.f, 20.f),
			Params))
		{
			if (OutHit.Normal.Dot(FVector::UpVector) > .5f)
			{
				Character->TryActivateAbilityByTag(FASGameplayTags::Ability::ClamberUp);
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

void UHangingState::HandleSprint(const FInputActionValue& Value)
{
	float InputValue = Value.Get<float>();
	if (InputValue < .5f)
	{
		Character->TryCancelAbilitiesWithTag(FASGameplayTags::Ability::Sprint);
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
		StartWallRun();
	}
}

FVector UHangingState::GetJumpDirection()
{
	FVector LookDirection = Character->GetCameraComponent()->GetForwardVector();

	if (HangingPoint.GetValue().Normal.Dot(LookDirection) >= -.1f)
	{
		return (LookDirection + Character->GetActorUpVector()+ HangingPoint.GetValue().Normal).GetSafeNormal();
	}
	
	FHitResult OutHit;
	if (!TryGetWallInDirection(OutHit, LookDirection))
	{
		return LookDirection + Character->GetActorUpVector();
	}
	return FVector::ZeroVector;
}

bool UHangingState::GetHangingPoint(FHitResult& OutHangingPoint) const
{
	if (HangingPoint.IsSet())
	{
		OutHangingPoint = HangingPoint.GetValue();
		return true;
		
	}
	return false;
}

void UHangingState::SetHangingPoint(const FHitResult& HitResult)
{
	if (Character->GetGroundDistance() < 200.f)
	{
		StateMachine->NextState();
		return;
	}
	HangingPoint = HitResult;

	FHitResult OutHit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Character);
	const FVector Start = HangingPoint.GetValue().Location + FVector::UpVector * 25.f+ HangingPoint.GetValue().Normal * 10.f;
	if (GetWorld()->LineTraceSingleByChannel(OutHit,
		Start,
		Start - HangingPoint.GetValue().Normal * 35.f,
		 ECC_Visibility,
		 Params))
	{
		bGrabbedLedge = false;
	}else
	{
		bGrabbedLedge = true;
		if (FASCVars::ASDebugDraw && FASCVars::ASDrawWallCatchTests)
			DrawDebugLine(GetWorld(), Start, Start - HangingPoint.GetValue().Normal * 35.f, FColor::Green, false,
				FASCVars::ASDebugDrawDuration, 0.f, 1.f);
	}
}

bool UHangingState::StartWallRun(bool bResetPreviousVelocity)
{
	if (!OwnerIsPlayer())
		return false;
	
	if (bIsWallRunning || WallRunCount >= WallRunLimit)
		return bIsWallRunning = false;

	if (!HangingPoint.IsSet())
		return bIsWallRunning = false;

	const FVector WallRunDirection = GetWallRunDirection();
	if (WallRunDirection.IsNearlyZero())
		return bIsWallRunning = false;

	WallRunCount++;
	
	FHitResult HitResult;
	if (TryGetWallInDirection(HitResult, WallRunDirection))
	{
		if (bResetPreviousVelocity)
			Character->GetCharacterMovement()->Velocity = FVector::ZeroVector;

		float Magnitude = FMath::Lerp(650.f, 350.f, FMath::Clamp(FVector::DotProduct(WallRunDirection, FVector::UpVector), 0.f, 1.f));
		AdjustVelocityToDirection(WallRunDirection);
		PlayerCharacter->AddClampedVelocity(WallRunDirection, Magnitude);
		return bIsWallRunning = true;
	}
	return bIsWallRunning = false;
}

void UHangingState::StopWallRun()
{
	if (!bIsWallRunning)
		return;
	
	bIsWallRunning = false;
	Character->GetCharacterMovement()->StopMovementImmediately();
	Character->GetCharacterMovement()->Velocity = FVector::ZeroVector;
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
	const FVector WallNormal = HangingPoint.GetValue().Normal;
	if (FMath::Abs(WallNormal.Dot(Direction)) > .01f)
	{
		const FVector WallRunHelperVec = Direction.Cross(WallNormal).GetSafeNormal();
		const FVector ProperDirection = WallNormal.Cross(WallRunHelperVec).GetSafeNormal();
		Direction = ProperDirection;
	}
	const FVector Start = Character->GetCameraComponent()->GetComponentLocation() + Direction * CheckRange;
	const FVector End = Start - HangingPoint.GetValue().Normal * 100.f;
	
	if (FASCVars::ASDebugDraw && FASCVars::ASDrawDetailWallRun)
		DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 10.f, 0.f, 1.f);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Character);
	if (GetWorld()->SweepSingleByChannel(OutHitResult, Start, End,FQuat::Identity, ECC_Visibility,
		FCollisionShape::MakeCapsule(10.f, 25.f), Params))
	{
		return true;
	}
	return false;
}

FVector UHangingState::GetWallRunDirection()
{
	if (!HangingPoint.IsSet())
		return FVector::ZeroVector;

	const FVector WallNormal = HangingPoint.GetValue().Normal;

	if (FMath::Abs(Character->GetCameraComponent()->GetForwardVector().Dot(WallNormal)) > .85f)
		return FVector::ZeroVector;
	
	const FVector WallRight = FVector::CrossProduct(FVector::UpVector, WallNormal).GetSafeNormal();
	const FVector WallLeft = -WallRight;
	const FVector WallUp = FVector::VectorPlaneProject(FVector::UpVector, WallNormal);

	bool bHasEnterVelocity = !EnterVelocity.IsNearlyZero();
	const FVector LookIntent = FVector::VectorPlaneProject(Character->GetCameraComponent()->GetForwardVector(), WallNormal).GetSafeNormal();

	float ScoreRight;
	float ScoreLeft;
	float ScoreUp;

	const FVector DebugOrigin = HangingPoint.GetValue().Location + WallNormal * 25.f;
	
	if (bHasEnterVelocity && FVector::DotProduct(EnterVelocity, WallNormal) > -.9f)
	{
		const FVector ApproachIntent = FVector::VectorPlaneProject(EnterVelocity, WallNormal).GetSafeNormal();

		ScoreRight = FVector::DotProduct(WallRight, ApproachIntent)
			* (1.f - SidesWallRunLookWeight) + FVector::DotProduct(WallRight, LookIntent) * SidesWallRunLookWeight;
		ScoreLeft = FVector::DotProduct(WallLeft, ApproachIntent)
			* (1.f - SidesWallRunLookWeight) + FVector::DotProduct(WallLeft, LookIntent) * SidesWallRunLookWeight;
		ScoreUp = FVector::DotProduct(WallUp, ApproachIntent)
			* (1.f - UpWallRunLookWeight) + FVector::DotProduct(WallUp, LookIntent) * UpWallRunLookWeight;

		if (FASCVars::ASDebugDraw && FASCVars::ASDrawWallRunDirection)
		{
			if (!ApproachIntent.IsNearlyZero())
			{
				DrawDebugDirectionalArrow(GetWorld(), DebugOrigin,
				   DebugOrigin + ApproachIntent * 50.f, 3.f, FColor::Blue,
				   false, FASCVars::ASDebugDrawDuration, 0.f, 1.f);
			}
		}
	}
	else
	{
		ScoreRight = FVector::DotProduct(WallRight, LookIntent);
		ScoreLeft = FVector::DotProduct(WallLeft, LookIntent);
		ScoreUp = FVector::DotProduct(WallUp, LookIntent);
	}

	constexpr float MinScore = .3f; 
	bool bCanRight = ScoreRight > MinScore;
	bool bCanLeft = ScoreLeft > MinScore;
	bool bCanUp = ScoreUp > MinScore;
	
	float MaxScore = MinScore; 

	FVector ChosenDirection = FVector::ZeroVector;
	if (bCanRight && ScoreRight > MaxScore)
	{
		MaxScore = ScoreRight;
		ChosenDirection = WallRight;
	}
	if (bCanLeft && ScoreLeft > MaxScore)
	{
		MaxScore = ScoreLeft;
		ChosenDirection = WallLeft;
	}
	if (bCanUp && ScoreUp > MaxScore)
	{
		ChosenDirection = WallUp;
	}
	
	if (FASCVars::ASDebugDraw && FASCVars::ASDrawWallRunDirection)
	{
		if (!LookIntent.IsNearlyZero())
		{
			DrawDebugDirectionalArrow(GetWorld(), DebugOrigin,
			   DebugOrigin + LookIntent * 50.f, 3.f, FColor::Yellow,
			   false, FASCVars::ASDebugDrawDuration, 0.f, 1.f);
		}
		if (!ChosenDirection.IsNearlyZero())
		{
			DrawDebugDirectionalArrow(GetWorld(), DebugOrigin,
			   DebugOrigin + ChosenDirection * 50.f, 3.f, FColor::Green,
			   false, FASCVars::ASDebugDrawDuration, 0.f, 1.f);
		}
	}
	return ChosenDirection;
}

void UHangingState::AdjustVelocityToDirection(const FVector& Direction, bool bDiscardDownVelocity)
{
	FVector NewVelocity = Character->GetVelocity();
	if (!EnterVelocity.IsNearlyZero())
		NewVelocity = EnterVelocity;

	if (bDiscardDownVelocity)
	{
		if (NewVelocity.Z < 0.f) NewVelocity.Z = 0.f;
	}

	NewVelocity = Direction * Direction.Dot(NewVelocity);

	NewVelocity = Direction * NewVelocity.Size();
	Character->GetCharacterMovement()->Velocity = NewVelocity;
}
