// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Tasks/GTAimTransition.h"

#include "GameplayTasksComponent.h"
#include "Character/ASCharacter.h"

UGTAimTransition::UGTAimTransition(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer),
	TargetLocation(FVector::ZeroVector),
	ControlledCharacter(nullptr),
	TaskDuration(.5f),
	ElapsedTime(0.f)
{
	bTickingTask = true;
}

UGTAimTransition* UGTAimTransition::Create(const TScriptInterface<IGameplayTaskOwnerInterface>& TaskOwner,
	const FVector& TargetLocation, float TaskDuration)
{
	UGTAimTransition* Task = NewTask<UGTAimTransition>(TaskOwner);
	Task->TargetLocation = TargetLocation;
	Task->TaskDuration = FMath::Max(TaskDuration, KINDA_SMALL_NUMBER);
	return Task;
}

void UGTAimTransition::Activate()
{
	if (auto GTC = Cast<UGameplayTasksComponent>(TaskOwner.GetObject()))
		ControlledCharacter = Cast<AASCharacter>(GTC->GetOwner());

	if (!ControlledCharacter)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("GTAimTransition failed to get controlled character!"));
		EndTask();
		return;
	}
	Super::Activate();
}

void UGTAimTransition::TickTask(float DeltaTime)
{
	ElapsedTime += DeltaTime;
	float LerpFactor = FMath::Min(ElapsedTime / TaskDuration, 1.f);

	if (LerpFactor >= 1.f)
	{
		EndTask();
	}

	ControlledCharacter->GetFirstPersonMesh()->SetRelativeLocation(FMath::Lerp(ControlledCharacter->GetFirstPersonMesh()->GetRelativeLocation(), TargetLocation, LerpFactor));
}

void UGTAimTransition::ExternalCancel()
{
	EndTask();	
}
