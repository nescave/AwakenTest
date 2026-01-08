// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Bot/Tasks/BotTask_IdleRoam.h"

#include "NavigationSystem.h"
#include "Character/Bot/ASBotCharacter.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "General/DebugCVars.h"
#include "Navigation/PathFollowingComponent.h"

UBotTask_IdleRoam* UBotTask_IdleRoam::Create(TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner, float RadiusToSearch)
{
	UBotTask_IdleRoam* Task = NewBotTask<UBotTask_IdleRoam>(TaskOwner);
	Task->SearchRadius = RadiusToSearch;
	return Task;
}

void UBotTask_IdleRoam::Activate()
{
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(BotController->GetWorld());
	FNavLocation RandomNavLoc;
	NavSys->GetRandomPointInNavigableRadius(
		BotController->GetBotCharacter()->GetInitialLocation(),
		SearchRadius,
		RandomNavLoc);

	if (auto BotCharacter = Cast<ACharacter>(BotController->GetPawn()))
	{
		BotCharacter->GetCharacterMovement()->bOrientRotationToMovement = true;
		BotCharacter->GetCharacterMovement()->MaxWalkSpeed = 350.f;
	}
	
	BotController->MoveToLocation(RandomNavLoc.Location);
	
	Super::Activate();
}

void UBotTask_IdleRoam::TickTask(float DeltaTime)
{
	if (FASCVars::ASDebugDraw && FASCVars::ASDrawBotPathfinding)
		DrawDebugSphere(GetWorld(), TargetLocation, 10.f, 3, FColor::Red, false, 0.f);
	
	if (BotController->GetMoveStatus() == EPathFollowingStatus::Idle)
	{
		EndTask();
	}
}
