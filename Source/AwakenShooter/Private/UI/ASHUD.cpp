// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ASHUD.h"

#include "AwakenShooter.h"
#include "Character/ASCharacter.h"
#include "UI/ASMainWidget.h"

AASHUD::AASHUD()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.5f;
}

void AASHUD::BeginPlay()
{
	Super::BeginPlay();

	if (!MainWidgetClass)
	{
		UE_LOG(LogAwakenShooter, Warning, TEXT("MainWidgetClass not set for HUD %s"), *GetName());
		MainWidget = CreateWidget<UASMainWidget>(GetWorld(), UUserWidget::StaticClass());
		return;
	}
	MainWidget = CreateWidget<UASMainWidget>(GetWorld(), MainWidgetClass);
	if (auto GameInstance = GetGameInstance())
	{
		if (AASCharacter* ControlledCharacter = Cast<AASCharacter>(GameInstance->GetPrimaryPlayerController()->GetPawn()))
		{
			MainWidget->SetCharacter(ControlledCharacter);
			MainWidget->AddToViewport();
		}
	}
}

void AASHUD::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AASHUD::DrawHUD()
{
	Super::DrawHUD();
}
