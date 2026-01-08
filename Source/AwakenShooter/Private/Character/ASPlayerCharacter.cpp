// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ASPlayerCharacter.h"

#include "AwakenShooter.h"
#include "EnhancedInputComponent.h"
#include "AbilitySystem/GameplayTags.h"
#include "Camera/CameraComponent.h"
#include "Character/MovementState/MovementStateComponent.h"
#include "Character/MovementState/States/HangingState.h"
#include "Components/SphereComponent.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Items/Interactable.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "General/DebugCVars.h"
#include "Items/Gun.h"
#include "Perception/AISense_Damage.h"
#include "Perception/AISense_Hearing.h"


AASPlayerCharacter::AASPlayerCharacter() :
	MaxCharacterVelocity(1400.f)
{
	Tags.Add(FName("Player"));
	
	StimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("StimuliSourceComponent"));
	StimuliSource->RegisterForSense(UAISense_Sight::StaticClass());
	StimuliSource->RegisterForSense(UAISense_Hearing::StaticClass());
	StimuliSource->RegisterForSense(UAISense_Damage::StaticClass());
	StimuliSource->RegisterWithPerceptionSystem();
	
	MeleeHitBox = CreateDefaultSubobject<USphereComponent>("MeleeHitBox");
	MeleeHitBox->SetupAttachment(RootComponent);
	MeleeHitBox->SetCollisionProfileName(FName("OverlapAllDynamic"));
	MeleeHitBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	MeleeHitBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	MeleeHitBox->SetCollisionResponseToChannel(ECC_Destructible, ECR_Overlap);
	MeleeHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeleeHitBox->SetRelativeLocation(FVector(60.f, 0.f, 10.f));
	MeleeHitBox->SetSphereRadius(30.f, false);
	
	SlideHitBox = CreateDefaultSubobject<USphereComponent>("SlideHitBox");
	SlideHitBox->SetupAttachment(RootComponent);
	SlideHitBox->SetCollisionProfileName(FName("OverlapAllDynamic"));
	SlideHitBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	SlideHitBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SlideHitBox->SetCollisionResponseToChannel(ECC_Destructible, ECR_Overlap);
	SlideHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SlideHitBox->SetRelativeLocation(FVector(100.f, 0.f, -10.f));
	SlideHitBox->SetSphereRadius(40.f, false);
	
	FallHitBox = CreateDefaultSubobject<USphereComponent>("FallHitBox");
	FallHitBox->SetupAttachment(RootComponent);
	FallHitBox->SetCollisionProfileName(FName("OverlapAllDynamic"));
	FallHitBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	FallHitBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	FallHitBox->SetCollisionResponseToChannel(ECC_Destructible, ECR_Overlap);
	FallHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FallHitBox->SetRelativeLocation(FVector(0.f, 0.f, -80.f));
	FallHitBox->SetSphereRadius(35.f, false);
}

void AASPlayerCharacter::EmitMovementNoise(float DeltaSeconds, FVector OldLocation, FVector OldVelocity)
{
	if (GetMovementStateMachine()->GetCurrentStateID() != EMovementState::Falling)
	{
		ReportNoiseEvent(GetVelocity().Size()*.001f, 1000.f);
	}
}

void AASPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	OnCharacterMovementUpdated.AddDynamic(this, &AASPlayerCharacter::EmitMovementNoise);
	CachedInternalMovementSpeed = GetCharacterMovement()->MaxWalkSpeed;
}

void AASPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	AutoEquip();
}

void AASPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AASPlayerCharacter::HandleLookInput);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AASPlayerCharacter::HandleMoveInput);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AASPlayerCharacter::HandleJumpInput);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &AASPlayerCharacter::HandleCrouchInput);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &AASPlayerCharacter::HandleSprintInput);
		EnhancedInputComponent->BindAction(WallRunAction, ETriggerEvent::Triggered, this, &AASPlayerCharacter::HandleWallRunInput);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &AASPlayerCharacter::HandleInteractInput);
		EnhancedInputComponent->BindAction(GunMainAction, ETriggerEvent::Triggered, this, &AASPlayerCharacter::HandleGunMainInput);
		EnhancedInputComponent->BindAction(GunSecondaryAction, ETriggerEvent::Triggered, this, &AASPlayerCharacter::HandleGunSecondaryInput);
		EnhancedInputComponent->BindAction(ThrowAction, ETriggerEvent::Triggered, this, &AASPlayerCharacter::HandleThrowInput);
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &AASPlayerCharacter::HandleReloadInput);
		EnhancedInputComponent->BindAction(AimDownSightsAction, ETriggerEvent::Triggered, this, &AASPlayerCharacter::HandleAimDownSightsInput);
		EnhancedInputComponent->BindAction(BulletTimeAction, ETriggerEvent::Triggered, this, &AASPlayerCharacter::HandleBulletTime);
	}
	else
	{
		UE_LOG(LogAwakenShooter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AASPlayerCharacter::AutoEquip()
{
	if (TryFindInteraction() && !EquippedGun)
	{
		if (AGun* FoundGun = Cast<AGun>(PossibleInteraction))
		{
			IInteractable::Execute_Interact(FoundGun, this);
		}
	}
}


void AASPlayerCharacter::HandleLookInput(const FInputActionValue& Value)
{
	MovementStateMachine->HandleLook(Value);
}

void AASPlayerCharacter::HandleMoveInput(const FInputActionValue& Value)
{
	MovementStateMachine->HandleMove(Value);
}

void AASPlayerCharacter::HandleJumpInput(const FInputActionValue& Value)
{
	MovementStateMachine->HandleJump(Value);
}

void AASPlayerCharacter::HandleCrouchInput(const FInputActionValue& Value)
{
	MovementStateMachine->HandleCrouch(Value);
}

void AASPlayerCharacter::HandleSprintInput(const FInputActionValue& Value)
{
	MovementStateMachine->HandleSprint(Value);
}

void AASPlayerCharacter::HandleWallRunInput(const FInputActionValue& Value)
{
	MovementStateMachine->HandleWallRun(Value);
}

void AASPlayerCharacter::HandleInteractInput(const FInputActionValue& Value)
{
	if (!PossibleInteraction)
	{
		return;
	}
	IInteractable::Execute_Interact(GetPossibleInteraction(), this);
}

void AASPlayerCharacter::HandleGunMainInput(const FInputActionValue& Value)
{
	MovementStateMachine->HandleGunMain(Value);
}

void AASPlayerCharacter::HandleGunSecondaryInput(const FInputActionValue& Value)
{
	MovementStateMachine->HandleGunSecondary(Value);
}

void AASPlayerCharacter::HandleThrowInput(const FInputActionValue& Value)
{
	MovementStateMachine->HandleThrow(Value);
}

void AASPlayerCharacter::HandleReloadInput(const FInputActionValue& Value)
{
	MovementStateMachine->HandleReload(Value);
}

void AASPlayerCharacter::HandleAimDownSightsInput(const FInputActionValue& InputActionValue)
{
	MovementStateMachine->HandleADS(InputActionValue);
}

void AASPlayerCharacter::HandleBulletTime(const FInputActionValue& InputActionValue)
{
	if (AbilitySystemComponent->HasMatchingGameplayTag(FASGameplayTags::Ability::BulletTimeActive))
	{
		TryCancelAbilitiesWithTag(FASGameplayTags::Ability::BulletTime);
		return;
	}
	
	TryActivateAbilityByTag(FASGameplayTags::Ability::BulletTime);
}


bool AASPlayerCharacter::TryFindInteraction()
{
	const FVector Position = GetCameraComponent()->GetComponentLocation() + GetCameraComponent()->GetForwardVector() * 100.f - GetActorUpVector() * 70.f;
	FCollisionQueryParams Params;
	if (EquippedGun)
		Params.AddIgnoredActor(EquippedGun);

	TArray<FOverlapResult> Interactions;
	TArray<AActor*> Interactables;
	
	AActor* Interactive = nullptr;
	if (FASCVars::ASDebugDraw && FASCVars::ASDrawInteractionsZone)
	{
		DrawDebugSphere(GetWorld(), Position, 120.f,12,
			FColor::Red, false, 0.f, 0.f, .25f);
	}
	if (GetWorld()->OverlapMultiByChannel(Interactions, Position, FQuat::Identity, ECC_GameTraceChannel2, 
		FCollisionShape::MakeSphere(100.f), Params))
	{
		float ClosestMatch = -1.f;
		for (const auto& Item : Interactions)
		{
			if(!(Item.GetActor() && Item.GetActor()->Implements<UInteractable>()) )
			{
				continue;
			}

			if (!IInteractable::Execute_IsInteractable(Item.GetActor()))
			{
				continue;
			}
			
			if (FASCVars::ASDebugDraw && FASCVars::ASDrawDetailedInteractions)
			{
				Interactables.Add(Item.GetActor());
			}
			FVector DirectionToItem = 
				(Item.GetActor()->GetActorLocation() - GetCameraComponent()->GetComponentLocation()).GetSafeNormal();
			float IDotC = DirectionToItem.Dot(GetCameraComponent()->GetForwardVector());
			if (IDotC > ClosestMatch)
			{
				Interactive =  Item.GetActor();
				ClosestMatch = IDotC;
			}
		}
	}

	if (FASCVars::ASDebugDraw && FASCVars::ASDrawDetailedInteractions)
	{
		for (const auto& Item : Interactables)
		{
			AActor* Actor = Cast<AActor>(Item);
			FColor Color = FColor::Red;
			if (Item == Interactive)
			{
				Color = FColor::Green;
			}
			DrawDebugSphere(GetWorld(), Actor->GetActorLocation(), 20.f,4,
				Color, false, 0.f, 0.f, 1.f);
		}
	}
	
	SetPossibleInteraction(Interactive);
	
	if (PossibleInteraction)
	{
		return true;
	}
	return false;
}

void AASPlayerCharacter::SetPossibleInteraction(AActor* NewInteraction)
{
	if (GetPossibleInteraction() == NewInteraction)
		return;
		
	if (AActor* InteractableActor = Cast<AActor>(NewInteraction))
	{
		IInteractable::Execute_SetHighlighted(InteractableActor, 1.f);
		PossibleInteraction = InteractableActor;
	}
	else
	{
		PossibleInteraction = nullptr;
	}
}

AActor* AASPlayerCharacter::GetPossibleInteraction() const
{
	return PossibleInteraction;
}

void AASPlayerCharacter::AddClampedVelocity(const FVector& VelocityToAdd, float Magnitude, bool bIgnoreAtMaxVelocity)
{
	if (bIgnoreAtMaxVelocity && GetVelocity().Size() >= GetMaxCharacterVelocity())
		return;
	
	const FVector VelocityDirection = GetVelocity().GetSafeNormal();
	if (GetVelocity().Size() < GetMaxCharacterVelocity())
	{
		GetCharacterMovement()->Velocity += VelocityToAdd * Magnitude;
		return;
	}

	if (VelocityToAdd.Dot(VelocityDirection) > 0.f)
	{
		const FVector HelpVector = VelocityToAdd.Cross(VelocityDirection).GetSafeNormal();
		const FVector RedirectedVelocity = VelocityDirection.Cross(HelpVector);
		GetCharacterMovement()->Velocity += RedirectedVelocity * FMath::Max(VelocityToAdd.Dot(RedirectedVelocity), 0.f) * Magnitude;

		if (FASCVars::ASDebugDraw && FASCVars::ASDrawVelocityClamping)
		{
			DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + RedirectedVelocity * Magnitude, 10.f, FColor::Green, false, 10.f, 0.f, 2.f);
			DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + VelocityToAdd * Magnitude, 10.f, FColor::Red, false, 10.f, 0.f, 2.f);
			DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + HelpVector * Magnitude, 10.f, FColor::Yellow, false, 10.f, 0.f, 2.f);
		}
	}
}

bool AASPlayerCharacter::TryFindWall(FHitResult& OutHitResult)
{
	if (GetGroundDistance() < 200.f)
	{
		return false;
	}
	const FVector Start = GetCameraComponent()->GetComponentLocation();
	const FVector Forward = (FVector(GetCameraComponent()->GetForwardVector().X, GetCameraComponent()->GetForwardVector().Y, 0.f).GetSafeNormal()
		+ FVector(GetVelocity().X, GetVelocity().Y, 0.f).GetSafeNormal()).GetSafeNormal();
	const FVector EndFront = Start + Forward * 50.f;
	const FVector EndRight = Start + Forward.RotateAngleAxis(35.f, FVector::UpVector) * 50.f;
	const FVector EndLeft = Start + Forward.RotateAngleAxis(-35.f, FVector::UpVector) * 50.f;
	constexpr float CheckCapsuleHalfHeight = 15.f;
	if (FASCVars::ASDebugDraw && FASCVars::ASDrawWallCatchTests)
	{
		DrawDebugCapsule(GetWorld(), EndFront, CheckCapsuleHalfHeight, 10.f, FQuat::Identity, FColor::Red, false, 0.f, 0.f, .25f);
		DrawDebugCapsule(GetWorld(), EndRight, CheckCapsuleHalfHeight, 10.f, FQuat::Identity, FColor::Red, false, 0.f, 0.f, .25f);
		DrawDebugCapsule(GetWorld(), EndLeft, CheckCapsuleHalfHeight, 10.f, FQuat::Identity, FColor::Red, false, 0.f, 0.f, .25f);
	}
	
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	if (GetWorld()->SweepSingleByChannel(OutHitResult,Start,EndFront,FQuat::Identity,ECC_WorldStatic,
		FCollisionShape::MakeCapsule(10.f, CheckCapsuleHalfHeight),Params)
		
		|| GetWorld()->SweepSingleByChannel(OutHitResult, Start, EndRight, FQuat::Identity, ECC_WorldStatic,
			FCollisionShape::MakeCapsule(10.f, CheckCapsuleHalfHeight), Params)
			
		|| GetWorld()->SweepSingleByChannel(OutHitResult, Start, EndLeft,FQuat::Identity,ECC_WorldStatic,
		FCollisionShape::MakeCapsule(10.f, CheckCapsuleHalfHeight), Params))
	{
		const FVector End = Start - OutHitResult.Normal * 70.f + FVector::UpVector * 15.f;
		if (GetWorld()->SweepSingleByChannel( OutHitResult, Start, End, FQuat::Identity, ECC_WorldStatic,
			FCollisionShape::MakeSphere(15.f), Params))
		{
			if (FASCVars::ASDebugDraw && FASCVars::ASDrawWallCatchTests)
			{
				DrawDebugCapsule(GetWorld(),
					OutHitResult.ImpactPoint, CheckCapsuleHalfHeight, 10.f, FQuat::Identity,
					FColor::Green, false, FASCVars::ASDebugDrawDuration, 0.f, .25f);
			}	
		}
		if (FMath::Abs(OutHitResult.Normal.Dot(FVector::UpVector)) < .25f)
		{
			return true;
		}
	}
	return false;
}

void AASPlayerCharacter::ReportNoiseEvent(float InLoudness, float InMaxRange)
{
	UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), InLoudness, this, InMaxRange);
}

bool AASPlayerCharacter::GetHangingPoint(FHitResult& OutHangingPoint) const
{
	if (MovementStateMachine->GetCurrentStateID() == EMovementState::Hanging)
	{
		UHangingState* HangingState = Cast<UHangingState>(MovementStateMachine->GetCurrentState());
		return HangingState->GetHangingPoint(OutHangingPoint);
	}
	return false;
}
