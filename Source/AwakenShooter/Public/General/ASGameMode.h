// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ASGameMode.generated.h"

/**
 *  Simple GameMode for a first person game
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameEvent);

UCLASS(abstract)
class AASGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, Category = "GameData")
	bool bRoundInProgress;
	UPROPERTY(BlueprintReadWrite, Category = "GameData")
	double RoundStartTime;
	UPROPERTY(BlueprintReadWrite, Category = "GameData")
	double RoundEndTime;
	UPROPERTY(BlueprintReadWrite, Category = "GameData")
	bool bRoundWon;
	
public:
	AASGameMode();
	
protected:
	virtual void BeginPlay() override;

	void LateInit();
	
public:
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(BlueprintReadWrite, Category = "GameData")
	double BestRoundTime;

	UPROPERTY(BlueprintReadWrite, Category = "GameData")
	int32 RemainingEnemies;

	UPROPERTY(BlueprintAssignable, Category = "GameEvents")
	FOnGameEvent OnRoundResetDelegate;
	UPROPERTY(BlueprintAssignable, Category = "GameEvents")
	FOnGameEvent OnRoundStartDelegate;
	UPROPERTY(BlueprintAssignable, Category = "GameEvents")
	FOnGameEvent OnRoundEndDelegate;
	UPROPERTY(BlueprintAssignable, Category = "GameEvents")
	FOnGameEvent OnRoundWinDelegate;
	UPROPERTY(BlueprintAssignable, Category = "GameEvents")
	FOnGameEvent OnRoundLoseDelegate;
	UPROPERTY(BlueprintAssignable, Category = "GameEvents")
	FOnGameEvent OnPlayerDeathDelegate;
	UPROPERTY(BlueprintAssignable, Category = "GameEvents")
	FOnGameEvent OnEnemyKilledDelegate;
	UPROPERTY(BlueprintAssignable, Category = "GameEvents")
	FOnGameEvent OnLateInitDelegate;
	
	UFUNCTION(BlueprintNativeEvent, Category = "GameEvents")
	void OnRoundReset();
	UFUNCTION(BlueprintNativeEvent, Category = "GameEvents")
	void OnRoundStart();
	UFUNCTION(BlueprintNativeEvent, Category = "GameEvents")
	void OnRoundEnd();
	UFUNCTION(BlueprintNativeEvent, Category = "GameEvents")
	void OnRoundWin();
	UFUNCTION(BlueprintNativeEvent, Category = "GameEvents")
	void OnRoundLose();
	UFUNCTION(BlueprintNativeEvent, Category = "GameEvents")
	void OnPlayerDeath();
	UFUNCTION(BlueprintNativeEvent, Category = "GameEvents")
	void OnEnemyKilled();
	UFUNCTION(BlueprintNativeEvent, Category = "GameEvents")
	void OnLateInit();
	
	UFUNCTION(BlueprintCallable, Category = "GameEvents")
	void ResetRound();
	UFUNCTION(BlueprintCallable, Category = "GameEvents")
	void StartRound();
	UFUNCTION(BlueprintCallable, Category = "GameEvents")
	void EndRound();
	UFUNCTION(BlueprintCallable, Category = "GameEvents")
	void WinRound();
	UFUNCTION(BlueprintCallable, Category = "GameEvents")
	void LoseRound();

	void PlayerDeath();
	void EnemyKilled();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GameData")
	float GetRoundTime() const;

private:
	FTimerHandle OnLateInitHandle;
};



