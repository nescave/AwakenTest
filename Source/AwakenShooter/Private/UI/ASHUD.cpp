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
		AASCharacter* ControlledCharacter = Cast<AASCharacter>(GameInstance->GetPrimaryPlayerController()->GetPawn());
		ControlledCharacter->OnAttributeChangedDelegate().AddDynamic(this, &AASHUD::FlagForHudUpdate);
		MainWidget->SetCharacter(ControlledCharacter);
	}
	MainWidget->AddToViewport();
}

void AASHUD::FlagForHudUpdate(const FGameplayAttribute& Attribute, float NewValue)
{
	bNeedsUpdate = true;
}

void AASHUD::UpdateMainWidget()
{
	MainWidget->UpdateHealthBar();
	MainWidget->UpdateEnergyBar();
	MainWidget->UpdateAmmo();
}

void AASHUD::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bNeedsUpdate)
	{
		UpdateMainWidget();
		bNeedsUpdate = false;
	}
}

void AASHUD::DrawHUD()
{
	Super::DrawHUD();
}
