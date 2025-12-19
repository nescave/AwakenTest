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
	SetAssetTags(FGameplayTagContainer(FGameplayTags::Ability_ClamberUp));
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
		UGTClamberUp::Create(Character->GetGameplayTasksComponent(),
		   State->GetHangingPoint().Location - State->GetHangingPoint().Normal * 50.f,
		   Character, 1.f)->ReadyForActivation();
	}
	MoveComp->Velocity += FVector::UpVector * 800.f;
	MoveComp->SetMovementMode(MOVE_Falling);
	EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}
