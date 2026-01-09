// Copyright Epic Games, Inc. All Rights Reserved.

#include "General/ASGameMode.h"

#include "General/ASGameInstance.h"
#include "Kismet/GameplayStatics.h"

AASGameMode::AASGameMode() :
	bRoundInProgress(false),
	RoundStartTime(0.f),
	RoundEndTime(0.f),
	bRoundWon(false),
	BestRoundTime(0.f),
	RemainingEnemies(0)
{}

void AASGameMode::BeginPlay()
{
	Super::BeginPlay();
	OnRoundResetDelegate.AddDynamic(this, &AASGameMode::OnRoundReset);
	OnRoundStartDelegate.AddDynamic(this, &AASGameMode::OnRoundStart);
	OnRoundEndDelegate.AddDynamic(this, &AASGameMode::OnRoundEnd);
	OnRoundWinDelegate.AddDynamic(this, &AASGameMode::OnRoundWin);
	OnRoundLoseDelegate.AddDynamic(this, &AASGameMode::OnRoundLose);
	OnPlayerDeathDelegate.AddDynamic(this, &AASGameMode::OnPlayerDeath);
	OnEnemyKilledDelegate.AddDynamic(this, &AASGameMode::OnEnemyKilled);
	OnLateInitDelegate.AddDynamic(this, &AASGameMode::OnLateInit);
	GetWorldTimerManager().SetTimer(OnLateInitHandle, this, &AASGameMode::LateInit, 2.0f, false);

	if (UASGameInstance* GI = Cast<UASGameInstance>(GetGameInstance()))
	{
		BestRoundTime = GI->InstanceBestRoundTime;
	}
}

void AASGameMode::LateInit()
{
	OnLateInitDelegate.Broadcast();
}

void AASGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AASGameMode::OnRoundReset_Implementation()
{
}

void AASGameMode::OnRoundStart_Implementation()
{

}

void AASGameMode::OnRoundEnd_Implementation()
{
	APawn* Pawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (!Pawn)
	{
		return;
	}
	
	Pawn->InputComponent->ClearBindingsForObject(Pawn);
}

void AASGameMode::OnRoundWin_Implementation()
{
	if (BestRoundTime == .0)
	{
		BestRoundTime = GetRoundTime();
	}
	else
	{
		BestRoundTime = FMath::Min(BestRoundTime, GetRoundTime());
	}
	if (UASGameInstance* GI = Cast<UASGameInstance>(GetGameInstance()))
	{
		GI->InstanceBestRoundTime = BestRoundTime;
	}
	OnRoundEndDelegate.Broadcast();
}

void AASGameMode::OnRoundLose_Implementation()
{
	OnRoundEndDelegate.Broadcast();
}

void AASGameMode::OnPlayerDeath_Implementation()
{
	OnRoundLoseDelegate.Broadcast();	
}

void AASGameMode::OnEnemyKilled_Implementation()
{
}

void AASGameMode::OnLateInit_Implementation()
{
	ResetRound();
}

void AASGameMode::ResetRound()
{
	TArray<AActor*> Enemies;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Enemy"), Enemies);
	RemainingEnemies = Enemies.Num();
	
	OnRoundResetDelegate.Broadcast();
}

void AASGameMode::StartRound()
{
	bRoundInProgress = true;
	RoundStartTime = GetWorld()->GetTimeSeconds();
	OnRoundStartDelegate.Broadcast();
}

void AASGameMode::EndRound()
{
	RoundEndTime = GetWorld()->GetTimeSeconds();
	bRoundInProgress = false;
	OnRoundEndDelegate.Broadcast();
}

void AASGameMode::WinRound()
{
	bRoundWon = true;
	EndRound();
	OnRoundWinDelegate.Broadcast();
}

void AASGameMode::LoseRound()
{
	EndRound();
	OnRoundLoseDelegate.Broadcast();
}

void AASGameMode::PlayerDeath()
{
	OnPlayerDeathDelegate.Broadcast();
}

void AASGameMode::EnemyKilled()
{
	RemainingEnemies--;
	OnEnemyKilledDelegate.Broadcast();
}

float AASGameMode::GetRoundTime() const
{
	return (bRoundInProgress ? GetWorld()->GetTimeSeconds() : RoundEndTime) - RoundStartTime;
}

