// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "Interactive.h"
#include "GameFramework/Actor.h"
#include "Gun.generated.h"

UENUM(BlueprintType)
enum class EGunType : uint8
{
	One_Hand,
	Two_Hands
};

UCLASS()
class AWAKENSHOOTER_API AGun : public AActor, public IInteractive
{
	GENERATED_BODY()
public:
	AGun();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="GunSpecs")
	EGunType GunType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="GunSpecs")
	TArray<TSubclassOf<class UGameplayEffect>> GunPassiveEffects;
	
	UPROPERTY()
	UPrimitiveComponent* Primitive;

public:
	UFUNCTION(BlueprintCallable, Category="GunSpecs" )
	EGunType GetGunType() const { return GunType; }
	UFUNCTION(BlueprintCallable, Category="GunFunctions")
	void Throw(const FVector& ForceVector);
	
	virtual void Interact(AASCharacter* Interactor) override;
	virtual void SetHighlighted(float HighlightValue) override;
	
	virtual void Tick(float DeltaSeconds) override;

private:
	UFUNCTION(BlueprintCallable, Category="GunSpecs" )
	void ApplyEffects(UAbilitySystemComponent* ASC);
	UFUNCTION(BlueprintCallable, Category="GunSpecs" )
	void RemoveEffects(UAbilitySystemComponent* ASC);
	UFUNCTION(BlueprintCallable, Category="GunSpecs" )
	bool IsOnGround();	
	UPROPERTY()
	TArray<FActiveGameplayEffectHandle> EffectHandles;
};
