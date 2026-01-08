// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "ASBotAIController.generated.h"

UCLASS(Abstract)
class AWAKENSHOOTER_API AASBotAIController : public AAIController
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<UAIPerceptionComponent> Perception;
	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<UGameplayTasksComponent> GameplayTasksComponent;
	UPROPERTY()
	class UAISenseConfig_Sight* SightConfig;
	UPROPERTY()
	AActor* TargetActor;
	
public:
	AASBotAIController();

protected:
	UFUNCTION()
	void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	UPROPERTY(BlueprintReadOnly, Category="Bot")
	bool bActiveHearingStimulus;
	UPROPERTY(BlueprintReadOnly, Category="Bot")
	bool bActiveSightStimulus;
	UPROPERTY(BlueprintReadOnly, Category="Bot")
	bool bActiveDamageStimulus;
	
	UFUNCTION(BlueprintImplementableEvent, Category="Bot")
	void OnHearingStimulus(AActor* Actor, FAIStimulus Stimulus);
	UFUNCTION(BlueprintImplementableEvent, Category="Bot")
	void OnHearingStimulusEnded(AActor* Actor, FAIStimulus Stimulus);
	UFUNCTION(BlueprintImplementableEvent, Category="Bot")
	void OnSightStimulus(AActor* Actor, FAIStimulus Stimulus);
	UFUNCTION(BlueprintImplementableEvent, Category="Bot")
	void OnSightStimulusEnded(AActor* Actor, FAIStimulus Stimulus);
	UFUNCTION(BlueprintImplementableEvent, Category="Bot")
	void OnDamageStimulus(AActor* DamagedActor, FAIStimulus Stimulus);
	
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category="Bot")
	void SetTarget(AActor* Target);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Bot")
	AActor* GetTarget() const { return TargetActor; }

	UFUNCTION(BlueprintCallable, Category="Bot")
	AASBotCharacter* GetBotCharacter() const;

};
