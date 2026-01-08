// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GAEquipGun.generated.h"

/**
 * 
 */
UCLASS()
class AWAKENSHOOTER_API UGAEquipGun : public UGameplayAbility
{
	GENERATED_BODY()

	UPROPERTY()
	class AASPlayerCharacter* Character;
	UPROPERTY()
	class AGun* Gun;
	
public:
	UGAEquipGun();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
	                             const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UFUNCTION()
	void OnMontageEnded();
	UFUNCTION()
	void PickUpGun(FGameplayEventData Payload);
};
