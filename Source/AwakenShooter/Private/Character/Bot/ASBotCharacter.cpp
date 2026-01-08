// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Bot/ASBotCharacter.h"

#include "AbilitySystem/GameplayTags.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Camera/CameraComponent.h"
#include "Character/Bot/ASBotAIController.h"
#include "Components/CapsuleComponent.h"
#include "General/ProjectGlobals.h"
#include "Items/Gun.h"


// Sets default values
AASBotCharacter::AASBotCharacter()
{
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	WorldMesh->SetCollisionProfileName(ASCollisionProfile::Enemies_ProfileName);
}

void AASBotCharacter::OnStunTagChanged(FGameplayTag GameplayTag, int NewCount)
{
	const bool bIsStunned = (NewCount > 0);

	Blackboard->SetValueAsBool(TEXT("Stunned"), bIsStunned);
}

// Called when the game starts or when spawned
void AASBotCharacter::BeginPlay()
{
	Super::BeginPlay();
	InitialBotLocation = GetActorLocation();

	if (GetController())
	{
		if (EquippedGun)
		{
			EquippedGun->GetOnGunReloadedDelegate().AddDynamic(this, &AASBotCharacter::HandleReload);
		}
		if (AbilitySystemComponent)
		{
			AbilitySystemComponent
				->RegisterGameplayTagEvent(
					FASGameplayTags::Effect::Stunned,
					EGameplayTagEventType::NewOrRemoved
				)
				.AddUObject(this, &AASBotCharacter::OnStunTagChanged);
		}
		AIController = Cast<AASBotAIController>(GetController());
		
		Blackboard = AIController ? AIController->GetBlackboardComponent() : nullptr;

		if (!(Blackboard && AIController))
		{
			UE_LOG(LogTemp, Error, TEXT("CRITICAL: Blackboard or AIController is null!"));
		}
	}
}

void AASBotCharacter::HandleReload()
{
	Blackboard->SetValueAsBool(FName("NeedsReload"), false);
}

void AASBotCharacter::Die()
{
	Super::Die();

	if (Blackboard)
		Blackboard->SetValueAsBool(FName("Dead"), true);
}

// Called every frame
void AASBotCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AASBotCharacter::GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const
{
	Super::GetActorEyesViewPoint(OutLocation, OutRotation);
	OutLocation = GetCameraComponent()->GetComponentLocation();
}

void AASBotCharacter::HandleOutOfAmmo()
{
	Super::HandleOutOfAmmo();
	Blackboard->SetValueAsBool(FName("NeedsReload"), true);
}

void AASBotCharacter::Shoot()
{
	if (EquippedGun)
	{
		if (!EquippedGun->MainAction() && EquippedGun->IsAmmoEmpty())
		{
			OnOutOfAmmo.Broadcast();
		}
	}
}

void AASBotCharacter::Reload()
{
	if (!Blackboard->GetValueAsBool(FName("NeedsReload")))
	{
		return;
	}
	if (EquippedGun)
	{
		TryActivateAbilityByTag(FASGameplayTags::Ability::Reload);
	}
}

void AASBotCharacter::SetLastSeenLocation(const FVector& NewLastSeenLocation)
{
	LastSeenLocation = NewLastSeenLocation;
	Blackboard->SetValueAsVector(TEXT("LastSeenLocation"), NewLastSeenLocation);
}

AActor* AASBotCharacter::GetTarget() const
{
	return AIController ? AIController->GetTarget() : nullptr;
}

FVector AASBotCharacter::GetTargetPosition() const
{
	if (auto Character = Cast<AASCharacter>(GetTarget()))
	{
		return Character->GetCapsuleComponent()->GetComponentLocation() - FVector::UpVector * 15.f;
	}
	return GetTarget() ? GetTarget()->GetActorLocation() : LastSeenLocation;
}

FVector AASBotCharacter::GetTargetDirectionHazed() const
{
	const FVector TargetPosHazed = GetTargetPosition() + (GetTarget() ? GetTarget()->GetVelocity() * FMath::RandRange(-.5f, .5f) : FVector::ZeroVector);
	return (TargetPosHazed - GetActorLocation()).GetSafeNormal();
}


