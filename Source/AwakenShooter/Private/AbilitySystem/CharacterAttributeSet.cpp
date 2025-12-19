// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/CharacterAttributeSet.h"

#include "Character/ASCharacter.h"
#include "GameFramework/PawnMovementComponent.h"

UCharacterAttributeSet::UCharacterAttributeSet()
	: Super()
	, Health(100.f)
	, MaxHealth(100.f)
	, HealthRegen(5.f)
	, Energy(100)
	, MaxEnergy(100.f)
	, EnergyRegen(10.f)
	, MovementSpeed(1.f)
	, JumpPower(350.f)
{}

void UCharacterAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute.AttributeName == GetHealthAttribute().GetName())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	else if (Attribute.AttributeName == GetEnergyAttribute().GetName())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxEnergy());
	}
}

void UCharacterAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (OldValue == NewValue)
		return;
	
	if (AASCharacter* OwningCharacter = Cast<AASCharacter>(GetOwningActor()))
	{
		OwningCharacter->OnAttributeChangedDelegate().Broadcast(Attribute, NewValue);
	}
}


