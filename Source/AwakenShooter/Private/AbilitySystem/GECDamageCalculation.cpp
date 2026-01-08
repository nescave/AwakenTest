// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/GECDamageCalculation.h"

#include "AbilitySystem/CharacterAttributeSet.h"
#include "AbilitySystem/GunAttributeSet.h"
#include "Character/ASCharacter.h"
#include "Items/Gun.h"

void UGECDamageCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                                   FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	float BonusDamageMultiplier = 0.f;
	if (auto Gun = Cast<AGun>(Spec.GetContext().GetEffectCauser()))
	{
		BonusDamageMultiplier += Gun->GetGunHolder()->GetDamageOutputModifier() -1.f;
	}
	float BaseDamage = Spec.GetModifierMagnitude(0);
	const FName BoneName = Spec.GetContext().GetHitResult() ? Spec.GetContext().GetHitResult()->BoneName : NAME_None;
	if (BoneName != NAME_None)
	{
		if (BoneName ==  "head" || BoneName == "neck_01" || BoneName == "neck_02")
		{
			BonusDamageMultiplier += 1.0f;
		}
	}
	float FinalDamage = BaseDamage * BonusDamageMultiplier;
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
		UCharacterAttributeSet::GetDamageAttribute(), EGameplayModOp::AddBase, FinalDamage));
}
