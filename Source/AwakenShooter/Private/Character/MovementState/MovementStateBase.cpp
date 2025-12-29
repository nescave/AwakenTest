// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MovementState/MovementStateBase.h"

#include "AwakenShooter.h"
#include "CharacterHeightTransition.h"
#include "InputActionValue.h"
#include "AbilitySystem/GameplayTags.h"
#include "Camera/CameraComponent.h"
#include "Character/ASCharacter.h"
#include "Character/MovementState/MovementStateComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Items/Gun.h"

void UMovementStateBase::SetStateLocked(bool bInLocked)
{
	bStateLocked = bInLocked;
}

bool UMovementStateBase::CharacterHeightNeedsUpdate() const
{
	if (FMath::IsNearlyEqual(TargetCameraHeight, Character->GetCameraComponent()->GetComponentLocation().Z)
		&& FMath::IsNearlyEqual(TargetCapsuleHalfHeight, Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()))
	{
		return false;
	}
	return true;
}

float UMovementStateBase::GetGroundDistance()
{
	FHitResult HitResult;
	FVector Start = Character->GetActorLocation();
	FVector End = Start - Character->GetActorUpVector() * 200.f;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Character);

	if (GetWorld()->SweepSingleByChannel(
		HitResult,
		Start,
		End,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeCapsule(30.f, 60.f),
		Params
	))
	{
		return HitResult.Distance;
	}
	return UE_BIG_NUMBER;
}

UMovementStateBase::UMovementStateBase() :
	StateMachine(nullptr),
	Character(nullptr),
	StateID(EMovementState::NONE),
	bTickEnabled(false),
	NextState(EMovementState::Walking),
	bStateLocked(false),
	TargetCameraHeight(70.f),
	TargetCapsuleHalfHeight(96.f)
{}

void UMovementStateBase::Initialize(UMovementStateComponent* InStateMachine, AASCharacter* InCharacter)
{
	StateMachine = InStateMachine;
	Character = InCharacter;
}

void UMovementStateBase::OnEnterState_Implementation()
{
	if (bDebugLogging)
	{
		FString Msg = FString::Printf(TEXT("Entering Movement State: %s"), *UEnum::GetValueAsString(StateID));
		UE_LOG(LogAwakenShooter, Warning, TEXT("%s"), *Msg);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Msg);
	}

	if (CharacterHeightNeedsUpdate())
	{
		HeightTransitionTask = UCharacterHeightTransition::CharacterHeightTransition(
			Character->GetGameplayTasksComponent(),
			TargetCameraHeight,
			Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight(), // hack for now (changing collider scale is not the brightest idea)
			Character->GetCameraComponent(),
			Character->GetCapsuleComponent(),
			1.f);
		
		HeightTransitionTask->ReadyForActivation();
	}

	if (auto ASC = Character ? Character->GetAbilitySystemComponent() : nullptr)
	{
		ApplyEffects(ASC);
	}
}

void UMovementStateBase::OnExitState_Implementation()
{
	if (HeightTransitionTask)
	{
		HeightTransitionTask->ExternalCancel();
	}

	if (auto ASC = Character ? Character->GetAbilitySystemComponent() : nullptr)
	{
		RemoveEffects(ASC);
	}
}

void UMovementStateBase::OnStateTick_Implementation(float DeltaTime)
{
}

void UMovementStateBase::HandleLook(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	Character->AddControllerYawInput(LookAxisVector.X);
	Character->AddControllerPitchInput(-LookAxisVector.Y);
}

void UMovementStateBase::HandleMove(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	Character->AddMovementInput(Character->GetActorRightVector(), MovementVector.X);
	Character->AddMovementInput(Character->GetActorForwardVector(), MovementVector.Y);
}

void UMovementStateBase::HandleJump(const FInputActionValue& Value)
{
	Character->TryActivateAbilityByTag(FASGameplayTags::Ability_Jump);		
}

void UMovementStateBase::HandleCrouch(const FInputActionValue& Value)
{
	StateMachine->ChangeState(EMovementState::Crouching);
}

void UMovementStateBase::HandleSprint(const FInputActionValue& Value)
{
	if (Character->GetCharacterMovement()->Velocity.SizeSquared() > 0)
	{
		StateMachine->ChangeState(EMovementState::Sprinting);
	}
}

void UMovementStateBase::HandleGunMain(const FInputActionValue& Value)
{
	if (auto ASC = Character ? Character->GetAbilitySystemComponent() : nullptr)
	{
		ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(FASGameplayTags::Ability_Secondary));
	}
}

void UMovementStateBase::HandleGunSecondary(const FInputActionValue& Value)
{
	if (auto ASC = Character ? Character->GetAbilitySystemComponent() : nullptr)
	{
		ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(FASGameplayTags::Ability_Secondary));
	}
}

void UMovementStateBase::HandleThrow(const FInputActionValue& Value)
{
	if (auto ASC = Character ? Character->GetAbilitySystemComponent() : nullptr)
	{
		ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(FASGameplayTags::Ability_Throw));
	}
}

FVector UMovementStateBase::GetJumpDirection()
{
	return Character->GetActorUpVector();
}

UWorld* UMovementStateBase::GetWorld() const
{
	if (!Character)
		return nullptr;
	return Character->GetWorld();
}

void UMovementStateBase::ApplyEffects(UAbilitySystemComponent* ASC)
{
	for (const auto& Effect : EffectsToApply)
	{
		EffectsHandles.Add(ASC->BP_ApplyGameplayEffectToSelf(Effect, 1.f, ASC->MakeEffectContext()));
	}
}

void UMovementStateBase::RemoveEffects(UAbilitySystemComponent* ASC)
{
	for (const auto& EffectHandle : EffectsHandles)
	{
		ASC->RemoveActiveGameplayEffect(EffectHandle);
	}
	EffectsHandles.Empty();
}
