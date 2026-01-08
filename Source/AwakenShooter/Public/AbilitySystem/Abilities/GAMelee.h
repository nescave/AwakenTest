// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GAMelee.generated.h"

class UGameplayCueNotify_HitImpact;
/**
 * 
 */
UCLASS()
class AWAKENSHOOTER_API UGAMelee : public UGameplayAbility
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Melee")
	TSubclassOf<UGameplayEffect> MeleeEffect;
	UPROPERTY(EditDefaultsOnly, Category = "Melee")
	float PushbackForce;
	UPROPERTY(Transient)
	TObjectPtr<class AGun> Gun;
	UPROPERTY(Transient)
	TObjectPtr<class AASPlayerCharacter> Character;

public:
	UGAMelee();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
	                             const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
protected:
	UFUNCTION()
	void ApplyHitEffects(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnHitBoxActivated(FGameplayEventData Payload);
	UFUNCTION()
	void OnHitBoxDeactivated(FGameplayEventData Payload);
	UFUNCTION()
	void OnMontageEnded();
};
