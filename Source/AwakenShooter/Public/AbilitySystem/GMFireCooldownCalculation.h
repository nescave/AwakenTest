// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "GMFireCooldownCalculation.generated.h"

/**
 * 
 */
UCLASS()
class AWAKENSHOOTER_API UGMFireCooldownCalculation : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
public:
	UGMFireCooldownCalculation();
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};
