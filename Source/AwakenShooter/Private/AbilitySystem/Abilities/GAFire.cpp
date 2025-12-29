// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GAFire.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystem/GameplayTags.h"
#include "AbilitySystem/GunAttributeSet.h"
#include "Camera/CameraComponent.h"
#include "Character/ASCharacter.h"
#include "General/DebugCVars.h"
#include "Items/Gun.h"

void UGAFire::ApplyCameraRecoil(const AGun* Gun)
{
	if (!Gun)
		return;

	if (auto GunHolder = Gun->GetGunHolder())
	{
		float PitchKick = Gun->GetGunAttributes()->GetRecoilVertical();
		float YawKick = FMath::RandRange(-Gun->GetGunAttributes()->GetRecoilHorizontal(), Gun->GetGunAttributes()->GetRecoilHorizontal());
		GunHolder->AddControllerPitchInput(-PitchKick);
		GunHolder->AddControllerYawInput(YawKick);
	}
}

bool UGAFire::TraceFire(AASCharacter& HitCharacter)
{
	AASCharacter* Character = Cast<AASCharacter>(CurrentActorInfo->AvatarActor);
	if (!Character)
		return false;
	AGun* Gun = Character->GetEquippedGun();
	if (!Gun)
		return false;

	
	const FVector TraceStart = Gun->GetMeshComponent()->GetSocketLocation(FName("Muzzle"));
	const FVector VirtualStart = Character->GetCameraComponent()->GetComponentLocation(); 

	if (FASCVars::ASDebugDraw && FASCVars::ASDrawShotTraces)
	{
		DrawDebugLine(GetWorld(), TraceStart, VirtualStart + Character->GetCameraComponent()->GetForwardVector() * 100000.f,
			FColor::Red, false, 1.f, 0, 1.f);
	}

	return false;
}

UGAFire::UGAFire()
{
	SetAssetTags(FGameplayTagContainer(FGameplayTags::Ability_Fire));
}

void UGAFire::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	
	AGun* Gun = Cast<AGun>(ActorInfo->AvatarActor.Get());
	if (!Gun)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	if (auto ASC = Gun->GetGunAbilitySystemComponent())
	{
		ASC->ExecuteGameplayCue(FGameplayTags::GameplayCue_Gun_MuzzleFlare0);

		// cancel reload if we start firing
		FGameplayTagContainer CancelAbilitiesTags = FGameplayTagContainer(FGameplayTags::Ability_Reload);		
		ASC->CancelAbilities(&CancelAbilitiesTags);
	}
	
	if (RecoilMontage)
	{
		if (AASCharacter* GunHolder = Gun->GetGunHolder())
		{
			if (UAnimInstance* AnimInstance = GunHolder->GetFirstPersonMesh()->GetAnimInstance())
			{
				AnimInstance->Montage_Play(RecoilMontage);
			}
		}
	}
	int32 Ammo = static_cast<int32>(Gun->GetGunAttributes()->GetAmmo());
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
		TEXT("Fire Ability Activated. Ammo: ") + FString::FromInt(Ammo));
	
	ApplyCameraRecoil(Gun);
	
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}