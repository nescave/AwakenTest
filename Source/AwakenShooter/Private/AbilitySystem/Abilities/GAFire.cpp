// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GAFire.h"

#include "AbilitySystem/GameplayTags.h"

UGAFire::UGAFire()
{
	SetAssetTags(FGameplayTagContainer(FGameplayTags::Ability_Fire));
}
