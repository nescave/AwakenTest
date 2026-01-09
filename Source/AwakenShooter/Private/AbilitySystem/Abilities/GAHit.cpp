// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GAHit.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystem/GameplayTags.h"
#include "Character/ASCharacter.h"
#include "Perception/AISense_Damage.h"

UGAHit::UGAHit()
{
	SetAssetTags(FGameplayTagContainer(FASGameplayTags::Ability::Hit));
}

void UGAHit::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
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

	const FGameplayEventData EventData = CurrentEventData;
	if (EventData.Target != Character) return;

	const FVector HitLocation = EventData.ContextHandle.GetHitResult()->ImpactPoint;
	const FVector IncomingDirection = -EventData.ContextHandle.GetHitResult()->Normal;

	UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this,
		FName("HitMontage"), Character->GetBestHitMontage(HitLocation, IncomingDirection));
	Task->OnCompleted.AddDynamic(this, &UGAHit::OnHitMontageEnded);
	Task->ReadyForActivation();

	Character->Stun(EventData.EventMagnitude * 10.f);

	UAISense_Damage::ReportDamageEvent(GetWorld(), Character, EventData.Instigator->GetInstigator(), EventData.EventMagnitude, EventData.Instigator->GetActorLocation(), Character->GetActorLocation());
	
	const FVector PushbackDirection = (FVector::VectorPlaneProject(IncomingDirection, FVector::UpVector).GetSafeNormal() + FVector::UpVector/ EventData.EventMagnitude * 3.f).GetSafeNormal();
	Character->LaunchCharacter(PushbackDirection * EventData.EventMagnitude * 10.f, true, false);
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UGAHit::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGAHit::OnHitMontageEnded()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, true);
}
