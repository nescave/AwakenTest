// Copyright Epic Games, Inc. All Rights Reserved.


#include "General/ASPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "General/ASCameraManager.h"
#include "AwakenShooter.h"
#include "EnhancedInputComponent.h"
#include "Character/ASPlayerCharacter.h"
#include "General/ASGameMode.h"
#include "Kismet/GameplayStatics.h"

AASPlayerController::AASPlayerController()
{
	// set the player camera manager class
	PlayerCameraManagerClass = AASCameraManager::StaticClass();
}

void AASPlayerController::BeginPlay()
{
	Super::BeginPlay();

	GameMode = Cast<AASGameMode>(UGameplayStatics::GetGameMode(this));

	if (!GameMode)
	{
		return;
	}
	GameMode->OnRoundEndDelegate.AddDynamic(this, &AASPlayerController::SetupInputMappingsAfterRoundsEnd);
}

void AASPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Context
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}
		}
	}
	
}

void AASPlayerController::HandleRestartAction()
{
	UWorld* World = GetWorld();
	if (!World) return;

	UGameplayStatics::OpenLevel(this, FName(*World->GetName()), true);
}

void AASPlayerController::HandleQuitAction()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), this, EQuitPreference::Quit,true);
}

void AASPlayerController::SetupInputMappingsAfterRoundsEnd()
{
	if (auto EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(RestartAction, ETriggerEvent::Triggered, this, &AASPlayerController::HandleRestartAction);
		EnhancedInputComponent->BindAction(QuitAction, ETriggerEvent::Triggered, this, &AASPlayerController::HandleQuitAction);
	}
}
