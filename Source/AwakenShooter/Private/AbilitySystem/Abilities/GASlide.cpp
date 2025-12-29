// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GASlide.h"

#include "AwakenShooter.h"
#include "AbilitySystem/GameplayTags.h"
#include "Character/ASCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

UGASlide::UGASlide() :
	SlideSpeedBoost(1.5f)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	SetAssetTags(FGameplayTagContainer(FASGameplayTags::Ability_Slide));
}

void UGASlide::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	ACharacter* Character = Cast<AASCharacter>(ActorInfo->AvatarActor.Get());
	
	if (!Character)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	Character->GetCharacterMovement()->Velocity *= SlideSpeedBoost;
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
