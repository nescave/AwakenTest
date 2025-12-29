// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/CharacterAttributeSet.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "MovementState/MovementStateBase.h"
#include "ASCharacter.generated.h"

class UMovementStateComponent;
class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAttributeChanged, const FGameplayAttribute&, Attribute, float, NewValue);

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A basic first person character
 */
UCLASS(abstract)
class AASCharacter : public ACharacter
{
	GENERATED_BODY()
	// General Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> FirstPersonMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> CameraRoot;	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FirstPersonCameraComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMovementStateComponent> MovementStateMachine;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> GunSocket;
	
	// Gameplay Abilities System
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UGameplayTasksComponent> GameplayTasksComponent;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Attributes")
	UCharacterAttributeSet* AttributeSet;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AbilitiesSystem")
	TArray<TSubclassOf<UGameplayEffect>> StartupGameplayEffects;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AbilitiesSystem")
	TMap<FGameplayTag, TSubclassOf<UGameplayEffect>> CharacterEffects;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapons")
	TSubclassOf<class AGun> InitialGunClass; 
	UPROPERTY(BlueprintReadOnly, Category="Weapons")
	AGun* EquippedGun;
	
	UPROPERTY(BlueprintReadOnly, Category="Interaction")
	TObjectPtr<AActor> PossibleInteraction;
	
	// InputActions
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* MouseLookAction;
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* CrouchAction;
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* SprintAction;	
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* WallRunAction;
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* InteractAction;
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* GunMainAction;
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* GunSecondaryAction;
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* ThrowAction;
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* ReloadAction;
	
	UPROPERTY(EditDefaultsOnly, Category="Attributes")
	TSubclassOf<UGameplayEffect> AttributeDefaults;

	UPROPERTY(BlueprintAssignable, Category="Attributes")
	FOnAttributeChanged OnAttributesChanged;

public:
	AASCharacter();

protected:
	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginPlay() override;

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	virtual void PossessedBy(AController* NewController) override;
	
	void InitializeAttributes();
	
	void HandleLookInput(const FInputActionValue& Value);
	void HandleMoveInput(const FInputActionValue& Value);
	void HandleJumpInput(const FInputActionValue& Value);
	void HandleCrouchInput(const FInputActionValue& Value);
	void HandleSprintInput(const FInputActionValue& Value);
	void HandleWallRunInput(const FInputActionValue& Value);
	void HandleInteractInput(const FInputActionValue& Value);
	void HandleGunMainInput(const FInputActionValue& Value);
	void HandleGunSecondaryInput(const FInputActionValue& Value);
	void HandleThrowInput(const FInputActionValue& Value);
	void HandleReloadInput(const FInputActionValue& Value);
	
protected:
	void ApplyGameplayEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectToApply);
	void ApplyStartupGameplayEffects();
	bool TryFindInteraction();

	void SetPossibleInteraction(class IInteractable* NewInteraction);
	IInteractable* GetPossibleInteraction() const;
public:
	virtual void Tick(float DeltaSeconds) override;
	
	bool TryFindWall(FHitResult& OutHitResult);

	UFUNCTION(BlueprintCallable, Category="AbilitySystem")
	bool TryActivateAbilityByTag(FGameplayTag AbilityTag);
	UFUNCTION(BlueprintCallable, Category="Character")
	void UpdateMovementSpeed(const FGameplayAttribute& Attribute, float NewValue);

	// Guns
	UFUNCTION(BlueprintCallable, Category="Equipment")
	void EquipGun(AGun* Gun);
	UFUNCTION(BlueprintCallable, Category="Equipment")
	void ThrowEquipped();
	
	UFUNCTION(BlueprintCallable, Category="Equipment")
	AGun* GetEquippedGun() const { return EquippedGun; }
	
	// General Getters
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Character")
	USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Character")
	UMovementStateComponent* GetMovementStateMachine() const { return MovementStateMachine; }	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Character")
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
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
	float GetHealth() const { return AttributeSet->GetHealth(); }
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Attributes")
	float GetMaxHealth() const { return AttributeSet->GetMaxHealth(); }
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Attributes")
	float GetHealthRegen() const { return AttributeSet->GetHealthRegen(); }	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Attributes")
	float GetEnergy() const { return AttributeSet->GetEnergy(); }
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Attributes")
	float GetMaxEnergy() const { return AttributeSet->GetMaxEnergy(); }
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Attributes")
	float GetEnergyRegen() const { return AttributeSet->GetEnergyRegen(); }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="CachedValues")
	float GetCachedInternalMovementSpeed() const { return CachedInternalMovementSpeed; }
	
	FOnAttributeChanged& OnAttributeChangedDelegate() { return OnAttributesChanged; }
private:
	float CachedInternalMovementSpeed;
};

