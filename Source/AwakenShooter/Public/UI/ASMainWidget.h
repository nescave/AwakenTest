// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ASMainWidget.generated.h"

/**
 * 
 */
UCLASS()
class AWAKENSHOOTER_API UASMainWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, Category = "ASUI")
	TObjectPtr<class AASCharacter> ControlledCharacter;

public:
	void SetCharacter(AASCharacter* NewCharacter);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "ASUI")
	void UpdateHealthBar(float NewHealth);
	UFUNCTION(BlueprintImplementableEvent, Category = "ASUI")
	void UpdateEnergyBar(float NewEnergy);
	UFUNCTION(BlueprintImplementableEvent, Category = "ASUI")
	void UpdateCrosshair(float NewValue);

};
