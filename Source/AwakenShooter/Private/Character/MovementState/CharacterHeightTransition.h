// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTask.h"
#include "CharacterHeightTransition.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTransitionUpdate, float, Value);

/**
 * 
 */
UCLASS()
class AWAKENSHOOTER_API UCharacterHeightTransition : public UGameplayTask
{
	GENERATED_BODY()

	float TargetCameraHeight;
	float TargetCapsuleHalfHeight;
	float TransitionDuration;

	UPROPERTY()
	class UCameraComponent* CameraComponent;
	UPROPERTY()
	class UCapsuleComponent* CapsuleComponent;

	float ElapsedTime;
	
public:
	UCharacterHeightTransition(const FObjectInitializer& ObjectInitializer);
	
	static UCharacterHeightTransition* CharacterHeightTransition(
		const TScriptInterface<IGameplayTaskOwnerInterface>& TaskOwner,
		float InTargetCameraHeight,
		float InTargetCapsuleHalfHeight,
		UCameraComponent* CameraComponent,
		UCapsuleComponent* CapsuleComponent,
		float TransitionDuration);

protected:
	virtual void TickTask(float DeltaTime) override;

};
