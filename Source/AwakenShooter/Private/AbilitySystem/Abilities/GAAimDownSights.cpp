// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GAAimDownSights.h"

#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/GameplayTags.h"
#include "AbilitySystem/Tasks/GTAimTransition.h"
#include "Character/ASCharacter.h"
#include "Character/ASPlayerCharacter.h"
#include "Items/Gun.h"

class UAbilityTask_WaitGameplayEvent;

UGAAimDownSights::UGAAimDownSights()
{
	SetAssetTags(FGameplayTagContainer(FASGameplayTags::Ability::AimDownSights));
}

void UGAAimDownSights::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	Character = Cast<AASPlayerCharacter>(ActorInfo->AvatarActor.Get());
	if (!Character)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	if (!Character->GetEquippedGun())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	DefaultPosition = Character->GetEquippedGun()->GetDefaultGunPosition();
	
	TransitionTask(Character->GetEquippedGun()->GetADSPosition());
	
	if (ADSEffect)
	{
		AppliedEffectHandle = BP_ApplyGameplayEffectToOwner(ADSEffect, 1.f,1);
		Character->TryActivateAbilityByTag(FASGameplayTags::Ability::BulletTime);
	}
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UGAAimDownSights::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (Character->GetEquippedGun())
	{
		TransitionTask(DefaultPosition);
	}
	
	if (AppliedEffectHandle.IsValid())
	{
		BP_RemoveGameplayEffectFromOwnerWithHandle(AppliedEffectHandle);
		Character->TryCancelAbilitiesWithTag(FASGameplayTags::Ability::BulletTime);
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGAAimDownSights::TransitionTask(const FVector& TargetLocation)
{
	if (Character->ADSTransitionTaskInstance && Character->ADSTransitionTaskInstance->IsActive())
	{
		Character->ADSTransitionTaskInstance->ExternalCancel();
	}
	Character->ADSTransitionTaskInstance = UGTAimTransition::Create(Character->GetGameplayTasksComponent(), TargetLocation, .4f);
	Character->ADSTransitionTaskInstance->ReadyForActivation();
}
