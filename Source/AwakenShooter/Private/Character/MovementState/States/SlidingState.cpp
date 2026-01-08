// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MovementState/States/SlidingState.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "InputActionValue.h"
#include "AbilitySystem/GameplayTags.h"
#include "AbilitySystem/Tasks/GTChangeSlideDirection.h"
#include "Camera/CameraComponent.h"
#include "Character/ASPlayerCharacter.h"
#include "Character/MovementState/MovementStateComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "General/DebugCVars.h"

USlidingState::USlidingState() :
	CachedBreakingFrictionFactor(0.f),
	CachedGroundFriction(0.f),
	CachedDeceleration(0.f),
	VelocityNeededToExitSlide(250.f),
	MoveForwardPressed(false),
	LastRightInput(0.f),
	MinVelocityThreshold(250.f),
	SlideBaseDecelerationFactor(.1f),
	SlideStopDecelerationFactor(1.f)
{
	StateID = EMovementState::Sliding;
	NextState = EMovementState::Crouching;
	bTickEnabled = true;
	
	TargetCameraHeight = 32.f;
	TargetCapsuleHalfHeight = 36.f;
}

void USlidingState::Initialize(UMovementStateComponent* InStateMachine, AASCharacter* InCharacter)
{
	Super::Initialize(InStateMachine, InCharacter);
	if (OwnerIsPlayer())
		PlayerCharacter->GetSlideHitBox()->OnComponentBeginOverlap.AddDynamic(this, &USlidingState::OnSlideHit);
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
	if ((LookAxisVector.X > 0.f && SignedAngle < 150) ||
		(LookAxisVector.X < 0.f && SignedAngle > -150))
	{
		Character->AddControllerYawInput(LookAxisVector.X);
	}
}

void USlidingState::HandleMove(const FInputActionValue& Value)
{
	FVector InputVector = FVector(Value.Get<FVector2D>(), 0.f);
	float SlideFrictionFactor = SlideBaseDecelerationFactor;
	MoveForwardPressed = InputVector.Y >= .5f;
	if (!MoveForwardPressed)
	{
		SlideFrictionFactor = SlideStopDecelerationFactor;
		NextStateOverride = EMovementState::NONE;
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
	float InputValue = Value.Get<float>();
	if (InputValue < .5f)
	{
		if (VelocityNeededToExitSlide != MinVelocityThreshold)
			VelocityNeededToExitSlide = MinVelocityThreshold;
	}
	else if (MoveForwardPressed && VelocityNeededToExitSlide <= MinVelocityThreshold )
	{
		VelocityNeededToExitSlide = MinVelocityThreshold * 2.f;
	}
}

void USlidingState::OnEnterState_Implementation()
{
	Super::OnEnterState_Implementation();

	if (!OwnerIsPlayer())
		return;
	
	PlayerCharacter->Crouch();

	PlayerCharacter->GetCharacterMovement()->bUseSeparateBrakingFriction = true;
	CachedBreakingFrictionFactor = PlayerCharacter->GetCharacterMovement()->BrakingFrictionFactor;
	CachedGroundFriction = PlayerCharacter->GetCharacterMovement()->GroundFriction;
	CachedDeceleration = PlayerCharacter->GetCharacterMovement()->BrakingDecelerationWalking;
	CachedVelocityDirection = PlayerCharacter->GetCharacterMovement()->Velocity.GetSafeNormal();
	
	PlayerCharacter->GetCharacterMovement()->BrakingFrictionFactor = 1.f;
	PlayerCharacter->GetCharacterMovement()->GroundFriction = 0.1f;
	PlayerCharacter->GetCharacterMovement()->BrakingDecelerationWalking = CachedDeceleration * SlideBaseDecelerationFactor;
	PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = PlayerCharacter->GetCachedInternalMovementSpeed()*.25f;

	PlayerCharacter->GetSlideHitBox()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PlayerCharacter->SetIgnoreProjectiles(true);
	
	if (auto ASC = PlayerCharacter ? PlayerCharacter->GetAbilitySystemComponent() : nullptr)
	{
		// cancel reload ability
		FGameplayTagContainer CancelAbilitiesTags = FGameplayTagContainer(FASGameplayTags::Ability::Reload);		
		ASC->CancelAbilities(&CancelAbilitiesTags);
	}
}

void USlidingState::OnExitState_Implementation()
{
	Super::OnExitState_Implementation();

	if (!OwnerIsPlayer())
		return;
	
	PlayerCharacter->UnCrouch();
	
	PlayerCharacter->GetCharacterMovement()->bUseSeparateBrakingFriction = false;
	PlayerCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
	PlayerCharacter->GetCharacterMovement()->BrakingFrictionFactor = CachedBreakingFrictionFactor;
	PlayerCharacter->GetCharacterMovement()->GroundFriction = CachedGroundFriction;
	PlayerCharacter->GetCharacterMovement()->BrakingDecelerationWalking = CachedDeceleration;
	PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed =  PlayerCharacter->GetCachedInternalMovementSpeed();
	VelocityNeededToExitSlide = MinVelocityThreshold;
	if (SlideDirectionTask)
		SlideDirectionTask->ExternalCancel();

	PlayerCharacter->SetIgnoreProjectiles(false);
	PlayerCharacter->GetSlideHitBox()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ThisSlideHits.Empty();
}

void USlidingState::OnStateTick_Implementation(float DeltaTime)
{
	Super::OnStateTick_Implementation(DeltaTime);
	
	if (Character->GetCharacterMovement()->Velocity.Size() < VelocityNeededToExitSlide)
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

FVector USlidingState::GetJumpDirection()
{
	FVector LookDirection = Character->GetCameraComponent()->GetForwardVector();
	return (LookDirection + Character->GetActorUpVector()).GetSafeNormal();
}

void USlidingState::OnSlideHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ThisSlideHits.Contains(OtherActor))
		return;
	
	if (AASCharacter* OtherCharacter = Cast<AASCharacter>(OtherActor))
	{
		if (SlideDamageEffect)
		{
			FHitResult HitResult = SweepResult;
			if (!bFromSweep)
			{
				TArray<FHitResult> HitResults;
				FCollisionQueryParams Params;
				Params.AddIgnoredActor(Character);
				GetWorld()->SweepMultiByChannel(HitResults, Character->GetActorLocation(), OtherCharacter->GetActorLocation(), FQuat::Identity, ECC_Pawn, FCollisionShape::MakeSphere(20.f), Params);				

				for (const auto& Hit : HitResults)
				{
					if (Hit.GetActor() == OtherCharacter)
					{
						HitResult = Hit;
						break;
					}
				}
			}
			
			FGameplayEffectContextHandle EffectContext = Character->GetAbilitySystemComponent()->MakeEffectContext();
			EffectContext.AddSourceObject(Character);
			EffectContext.AddHitResult(HitResult);
			FGameplayEventData EventData;
			EventData.EventTag = FASGameplayTags::Event::Hit;
			EventData.Instigator = Character;
			EventData.Target = OtherActor;
			EventData.ContextHandle = EffectContext;
			EventData.EventMagnitude = Character->GetVelocity().Size() * .1f;
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OtherCharacter, FASGameplayTags::Event::Hit, EventData);

			Character->GetAbilitySystemComponent()->BP_ApplyGameplayEffectToTarget(SlideDamageEffect, OtherCharacter->GetAbilitySystemComponent(), 1.f,EffectContext);
		}
	}
	ThisSlideHits.AddUnique(OtherActor);
}

