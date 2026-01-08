// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ASHUD.generated.h"

/**
 * 
 */
UCLASS()
class AWAKENSHOOTER_API AASHUD : public AHUD
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="HUD", meta=(AllowPrivateAccess="true"))
	TObjectPtr<class UASMainWidget> MainWidget;

	bool bNeedsUpdate = false;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="HUD")
	TSubclassOf<UASMainWidget> MainWidgetClass;
	
public:
	AASHUD();
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="HUD")
	UASMainWidget* GetMainWidget() const { return MainWidget; }

	
protected:
	virtual void Tick(float DeltaTime) override;
	virtual void DrawHUD() override;
};
