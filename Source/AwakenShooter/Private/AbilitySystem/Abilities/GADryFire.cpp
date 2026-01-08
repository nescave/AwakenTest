// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GADryFire.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystem/GameplayTags.h"
#include "Character/ASCharacter.h"
#include "Items/Gun.h"

UGADryFire::UGADryFire()
{
	SetAssetTags(FGameplayTagContainer(FASGameplayTags::Ability::DryFire));
}

void UGADryFire::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	AASCharacter* Character = Cast<AASCharacter>(ActorInfo->AvatarActor.Get());
	if (!Character)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	AGun* Gun = Character->GetEquippedGun();
	if (!Gun)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy( this,
		FName("DryFireMontage"), Gun->GetDryFireMontage());
	Task->OnCompleted.AddDynamic(this, &UGADryFire::OnMontageEnded);
	Task->ReadyForActivation();
}

void UGADryFire::OnMontageEnded()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
