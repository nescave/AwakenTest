// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GAThrow.h"

#include "AbilitySystem/GameplayTags.h"
#include "Character/ASCharacter.h"
#include "Items/Gun.h"

UGAThrow::UGAThrow()
{
	SetAssetTags(FGameplayTagContainer(FASGameplayTags::Ability_Throw));
}

void UGAThrow::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
		return;
	}
	if (AASCharacter* Character = Cast<AASCharacter>(ActorInfo->AvatarActor))
	{
		if (!Character->GetEquippedGun())
			return;
	
		Character->GetEquippedGun()->Throw(Character->GetActorForwardVector() * 100.f);
		Character->SetEquippedGun(nullptr);
	}
	EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
	
}
