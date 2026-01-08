// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GABulletTime.h"

#include "Abilities/Tasks/AbilityTask_WaitAttributeChange.h"
#include "AbilitySystem/CharacterAttributeSet.h"
#include "AbilitySystem/GameplayTags.h"
#include "Character/ASCharacter.h"

UGABulletTime::UGABulletTime() :
	BulletTimeValue(.33f)
{
	SetAssetTags(FGameplayTagContainer(FASGameplayTags::Ability::BulletTime));
}

void UGABulletTime::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
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
	
	UAbilityTask_WaitAttributeChange* Task = UAbilityTask_WaitAttributeChange::WaitForAttributeChangeWithComparison(this,
		Character->GetAttributeSet()->GetEnergyAttribute(), FGameplayTag(), FGameplayTag(),
		EWaitAttributeChangeComparison::LessThanOrEqualTo, 0.f);
	Task->OnChange.AddDynamic(this, &UGABulletTime::OnEnergyDepleted);
	Task->ReadyForActivation();
	
	BP_ApplyGameplayEffectToOwner(BulletTimeEffect);
	BP_ApplyGameplayEffectToOwner(PeriodicCost);
	
	Character->SetBulletTime(BulletTimeValue);
}

void UGABulletTime::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	AASCharacter* Character = Cast<AASCharacter>(ActorInfo->AvatarActor.Get());
	if (!Character)
	{
		return;
	}

	ActorInfo->AbilitySystemComponent->RemoveActiveGameplayEffectBySourceEffect(BulletTimeEffect, ActorInfo->AbilitySystemComponent.Get());
	ActorInfo->AbilitySystemComponent->RemoveActiveGameplayEffectBySourceEffect(PeriodicCost, ActorInfo->AbilitySystemComponent.Get());
	Character->SetBulletTime(1.f);
}

void UGABulletTime::OnEnergyDepleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
