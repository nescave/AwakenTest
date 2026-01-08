// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GAEquipGun.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/GameplayTags.h"
#include "Character/ASPlayerCharacter.h"
#include "Items/Gun.h"

UGAEquipGun::UGAEquipGun()
{
	SetAssetTags(FGameplayTagContainer(FASGameplayTags::Ability::EquipGun));
}

void UGAEquipGun::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                  const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);


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

	Gun = Cast<AGun>(Character->GetPossibleInteraction());
	if (!Gun)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy( this,
		FName("DryFireMontage"), Gun->GetEquipMontage());
	MontageTask->OnCompleted.AddDynamic(this, &UGAEquipGun::OnMontageEnded);
	MontageTask->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* PickUpGunTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this,
		FASGameplayTags::Ability::Event::GunEquipped);
	PickUpGunTask->EventReceived.AddDynamic(this, &UGAEquipGun::PickUpGun);
	PickUpGunTask->ReadyForActivation();
}

void UGAEquipGun::OnMontageEnded()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGAEquipGun::PickUpGun(FGameplayEventData Payload)
{
	Character->EquipGun(Gun);
}
