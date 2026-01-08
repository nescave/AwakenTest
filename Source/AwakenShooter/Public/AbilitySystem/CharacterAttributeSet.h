// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "CharacterAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class AWAKENSHOOTER_API UCharacterAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UCharacterAttributeSet();

	UPROPERTY(BlueprintReadOnly, Category = "Health")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS_BASIC(UCharacterAttributeSet, Health)
	UPROPERTY(BlueprintReadOnly, Category = "Health")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS_BASIC(UCharacterAttributeSet, MaxHealth)
	UPROPERTY(BlueprintReadOnly, Category = "Health")
	FGameplayAttributeData HealthRegen;
	ATTRIBUTE_ACCESSORS_BASIC(UCharacterAttributeSet, HealthRegen)

	UPROPERTY(BlueprintReadOnly, Category = "Energy")
	FGameplayAttributeData Energy;
	ATTRIBUTE_ACCESSORS_BASIC(UCharacterAttributeSet, Energy)
	UPROPERTY(BlueprintReadOnly, Category = "Energy")
	FGameplayAttributeData MaxEnergy;
	ATTRIBUTE_ACCESSORS_BASIC(UCharacterAttributeSet, MaxEnergy)
	UPROPERTY(BlueprintReadOnly, Category = "Energy")
	FGameplayAttributeData EnergyRegen;
	ATTRIBUTE_ACCESSORS_BASIC(UCharacterAttributeSet, EnergyRegen)
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	FGameplayAttributeData MovementSpeed;
	ATTRIBUTE_ACCESSORS_BASIC(UCharacterAttributeSet, MovementSpeed)
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	FGameplayAttributeData JumpPower;
	ATTRIBUTE_ACCESSORS_BASIC(UCharacterAttributeSet, JumpPower)

	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS_BASIC(UCharacterAttributeSet, Damage)

	UPROPERTY(BlueprintReadOnly, Category = "Weapon Handling")
	FGameplayAttributeData AccuracyModifier;
	ATTRIBUTE_ACCESSORS_BASIC(UCharacterAttributeSet, AccuracyModifier)
	UPROPERTY(BlueprintReadOnly, Category = "Weapon Handling")
	FGameplayAttributeData DamageOutputModifier;
	ATTRIBUTE_ACCESSORS_BASIC(UCharacterAttributeSet, DamageOutputModifier)
	
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	virtual bool PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
};
