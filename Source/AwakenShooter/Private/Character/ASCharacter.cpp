// Copyright Epic Games, Inc. All Rights Reserved.

#include "Character/ASCharacter.h"

#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AwakenShooter.h"
#include "AbilitySystem/CharacterAttributeSet.h"
#include "AbilitySystem/GameplayTags.h"
#include "Character/MovementState/MovementStateComponent.h"
#include "General/ASGameMode.h"
#include "General/DebugCVars.h"
#include "Items/Gun.h"
#include "Kismet/GameplayStatics.h"

AASCharacter::AASCharacter() :
	CachedInternalMovementSpeed(100.f)
{
	// Set size for collision capsule
	GetCapsuleComponent()->SetCapsuleSize(34.f, 96.0f);
	GetCapsuleComponent()->SetCapsuleSize(34.0f, 96.0f);
	
	// Create the Camera Component	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FirstPersonCameraComponent->SetupAttachment(RootComponent);
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-0.f, 0.f, 70.0f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->bEnableFirstPersonFieldOfView = true;
	FirstPersonCameraComponent->bEnableFirstPersonScale = true;
	FirstPersonCameraComponent->FirstPersonFieldOfView = 95.0f;
	FirstPersonCameraComponent->FirstPersonScale = 0.6f;

	CameraRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Camera Root"));
	CameraRoot->SetupAttachment(FirstPersonCameraComponent);
	
	// Create the first person mesh that will be viewed only by this character's owner
	GetMesh()->SetupAttachment(CameraRoot);
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	GetMesh()->SetOwnerNoSee(false);
	GetMesh()->SetOnlyOwnerSee(true);
	GetMesh()->SetRelativeLocation(FVector(2.f, 1.f, -194.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	GetMesh()->SetRelativeScale3D(FVector(1.2f, 1.2f, 1.2f));
	
	WorldMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("World Mesh"));
	WorldMesh->SetupAttachment(RootComponent);
	WorldMesh->SetOwnerNoSee(true);
	WorldMesh->SetOnlyOwnerSee(false);
	WorldMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

	MovementStateMachine = CreateDefaultSubobject<UMovementStateComponent>(TEXT("MovementStateComponent"));
	
	// Configure character movement
	GetCharacterMovement()->BrakingDecelerationFalling = 150.0f;
	GetCharacterMovement()->AirControl = 0.4f;
	GetCharacterMovement()->GravityScale = 2.0f;
	GetCharacterMovement()->SetCrouchedHalfHeight(48.f);

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(false); // <- No Multiplayer :(

	AttributeSet = CreateDefaultSubobject<UCharacterAttributeSet>(TEXT("AttributeSet"));
	GameplayTasksComponent = CreateDefaultSubobject<UGameplayTasksComponent>(TEXT("GameplayTasksComponent"));
}

void AASCharacter::BeginPlay()
{
	Super::BeginPlay();

	OnAnyAttributeChanged.AddDynamic(this, &AASCharacter::HandleAnyAttributeChanged);
	OnHealthChanged.AddDynamic(this, &AASCharacter::HandleHealthChanged);
	OnEnergyChanged.AddDynamic(this, &AASCharacter::HandleEnergyChanged);
	OnMovementSpeedChanged.AddDynamic(this, &AASCharacter::HandleMovementSpeedChanged);
	OnAppliedDamage.AddDynamic(this, &AASCharacter::HandleAppliedDamage);
	OnOutOfAmmo.AddDynamic(this, &AASCharacter::HandleOutOfAmmo);
	ApplyStartupGameplayEffects();

	if (InitialGunClass)
	{
		AGun* NewGun = GetWorld()->SpawnActor<AGun>(InitialGunClass);
		EquipGun(NewGun);
	}

	
	MovementStateMachine->InitializeStates();
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
	}
	else if (NewMovementMode == MOVE_Walking)
	{
		MovementStateMachine->NextState();
	}
}

void AASCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		AbilitySystemComponent->AbilityActorInfo->SkeletalMeshComponent = GetVisibleMesh();
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

void AASCharacter::HandleMovementSpeedChanged(float NewValue)
{
	GetCharacterMovement()->MaxWalkSpeed = CachedInternalMovementSpeed * NewValue;
}

void AASCharacter::HandleAppliedDamage(float DamageValue, const FHitResult& SourceHit)
{
	if (DamageValue >= AttributeSet->GetHealth() && GetCurrentMovementState() != EMovementState::Death)
	{
		Die();
	}
	// if (SourceHit.bBlockingHit)
	// {
	// 	const FVector DamageDirection = (SourceHit.ImpactPoint - SourceHit.TraceStart).GetSafeNormal();
	// 	GetVisibleMesh()->AddImpulseAtLocation(GetActorLocation(), DamageDirection * 1000.f);
	// }
}

void AASCharacter::HandleOutOfAmmo()
{
	TryActivateAbilityByTag(FASGameplayTags::Ability::DryFire);
}

void AASCharacter::Die()
{
	MovementStateMachine->ChangeState(EMovementState::Death, true, true);
	for (auto GameplayEffect : StartupGameplayEffects)
	{
		AbilitySystemComponent->RemoveActiveEffectsWithSourceTags(
			FGameplayTagContainer(GameplayEffect->GetDefaultObject<UGameplayEffect>()->GetAssetTags()));
	}
	
	AASGameMode* GameMode = Cast<AASGameMode>(UGameplayStatics::GetGameMode(this));
	if (!GameMode)
		return;
	
	if (ActorHasTag(FName("Player")))
	{
		GameMode->PlayerDeath();
	}
	else
	{
		GameMode->EnemyKilled();
	}
}

void AASCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	// Debug
	if (FASCVars::ASDebugDraw)
	{
		if (FASCVars::ASDrawFireCone && IsPlayerControlled())
		{
			const float GunSpreadRad = FMath::DegreesToRadians(GetGunSpread());
			DrawDebugCone(GetWorld(), FirstPersonCameraComponent->GetComponentLocation()+FirstPersonCameraComponent->GetForwardVector()* 10.f,
				FirstPersonCameraComponent->GetForwardVector(),30000.f, GunSpreadRad,
				GunSpreadRad, 32.f, FColor::Red, false, 0.f);
		}
	}
}

bool AASCharacter::TryActivateAbilityByTag(FGameplayTag AbilityTag)
{
	if (AbilitySystemComponent)
	{
		return AbilitySystemComponent->TryActivateAbilitiesByTag(FGameplayTagContainer(AbilityTag));
	}
	return false;
}

void AASCharacter::TryCancelAbilitiesWithTag(FGameplayTag AbilityTag)
{
	if (AbilitySystemComponent)
	{
		FGameplayTagContainer TagContainer(AbilityTag);
		AbilitySystemComponent->CancelAbilities(&TagContainer);
	}
}

void AASCharacter::SetIgnoreProjectiles(bool bIgnoreProjectiles)
{
	if (bIgnoreProjectiles)
		GetMesh()->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Ignore);
	else
		GetMesh()->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Block);
}

void AASCharacter::EquipGun(AGun* Gun)
{
	if (EquippedGun == Gun)
		return;

	if (EquippedGun)
	{
		EquippedGun->Drop();
	}
	if (Gun)
	{
		Gun->SetGunHolder(this);
	}
	SetEquippedGun(Gun);
}

void AASCharacter::SetEquippedGun(AGun* Gun)
{
	if (Gun)
	{
		GetFirstPersonMesh()->SetRelativeLocation(Gun->GetDefaultGunPosition());
	}else
	{
		GetFirstPersonMesh()->SetRelativeLocation(FVector(5.f, 0.f, -190.f));
	}
	EquippedGun = Gun;
	OnAccuracyModifierChanged.Broadcast(GetAccuracyModifier());
}

USkeletalMeshComponent* AASCharacter::GetVisibleMesh() const
{
	return IsPlayerControlled() ? GetMesh() : WorldMesh.Get();
}

EMovementState AASCharacter::GetCurrentMovementState() const
{
	if (!MovementStateMachine->IsValidLowLevel())
	{
		return EMovementState::Walking;
	}
	
	return MovementStateMachine->GetCurrentStateID();
}

float AASCharacter::GetHealth() const
{
	return AttributeSet->GetHealth();
}

float AASCharacter::GetMaxHealth() const
{
	return AttributeSet->GetMaxHealth();
}

float AASCharacter::GetHealthRegen() const
{
	return AttributeSet->GetHealthRegen();
}

float AASCharacter::GetEnergy() const
{
	return AttributeSet->GetEnergy();
}

float AASCharacter::GetMaxEnergy() const
{
	return AttributeSet->GetMaxEnergy();
}

float AASCharacter::GetEnergyRegen() const
{
	return AttributeSet->GetEnergyRegen();
}

float AASCharacter::GetMovementSpeed() const
{
	return AttributeSet->GetMovementSpeed();
}

float AASCharacter::GetJumpPower() const
{
	return AttributeSet->GetJumpPower();
}

float AASCharacter::GetAccuracyModifier() const
{
	return AttributeSet->GetAccuracyModifier();
}

float AASCharacter::GetDamageOutputModifier() const
{
	return AttributeSet->GetDamageOutputModifier();
}

void AASCharacter::SetHealth(float NewHealth)
{
	AttributeSet->SetHealth(NewHealth);
}

void AASCharacter::SetMaxHealth(float NewMaxHealth)
{
	AttributeSet->SetMaxHealth(NewMaxHealth);
}

void AASCharacter::SetHealthRegen(float NewHealthRegen)
{
	AttributeSet->SetHealthRegen(NewHealthRegen);
}

void AASCharacter::SetEnergy(float NewEnergy)
{
	AttributeSet->SetEnergy(NewEnergy);
}

void AASCharacter::SetMaxEnergy(float NewMaxEnergy)
{
	AttributeSet->SetMaxEnergy(NewMaxEnergy);
}

void AASCharacter::SetEnergyRegen(float NewEnergyRegen)
{
	AttributeSet->SetEnergyRegen(NewEnergyRegen);
}

void AASCharacter::SetMovementSpeed(float NewMovementSpeed)
{
	AttributeSet->SetMovementSpeed(NewMovementSpeed);
}

void AASCharacter::SetJumpPower(float NewJumpPower)
{
	AttributeSet->SetJumpPower(NewJumpPower);
}

void AASCharacter::SetAccuracyModifier(float NewAccuracyModifier)
{
	AttributeSet->SetAccuracyModifier(NewAccuracyModifier);
}

void AASCharacter::SetDamageOutputModifier(float NewDamageOutputModifier)
{
	AttributeSet->SetDamageOutputModifier(NewDamageOutputModifier);
}

float AASCharacter::GetGunSpread() const
{
	if (!EquippedGun)
		return 90.f;
	
	const float MaxSpread = EquippedGun->GetBaseMaxSpread();
	const float Accuracy = FMath::Clamp(EquippedGun->GetBaseAccuracy() * GetAccuracyModifier() * .01f, 0.f, 1.f);
	const float ReturnSpread = FMath::Lerp(MaxSpread, 0.0f, Accuracy);
	GEngine->AddOnScreenDebugMessage(2, 1.f, FColor::Red, TEXT("") + FString::FromInt(ReturnSpread));
	return ReturnSpread;
}

UAnimMontage* AASCharacter::GetBestHitMontage(const FVector& InHitLocation, const FVector& InHitDirection)
{
	float BestMontageScore = -1.f;
	UAnimMontage* BestMontage = nullptr;

	for (const auto& MontageData : ReceiveHitMontages)
	{
		if (!MontageData.Montage) continue;

		FVector MontageHitLocation = GetVisibleMesh()->GetComponentTransform().TransformPositionNoScale(MontageData.HitLocation);
		if (MontageData.BoneName != NAME_None)
		{
			if (MontageData.HitLocation.IsNearlyZero())
				MontageHitLocation = GetVisibleMesh()->GetBoneLocation(MontageData.BoneName);
		}
		const float MontageScore = FMath::Clamp(100.f / FVector::DistSquared(InHitLocation, MontageHitLocation), 0.f, 1.f)
			+ InHitDirection.Dot(GetVisibleMesh()->GetComponentTransform().TransformVectorNoScale(MontageData.HitDirection));
		if ( MontageScore > BestMontageScore )
		{
			BestMontageScore = MontageScore;
			BestMontage = MontageData.Montage;
		}
	}
	return BestMontage;
}

void AASCharacter::Stun(float Duration)
{
	if (!StunEffect)
		return;
	
	FGameplayEffectSpec Spec = FGameplayEffectSpec(StunEffect.GetDefaultObject(), AbilitySystemComponent->MakeEffectContext());
	Spec.SetSetByCallerMagnitude(FASGameplayTags::Data::StunDuration, FMath::Clamp(Duration, 0.1f, 1.f));
	AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(Spec);
}

void AASCharacter::SetBulletTime(float NewBulletTimeValue)
{
	SetIgnoreProjectiles(NewBulletTimeValue < 1.f);

	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), NewBulletTimeValue);
}
	
float AASCharacter::GetGroundDistance()
{
	FHitResult HitResult;
	FVector Start = GetActorLocation();
	FVector End = Start - GetActorUpVector() * 300.f;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	if (GetWorld()->SweepSingleByChannel(HitResult, Start, End,FQuat::Identity, ECC_Visibility,
		FCollisionShape::MakeSphere(30.f), Params))
	{
		return HitResult.Distance;
	}
	return UE_BIG_NUMBER;
}

UAISense_Sight::EVisibilityResult AASCharacter::CanBeSeenFrom(const FCanBeSeenFromContext& Context,
	FVector& OutSeenLocation, int32& OutNumberOfLoSChecksPerformed, int32& OutNumberOfAsyncLosCheckRequested,
	float& OutSightStrength, int32* UserData, const FOnPendingVisibilityQueryProcessedDelegate* Delegate)
{
	const bool bDebugDraw = FASCVars::ASDebugDraw && FASCVars::AILineOfSightTraces;
	
	OutNumberOfLoSChecksPerformed = 0;
    OutNumberOfAsyncLosCheckRequested = 0;
    OutSightStrength = 0.f;

    UWorld* World = GetWorld();
    if (!World)
    {
        return UAISense_Sight::EVisibilityResult::NotVisible;
    }

    USkeletalMeshComponent* MeshComp = GetMesh();
    if (!MeshComp || !MeshComp->SkeletalMesh)
    {
        return UAISense_Sight::EVisibilityResult::NotVisible;
    }

    const FVector ObserverLocation = Context.ObserverLocation;

    // ---- Bone list ----

    static const FName CenterBone(TEXT("spine_03"));
    static const FName LeftBone(TEXT("clavicle_l"));
    static const FName RightBone(TEXT("clavicle_r"));

    TArray<FVector, TInlineAllocator<3>> TestPoints;

    auto AddBoneLocationIfValid = [&](const FName& BoneName)
    {
        if (MeshComp->DoesSocketExist(BoneName))
        {
            TestPoints.Add(MeshComp->GetSocketLocation(BoneName));
        }
    };

    AddBoneLocationIfValid(CenterBone);
    AddBoneLocationIfValid(LeftBone);
    AddBoneLocationIfValid(RightBone);

    if (TestPoints.Num() == 0)
    {
        TestPoints.Add(GetActorLocation());
    }

    FCollisionQueryParams Params(SCENE_QUERY_STAT(AISight), /*bTraceComplex*/ true);
    Params.AddIgnoredActor(this);

    if (Context.IgnoreActor)
    {
        Params.AddIgnoredActor(Context.IgnoreActor);
    }

    for (const FVector& TargetPoint : TestPoints)
    {
        ++OutNumberOfLoSChecksPerformed;

        FHitResult HitResult;
        const bool bHit = World->LineTraceSingleByChannel(
            HitResult,
            ObserverLocation,
            TargetPoint,
            ECC_Visibility,
            Params
        );

        if (!bHit || HitResult.GetActor() == this)
        {
            OutSeenLocation = TargetPoint;

            const float DistanceSq = FVector::DistSquared(ObserverLocation, TargetPoint);
            OutSightStrength = 1.f / FMath::Max(DistanceSq, 1.f);

        	if (bDebugDraw)
        	{
        		DrawDebugLine(World, ObserverLocation, TargetPoint, FColor::Green, false,
        			FASCVars::ASDebugDrawDuration, 0.f, 1.f);
        	}
            return UAISense_Sight::EVisibilityResult::Visible;
        }
    	
    	if (bDebugDraw)
    	{
    		DrawDebugLine(World, ObserverLocation, HitResult.Location, FColor::Red, false,
    				FASCVars::ASDebugDrawDuration, 0.f, 1.f);
    	}
    }

    return UAISense_Sight::EVisibilityResult::NotVisible;
}
