// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/ASCharacter.h"
#include "ASBotCharacter.generated.h"

UCLASS()
class AWAKENSHOOTER_API AASBotCharacter : public AASCharacter
{
	GENERATED_BODY()

protected:
	
	UPROPERTY(BlueprintReadWrite, Category = "Bot")
	FVector LastSeenLocation;
	UPROPERTY(BlueprintReadWrite, Category = "Bot")
	FVector InitialBotLocation;
	UPROPERTY(BlueprintReadOnly, Category = "Bot")
	TObjectPtr<class AASBotAIController> AIController;
	UPROPERTY(BlueprintReadOnly, Category = "Bot")
	TObjectPtr<class UBlackboardComponent> Blackboard;
	
public:
	AASBotCharacter();

protected:
	void OnStunTagChanged(FGameplayTag GameplayTag, int NewCount);
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleReload();

	virtual void Die() override;
public:
	virtual void Tick(float DeltaTime) override;

	virtual void GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const override;
	
	virtual void HandleOutOfAmmo() override;
	
	UFUNCTION(BlueprintCallable, Category="Bot")
	void Shoot();
	UFUNCTION(BlueprintCallable, Category="Bot")
	void Reload();

	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Bot")
	const FVector& GetLastSeenLocation() const { return LastSeenLocation; }	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Bot")
	const FVector& GetInitialLocation() const { return InitialBotLocation; }

	UFUNCTION(BlueprintCallable, Category="Bot")
	void SetLastSeenLocation(const FVector& NewLastSeenLocation);

	UFUNCTION(BlueprintCallable, Category="Bot")
	AActor* GetTarget() const;
	UFUNCTION(BlueprintCallable, Category="Bot")
	FVector GetTargetPosition() const;
	UFUNCTION(BlueprintCallable, Category="Bot")
	FVector GetTargetDirectionHazed() const;

private:
};
