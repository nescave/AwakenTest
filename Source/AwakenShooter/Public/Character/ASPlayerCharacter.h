// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ASCharacter.h"
#include "ASPlayerCharacter.generated.h"

UCLASS()
class AWAKENSHOOTER_API AASPlayerCharacter : public AASCharacter
{
	GENERATED_BODY()
	// Components
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAIPerceptionStimuliSourceComponent> StimuliSource;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> MeleeHitBox;	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> SlideHitBox;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> FallHitBox;	
	
protected:
		
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
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* AimDownSightsAction;
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* BulletTimeAction;
	
	UPROPERTY(EditAnywhere, Category ="Movement")
	float MaxCharacterVelocity;
	
public:
	// Sets default values for this character's properties
	AASPlayerCharacter();

protected:
	UFUNCTION()
	void EmitMovementNoise(float DeltaSeconds, FVector OldLocation, FVector OldVelocity);
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	void AutoEquip();
	
	// Input
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
	void HandleAimDownSightsInput(const FInputActionValue& InputActionValue);
	void HandleBulletTime(const FInputActionValue& InputActionValue);

	bool TryFindInteraction();
	void SetPossibleInteraction(AActor* NewInteraction);
	AActor* GetPossibleInteraction() const;

	
	void AddClampedVelocity(const FVector& VelocityToAdd, float Magnitude = 1.f, bool bIgnoreAtMaxVelocity = false);
	bool TryFindWall(FHitResult& OutHitResult);

	UFUNCTION(BlueprintCallable, Category="Character")
	void ReportNoiseEvent(float InLoudness, float InMaxRange);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Character")
	USphereComponent* GetMeleeHitBox() const { return MeleeHitBox; }
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Character")
	USphereComponent* GetSlideHitBox() const { return SlideHitBox; }
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Character")
	USphereComponent* GetFallHitBox() const { return FallHitBox; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="CachedValues")
	bool GetHangingPoint(FHitResult& OutHangingPoint) const;
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="CachedValues")
	float GetMaxCharacterVelocity() const { return MaxCharacterVelocity; }

private:
	
public:
	UPROPERTY()
	class UGameplayTask* ADSTransitionTaskInstance;
};
