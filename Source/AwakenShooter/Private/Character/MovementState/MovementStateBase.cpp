// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MovementState/MovementStateBase.h"

#include "AwakenShooter.h"
#include "CharacterHeightTransition.h"
#include "InputActionValue.h"
#include "AbilitySystem/GameplayTags.h"
#include "Camera/CameraComponent.h"
#include "Character/ASCharacter.h"
#include "Character/ASPlayerCharacter.h"
#include "Character/MovementState/MovementStateComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Items/Gun.h"

UMovementStateBase::UMovementStateBase() :
	StateMachine(nullptr),
	Character(nullptr),
	StateID(EMovementState::NONE),
	bTickEnabled(false),
	NextState(EMovementState::Walking),
	NextStateOverride(EMovementState::NONE),
	bStateLocked(false),
	TargetCameraHeight(70.f),
	TargetCapsuleHalfHeight(96.f)
{}

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

void UMovementStateBase::Initialize(UMovementStateComponent* InStateMachine, AASCharacter* InCharacter)
{
	StateMachine = InStateMachine;
	Character = InCharacter;
	PlayerCharacter = Cast<AASPlayerCharacter>(InCharacter);
	bIsPlayer = PlayerCharacter != nullptr;
}

void UMovementStateBase::OnEnterState_Implementation()
{
	NextStateOverride = EMovementState::NONE;
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
			TargetCapsuleHalfHeight,
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
	if (MovementVector.SizeSquared() < .01f)
	{
		Character->TryCancelAbilitiesWithTag(FASGameplayTags::Ability::Sprint);
		FGameplayTagContainer SprintEffectTags;
		SprintEffectTags.AddTag(FASGameplayTags::Effect::SprintEaseOut);
		SprintEffectTags.AddTag(FASGameplayTags::Effect::Sprint);
		Character->GetAbilitySystemComponent()->RemoveActiveEffectsWithTags(SprintEffectTags);
	}
}

void UMovementStateBase::HandleJump(const FInputActionValue& Value)
{
	Character->TryActivateAbilityByTag(FASGameplayTags::Ability::Jump);		
}

void UMovementStateBase::HandleCrouch(const FInputActionValue& Value)
{
	StateMachine->ChangeState(EMovementState::Crouching);
}

void UMovementStateBase::HandleSprint(const FInputActionValue& Value)
{
	float InputValue = Value.Get<float>();
	if (InputValue < .5f)
	{
		Character->TryCancelAbilitiesWithTag(FASGameplayTags::Ability::Sprint);
	}
	else
	{
		if (Character->GetCharacterMovement()->Velocity.SizeSquared() > 0)
		{
			Character->TryActivateAbilityByTag(FASGameplayTags::Ability::Sprint);
		}
	}
	
}

void UMovementStateBase::HandleGunMain(const FInputActionValue& Value)
{
	if (Character->GetEquippedGun())
	{
		if (!Character->GetEquippedGun()->MainAction() && Character->GetEquippedGun()->IsAmmoEmpty())
		{
			Character->GetOnOutOfAmmoDelegate().Broadcast();
		}
	}
}

void UMovementStateBase::HandleGunSecondary(const FInputActionValue& Value)
{
	if (Character->GetEquippedGun())
	{
		Character->GetEquippedGun()->SecondaryAction();
	}
}

void UMovementStateBase::HandleThrow(const FInputActionValue& Value)
{
	if (auto ASC = Character ? Character->GetAbilitySystemComponent() : nullptr)
	{
		ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(FASGameplayTags::Ability::Throw));
	}
}

void UMovementStateBase::HandleADS(const FInputActionValue& Value)
{
	float InputValue = Value.Get<float>();
	if ( InputValue >= .5f)
	{
		Character->TryActivateAbilityByTag(FASGameplayTags::Ability::AimDownSights);
	}
	else
	{
		Character->TryCancelAbilitiesWithTag(FASGameplayTags::Ability::AimDownSights);
	}
}

FVector UMovementStateBase::GetJumpDirection()
{
	return Character->GetActorUpVector();
}

EMovementState UMovementStateBase::GetNextState() const
{
	if (NextStateOverride != EMovementState::NONE)
		return NextStateOverride;

	return NextState;
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
