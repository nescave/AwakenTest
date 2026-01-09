// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GAMelee.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/GameplayTags.h"
#include "Character/ASPlayerCharacter.h"
#include "Components/SphereComponent.h"
#include "Items/Gun.h"

UGAMelee::UGAMelee() :
	PushbackForce(10.f)
{
	SetAssetTags(FGameplayTagContainer(FASGameplayTags::Ability::Melee));
}

void UGAMelee::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
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
	
	Gun = Character->GetEquippedGun();
	if (!Gun)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	Character->GetMeleeHitBox()->OnComponentBeginOverlap.AddDynamic(this, &UGAMelee::ApplyHitEffects);

	Gun->GetGunAbilitySystemComponent()->ExecuteGameplayCue(FASGameplayTags::GameplayCue::Gun::Melee);
	
	UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this,
		FName("MeleeMontage"), Gun->GetMeleeMontage()); 
	Task->OnCompleted.AddDynamic(this, &UGAMelee::OnMontageEnded);
	Task->OnInterrupted.AddDynamic(this, &UGAMelee::OnMontageEnded);
	Task->ReadyForActivation();
	
	UAbilityTask_WaitGameplayEvent* HitBoxActivatedTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		FASGameplayTags::Ability::Event::ActivatedHitBox);
	HitBoxActivatedTask->EventReceived.AddDynamic(this, &UGAMelee::OnHitBoxActivated);
	HitBoxActivatedTask->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* HitBoxDeactivatedTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		FASGameplayTags::Ability::Event::DeactivatedHitBox);
	HitBoxDeactivatedTask->EventReceived.AddDynamic(this, &UGAMelee::OnHitBoxDeactivated);
	HitBoxDeactivatedTask->ReadyForActivation();

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UGAMelee::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Character->GetMeleeHitBox()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGAMelee::ApplyHitEffects(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == Character)
		return;
	
	if (AASCharacter* OtherCharacter = Cast<AASCharacter>(OtherActor))
	{
		if (MeleeEffect)
		{
			FHitResult HitResult = SweepResult;
			if (!bFromSweep)
			{
				TArray<FHitResult> HitResults;
				FCollisionQueryParams Params;
				Params.AddIgnoredActor(Character);
				GetWorld()->SweepMultiByChannel(HitResults, Character->GetActorLocation(), OtherCharacter->GetActorLocation(), FQuat::Identity, ECC_Pawn, FCollisionShape::MakeSphere(5.f), Params);				

				for (const auto& Hit : HitResults)
				{
					if (Hit.GetActor() == OtherCharacter)
					{
						HitResult = Hit;
						break;
					}
				}
			}
			
			FGameplayEffectContextHandle EffectContext = Character->GetAbilitySystemComponent()->MakeEffectContext();
			EffectContext.AddSourceObject(Character);
			EffectContext.AddHitResult(HitResult);
			FGameplayEventData EventData;
			EventData.EventTag = FASGameplayTags::Event::Hit;
			EventData.Instigator = Character;
			EventData.Target = OtherActor;
			EventData.ContextHandle = EffectContext;
			EventData.EventMagnitude = PushbackForce;
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OtherActor, FASGameplayTags::Event::Hit, EventData);
			
			Gun->GetGunAbilitySystemComponent()->BP_ApplyGameplayEffectToTarget(MeleeEffect, OtherCharacter->GetAbilitySystemComponent(), 1.f,EffectContext);
			Gun->GetGunAbilitySystemComponent()->ExecuteGameplayCue(FASGameplayTags::GameplayCue::Hit::MeleeImpact);
		}
	}
}

void UGAMelee::OnHitBoxActivated(FGameplayEventData Payload)
{
	Character->GetMeleeHitBox()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void UGAMelee::OnHitBoxDeactivated(FGameplayEventData Payload)
{
	Character->GetMeleeHitBox()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	bRetriggerInstancedAbility = true;
}

void UGAMelee::OnMontageEnded()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}


