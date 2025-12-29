// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"

/**
 * 
 */
namespace FGameplayTags
{
	// Gameplay effects
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Walking);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Sprint);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Crouch);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Falling);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Hanging);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Sliding);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Reloading);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Health_Regen);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Energy_Regen);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Damage_Instant);
	
	// Character states
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(MovementState_Sprinting);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(MovementState_Crouching);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(MovementState_Falling);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(MovementState_Sliding);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(MovementState_Hanging);

	// Gameplay abilities
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Jump);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ClamberUp);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Slide);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Fire);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Hit);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Throw);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Reload);

	// Events
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Event_FinishedReloading);

	// Cooldown
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Cooldown_Fire);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Cooldown_Hit);
	
	// Data
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Data_JumpCost);

	// Cues
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Gun_MuzzleFlare0);
	
};
