// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GAJump.h"

#include "AbilitySystem/CharacterAttributeSet.h"
#include "AbilitySystem/GameplayTags.h"
#include "Character/ASPlayerCharacter.h"
#include "Character/MovementState/MovementStateComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

UGAJump::UGAJump()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	SetAssetTags(FGameplayTagContainer(FASGameplayTags::Ability::Jump));
}

void UGAJump::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	AASPlayerCharacter* Character = Cast<AASPlayerCharacter>(ActorInfo->AvatarActor.Get());
	
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
		Character->AddClampedVelocity(JumpDirection, AttributeSet->GetJumpPower());
		MoveComp->SetMovementMode(MOVE_Falling);
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}