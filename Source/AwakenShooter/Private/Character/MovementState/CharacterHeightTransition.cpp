// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterHeightTransition.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"

UCharacterHeightTransition::UCharacterHeightTransition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, ElapsedTime(0.f)
{
	bTickingTask = true;
}

UCharacterHeightTransition* UCharacterHeightTransition::CharacterHeightTransition(
	const TScriptInterface<IGameplayTaskOwnerInterface>& TaskOwner, float InTargetCameraHeight, float InTargetCapsuleHalfHeight,
	UCameraComponent* CameraComponent, UCapsuleComponent* CapsuleComponent, float TransitionDuration)
{
	UCharacterHeightTransition* Task = NewTask<UCharacterHeightTransition>(TaskOwner);
	Task->TargetCameraHeight = InTargetCameraHeight;
	Task->TargetCapsuleHalfHeight = InTargetCapsuleHalfHeight;
	Task->CameraComponent = CameraComponent;
	Task->CapsuleComponent = CapsuleComponent;
	Task->TransitionDuration = FMath::Max(TransitionDuration, KINDA_SMALL_NUMBER);
	return Task;
}

void UCharacterHeightTransition::TickTask(float DeltaTime)
{
	ElapsedTime += DeltaTime;
	float LerpFactor = FMath::Min(ElapsedTime / TransitionDuration, 1.f);

	if (LerpFactor >= 1.f)
	{
		EndTask();
	}

	CameraComponent->SetRelativeLocation(FVector(0.f, 0.f, FMath::Lerp(CameraComponent->GetRelativeLocation().Z, TargetCameraHeight, LerpFactor)));
	CapsuleComponent->SetCapsuleHalfHeight(FMath::Lerp(CapsuleComponent->GetScaledCapsuleHalfHeight(), TargetCapsuleHalfHeight, LerpFactor));
}