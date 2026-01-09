// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GAClamberUp.h"

#include "AbilitySystem/GameplayTags.h"
#include "AbilitySystem/Tasks/GTClamberUp.h"
#include "Character/ASCharacter.h"
#include "Character/MovementState/MovementStateComponent.h"
#include "Character/MovementState/States/HangingState.h"
#include "GameFramework/CharacterMovementComponent.h"


UGAClamberUp::UGAClamberUp()
{
	SetAssetTags(FGameplayTagContainer(FASGameplayTags::Ability::ClamberUp));
}

void UGAClamberUp::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	AASCharacter* Character = Cast<AASCharacter>(ActorInfo->AvatarActor.Get());
	
	if (!Character)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement();
	if (const UHangingState* State = Cast<UHangingState>(Character->GetMovementStateMachine()->GetCurrentState()))
	{
		FHitResult HangingPoint;
		if (State->GetHangingPoint(HangingPoint) && HangingPoint.bBlockingHit)
		{
			UGTClamberUp::Create(Character->GetGameplayTasksComponent(),
				HangingPoint.Location - HangingPoint.Normal * 50.f,Character,
				.35f)->ReadyForActivation();
		}
	}
	MoveComp->Velocity += FVector::UpVector * 1400.f;
	MoveComp->SetMovementMode(MOVE_Falling);
	EndAbility(Handle, ActorInfo, ActivationInfo, false, false);

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}
