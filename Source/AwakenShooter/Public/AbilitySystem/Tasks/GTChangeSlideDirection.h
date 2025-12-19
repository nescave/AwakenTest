// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTask.h"
#include "GTChangeSlideDirection.generated.h"

/**
 * 
 */
UCLASS()
class AWAKENSHOOTER_API UGTChangeSlideDirection : public UGameplayTask
{
	GENERATED_BODY()

	FVector TargetDirection;
	
	UPROPERTY()
	class USlidingState* SlidingState;
	
	float ElapsedTime;
	float TransitionDuration;
	
public:
	UGTChangeSlideDirection(const FObjectInitializer& ObjectInitializer);

	static UGTChangeSlideDirection* Create(const TScriptInterface<IGameplayTaskOwnerInterface>& TaskOwner,
		const FVector& TargetDirection, USlidingState* SlidingState, float TransitionDuration);

protected:
	virtual void TickTask(float DeltaTime) override;	
};
