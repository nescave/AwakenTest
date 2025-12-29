// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GAUseMain.h"

#include "AbilitySystem/GameplayTags.h"
#include "Character/ASCharacter.h"
#include "Items/Gun.h"

UGAUseMain::UGAUseMain()
{
	SetAssetTags(FGameplayTagContainer(FASGameplayTags::Ability_Main));
}

void UGAUseMain::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                 const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
		return;
	}
	AASCharacter* Character = Cast<AASCharacter>(ActorInfo->AvatarActor);
	if (!Character)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
		return;
	}
	
	if (AGun* Gun = Character->GetEquippedGun())
	{
		Gun->MainAction();
	}
	EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}
