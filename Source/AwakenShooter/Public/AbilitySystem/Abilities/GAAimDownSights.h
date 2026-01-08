// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GAAimDownSights.generated.h"

/**
 * 
 */
UCLASS()
class AWAKENSHOOTER_API UGAAimDownSights : public UGameplayAbility
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category = "ADS")
	TSubclassOf<UGameplayEffect> ADSEffect;
	UPROPERTY(Transient)
	TObjectPtr<class AASPlayerCharacter> Character;

	FVector DefaultPosition;
	
	FActiveGameplayEffectHandle AppliedEffectHandle;
	
public:
	UGAAimDownSights();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	void TransitionTask(const FVector& TargetLocation);
};
