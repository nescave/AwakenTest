// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GAFire.generated.h"

/**
 * 
 */
UCLASS()
class AWAKENSHOOTER_API UGAFire : public UGameplayAbility
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditDefaultsOnly, Category = "AbilitySpecs")
	TSubclassOf<UGameplayEffect> DamageEffect;	

	UPROPERTY(Transient)
	TObjectPtr<class AGun> Gun;
	UPROPERTY(Transient)
	TObjectPtr<class AASCharacter> Character;

public:
	UGAFire();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	void ApplyCameraRecoil();
	bool TraceFire(FHitResult& OutHitResult, float InBulletRadius = 2.f);
	FVector GetShotDirectionWithSpread(const FVector& InShotDirection);
	
};
