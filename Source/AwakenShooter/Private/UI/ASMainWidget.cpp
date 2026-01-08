// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ASMainWidget.h"

#include "AwakenShooter.h"
#include "Character/ASCharacter.h"

void UASMainWidget::SetCharacter(AASCharacter* NewCharacter)
{
	if (!NewCharacter)
	{
		UE_LOG(LogAwakenShooter, Warning, TEXT("NewCharacter is null"));
		return;
	}
	ControlledCharacter = NewCharacter;
	ControlledCharacter->GetOnHealthChangedDelegate().AddDynamic(this, &UASMainWidget::UpdateHealthBar);
	ControlledCharacter->GetOnEnergyChangedDelegate().AddDynamic(this, &UASMainWidget::UpdateEnergyBar);
	ControlledCharacter->GetOnAccuracyModifierChangedDelegate().AddDynamic(this, &UASMainWidget::UpdateCrosshair);
}
