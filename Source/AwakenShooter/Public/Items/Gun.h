// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemBase.h"
#include "GameFramework/Actor.h"
#include "Gun.generated.h"

UENUM(BlueprintType)
enum class EGunType : uint8
{
	Pistol,
	Rifle
};

UCLASS()
class AWAKENSHOOTER_API AGun : public AActor, public IItemBase
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Specs")
	EGunType GunType;
	
public:
	AGun();

protected:

public:
	EGunType GetGunType() const { return GunType; }
};
