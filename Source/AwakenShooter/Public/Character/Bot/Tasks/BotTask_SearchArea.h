// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BotTaskBase.h"
#include "BotTask_SearchArea.generated.h"

/**
 * 
 */
UCLASS()
class AWAKENSHOOTER_API UBotTask_SearchArea : public UBotTaskBase
{
	GENERATED_BODY()
	
	FVector SearchAreaCenter;
	float SearchRadius;
	float SearchTime;
	float ElapsedTime;
public:
	UBotTask_SearchArea(const FObjectInitializer& ObjectInitializer);
	
protected:
	FVector GetNavLocationInArea();
	void WalkToNextLocation();
public:
	static UBotTask_SearchArea* Create(const TScriptInterface<IGameplayTaskOwnerInterface>& TaskOwner,
		const FVector& InTargetLocation, float InSearchRadius, float InSearchTime);

	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;

private:
	FVector TargetLocation;
};
