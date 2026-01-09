// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GASprint.h"

#include "AbilitySystem/GameplayTags.h"
#include "Character/ASCharacter.h"

UGASprint::UGASprint()
{
	SetAssetTags(FGameplayTagContainer(FASGameplayTags::Ability::Sprint));
}

void UGASprint::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	Character = Cast<AASCharacter>(ActorInfo->AvatarActor.Get());
	if (!Character)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	if (SprintEffect)
	{
		AppliedEffectHandle = BP_ApplyGameplayEffectToOwner(SprintEffect, 1.f,1);
	}
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UGASprint::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (AppliedEffectHandle.IsValid())
	{
		BP_RemoveGameplayEffectFromOwnerWithHandle(AppliedEffectHandle);
	}
	if (SprintEaseOutEffect)
	{
		BP_ApplyGameplayEffectToOwner(SprintEaseOutEffect, 1.f,1);
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
