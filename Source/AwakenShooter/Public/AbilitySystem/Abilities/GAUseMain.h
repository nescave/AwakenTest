// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GAUseMain.generated.h"

/**
 * 
 */
UCLASS()
class AWAKENSHOOTER_API UGAUseMain : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UGAUseMain();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
};
