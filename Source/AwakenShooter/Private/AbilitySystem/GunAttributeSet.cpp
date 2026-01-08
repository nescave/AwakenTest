// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/GunAttributeSet.h"

UGunAttributeSet::UGunAttributeSet() :
	Super(),
	Damage(30.f),
	FireRate(10.f),
	RecoilVertical(3.f),
	RecoilHorizontal(1.f),
	Ammo(8.f),
	MaxAmmo(8.f),
	Accuracy(35.f),
	ShotsPerRound(1.f),
	MaxSpread(10.f)
{
}
