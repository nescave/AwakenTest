// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "Interactable.h"
#include "GameFramework/Actor.h"
#include "Gun.generated.h"

UENUM(BlueprintType)
enum class EGunType : uint8
{
	One_Hand,
	Two_Hands
};

UCLASS()
class AWAKENSHOOTER_API AGun : public AActor, public IInteractable
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UAbilitySystemComponent* GunAbilitySystemComponent;

public:
	AGun();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="GunSpecs")
	TSubclassOf<class UGameplayEffect> AttributeDefaults;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GunSpecs")
	class UGunAttributeSet* GunAttributeSet;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="GunSpecs")
	EGunType GunType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="GunSpecs")
	TSubclassOf<UGameplayEffect> ReloadEffect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="GunSpecs")
	TArray<TSubclassOf<UGameplayEffect>> GunPassiveEffects;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="GunSpecs")
	TSubclassOf<class UGameplayAbility> MainActionAbility;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="GunSpecs")
	TSubclassOf<UGameplayAbility> SecondaryActionAbility;

	UPROPERTY(Transient)
	TObjectPtr<AASCharacter> GunHolder;

	void InitializeAttributes();
	
	virtual void BeginPlay() override;
public:
	UFUNCTION(BlueprintCallable, Category="GunSpecs" )
	EGunType GetGunType() const { return GunType; }
	
	UFUNCTION(BlueprintCallable, Category="Gun" )
	void Reload();
	UFUNCTION(BlueprintCallable, Category="Equipable" )
	void Throw(const FVector& ForceVector);
	UFUNCTION(BlueprintCallable, Category="Equipable" )
	void Drop();
	UFUNCTION(BlueprintCallable, Category="Equipable" )
	void MainAction();
	UFUNCTION(BlueprintCallable, Category="Equipable" )
	void SecondaryAction();	
	
	UFUNCTION(BlueprintCallable, Category="Components")
	UStaticMeshComponent* GetMeshComponent() const { return Mesh; }
	UFUNCTION(BlueprintCallable, Category="Components")
	UAbilitySystemComponent* GetGunAbilitySystemComponent() const { return GunAbilitySystemComponent; }	
	UFUNCTION(BlueprintCallable, Category="Components")
	UGunAttributeSet* GetGunAttributes() const { return GunAttributeSet; }

	virtual void Interact(AASCharacter* Interactor) override;
	virtual void SetHighlighted(float HighlightValue) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void OnConstruction(const FTransform& Transform) override;

	void SetGunHolder(AASCharacter* NewHolder);
	AASCharacter* GetGunHolder() const { return GunHolder; }

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
