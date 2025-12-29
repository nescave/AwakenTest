// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GAHit.h"

#include "AbilitySystem/GameplayTags.h"

UGAHit::UGAHit()
{
	SetAssetTags(FGameplayTagContainer(FASGameplayTags::Ability_Hit));
}
