// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GASprint.generated.h"

/**
 * 
 */
UCLASS()
class AWAKENSHOOTER_API UGASprint : public UGameplayAbility
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Sprint")
	TSubclassOf<UGameplayEffect> SprintEffect;
	UPROPERTY(EditDefaultsOnly, Category = "Sprint")
	TSubclassOf<UGameplayEffect> SprintEaseOutEffect;
	UPROPERTY(Transient)
	TObjectPtr<class AASCharacter> Character;

	FActiveGameplayEffectHandle AppliedEffectHandle;
public:
	UGASprint();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
protected:
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
};
