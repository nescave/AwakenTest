// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/CharacterAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "Character/ASCharacter.h"
#include "General/DebugCVars.h"


UCharacterAttributeSet::UCharacterAttributeSet():
	Super(),
	Health(100.f),
	MaxHealth(100.f), 
	HealthRegen(5.f), 
	Energy(100), 
	MaxEnergy(100.f), 
	EnergyRegen(10.f), 
	MovementSpeed(1.f), 
	JumpPower(350.f),
	AccuracyModifier(1.f),
	DamageOutputModifier(.5f)
{}

void UCharacterAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	else if (Attribute== GetEnergyAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxEnergy());
	}

	Super::PreAttributeChange(Attribute, NewValue);
}

void UCharacterAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		if (NewValue > GetMaxHealth())
		{
			UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
			check(ASC);

			ASC->ApplyModToAttribute(GetHealthAttribute(), EGameplayModOp::Override, GetMaxHealth());
		}
	}
	
	if (Attribute == GetMaxEnergyAttribute())
	{
		// Make sure current health is not greater than the new max health.
		if (NewValue > GetMaxEnergy())
		{
			UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
			check(ASC);

			ASC->ApplyModToAttribute(GetHealthAttribute(), EGameplayModOp::Override, GetMaxEnergy());
		}
	}
	if (OldValue == NewValue)
		return;
	
	if (AASCharacter* OwningCharacter = Cast<AASCharacter>(GetOwningActor()))
	{
		if (Attribute == GetHealthAttribute())
			OwningCharacter->GetOnHealthChangedDelegate().Broadcast(NewValue);
		if (Attribute == GetEnergyAttribute())
			OwningCharacter->GetOnEnergyChangedDelegate().Broadcast(NewValue);
		if (Attribute == GetMovementSpeedAttribute())
			OwningCharacter->GetOnMovementSpeedChangedDelegate().Broadcast(NewValue);
		if (Attribute == GetAccuracyModifierAttribute())
			OwningCharacter->GetOnAccuracyModifierChangedDelegate().Broadcast(NewValue);

		OwningCharacter->GetOnAttributeChangedDelegate().Broadcast(Attribute, NewValue);
	}
}

bool UCharacterAttributeSet::PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data)
{
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}
	
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		if (Data.EvaluatedData.Magnitude > 0.0f && GetHealth() >= GetMaxHealth())
		{
			return false;
		}
	}
	else if (Data.EvaluatedData.Attribute == GetEnergyAttribute())
	{
		if (Data.EvaluatedData.Magnitude > 0.0f && GetEnergy() >= GetMaxEnergy())
		{
			return false;
		}
	}
	return true;
}

void UCharacterAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
		// Try to extract a hit result
		FHitResult HitResult;
		if (Context.GetHitResult())
		{
			HitResult = *Context.GetHitResult();
		}
		const float DamageDone = GetDamage();
		SetDamage(0.f);
		if (DamageDone > 0.f)
		{
			if (AASCharacter* OwningCharacter = Cast<AASCharacter>(GetOwningActor()))
			{
				OwningCharacter->GetOnAppliedDamageDelegate().Broadcast(DamageDone, HitResult);
				if (FASCVars::ASGodMode != 0 && OwningCharacter->IsPlayerControlled())
					return;
			}
			const float NewHealth = GetHealth() - DamageDone;
			SetHealth(FMath::Clamp(NewHealth, 0.0f, GetMaxHealth()));
		}
	}// damage
}


