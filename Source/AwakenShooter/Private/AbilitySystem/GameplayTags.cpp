// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/GameplayTags.h"

namespace FASGameplayTags
{
	// Gameplay effects
	namespace Effect
	{
		UE_DEFINE_GAMEPLAY_TAG(Walking, "Effect.Walking");
		UE_DEFINE_GAMEPLAY_TAG(Sprint, "Effect.Sprint");
		UE_DEFINE_GAMEPLAY_TAG(SprintEaseOut, "Effect.SprintEaseOut");
		UE_DEFINE_GAMEPLAY_TAG(Crouch, "Effect.Crouch");
		UE_DEFINE_GAMEPLAY_TAG(Falling, "Effect.Falling");
		UE_DEFINE_GAMEPLAY_TAG(Hanging, "Effect.Hanging");
		UE_DEFINE_GAMEPLAY_TAG(Sliding, "Effect.Sliding");
		UE_DEFINE_GAMEPLAY_TAG(Reloading, "Effect.Reloading");
		UE_DEFINE_GAMEPLAY_TAG(Health_Regen, "Effect.Health.Regen");
		UE_DEFINE_GAMEPLAY_TAG(Energy_Regen, "Effect.Energy.Regen");
		UE_DEFINE_GAMEPLAY_TAG(Energy_Drain, "Effect.Energy.Drain");
		UE_DEFINE_GAMEPLAY_TAG(Damage_Instant, "Effect.Damage.Instant");
		UE_DEFINE_GAMEPLAY_TAG(ADS, "Effect.ADS");
		UE_DEFINE_GAMEPLAY_TAG(Stunned, "Effect.Stunned");
		UE_DEFINE_GAMEPLAY_TAG(Damage, "Effect.Damage");

	}
	
	// Character states
	namespace MovementState
	{
		UE_DEFINE_GAMEPLAY_TAG(Sprinting, "MovementState.Sprinting");
		UE_DEFINE_GAMEPLAY_TAG(Crouching, "MovementState.Crouching");
		UE_DEFINE_GAMEPLAY_TAG(Falling, "MovementState.Falling");
		UE_DEFINE_GAMEPLAY_TAG(Sliding, "MovementState.Sliding");
		UE_DEFINE_GAMEPLAY_TAG(Hanging, "MovementState.Hanging");
		UE_DEFINE_GAMEPLAY_TAG(Death, "MovementState.Death");
	}

	// Gameplay abilities
	namespace Ability
	{
		UE_DEFINE_GAMEPLAY_TAG(Jump, "Ability.Jump");
		UE_DEFINE_GAMEPLAY_TAG(Sprint, "Ability.Sprint");
		UE_DEFINE_GAMEPLAY_TAG(ClamberUp, "Ability.ClamberUp");
		UE_DEFINE_GAMEPLAY_TAG(Slide, "Ability.Slide");
		UE_DEFINE_GAMEPLAY_TAG(Fire, "Ability.Fire");
		UE_DEFINE_GAMEPLAY_TAG(Hit, "Ability.Hit");
		UE_DEFINE_GAMEPLAY_TAG(Throw, "Ability.Throw");
		UE_DEFINE_GAMEPLAY_TAG(Reload, "Ability.Reload");
		UE_DEFINE_GAMEPLAY_TAG(Main, "Ability.Main");
		UE_DEFINE_GAMEPLAY_TAG(Secondary, "Ability.Secondary");
		UE_DEFINE_GAMEPLAY_TAG(AimDownSights, "Ability.AimDownSights");
		UE_DEFINE_GAMEPLAY_TAG(Melee, "Ability.Melee");
		UE_DEFINE_GAMEPLAY_TAG(DryFire, "Ability.DryFire");
		UE_DEFINE_GAMEPLAY_TAG(EquipGun, "Ability.EquipGun");
		UE_DEFINE_GAMEPLAY_TAG(BulletTime, "Ability.BulletTime");
		UE_DEFINE_GAMEPLAY_TAG(BulletTimeActive, "Ability.BulletTime.Active");
		UE_DEFINE_GAMEPLAY_TAG(SetSlowMotion, "Ability.SetSlowMotion");
		UE_DEFINE_GAMEPLAY_TAG(UnsetSlowMotion, "Ability.UnsetSlowMotion");

		// Events
		namespace Event
		{
			UE_DEFINE_GAMEPLAY_TAG(FinishedReloading, "Ability.Event.FinishedReloading");
			UE_DEFINE_GAMEPLAY_TAG(FinishedADS, "Ability.Event.FinishedADS");
			UE_DEFINE_GAMEPLAY_TAG(ActivatedHitBox, "Ability.Event.ActivatedHitBox");
			UE_DEFINE_GAMEPLAY_TAG(DeactivatedHitBox, "Ability.Event.DeactivatedHitBox");
			UE_DEFINE_GAMEPLAY_TAG(GunEquipped, "Ability.Event.GunEquipped");
			UE_DEFINE_GAMEPLAY_TAG(BulletTimeStarted, "Ability.Event.BulletTimeStarted");
			UE_DEFINE_GAMEPLAY_TAG(BulletTimeEnded, "Ability.Event.BulletTimeEnded");
			UE_DEFINE_GAMEPLAY_TAG(Throw, "Ability.Event.Throw");

		}
	}

	// Events
	namespace Event
	{
		UE_DEFINE_GAMEPLAY_TAG(Hit, "Event.Hit");
	}
	
	// Cooldown
	UE_DEFINE_GAMEPLAY_TAG(Effect_Cooldown_Fire, "Effect.Cooldown.Fire");
	UE_DEFINE_GAMEPLAY_TAG(Effect_Cooldown_Hit, "Effect.Cooldown.Hit");
	
	// Data
	namespace Data
	{
		UE_DEFINE_GAMEPLAY_TAG(JumpCost, "Data.JumpCost");
		UE_DEFINE_GAMEPLAY_TAG(StunDuration, "Data.StunDuration");
		UE_DEFINE_GAMEPLAY_TAG(Damage, "Data.Damage");
	}

	// Cues
	namespace GameplayCue
	{
		namespace Gun
		{
			UE_DEFINE_GAMEPLAY_TAG(MuzzleFlare0, "GameplayCue.Gun.MuzzleFlare0");
			UE_DEFINE_GAMEPLAY_TAG(Melee, "GameplayCue.Gun.Melee");
		}
		namespace Hit
		{
			UE_DEFINE_GAMEPLAY_TAG(MeleeImpact, "GameplayCue.Hit.MeleeImpact");
			UE_DEFINE_GAMEPLAY_TAG(BulletImpact, "GameplayCue.Hit.BulletImpact");
		}
	}
}