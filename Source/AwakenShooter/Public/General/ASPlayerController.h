// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ASPlayerController.generated.h"

class AASGameMode;
class UInputMappingContext;
class UUserWidget;
class UInputAction;
/**
 *  Simple first person Player Controller
 *  Manages the input mapping context.
 *  Overrides the Player Camera Manager class.
 */
UCLASS(abstract)
class AWAKENSHOOTER_API AASPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	UPROPERTY(Transient)
	TObjectPtr<AASGameMode> GameMode;
	
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* RestartAction;
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* QuitAction;
	
public:

	AASPlayerController();

	
protected:

	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	TArray<UInputMappingContext*> DefaultMappingContexts;

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	void HandleRestartAction();
	void HandleQuitAction();

	UFUNCTION()
	void SetupInputMappingsAfterRoundsEnd();
};
