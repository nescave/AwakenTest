// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Tasks/GTClamberUp.h"

#include "Character/ASCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

UGTClamberUp::UGTClamberUp(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer),
	TargetLocation(),
	ControlledCharacter(nullptr),
	TaskDuration(1.f),
	ElapsedTime(0.f)
{
	bTickingTask = true;
}

UGTClamberUp* UGTClamberUp::Create(const TScriptInterface<IGameplayTaskOwnerInterface>& TaskOwner,
	const FVector& TargetLocation, AASCharacter* Character, float TaskDuration)
{
	UGTClamberUp* Task = NewTask<UGTClamberUp>(TaskOwner);
	Task->TargetLocation = TargetLocation;
	Task->ControlledCharacter = Character;
	Task->TaskDuration = FMath::Max(TaskDuration, KINDA_SMALL_NUMBER);;
	return Task;
}

void UGTClamberUp::Activate()
{
	Super::Activate();
	ControlledCharacter->GetCharacterMovement()->GravityScale *= 2.f;
}

void UGTClamberUp::OnDestroy(bool AbilityEndedNormally)
{
	ControlledCharacter->GetCharacterMovement()->GravityScale *= .5f;
	Super::OnDestroy(AbilityEndedNormally);
}

void UGTClamberUp::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);
	ElapsedTime += DeltaTime;
	if (ElapsedTime >= TaskDuration)
	{
		ControlledCharacter->GetCharacterMovement()->Velocity += (TargetLocation - ControlledCharacter->GetActorLocation()) * 5.f ;
		EndTask();
	}
}
