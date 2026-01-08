// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MovementState/States/FallingState.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "InputActionValue.h"
#include "AbilitySystem/GameplayTags.h"
#include "Camera/CameraComponent.h"
#include "Character/ASCharacter.h"
#include "Character/ASPlayerCharacter.h"
#include "Character/MovementState/MovementStateComponent.h"
#include "Character/MovementState/States/HangingState.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

UFallingState::UFallingState() :
	bCatchWall(true)
{
	StateID = EMovementState::Falling;
	NextState = EMovementState::Walking;
	TargetCapsuleHalfHeight = 48.f;
}

void UFallingState::Initialize(UMovementStateComponent* InStateMachine, AASCharacter* InCharacter)
{
	Super::Initialize(InStateMachine, InCharacter);
	if (OwnerIsPlayer())	
		PlayerCharacter->GetFallHitBox()->OnComponentBeginOverlap.AddDynamic(this, &UFallingState::OnFallHit);
}

void UFallingState::HandleLook(const FInputActionValue& Value)
{
	Super::HandleLook(Value);
}

void UFallingState::HandleCrouch(const FInputActionValue& Value)
{
	if (!OwnerIsPlayer())
		return;
	
	const FVector Velocity = Character->GetCharacterMovement()->Velocity;
	const FVector2D HorizontalVelocity = FVector2D(Velocity.X, Velocity.Y);

	FHitResult HitResult;
	if (PlayerCharacter->TryFindWall(HitResult))
	{
		bCatchWall = true;
	}
	else if (HorizontalVelocity.SquaredLength() > 360000.f)
	{
		NextStateOverride = EMovementState::Sliding;
	}
}

void UFallingState::HandleSprint(const FInputActionValue& Value)
{
	float InputValue = Value.Get<float>();
	if (InputValue < .5f)
	{
		Character->TryCancelAbilitiesWithTag(FASGameplayTags::Ability::Sprint);
	}
}

void UFallingState::OnEnterState_Implementation()
{
	Super::OnEnterState_Implementation();
	SetTickEnabled(true);
	
	if (auto ASC = Character ? Character->GetAbilitySystemComponent() : nullptr)
	{
		// cancel reload ability
		FGameplayTagContainer CancelAbilitiesTags = FGameplayTagContainer(FASGameplayTags::Ability::Reload);		
		ASC->CancelAbilities(&CancelAbilitiesTags);
	}
}

void UFallingState::OnExitState_Implementation()
{
	Super::OnExitState_Implementation();

	bCatchWall = true;
}

void UFallingState::OnStateTick_Implementation(float DeltaTime)
{
	if (Character->GetCharacterMovement()->Velocity.Z < 50.f
		&& bCatchWall)
	{
		TryCatchWall();
	}
}

void UFallingState::OnFallHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ThisSlideHits.Contains(OtherActor))
		return;
	
	if (AASCharacter* OtherCharacter = Cast<AASCharacter>(OtherActor))
	{
		if (FallDamageEffect)
		{
			FGameplayEffectContextHandle EffectContext = Character->GetAbilitySystemComponent()->MakeEffectContext();
			EffectContext.AddSourceObject(Character);
			EffectContext.AddHitResult(SweepResult);
			FGameplayEventData EventData;
			EventData.EventTag = FASGameplayTags::Event::Hit;
			EventData.Instigator = Character;
			EventData.Target = OtherActor;
			EventData.ContextHandle = EffectContext;
			EventData.EventMagnitude = 1.f;
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OtherCharacter, FASGameplayTags::Event::Hit, EventData);

			Character->GetAbilitySystemComponent()->BP_ApplyGameplayEffectToTarget(FallDamageEffect, OtherCharacter->GetAbilitySystemComponent(), 1.f,EffectContext);
		}
	}
	ThisSlideHits.AddUnique(OtherActor);
}

bool UFallingState::TryCatchWall()
{
	FHitResult HitResult;
	if (!OwnerIsPlayer())
		return false;

	if (PlayerCharacter->TryFindWall(HitResult))
	{
		UHangingState* HangingState = nullptr;
		if (UMovementStateBase* State = Character->GetMovementStateMachine()->GetStateFromMap(EMovementState::Hanging))
		{
			HangingState = Cast<UHangingState>(State);
		}
		if (!HangingState)
			return false;

		FVector HeadLocation = Character->GetCameraComponent()->GetComponentLocation();
		FVector TestDirection = (HitResult.Location - HeadLocation).GetSafeNormal();
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(Character);
		FHitResult HeadHitResult;
		if (!GetWorld()->SweepSingleByChannel(HeadHitResult, HeadLocation, HeadLocation + TestDirection * 100.f,
			 FQuat::Identity, ECC_WorldStatic, FCollisionShape::MakeSphere(10.f), Params))
		{
			DrawDebugSphere(GetWorld(), HeadLocation + TestDirection * 100.f,
				10.f, 10.f,
				FColor::Green, false, 5.f);
			Character->TryActivateAbilityByTag(FASGameplayTags::Ability::ClamberUp);
		}else
		{
			HangingState->SetHangingPoint(HitResult);
			StateMachine->ChangeState(EMovementState::Hanging);
		}

		return true;
	}
	return false;
}


