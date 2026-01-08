// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GAThrow.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/GameplayTags.h"
#include "Camera/CameraComponent.h"
#include "Character/ASCharacter.h"
#include "Engine/OverlapResult.h"
#include "General/DebugCVars.h"
#include "General/ProjectGlobals.h"
#include "Items/Gun.h"

UGAThrow::UGAThrow()
{
	SetAssetTags(FGameplayTagContainer(FASGameplayTags::Ability::Throw));
}

void UGAThrow::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, false, true);
		return;
	}
	Character = Cast<AASCharacter>(ActorInfo->AvatarActor);
	if (!Character)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, false, true);
		return;		
	}
	
	if (!Character->GetEquippedGun())
		return;
	
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this,
		FName("ThrowMontage"), Character->GetEquippedGun()->GetThrowMontage());
	MontageTask->OnCompleted.AddDynamic(this, &UGAThrow::OnThrowMontageEnded);
	MontageTask->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* OnThrowTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this,
		FASGameplayTags::Ability::Event::Throw);
	OnThrowTask->EventReceived.AddDynamic(this, &UGAThrow::OnThrow);
	OnThrowTask->ReadyForActivation();
}

void UGAThrow::OnThrow(FGameplayEventData Payload)
{
	if (!Character->GetEquippedGun())
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	
	const bool DebugDraw = FASCVars::ASDebugDraw && FASCVars::ASDrawThrowTargeting;
	constexpr float SearchAreaRadius = 600.f;
	float ThrowPower = 1000.f;
	FVector ThrowDirection = Character->GetCameraComponent()->GetForwardVector();
	const FVector Start = Character->GetActorLocation() + ThrowDirection * SearchAreaRadius;
	TArray<FOverlapResult> OverlapResults;

	if (DebugDraw)
		DrawDebugSphere(GetWorld(), Start, SearchAreaRadius, 24, FColor::Blue, false, 10.f);
		
	if (GetWorld()->OverlapMultiByChannel(OverlapResults, Start,
		FQuat::Identity, ASTraceChannel::EnemyTrace,
		FCollisionShape::MakeSphere(SearchAreaRadius)))
	{
		const FVector CharacterHeadPosition = Character->GetCameraComponent()->GetComponentLocation();
		float BestScore = UE_SMALL_NUMBER;
		FVector BestTarget = FVector::ZeroVector;
		for (const auto& OverlapResult : OverlapResults)
		{
			const FVector OverlapPositionRaised = OverlapResult.GetActor()->GetActorLocation()+ FVector::UpVector * 50.f;
			const FVector OverlapOffset = OverlapPositionRaised - CharacterHeadPosition;
			const FVector OverlapDirection = OverlapOffset.GetSafeNormal();
			const float OverlapDistance = OverlapOffset.Size();
			const float DirectionScore = ThrowDirection.Dot(OverlapDirection);
			
			if (DirectionScore < .5f)
			{
				if (DebugDraw)
					DrawDebugSphere(GetWorld(), OverlapPositionRaised, 25.f, 4, FColor::Red,
						false, FASCVars::ASDebugDrawDuration);
				continue; // not in view cone
			}
			FHitResult HitResult;
			GetWorld()->LineTraceSingleByChannel(HitResult, CharacterHeadPosition,
				OverlapPositionRaised + OverlapDirection * 50.f, ECC_Visibility);
			if (!HitResult.bBlockingHit || HitResult.GetActor() != OverlapResult.GetActor())
			{
				if (DebugDraw)
					DrawDebugLine(GetWorld(), CharacterHeadPosition, OverlapPositionRaised, FColor::Red,
					false, FASCVars::ASDebugDrawDuration);
				continue; // not in line of sight
			}
			
			float DistanceScore = 1 / OverlapDistance;
			float Score = DirectionScore * DistanceScore;
			if (Score > BestScore)
			{
				if (DebugDraw)
					DrawDebugSphere(GetWorld(), BestTarget, 25.f, 4, FColor::Yellow,
						false, FASCVars::ASDebugDrawDuration);
				BestScore = Score;
				BestTarget = OverlapPositionRaised;
				ThrowPower = FMath::Clamp(OverlapDistance * 2.f, 500.f, 2000.f);
			}
		}
		if (!BestTarget.IsNearlyZero())
		{
			if (DebugDraw)
				DrawDebugSphere(GetWorld(), BestTarget, 25.f, 4, FColor::Green,
					false, FASCVars::ASDebugDrawDuration);

			FVector FinalTarget = BestTarget + FVector::UpVector * FMath::Lerp(0.f, 100.f, ThrowPower / 2000.f);
			
			ThrowDirection = (FinalTarget  - Character->GetEquippedGun()->GetActorLocation()).GetSafeNormal();
		}
		else if (DebugDraw)
		{
			DrawDebugDirectionalArrow(GetWorld(), CharacterHeadPosition, CharacterHeadPosition + ThrowDirection * 100.f,
				5.f, FColor::Green, false, FASCVars::ASDebugDrawDuration, 0.f, 1.f);
		}
	}
	if (DebugDraw)
		GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Cyan, TEXT("Throw Power: ") + FString::SanitizeFloat(ThrowPower));
	
	Character->GetEquippedGun()->Throw(ThrowDirection * ThrowPower + Character->GetVelocity() * .075f);
}

void UGAThrow::OnThrowMontageEnded()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
