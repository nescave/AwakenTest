// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Bot/ASBotAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Character/Bot/ASBotCharacter.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Hearing.h"


// Sets default values
AASBotAIController::AASBotAIController()
{
	PrimaryActorTick.bCanEverTick = true;

	Perception = CreateDefaultSubobject<UAIPerceptionComponent>("Perception");
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>("Sight Config");

	SightConfig->SightRadius = 2000.f;
	SightConfig->LoseSightRadius = 2400.f;
	SightConfig->PeripheralVisionAngleDegrees = 90.f;
	SightConfig->SetMaxAge(5.f);
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	Perception->ConfigureSense(*SightConfig);
	Perception->SetDominantSense(UAISense_Sight::StaticClass());
}

void AASBotAIController::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!Actor->ActorHasTag(FName("Player")))
	{
		return;
	}
	if (Stimulus.WasSuccessfullySensed())
	{
		if (Stimulus.Type == UAISense_Hearing::StaticClass()->GetDefaultObject<UAISense_Hearing>()->GetSenseID())
		{
			bActiveHearingStimulus = true;
			SetTarget(Actor);
			OnHearingStimulus(Actor, Stimulus);
		}
		else if (Stimulus.Type == UAISense_Sight::StaticClass()->GetDefaultObject<UAISense_Sight>()->GetSenseID())
		{
			bActiveSightStimulus = true;
			SetTarget(Actor);
			OnSightStimulus(Actor, Stimulus);
		}
		else
		{
			bActiveDamageStimulus = true;
			SetTarget(Actor);
			OnDamageStimulus(Actor, Stimulus);
		}
	}
	else
	{
		if (Stimulus.Type == UAISense_Hearing::StaticClass()->GetDefaultObject<UAISense_Hearing>()->GetSenseID())
		{
			if (!Perception->HasAnyActiveStimulus(*Actor))
				bActiveHearingStimulus = false;
			
			OnHearingStimulusEnded(Actor, Stimulus);
		}
		else if (Stimulus.Type == UAISense_Sight::StaticClass()->GetDefaultObject<UAISense_Sight>()->GetSenseID())
		{
			bActiveSightStimulus = false;
			OnSightStimulusEnded(Actor, Stimulus);
		}
		if (!bActiveHearingStimulus && !bActiveSightStimulus && !bActiveDamageStimulus)
		{
			SetTarget(nullptr);
		}
	}
}

void AASBotAIController::BeginPlay()
{
	Super::BeginPlay();
	Perception->OnTargetPerceptionUpdated.AddDynamic(this, &AASBotAIController::OnPerceptionUpdated);
}

void AASBotAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AASBotAIController::SetTarget(AActor* Target)
{
	if (Target == TargetActor)
	{
		return;
	}

	TargetActor = Target;
	if (GetBlackboardComponent())
		GetBlackboardComponent()->SetValueAsObject(FName("TargetActor"), TargetActor);
}

AASBotCharacter* AASBotAIController::GetBotCharacter() const
{
	return Cast<AASBotCharacter>(GetPawn());
}

