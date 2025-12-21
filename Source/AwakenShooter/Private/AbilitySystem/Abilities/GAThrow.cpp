// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GAThrow.h"

#include "AbilitySystem/GameplayTags.h"

UGAThrow::UGAThrow()
{
	SetAssetTags(FGameplayTagContainer(FGameplayTags::Ability_Throw));
}
