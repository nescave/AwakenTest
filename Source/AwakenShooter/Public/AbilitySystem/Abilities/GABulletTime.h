// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GABulletTime.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class AWAKENSHOOTER_API UGABulletTime : public UGameplayAbility
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "BulletTime")
	TSubclassOf<UGameplayEffect> BulletTimeEffect;
	UPROPERTY(EditDefaultsOnly, Category = "BulletTime")
	TSubclassOf<UGameplayEffect> PeriodicCost;
	
	UPROPERTY(EditDefaultsOnly, Category = "BulletTime")
	float BulletTimeValue;
public:
	UGABulletTime();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UFUNCTION()
	void OnEnergyDepleted();
};
