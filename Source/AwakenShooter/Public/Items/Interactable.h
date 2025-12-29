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
	virtual void Interact(class AASCharacter* Interactor) = 0;
	virtual void SetHighlighted(float HighlightValue) = 0;
};
