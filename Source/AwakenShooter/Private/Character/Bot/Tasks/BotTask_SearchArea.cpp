// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Bot/Tasks/BotTask_SearchArea.h"

#include "NavigationSystem.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"

UBotTask_SearchArea::UBotTask_SearchArea(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer),
	SearchAreaCenter(FVector::ZeroVector),
	SearchRadius(1000.f),
	SearchTime(10.f),
	ElapsedTime(0.f)
{}

FVector UBotTask_SearchArea::GetNavLocationInArea()
{
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(BotController->GetWorld());
	FNavLocation RandomNavLoc;
	NavSys->GetRandomPointInNavigableRadius(
		SearchAreaCenter,
		SearchRadius,
		RandomNavLoc);

	return RandomNavLoc.Location;
}

void UBotTask_SearchArea::WalkToNextLocation()
{
	FVector NextLocation = TargetLocation = GetNavLocationInArea();
	BotController->MoveToLocation(NextLocation);
}

UBotTask_SearchArea* UBotTask_SearchArea::Create(const TScriptInterface<IGameplayTaskOwnerInterface>& TaskOwner,
	const FVector& InTargetLocation, float InSearchRadius, float InSearchTime)
{
	UBotTask_SearchArea* Task = NewBotTask<UBotTask_SearchArea>(TaskOwner);
	Task->SearchAreaCenter = InTargetLocation;
	Task->SearchRadius = InSearchRadius;
	Task->SearchTime = InSearchTime;
	return Task;
}

void UBotTask_SearchArea::Activate()
{
	if (!BotController)
	{
		EndTask();
		return;
	}
	if (auto BotCharacter = Cast<ACharacter>(BotController->GetPawn()))
	{
		BotCharacter->GetCharacterMovement()->bOrientRotationToMovement = true;
		BotCharacter->GetCharacterMovement()->MaxWalkSpeed = 200.f;
	}
	Super::Activate();
}

void UBotTask_SearchArea::TickTask(float DeltaTime)
{
	ElapsedTime += DeltaTime;

	if (ElapsedTime >= SearchTime)
	{
		EndTask();
	}
	if (BotController->GetMoveStatus() == EPathFollowingStatus::Idle)
	{
		WalkToNextLocation();
	}
	else
	{
		DrawDebugSphere(GetWorld(), TargetLocation, 10.f, 3, FColor::Red, false, 0.f);
	}
}
