// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GAFire.generated.h"

class AGun;
/**
 * 
 */
UCLASS()
class AWAKENSHOOTER_API UGAFire : public UGameplayAbility
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditDefaultsOnly, Category = "AbilitySpecs")
	UAnimMontage* RecoilMontage;
	
	void ApplyCameraRecoil(const AGun* Gun);
	bool TraceFire(class AASCharacter& HitCharacter);
public:
	UGAFire();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
