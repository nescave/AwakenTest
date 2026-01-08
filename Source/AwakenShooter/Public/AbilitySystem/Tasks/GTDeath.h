// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTask.h"
#include "GTDeath.generated.h"

/**
 * 
 */
UCLASS()
class AWAKENSHOOTER_API UGTDeath : public UGameplayTask
{
	GENERATED_BODY()

	UPROPERTY()
	class AASCharacter* ControlledCharacter;
	UPROPERTY()
	TArray<UMaterialInstanceDynamic*> MaterialInstances;
	float TaskDuration;
	float ElapsedTime;

public:
	UGTDeath(const FObjectInitializer& ObjectInitializer);

	static UGTDeath* Create(const TScriptInterface<IGameplayTaskOwnerInterface>& TaskOwner, AASCharacter* InCharacter,
		const TArray<UMaterialInstanceDynamic*>& InMIDsArray, float InTaskDuration);

	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;
	
};
