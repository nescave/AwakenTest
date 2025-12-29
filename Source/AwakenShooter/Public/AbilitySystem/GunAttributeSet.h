// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "GunAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class AWAKENSHOOTER_API UGunAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Gun")
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS_BASIC(UGunAttributeSet, Damage)
	UPROPERTY(BlueprintReadOnly, Category = "Gun")
	FGameplayAttributeData FireRate;
	ATTRIBUTE_ACCESSORS_BASIC(UGunAttributeSet, FireRate)
	UPROPERTY(BlueprintReadOnly, Category = "Gun")
	FGameplayAttributeData RecoilVertical;
	ATTRIBUTE_ACCESSORS_BASIC(UGunAttributeSet, RecoilVertical)
	UPROPERTY(BlueprintReadOnly, Category = "Gun")
	FGameplayAttributeData RecoilHorizontal;
	ATTRIBUTE_ACCESSORS_BASIC(UGunAttributeSet, RecoilHorizontal)
	UPROPERTY(BlueprintReadOnly, Category = "Gun")
	FGameplayAttributeData Ammo;
	ATTRIBUTE_ACCESSORS_BASIC(UGunAttributeSet, Ammo)
	UPROPERTY(BlueprintReadOnly, Category = "Gun")
	FGameplayAttributeData MaxAmmo;
	ATTRIBUTE_ACCESSORS_BASIC(UGunAttributeSet, MaxAmmo)
	UPROPERTY(BlueprintReadOnly, Category = "Gun")
	FGameplayAttributeData Accuracy;	
	ATTRIBUTE_ACCESSORS_BASIC(UGunAttributeSet, Accuracy)
	UPROPERTY(BlueprintReadOnly, Category = "Gun")
	FGameplayAttributeData ShotsPerRound;	
	ATTRIBUTE_ACCESSORS_BASIC(UGunAttributeSet, ShotsPerRound)
	

};
