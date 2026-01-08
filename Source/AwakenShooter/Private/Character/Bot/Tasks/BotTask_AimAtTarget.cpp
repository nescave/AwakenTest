// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Bot/Tasks/BotTask_AimAtTarget.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "General/DebugCVars.h"
#include "Kismet/KismetMathLibrary.h"

UBotTask_AimAtTarget::UBotTask_AimAtTarget(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer),
	TargetActor(nullptr)
{}

UBotTask_AimAtTarget* UBotTask_AimAtTarget::Create(TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner,
	AActor* InTargetActor)
{
	UBotTask_AimAtTarget* Task = NewBotTask<UBotTask_AimAtTarget>(TaskOwner);
	Task->TargetActor = InTargetActor;
	return Task;
}

void UBotTask_AimAtTarget::Activate()
{
	if (!TargetActor)
	{
		EndTask();
		return;
	}
	
	BotCharacter = Cast<ACharacter>(BotController->GetPawn());
	
	if (BotCharacter)
	{
		BotCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
	}
	
	BotController->StopMovement();
	
	Super::Activate();
}

void UBotTask_AimAtTarget::TickTask(float DeltaTime)
{
	FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(
		BotController->GetPawn()->GetActorLocation(), TargetActor->GetActorLocation());
	
	if (FASCVars::ASDebugDraw && FASCVars::ASDrawBotTargeting)
		DrawDebugDirectionalArrow(BotController->GetWorld(),BotController->GetPawn()->GetActorLocation(),
			TargetActor->GetActorLocation(), 5.f, FColor::Green, false, 0.f, 0.f, 2.5f);

	TargetRotation.Roll = 0.f;
	TargetRotation.Pitch = 0.f;
	
	BotCharacter->SetActorRotation(TargetRotation);
}
