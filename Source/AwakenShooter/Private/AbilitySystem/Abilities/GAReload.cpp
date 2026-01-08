// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GAReload.h"

#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/GameplayTags.h"
#include "AbilitySystem/GunAttributeSet.h"
#include "Character/ASCharacter.h"
#include "Items/Gun.h"

UGAReload::UGAReload()
{
	SetAssetTags(FGameplayTagContainer(FGameplayTagContainer(FASGameplayTags::Ability::Reload)));
}

void UGAReload::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
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
	if (!Character->GetEquippedGun())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	PlayReloadAnimation();

	UAbilityTask_WaitGameplayEvent* Task = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		FASGameplayTags::Ability::Event::FinishedReloading);

	Task->EventReceived.AddDynamic(
		this,
		&UGAReload::OnFinishedReloading
	);
	Task->ReadyForActivation();
}

bool UGAReload::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	AASCharacter* Owner = Cast<AASCharacter>(ActorInfo->AvatarActor.Get());
	if (!Owner)
		return false;

	AGun* Gun = Owner->GetEquippedGun();
	if (!Gun)
		return false;

	if (Gun->GetGunAttributes()->GetMaxAmmo() == Gun->GetGunAttributes()->GetAmmo())
		return false;

	return true;
}

void UGAReload::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGAReload::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	auto ASC = GetAbilitySystemComponentFromActorInfo();
	ASC->CurrentMontageStop();
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UGAReload::OnFinishedReloading(FGameplayEventData)
{
	Character->GetEquippedGun()->Reload();
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGAReload::PlayReloadAnimation()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();

	ASC->PlayMontage(
		this,
		CurrentActivationInfo,
		Character->GetEquippedGun()->GetReloadMontage(),
		1.0f
	);
}
