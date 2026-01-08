// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GADryFire.generated.h"

/**
 * 
 */
UCLASS()
class AWAKENSHOOTER_API UGADryFire : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGADryFire();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
protected:
	UFUNCTION()
	void OnMontageEnded();
};
