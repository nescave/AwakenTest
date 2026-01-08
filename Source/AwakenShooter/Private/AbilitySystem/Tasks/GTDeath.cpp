// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Tasks/GTDeath.h"

#include "Character/ASCharacter.h"

UGTDeath::UGTDeath(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer),
	ControlledCharacter(nullptr),
	TaskDuration(5.f),
	ElapsedTime(0.f)
{
	bTickingTask = true;
}

UGTDeath* UGTDeath::Create(const TScriptInterface<IGameplayTaskOwnerInterface>& TaskOwner, AASCharacter* InCharacter,
	const TArray<UMaterialInstanceDynamic*>& InMIDsArray, float InTaskDuration)
{
	UGTDeath* Task = NewTask<UGTDeath>(TaskOwner);
	check(InCharacter);
	Task->ControlledCharacter = InCharacter;
	Task->MaterialInstances = InMIDsArray;
	Task->TaskDuration = FMath::Max(InTaskDuration, KINDA_SMALL_NUMBER);
	return Task;
}

void UGTDeath::Activate()
{
	Super::Activate();
	int32 MaterialIndex = 0;
	for (const auto& MaterialInstance : MaterialInstances)
	{
		ControlledCharacter->GetVisibleMesh()->SetMaterial(MaterialIndex, MaterialInstance);
		MaterialIndex++;
	}
	ControlledCharacter->SetLifeSpan(TaskDuration);
}

void UGTDeath::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);
	ElapsedTime += DeltaTime;
	float LerpFactor = FMath::Min(ElapsedTime / TaskDuration, 1.f);

	for (const auto& MaterialInstance : MaterialInstances)
	{
		MaterialInstance->SetScalarParameterValue("DecayValue", LerpFactor);
	}
	
	if (LerpFactor >= 1.f)
	{
		EndTask();
	}
}
