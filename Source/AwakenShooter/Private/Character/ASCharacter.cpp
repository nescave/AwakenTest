// Copyright Epic Games, Inc. All Rights Reserved.

#include "Character/ASCharacter.h"

#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AwakenShooter.h"
#include "AbilitySystem/CharacterAttributeSet.h"
#include "Character/MovementState/MovementStateComponent.h"
#include "Engine/OverlapResult.h"
#include "General/DebugCVars.h"
#include "Items/Gun.h"
#include "Items/Interactable.h"

AASCharacter::AASCharacter()
	: CachedInternalMovementSpeed(100.f)
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// Create the Camera Component	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocationAndRotation(FVector(-2.8f, 5.89f, 0.0f), FRotator(0.0f, 90.0f, -90.0f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->bEnableFirstPersonFieldOfView = true;
	FirstPersonCameraComponent->bEnableFirstPersonScale = true;
	FirstPersonCameraComponent->FirstPersonFieldOfView = 95.0f;
	FirstPersonCameraComponent->FirstPersonScale = 0.6f;

	CameraRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Camera Root"));
	CameraRoot->SetupAttachment(FirstPersonCameraComponent);
	
	// Create the first person mesh that will be viewed only by this character's owner
	FirstPersonMesh = GetMesh();
	FirstPersonMesh->SetupAttachment(CameraRoot);
	FirstPersonMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	FirstPersonMesh->SetOwnerNoSee(false);
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));

	GunSocket = CreateDefaultSubobject<USceneComponent>(TEXT("GunSocket"));
	
	MovementStateMachine = CreateDefaultSubobject<UMovementStateComponent>(TEXT("MovementStateComponent"));

	GetCapsuleComponent()->SetCapsuleSize(34.0f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->BrakingDecelerationFalling = 150.0f;
	GetCharacterMovement()->AirControl = 0.4f;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(false); // <- No Multiplayer :(

	AttributeSet = CreateDefaultSubobject<UCharacterAttributeSet>(TEXT("AttributeSet"));
	GameplayTasksComponent = CreateDefaultSubobject<UGameplayTasksComponent>(TEXT("GameplayTasksComponent"));
}

void AASCharacter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	GunSocket->AttachToComponent(
		FirstPersonMesh,
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		TEXT("HandGrip_R"));
}

void AASCharacter::BeginPlay()
{
	Super::BeginPlay();

	OnAttributesChanged.AddDynamic(this, &AASCharacter::UpdateMovementSpeed);
	CachedInternalMovementSpeed = GetCharacterMovement()->MaxWalkSpeed;
	ApplyStartupGameplayEffects();

	if (InitialGunClass)
	{
		AGun* NewGun = GetWorld()->SpawnActor<AGun>(InitialGunClass);
		EquipGun(NewGun);
	}
}

void AASCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
	if (!MovementStateMachine->IsInitialized())
		return;
	
	EMovementMode NewMovementMode = GetCharacterMovement()->MovementMode;
	if (NewMovementMode == MOVE_Falling)
	{
		MovementStateMachine->ChangeState(EMovementState::Falling);
	}else if (NewMovementMode == MOVE_Walking)
	{
		MovementStateMachine->ChangeState(EMovementState::Walking);
	}
}

void AASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AASCharacter::HandleLookInput);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AASCharacter::HandleMoveInput);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AASCharacter::HandleJumpInput);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &AASCharacter::HandleCrouchInput);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &AASCharacter::HandleSprintInput);
		EnhancedInputComponent->BindAction(WallRunAction, ETriggerEvent::Triggered, this, &AASCharacter::HandleWallRunInput);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &AASCharacter::HandleInteractInput);
		EnhancedInputComponent->BindAction(GunMainAction, ETriggerEvent::Triggered, this, &AASCharacter::HandleGunMainInput);
		EnhancedInputComponent->BindAction(GunSecondaryAction, ETriggerEvent::Triggered, this, &AASCharacter::HandleGunSecondaryInput);
		EnhancedInputComponent->BindAction(ThrowAction, ETriggerEvent::Triggered, this, &AASCharacter::HandleThrowInput);
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &AASCharacter::HandleReloadInput);
	}
	else
	{
		UE_LOG(LogAwakenShooter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}

	MovementStateMachine->InitializeStates();
}

void AASCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		InitializeAttributes();
	}
}

void AASCharacter::InitializeAttributes()
{
	if (!AttributeSet || !AttributeDefaults)
	{
		UE_LOG(LogAwakenShooter, Error, TEXT("'%s' Failed to find an AttributeSet!"), *GetNameSafe(this));
		return;
	}
	AbilitySystemComponent->ApplyGameplayEffectToSelf(
		AttributeDefaults->GetDefaultObject<UGameplayEffect>(), 1.f, AbilitySystemComponent->MakeEffectContext());
}

void AASCharacter::HandleLookInput(const FInputActionValue& Value)
{
	MovementStateMachine->HandleLook(Value);
}

void AASCharacter::HandleMoveInput(const FInputActionValue& Value)
{
	MovementStateMachine->HandleMove(Value);
}

void AASCharacter::HandleJumpInput(const FInputActionValue& Value)
{
	MovementStateMachine->HandleJump(Value);
}

void AASCharacter::HandleCrouchInput(const FInputActionValue& Value)
{
	MovementStateMachine->HandleCrouch(Value);
}

void AASCharacter::HandleSprintInput(const FInputActionValue& Value)
{
	MovementStateMachine->HandleSprint(Value);
}

void AASCharacter::HandleWallRunInput(const FInputActionValue& Value)
{
	MovementStateMachine->HandleWallRun(Value);
}

void AASCharacter::HandleInteractInput(const FInputActionValue& Value)
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Interact"));
	if (!PossibleInteraction)
	{
		return;
	}
	GetPossibleInteraction()->Interact(this);
}

void AASCharacter::HandleGunMainInput(const FInputActionValue& Value)
{
	MovementStateMachine->HandleGunMain(Value);
}

void AASCharacter::HandleGunSecondaryInput(const FInputActionValue& Value)
{
	MovementStateMachine->HandleGunSecondary(Value);
}

void AASCharacter::HandleThrowInput(const FInputActionValue& Value)
{
	ThrowEquipped();
	// MovementStateMachine->HandleThrow(Value);
}

void AASCharacter::HandleReloadInput(const FInputActionValue& Value)
{
	MovementStateMachine->HandleReload(Value);
}

void AASCharacter::ApplyGameplayEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectToApply)
{
	AbilitySystemComponent->ApplyGameplayEffectToSelf(
		GameplayEffectToApply->GetDefaultObject<UGameplayEffect>(),
		1.f,
		AbilitySystemComponent->MakeEffectContext());
}

void AASCharacter::ApplyStartupGameplayEffects()
{
	for (const auto& GameplayEffect : StartupGameplayEffects)
	{
		ApplyGameplayEffectToSelf(GameplayEffect);
	}
}

bool AASCharacter::TryFindInteraction()
{
	const FVector Position = GetCameraComponent()->GetComponentLocation() + GetCameraComponent()->GetForwardVector() * 120.f;
	FCollisionQueryParams Params;
	if (EquippedGun)
		Params.AddIgnoredActor(EquippedGun);

	TArray<FOverlapResult> Interactions;
	TArray<IInteractable*> Interactables;
	
	IInteractable* Interactive = nullptr;
	if (FASCVars::ASDebugDraw && FASCVars::ASDrawInteractionsZone)
	{
		DrawDebugSphere(GetWorld(), Position, 85.f,12,
			FColor::Red, false, 0.f, 0.f, .25f);
	}
	if (GetWorld()->OverlapMultiByChannel(Interactions, Position, FQuat::Identity, ECC_GameTraceChannel2, 
		FCollisionShape::MakeSphere(85.f), Params))
	{
		float ClosestMatch = -1.f;
		for (const auto& Item : Interactions)
		{
			if(!(Item.GetActor() && Item.GetActor()->Implements<UInteractable>()) )
			{
				continue;
			}
			if (FASCVars::ASDebugDraw && FASCVars::ASDrawDetailedInteractions)
			{
				Interactables.Add(Cast<IInteractable>(Item.GetActor()));
			}
			FVector DirectionToItem = 
				(Item.GetActor()->GetActorLocation() - GetCameraComponent()->GetComponentLocation()).GetSafeNormal();
			float IDotC = DirectionToItem.Dot(GetCameraComponent()->GetForwardVector());
			if (IDotC > ClosestMatch)
			{
				Interactive = Cast<IInteractable>(Item.GetActor());
				ClosestMatch = IDotC;
			}
		}
	}

	if (FASCVars::ASDebugDraw && FASCVars::ASDrawDetailedInteractions)
	{
		for (const auto& Item : Interactables)
		{
			AActor* Actor = Cast<AActor>(Item);
			FColor Color = FColor::Red;
			if (Item == Interactive)
			{
				Color = FColor::Green;
			}
			DrawDebugSphere(GetWorld(), Actor->GetActorLocation(), 20.f,4,
				Color, false, 0.f, 0.f, 1.f);
		}
	}
	
	SetPossibleInteraction(Interactive);
	
	if (PossibleInteraction)
	{
		return true;
	}
	return false;
}

void AASCharacter::SetPossibleInteraction(IInteractable* NewInteraction)
{
	if (IInteractable* OldInteraction = GetPossibleInteraction())
	{
		if (OldInteraction == NewInteraction)
			return;
		
		OldInteraction->SetHighlighted(0.f);
	}
	if (AActor* InteractableActor = Cast<AActor>(NewInteraction))
	{
		NewInteraction->SetHighlighted(1.f);
		PossibleInteraction = InteractableActor;
	}
	else
	{
		PossibleInteraction = nullptr;
	}
}

IInteractable* AASCharacter::GetPossibleInteraction() const
{
	return Cast<IInteractable>(PossibleInteraction);
}

void AASCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (TryFindInteraction())
	{
		GEngine->AddOnScreenDebugMessage(-1, DeltaSeconds, FColor::Red, TEXT("Possible Interaction: ")
			+ PossibleInteraction->GetName());
	}
	
}

bool AASCharacter::TryFindWall(FHitResult& OutHitResult)
{
	const FVector Start = GetCameraComponent()->GetComponentLocation();
	const FVector Forward =(
		FVector(GetMovementComponent()->Velocity.X, GetMovementComponent()->Velocity.Y, 0.f).GetSafeNormal()
		+ GetActorForwardVector())
		.GetSafeNormal();
	const FVector EndFront = Start + Forward * 50.f;
	const FVector EndRight = Start + Forward.RotateAngleAxis(25.f, FVector::UpVector) * 50.f;
	const FVector EndLeft = Start + Forward.RotateAngleAxis(-25.f, FVector::UpVector) * 50.f;
	constexpr float CheckCapsuleHalfHeight = 15.f;
	if (FASCVars::ASDebugDraw && FASCVars::ASDrawWallCatchTests)
	{
		DrawDebugCapsule(GetWorld(), EndFront, CheckCapsuleHalfHeight, 10.f, FQuat::Identity, FColor::Red, false, 0.f, 0.f, .25f);
		DrawDebugCapsule(GetWorld(), EndRight, CheckCapsuleHalfHeight, 10.f, FQuat::Identity, FColor::Red, false, 0.f, 0.f, .25f);
		DrawDebugCapsule(GetWorld(), EndLeft, CheckCapsuleHalfHeight, 10.f, FQuat::Identity, FColor::Red, false, 0.f, 0.f, .25f);
	}
	
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	if (GetWorld()->SweepSingleByChannel(
		OutHitResult,
		Start,
		EndFront,
		FQuat::Identity,
		ECC_WorldStatic,
		FCollisionShape::MakeCapsule(10.f, CheckCapsuleHalfHeight),
		Params)
		|| GetWorld()->SweepSingleByChannel(
		OutHitResult,
		Start,
		EndRight,
		FQuat::Identity,
		ECC_WorldStatic,
		FCollisionShape::MakeCapsule(10.f, CheckCapsuleHalfHeight),
		Params)
		|| GetWorld()->SweepSingleByChannel(
		OutHitResult,
		Start,
		EndLeft,
		FQuat::Identity,
		ECC_WorldStatic,
		FCollisionShape::MakeCapsule(10.f, CheckCapsuleHalfHeight),
		Params))
	{
		const FVector End = Start - OutHitResult.Normal * 70.f;
		if (GetWorld()->SweepSingleByChannel(
			OutHitResult,
			Start,
			End,
			FQuat::Identity,
			ECC_WorldStatic,
			FCollisionShape::MakeCapsule(10.f, 20.f),
			Params))
		{
			if (FASCVars::ASDebugDraw && FASCVars::ASDrawWallCatchTests)
			{
				DrawDebugCapsule(GetWorld(),
					OutHitResult.ImpactPoint, CheckCapsuleHalfHeight, 10.f, FQuat::Identity,
					FColor::Green, false, FASCVars::ASDebugDrawDuration, 0.f, .25f);
			}	
		}
		if (FMath::Abs(OutHitResult.Normal.Dot(FVector::UpVector)) < .25f)
		{
			return true;
		}
	}
	return false;
}

bool AASCharacter::TryActivateAbilityByTag(FGameplayTag AbilityTag)
{
	if (AbilitySystemComponent)
	{
		return AbilitySystemComponent->TryActivateAbilitiesByTag(FGameplayTagContainer(AbilityTag));
	}
	return false;
}

void AASCharacter::UpdateMovementSpeed(const FGameplayAttribute& Attribute, float NewValue)
{
	if (Attribute.GetName() == AttributeSet->GetMovementSpeedAttribute().GetName())
	{
		GetCharacterMovement()->MaxWalkSpeed = CachedInternalMovementSpeed * NewValue;
	}
}

void AASCharacter::EquipGun(AGun* Gun)
{
	if (EquippedGun)
	{
		EquippedGun->SetGunHolder(nullptr);
	}
	EquippedGun = Gun;
	EquippedGun->SetGunHolder(this);
}

void AASCharacter::ThrowEquipped()
{
	if (!EquippedGun)
		return;
	
	EquippedGun->Throw(FirstPersonCameraComponent->GetForwardVector() * 100.f);
	EquippedGun = nullptr;
}

EMovementState AASCharacter::GetCurrentMovementState() const
{
	if (!MovementStateMachine->IsValidLowLevel())
	{
		return EMovementState::Walking;
	}
	
	return MovementStateMachine->GetCurrentStateID();
}
