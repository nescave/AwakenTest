// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GAUseSecondary.h"

#include "AbilitySystem/GameplayTags.h"
#include "Character/ASCharacter.h"
#include "Items/Gun.h"

UGAUseSecondary::UGAUseSecondary()
{
	SetAssetTags(FGameplayTagContainer(FASGameplayTags::Ability::Secondary));
}

void UGAUseSecondary::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, false, true);
		return;
	}
	AGun* Gun = Cast<AGun>(ActorInfo->AvatarActor);
	if (!Gun)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, false, true);
		return;
	}
	
	AASCharacter* Character = Gun->GetGunHolder();
	if (!Character)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, false, true);
		return;
	}

	Character->TryActivateAbilityByTag(FASGameplayTags::Ability::Secondary);

	EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}
