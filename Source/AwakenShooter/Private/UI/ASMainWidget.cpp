// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ASMainWidget.h"

#include "AwakenShooter.h"

void UASMainWidget::SetCharacter(AASCharacter* NewCharacter)
{
	if (!NewCharacter)
	{
		UE_LOG(LogAwakenShooter, Warning, TEXT("NewCharacter is null"));
		return;
	}
	ControlledCharacter = NewCharacter;
}
