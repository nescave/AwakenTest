// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "Interactable.h"
#include "AbilitySystem/GunAttributeSet.h"
#include "GameFramework/Actor.h"
#include "Gun.generated.h"

class USoundWave;

UENUM(BlueprintType)
enum class EGunType : uint8
{
	One_Hand,
	Two_Hands
};
// default ads value = (X=5.000000,Y=-17.780000,Z=-185.000000)
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGunReloaded);

UCLASS(Abstract)
class AWAKENSHOOTER_API AGun : public AActor, public IInteractable
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* WorldMesh;
	UPROPERTY(VisibleDefaultsOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* FPSMesh;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UAbilitySystemComponent* GunAbilitySystemComponent;

public:
	AGun();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="GunSpecs")
	TSubclassOf<UGameplayEffect> AttributeDefaults;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GunSpecs")
	UGunAttributeSet* GunAttributeSet;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="GunSpecs")
	EGunType GunType;;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="GunSpecs")
	TSubclassOf<UGameplayEffect> ReloadEffect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="GunSpecs")
	TArray<TSubclassOf<UGameplayEffect>> GunPassiveEffects;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="GunSpecs")
	TSubclassOf<UGameplayEffect> ThrowHitEffect;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="GunSpecs")
	TSubclassOf<UGameplayAbility> MainActionAbility;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="GunSpecs")
	TSubclassOf<UGameplayAbility> SecondaryActionAbility;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="GunSpecs")
	FName SocketName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="GunSpecs")
	FVector DefaultGunPosition;	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="GunSpecs")
	FVector ADSPosition;	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="GunSpecs")
	UAnimMontage* ReloadMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="GunSpecs")
	UAnimMontage* FireMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="GunSpecs")
	UAnimMontage* DryFireMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="GunSpecs")
	UAnimMontage* MeleeMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="GunSpecs")
	UAnimMontage* ThrowMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="GunSpecs")
	UAnimMontage* EquipMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="GunSpecs")
	USoundWave* FireSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="GunSpecs")
	USoundWave* DryFireSound;
	
	UPROPERTY(Transient)
	TObjectPtr<AASCharacter> GunHolder;

	UPROPERTY(BlueprintAssignable, Category="GunEvents")
	FOnGunReloaded OnGunReloaded;

	void InitializeAttributes();
	
	virtual void BeginPlay() override;

public:
	virtual void OnConstruction(const FTransform& Transform) override;

	UFUNCTION(BlueprintCallable, Category="GunSpecs" )
	EGunType GetGunType() const { return GunType; }
	
	UFUNCTION(BlueprintCallable, Category="Gun" )
	void Reload();
	UFUNCTION()
	void OnHitOther(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	UFUNCTION(BlueprintCallable, Category="Equipable" )
	void Throw(const FVector& ThrowVector);
	UFUNCTION(BlueprintCallable, Category="Equipable" )
	void Drop(bool bStopAllMovement = true);
	UFUNCTION(BlueprintCallable, Category="Equipable" )
	bool MainAction();
	UFUNCTION(BlueprintCallable, Category="Equipable" )
	bool SecondaryAction();	

	void SetIgnoreEnemiesCollision(bool bIgnoreCharacters);
	void SetFPSMode(bool bFPSVisible);
	
	UFUNCTION(BlueprintCallable, Category="Components")
	UStaticMeshComponent* GetMeshComponent() const { return WorldMesh; }
	UFUNCTION(BlueprintCallable, Category="Components")
	UAbilitySystemComponent* GetGunAbilitySystemComponent() const { return GunAbilitySystemComponent; }
	
	UFUNCTION(BlueprintCallable, Category="Attributes")
	UGunAttributeSet* GetGunAttributes() const { return GunAttributeSet; }
	UFUNCTION(BlueprintCallable, Category="Attributes")
	float GetBaseDamage() const;
	UFUNCTION(BlueprintCallable, Category="Attributes")
	float GetBaseFireRate() const;
	UFUNCTION(BlueprintCallable, Category="Attributes")
	float GetBaseRecoilVertical() const;
	UFUNCTION(BlueprintCallable, Category="Attributes")
	float GetBaseRecoilHorizontal() const;
	UFUNCTION(BlueprintCallable, Category="Attributes")
	float GetBaseAmmo() const;
	UFUNCTION(BlueprintCallable, Category="Attributes")
	float GetBaseMaxAmmo() const;
	UFUNCTION(BlueprintCallable, Category="Attributes")
	float GetBaseAccuracy() const;
	UFUNCTION(BlueprintCallable, Category="Attributes")
	float GetBaseShotsPerRound() const;
	UFUNCTION(BlueprintCallable, Category="Attributes")
	float GetBaseMaxSpread() const;

	virtual void Interact_Implementation(AASCharacter* Interactor) override;
	virtual void SetHighlighted_Implementation(float HighlightValue) override;
	virtual bool IsInteractable_Implementation() const override;
	void TryClearRigidbody();
	
	virtual void Tick(float DeltaSeconds) override;

	void SetGunHolder(AASCharacter* NewHolder);
	AASCharacter* GetGunHolder() const { return GunHolder; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="GunSpecs")
	FName GetSocketName() const { return SocketName; }
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="GunSpecs")
	FVector GetDefaultGunPosition() const { return DefaultGunPosition; }	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="GunSpecs")
	FVector GetADSPosition() const { return ADSPosition; }	
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="GunSpecs")
	UAnimMontage* GetReloadMontage() const { return ReloadMontage; }
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="GunSpecs")
	UAnimMontage* GetFireMontage() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="GunSpecs")
	UAnimMontage* GetMeleeMontage() const { return MeleeMontage; }
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="GunSpecs")
	UAnimMontage* GetDryFireMontage()const { return DryFireMontage; };
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="GunSpecs")
	UAnimMontage* GetThrowMontage() const { return ThrowMontage; }
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="GunSpecs")
	UAnimMontage* GetEquipMontage() const { return EquipMontage; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="GunSpecs")
	USoundWave* GetFireSound() const { return FireSound; }
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="GunSpecs")
	USoundWave* GetDryFireSound() const { return DryFireSound; }	
	
	FOnGunReloaded& GetOnGunReloadedDelegate() { return OnGunReloaded; }
	
	bool IsAmmoFull() const;
	bool IsAmmoEmpty() const;

private:
	UFUNCTION(BlueprintCallable, Category="GunSpecs" )
	void ApplyEffects(UAbilitySystemComponent* ASC);
	UFUNCTION(BlueprintCallable, Category="GunSpecs" )
	void RemoveEffects(UAbilitySystemComponent* ASC);
	UFUNCTION(BlueprintCallable, Category="GunSpecs" )
	bool IsOnGround();
	UPROPERTY()
	TArray<FActiveGameplayEffectHandle> EffectHandles;
	bool bThrown;
	UPROPERTY()
	AASCharacter* Thrower;
};
