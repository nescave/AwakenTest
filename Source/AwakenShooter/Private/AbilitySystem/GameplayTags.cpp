// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/GameplayTags.h"

namespace FGameplayTags
{
	// Gameplay effects
	UE_DEFINE_GAMEPLAY_TAG(Effect_Walking, "Effect.Walking");
	UE_DEFINE_GAMEPLAY_TAG(Effect_Sprint, "Effect.Sprint");
	UE_DEFINE_GAMEPLAY_TAG(Effect_Crouch, "Effect.Crouch");
	UE_DEFINE_GAMEPLAY_TAG(Effect_Falling, "Effect.Falling");
	UE_DEFINE_GAMEPLAY_TAG(Effect_Hanging, "Effect.Hanging");
	UE_DEFINE_GAMEPLAY_TAG(Effect_Sliding, "Effect.Sliding");
	
	UE_DEFINE_GAMEPLAY_TAG(Effect_Health_Regen, "Effect.Health.Regen");
	UE_DEFINE_GAMEPLAY_TAG(Effect_Energy_Regen, "Effect.Energy.Regen");
	UE_DEFINE_GAMEPLAY_TAG(Effect_Damage_Instant, "Effect.Damage.Instant");
		
	// Character states
	UE_DEFINE_GAMEPLAY_TAG(MovementState_Sprinting, "MovementState.Sprinting");
	UE_DEFINE_GAMEPLAY_TAG(MovementState_Crouching, "MovementState.Crouching");
	UE_DEFINE_GAMEPLAY_TAG(MovementState_Falling, "MovementState.Falling");
	UE_DEFINE_GAMEPLAY_TAG(MovementState_Sliding, "MovementState.Sliding");
	UE_DEFINE_GAMEPLAY_TAG(MovementState_Hanging, "MovementState.Hanging");

	// Gameplay abilities
	UE_DEFINE_GAMEPLAY_TAG(Ability_Jump, "Ability.Jump");
	UE_DEFINE_GAMEPLAY_TAG(Ability_ClamberUp, "Ability.ClamberUp");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Slide, "Ability.Slide");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Fire, "Ability.Fire");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Hit, "Ability.Hit");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Throw, "Ability.Throw");

	// Data
	UE_DEFINE_GAMEPLAY_TAG(Data_JumpCost, "Data.JumpCost");
}