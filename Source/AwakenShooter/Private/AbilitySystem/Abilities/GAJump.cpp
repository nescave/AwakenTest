// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GAJump.h"

#include "AbilitySystem/CharacterAttributeSet.h"
#include "AbilitySystem/GameplayTags.h"
#include "Character/ASCharacter.h"
#include "Character/MovementState/MovementStateComponent.h"
#include "Character/MovementState/States/HangingState.h"
#include "GameFramework/CharacterMovementComponent.h"

UGAJump::UGAJump()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	SetAssetTags(FGameplayTagContainer(FASGameplayTags::Ability_Jump));
}

void UGAJump::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	AASCharacter* Character = Cast<AASCharacter>(ActorInfo->AvatarActor.Get());
	
	if (!Character)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
		
	UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement();
	const UCharacterAttributeSet* AttributeSet = ActorInfo->AbilitySystemComponent->GetSet<UCharacterAttributeSet>();

	FVector JumpDirection = Character->GetMovementStateMachine()->GetCurrentState()->GetJumpDirection();
	if (JumpDirection.SizeSquared() > 0.f)
	{
		MoveComp->Velocity += JumpDirection * AttributeSet->GetJumpPower();
		MoveComp->SetMovementMode(MOVE_Falling);
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}