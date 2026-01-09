// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/GECDamageCalculation.h"

#include "AbilitySystem/CharacterAttributeSet.h"
#include "AbilitySystem/GunAttributeSet.h"
#include "Character/ASCharacter.h"
#include "Items/Gun.h"

#pragma optimize("", off)
void UGECDamageCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                                   FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	float BonusDamageMultiplier = 1.f;
	float BaseDamage = Spec.GetModifierMagnitude(0);
	if (auto Gun = Cast<AGun>(Spec.GetContext().GetEffectCauser()))
	{
		BonusDamageMultiplier = Gun->GetGunHolder()->GetDamageOutputModifier();
		BaseDamage = Gun->GetBaseDamage();
	}
	const FName BoneName = Spec.GetContext().GetHitResult() ? Spec.GetContext().GetHitResult()->BoneName : NAME_None;
	if (BoneName != NAME_None && BonusDamageMultiplier >= 1.f)
	{
		if (BoneName ==  "head" || BoneName == "neck_01" || BoneName == "neck_02")
		{
			BonusDamageMultiplier += 1.0f;
		}
	}
	float FinalDamage = BaseDamage * BonusDamageMultiplier;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::SanitizeFloat(FinalDamage));
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
		UCharacterAttributeSet::GetDamageAttribute(), EGameplayModOp::AddBase, FinalDamage));
}
#pragma optimize("", on)
