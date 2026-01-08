// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

UINTERFACE()
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AWAKENSHOOTER_API IInteractable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "Interactable")
	void Interact(class AASCharacter* Interactor);
	UFUNCTION(BlueprintNativeEvent, Category = "Interactable")
	void SetHighlighted(float HighlightValue);
	UFUNCTION(BlueprintNativeEvent, Category = "Interactable")
	bool IsInteractable() const;
};
