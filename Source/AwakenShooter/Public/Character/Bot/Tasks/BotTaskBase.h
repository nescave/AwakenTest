// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTask.h"
#include "GameplayTasksComponent.h"
#include "Character/Bot/ASBotAIController.h"
#include "BotTaskBase.generated.h"

DECLARE_DELEGATE(FOnBotTaskEnded)
/**
 * 
 */
UCLASS()
class AWAKENSHOOTER_API UBotTaskBase : public UGameplayTask
{
	GENERATED_BODY()

public:
	UBotTaskBase(const FObjectInitializer& ObjectInitializer);

protected:
	bool bExternalCancelRequested;
	
	UPROPERTY()
	TObjectPtr<AASBotAIController> BotController;
	FOnBotTaskEnded OnBotTaskEnded;

	virtual void OnDestroy(bool bInOwnerFinished) override;
public:
	FOnBotTaskEnded& GetOnBotTaskEndedDelegate() { return OnBotTaskEnded; }

	virtual void ExternalCancel() override;
	template <class T>
	static T* NewBotTask(TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner, FName InstanceName = FName())
	{
		T* NewBotTask = TaskOwner.GetInterface() != nullptr ? NewTask<T>(*TaskOwner, InstanceName) : nullptr;
		if (auto GameplayTasksComp = Cast<UGameplayTasksComponent>(TaskOwner.GetObject()))
		{
			if (APawn* BotPawn = Cast<APawn>(GameplayTasksComp->GetOwner()))
			NewBotTask->BotController = Cast<AASBotAIController>(BotPawn->GetController());
			return NewBotTask;
		}
		return nullptr;
	}
};
