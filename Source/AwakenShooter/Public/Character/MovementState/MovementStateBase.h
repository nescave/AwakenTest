// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MovementStateBase.generated.h"

struct FInputActionValue;

UENUM(BlueprintType)
enum class EMovementState : uint8
{
	NONE,
	Walking,
	Sprinting,
	Crouching,
	Sliding,
	Falling,
	Hanging
};
class UAbilitySystemComponent;
class UMovementStateComponent;
class AASCharacter;
/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class AWAKENSHOOTER_API UMovementStateBase : public UObject
{
	GENERATED_BODY()

	bool bDebugLogging = true;
	
	UPROPERTY()
	TObjectPtr<class UCharacterHeightTransition> HeightTransitionTask;
	
protected:
	UPROPERTY()
	TObjectPtr<UMovementStateComponent> StateMachine;
	UPROPERTY()
	TObjectPtr<AASCharacter> Character;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="State")
	EMovementState StateID;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="State")
	bool bTickEnabled;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="State")
	EMovementState NextState;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="State")
	bool bStateLocked;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="State")
	float TargetCameraHeight;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="State")
	float TargetCapsuleHalfHeight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="State")
	TArray<TSubclassOf<class UGameplayEffect>> EffectsToApply;

	UPROPERTY()
	TArray<struct FActiveGameplayEffectHandle> EffectsHandles;

protected:
	void SetStateLocked(bool bInLocked);
	bool CharacterHeightNeedsUpdate() const;
	float GetGroundDistance();

public:
	UMovementStateBase();
	
	virtual void Initialize(
		UMovementStateComponent* InStateMachine,
		AASCharacter* InCharacter);

	/* State lifecycle */
	UFUNCTION(BlueprintNativeEvent, Category="State Flow")
	void OnEnterState();
	UFUNCTION(BlueprintNativeEvent, Category="State Flow")
	void OnExitState();
	UFUNCTION(BlueprintNativeEvent, Category="State Flow")
	void OnStateTick(float DeltaTime);

	/* Input handlers (override per state) */
	virtual void HandleLook(const FInputActionValue& Value);
	virtual void HandleMove(const FInputActionValue& Value);
	virtual void HandleJump(const FInputActionValue& Value);
	virtual void HandleCrouch(const FInputActionValue& Value);
	virtual void HandleSprint(const FInputActionValue& Value);
	virtual void HandleWallRun(const FInputActionValue& Value) {}
	virtual void HandleGunMain(const FInputActionValue& Value);
	virtual void HandleGunSecondary(const FInputActionValue& Value);	
	virtual void HandleReload(const FInputActionValue& Value);
	virtual void HandleThrow(const FInputActionValue& Value);


	UFUNCTION(BlueprintCallable, Category="State")
	void SetTickEnabled(bool bInTickEnabled) { bTickEnabled = bInTickEnabled; }
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="State")
	bool HasTickEnabled() const { return bTickEnabled; }
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="State")
	AASCharacter* GetCharacter() const { return Character; }
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="State")
	virtual FVector GetJumpDirection();

	EMovementState GetStateID() const { return StateID; }
	bool IsLocked() const { return bStateLocked; }
	EMovementState GetNextState() const { return NextState; }

	virtual UWorld* GetWorld() const override;

private:
	// helper functions
	void ApplyEffects(UAbilitySystemComponent* ASC);
	void RemoveEffects(UAbilitySystemComponent* ASC);
	
};
