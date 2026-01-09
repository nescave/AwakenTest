// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GAFire.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystem/GameplayTags.h"
#include "AbilitySystem/GunAttributeSet.h"
#include "Camera/CameraComponent.h"
#include "Character/ASCharacter.h"
#include "Character/ASPlayerCharacter.h"
#include "Character/Bot/ASBotCharacter.h"
#include "Character/MovementState/MovementStateBase.h"
#include "General/DebugCVars.h"
#include "Items/Gun.h"


UGAFire::UGAFire()
{
	SetAssetTags(FGameplayTagContainer(FASGameplayTags::Ability::Fire));
}

void UGAFire::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
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
	
	Gun = Character->GetEquippedGun();
	if (!Gun)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	if (auto ASC = Gun->GetGunAbilitySystemComponent())
	{
		ASC->ExecuteGameplayCue(FASGameplayTags::GameplayCue::Gun::MuzzleFlare0);

		// cancel reload if we start firing
		FGameplayTagContainer CancelAbilitiesTags = FGameplayTagContainer(FASGameplayTags::Ability::Reload);		
		ASC->CancelAbilities(&CancelAbilitiesTags);
	}
	
	if (AASCharacter* GunHolder = Gun->GetGunHolder())
	{
		if (UAnimInstance* AnimInstance = GunHolder->GetFirstPersonMesh()->GetAnimInstance())
		{
			AnimInstance->Montage_Play(Gun->GetFireMontage());
		}
	}

	constexpr float BulletRadius = 1.f;
	FHitResult HitResult;
	for (int32 ShotIndex = 0; ShotIndex < Gun->GetGunAttributes()->GetShotsPerRound(); ++ShotIndex)
	{
		if (TraceFire(HitResult, BulletRadius))
		{
			auto ASC = Gun->GetGunAbilitySystemComponent();
			FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(DamageEffect, 1.f, ASC->MakeEffectContext());
			Spec.Data->GetContext().AddHitResult(HitResult);
			if (AASCharacter* HitCharacter = Cast<AASCharacter>(HitResult.GetActor()))
			{
				// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Hit Actor: ") + HitCharacter->GetName());

				if (FASCVars::ASDebugDraw && FASCVars::ASDrawHitBoxes)
				{
					DrawDebugSphere(GetWorld(), HitResult.Location, BulletRadius, 10, FColor::Green, false, FASCVars::ASDebugDrawDuration, 0, 1.f);
				}
				if (DamageEffect && HitCharacter->GetCurrentMovementState() != EMovementState::Death)
				{
					ASC->BP_ApplyGameplayEffectSpecToTarget(Spec, HitCharacter->GetAbilitySystemComponent());
					
					// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Current ")
					// 	+ HitCharacter->GetName() + " Health: " + FString::FromInt(HitCharacter->GetHealth()) );

					FGameplayEffectContextHandle EffectContext = Character->GetAbilitySystemComponent()->MakeEffectContext();
					EffectContext.AddSourceObject(Character);
					EffectContext.AddHitResult(HitResult);
					FGameplayEventData EventData;
					EventData.EventTag = FASGameplayTags::Event::Hit;
					EventData.Instigator = Character;
					EventData.Target = HitCharacter;
					EventData.ContextHandle = EffectContext;
					EventData.EventMagnitude = Gun->GetGunAttributes()->GetDamage() * .001f;
					UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(HitCharacter, FASGameplayTags::Event::Hit, EventData);
				}
			}
			else
			{
				if (FASCVars::ASDebugDraw && FASCVars::ASDrawHitBoxes)
				{
					DrawDebugSphere(GetWorld(), HitResult.Location, BulletRadius, 10, FColor::Red, false, FASCVars::ASDebugDrawDuration, 0, 1.f);
				}
			}
			FGameplayCueParameters CueParams;
			CueParams.Location = HitResult.ImpactPoint;
			CueParams.Normal = HitResult.ImpactNormal;
			CueParams.PhysicalMaterial = HitResult.PhysMaterial.Get();
			CueParams.EffectContext = Spec.Data->GetContext();
			
			ASC->ExecuteGameplayCue(FASGameplayTags::GameplayCue::Hit::BulletImpact, CueParams);
		}
	}
	
	ApplyCameraRecoil();

	if (Character->IsPlayerControlled())
	{
		if (AASPlayerCharacter* PlayerCharacter = Cast<AASPlayerCharacter>(Character))
		{
			PlayerCharacter->ReportNoiseEvent(1.f, 5000.f);
		}
	}
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UGAFire::ApplyCameraRecoil()
{
	float PitchKick = Gun->GetGunAttributes()->GetRecoilVertical();
	float YawKick = FMath::RandRange(-Gun->GetGunAttributes()->GetRecoilHorizontal(), Gun->GetGunAttributes()->GetRecoilHorizontal());
	Character->AddControllerPitchInput(-PitchKick);
	Character->AddControllerYawInput(YawKick);
}

bool UGAFire::TraceFire(FHitResult& OutHitResult, const float InBulletRadius)
{
	const FVector TraceStart = Gun->GetGunHolder()->GetCameraComponent()->GetComponentLocation() +
		Character->GetCameraComponent()->GetForwardVector() * 30.f;
	const FVector VirtualStart = Character->GetCameraComponent()->GetComponentLocation(); 
	FVector ShotDirection = Character->GetCameraComponent()->GetForwardVector();

	if (!Character->IsPlayerControlled())
	{
		if (AASBotCharacter* BotCharacter = Cast<AASBotCharacter>(Character))
		{
			ShotDirection = BotCharacter->GetTargetDirectionHazed();
		}
	}
	
	const FVector End = VirtualStart + GetShotDirectionWithSpread(ShotDirection) * 100000.f;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Character);
	if (GetWorld()->SweepSingleByChannel(OutHitResult, VirtualStart, End,
		FQuat::Identity, ECC_GameTraceChannel1, FCollisionShape::MakeSphere(InBulletRadius), Params))
	{
		if (FASCVars::ASDebugDraw && FASCVars::ASDrawShotTraces)
		{
			DrawDebugLine(GetWorld(), TraceStart, OutHitResult.Location,
				FColor::Red, false, FASCVars::ASDebugDrawDuration, 0, 1.f);
		}
		return true;
	}
	if (FASCVars::ASDebugDraw && FASCVars::ASDrawShotTraces)
	{
		DrawDebugLine(GetWorld(), TraceStart, VirtualStart + Character->GetCameraComponent()->GetForwardVector() * 100000.f,
			FColor::Red, false, FASCVars::ASDebugDrawDuration, 0, 1.f);
	}
	return false;
}

FVector UGAFire::GetShotDirectionWithSpread(const FVector& InShotDirection)
{
	const float SpreadAngleRad = FMath::DegreesToRadians(Character->GetGunSpread());
	
	return FMath::VRandCone(InShotDirection, SpreadAngleRad);
}
