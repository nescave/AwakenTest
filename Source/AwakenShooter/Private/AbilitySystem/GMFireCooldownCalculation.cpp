// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/GMFireCooldownCalculation.h"

#include "AbilitySystem/GunAttributeSet.h"

UGMFireCooldownCalculation::UGMFireCooldownCalculation()
{
	RelevantAttributesToCapture.Add(
	FGameplayEffectAttributeCaptureDefinition(
		UGunAttributeSet::GetFireRateAttribute(),
		EGameplayEffectAttributeCaptureSource::Target,
		false
	));
}

float UGMFireCooldownCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	FAggregatorEvaluateParameters Params;

	float FireRate = 1.f;
	GetCapturedAttributeMagnitude(
		FGameplayEffectAttributeCaptureDefinition(
			UGunAttributeSet::GetFireRateAttribute(),
			EGameplayEffectAttributeCaptureSource::Target,
			false),
		Spec,
		Params,
		FireRate
	);

	FireRate = FMath::Max(FireRate, 0.01f);

	return 1.f / FireRate;
}
