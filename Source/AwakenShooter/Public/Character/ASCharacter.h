// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Perception/AISightTargetInterface.h"
#include "ASCharacter.generated.h"

class UMovementStateComponent;
class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class UAIPerceptionStimuliSourceComponent;
class USphereComponent;
class UCharacterAttributeSet;
class UGameplayEffect;
struct FInputActionValue;
class IGameplayTaskOwnerInterface;

USTRUCT(BlueprintType)
struct FHitMontageData
{
	GENERATED_BODY()

	FHitMontageData() : HitLocation(FVector::ZeroVector),
		HitDirection(-FVector::ForwardVector),
		Montage(nullptr)
	{}
	
	UPROPERTY(EditAnywhere)
	FName BoneName;
	UPROPERTY(EditAnywhere)
	FVector HitLocation;
	UPROPERTY(EditAnywhere)
	FVector HitDirection;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* Montage;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAttributeChanged, const FGameplayAttribute&, Attribute, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpecificAttributeChanged, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAppliedDamage, float, DamageValue, const FHitResult&, SourceHit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOutOfAmmo);

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A basic first person character
 */
UCLASS(abstract)
class AASCharacter : public ACharacter, public IAISightTargetInterface
{
	GENERATED_BODY()

protected:
	// General Components
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> WorldMesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> CameraRoot;	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FirstPersonCameraComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMovementStateComponent> MovementStateMachine;
	
	// Gameplay Abilities System
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UGameplayTasksComponent> GameplayTasksComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Attributes")
	UCharacterAttributeSet* AttributeSet;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AbilitiesSystem")
	TArray<TSubclassOf<UGameplayEffect>> StartupGameplayEffects;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapons")
	TSubclassOf<class AGun> InitialGunClass; 

	UPROPERTY(BlueprintReadOnly, Category="Weapons")
	AGun* EquippedGun;
	
	UPROPERTY(BlueprintReadOnly, Category="Interaction")
	TObjectPtr<AActor> PossibleInteraction;

	UPROPERTY(EditDefaultsOnly, Category="Attributes")
	TSubclassOf<UGameplayEffect> AttributeDefaults;
	
	UPROPERTY(EditDefaultsOnly, Category="StatusEffects")
	TSubclassOf<UGameplayEffect> StunEffect;

	UPROPERTY(BlueprintAssignable, Category="Attributes")
	FOnAttributeChanged OnAnyAttributeChanged;
	UPROPERTY(BlueprintAssignable, Category="Attributes")
	FOnSpecificAttributeChanged OnHealthChanged;
	UPROPERTY(BlueprintAssignable, Category="Attributes")
	FOnSpecificAttributeChanged OnEnergyChanged;
	UPROPERTY(BlueprintAssignable, Category="Attributes")
	FOnSpecificAttributeChanged OnMovementSpeedChanged;
	UPROPERTY(BlueprintAssignable, Category="Attributes")
	FOnSpecificAttributeChanged OnAccuracyModifierChanged;
	
	UPROPERTY(BlueprintAssignable, Category="Attributes")
	FOnAppliedDamage OnAppliedDamage;
	UPROPERTY(BlueprintAssignable, Category="Attributes")
	FOnOutOfAmmo OnOutOfAmmo;
	
	UPROPERTY(EditDefaultsOnly, Category="HitMontages")
	TArray<FHitMontageData> ReceiveHitMontages;

public:
	AASCharacter();

protected:
	virtual void BeginPlay() override;

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;

	virtual void PossessedBy(AController* NewController) override;
	
	void InitializeAttributes();
	
protected:
	void ApplyGameplayEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectToApply);
	void ApplyStartupGameplayEffects();

	// default delegate handlers 
	UFUNCTION(BlueprintCallable, Category="CharacterEvents")
	virtual void HandleAnyAttributeChanged(const FGameplayAttribute& Attribute, float NewValue) {}
	UFUNCTION(BlueprintCallable, Category="CharacterEvents")
	virtual void HandleHealthChanged(float NewValue) {}
	UFUNCTION(BlueprintCallable, Category="CharacterEvents")
	virtual void HandleEnergyChanged(float NewValue) {}
	UFUNCTION(BlueprintCallable, Category="CharacterEvents")
	virtual void HandleMovementSpeedChanged(float NewValue);
	UFUNCTION(BlueprintCallable, Category="CharacterEvents")
	virtual void HandleAppliedDamage(float DamageValue, const FHitResult& SourceHit);
	UFUNCTION(BlueprintCallable, Category="CharacterEvents")
	virtual void HandleOutOfAmmo();
	
	virtual void Die();
	
public:
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category="AbilitySystem")
	bool TryActivateAbilityByTag(FGameplayTag AbilityTag);
	UFUNCTION(BlueprintCallable, Category="AbilitySystem")
	void TryCancelAbilitiesWithTag(FGameplayTag AbilityTag);

	UFUNCTION(BlueprintCallable, Category="Character")
	void SetIgnoreProjectiles(bool bIgnoreProjectiles);
	
	// Guns
	UFUNCTION(BlueprintCallable, Category="Equipment")
	void EquipGun(AGun* Gun);
	void SetEquippedGun(AGun* Gun);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Equipment")
	AGun* GetEquippedGun() const { return EquippedGun; }
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Equipment")
	bool HasGun() const { return EquippedGun != nullptr; }
	
	// General Getters
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Character")
	USkeletalMeshComponent* GetFirstPersonMesh() const { return GetMesh(); }
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Character")
	USkeletalMeshComponent* GetVisibleMesh() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Character")
	UMovementStateComponent* GetMovementStateMachine() const { return MovementStateMachine; }	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Character")
	UAbilitySystemComponent* GetAbilitySystemComponent() const { return AbilitySystemComponent; }
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Character")
	UGameplayTasksComponent* GetGameplayTasksComponent() const { return GameplayTasksComponent; }	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Character")
	UCameraComponent* GetCameraComponent() const { return FirstPersonCameraComponent; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Character")
	EMovementState GetCurrentMovementState() const;
	
	// Attributes Getters
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Attributes")
	UCharacterAttributeSet* GetAttributeSet() const { return AttributeSet; }
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Attributes")
	float GetHealth() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Attributes")
	float GetMaxHealth() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Attributes")
	float GetHealthRegen() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Attributes")
	float GetEnergy() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Attributes")
	float GetMaxEnergy() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Attributes")
	float GetEnergyRegen() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Attributes")
	float GetMovementSpeed() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Attributes")
	float GetJumpPower() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Attributes")
	float GetAccuracyModifier() const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Attributes")
	float GetDamageOutputModifier() const;

	// Attributes Setters
	UFUNCTION(BlueprintCallable, Category="Attributes")
	void SetHealth(float NewHealth);
	UFUNCTION(BlueprintCallable, Category="Attributes")
	void SetMaxHealth(float NewMaxHealth);
	UFUNCTION(BlueprintCallable, Category="Attributes")
	void SetHealthRegen(float NewHealthRegen);
	UFUNCTION(BlueprintCallable, Category="Attributes")
	void SetEnergy(float NewEnergy);
	UFUNCTION(BlueprintCallable, Category="Attributes")
	void SetMaxEnergy(float NewMaxEnergy);
	UFUNCTION(BlueprintCallable, Category="Attributes")
	void SetEnergyRegen(float NewEnergyRegen);
	UFUNCTION(BlueprintCallable, Category="Attributes")
	void SetMovementSpeed(float NewMovementSpeed);
	UFUNCTION(BlueprintCallable, Category="Attributes")
	void SetJumpPower(float NewJumpPower);
	UFUNCTION(BlueprintCallable, Category="Attributes")
	void SetAccuracyModifier(float NewAccuracyModifier);
	UFUNCTION(BlueprintCallable, Category="Attributes")
	void SetDamageOutputModifier(float NewDamageOutputModifier);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Attributes")
	float GetGunSpread() const;

	FOnAttributeChanged& GetOnAttributeChangedDelegate() { return OnAnyAttributeChanged; }
	FOnSpecificAttributeChanged& GetOnHealthChangedDelegate() { return OnHealthChanged; }
	FOnSpecificAttributeChanged& GetOnEnergyChangedDelegate() { return OnEnergyChanged; }
	FOnSpecificAttributeChanged& GetOnMovementSpeedChangedDelegate() { return OnMovementSpeedChanged; }
	FOnSpecificAttributeChanged& GetOnAccuracyModifierChangedDelegate() { return OnAccuracyModifierChanged; }
	
	FOnAppliedDamage& GetOnAppliedDamageDelegate() { return OnAppliedDamage; }
	FOnOutOfAmmo& GetOnOutOfAmmoDelegate() { return OnOutOfAmmo; }
	
	UFUNCTION(BlueprintCallable, Category="Damage")
	UAnimMontage* GetBestHitMontage(const FVector& InHitLocation, const FVector& InHitDirection);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="CachedValues")
	float GetCachedInternalMovementSpeed() const { return CachedInternalMovementSpeed; }

	UFUNCTION(BlueprintCallable, Category="Character")
	void Stun(float Duration);
	
	UFUNCTION(BlueprintCallable, Category="Character")
	void SetBulletTime(float NewBulletTimeValue);
	UFUNCTION(BlueprintCallable, Category="Character")
	float GetGroundDistance();

	// Sight interface
	virtual UAISense_Sight::EVisibilityResult CanBeSeenFrom(const FCanBeSeenFromContext& Context, FVector& OutSeenLocation,
		int32& OutNumberOfLoSChecksPerformed, int32& OutNumberOfAsyncLosCheckRequested, float& OutSightStrength,
		int32* UserData = nullptr, const FOnPendingVisibilityQueryProcessedDelegate* Delegate = nullptr) override;
protected:
	float CachedInternalMovementSpeed;
};

