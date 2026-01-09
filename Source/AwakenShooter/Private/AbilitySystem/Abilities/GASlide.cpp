// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GASlide.h"

#include "AbilitySystem/GameplayTags.h"
#include "Character/ASPlayerCharacter.h"
#include "Character/MovementState/MovementStateComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

UGASlide::UGASlide() :
	SlideSpeedBoost(250.f)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	SetAssetTags(FGameplayTagContainer(FASGameplayTags::Ability::Slide));
}

void UGASlide::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	AASPlayerCharacter* Character = Cast<AASPlayerCharacter>(ActorInfo->AvatarActor.Get());
	
	if (!Character)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	Character->AddClampedVelocity(Character->GetVelocity().GetSafeNormal(), SlideSpeedBoost, true);
	Character->GetMovementStateMachine()->ChangeState(EMovementState::Sliding);

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
