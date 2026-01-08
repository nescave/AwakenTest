// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Character/ASCharacter.h"
#include "GAThrow.generated.h"

/**
 * 
 */
UCLASS()
class AWAKENSHOOTER_API UGAThrow : public UGameplayAbility
{
	GENERATED_BODY()
	
	UPROPERTY()
	AASCharacter* Character;

public:
	UGAThrow();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
	                             const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UFUNCTION()
	void OnThrow(FGameplayEventData Payload);
	UFUNCTION()
	void OnThrowMontageEnded();
};
