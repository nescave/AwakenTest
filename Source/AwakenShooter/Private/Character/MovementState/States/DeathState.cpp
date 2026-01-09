// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MovementState/States/DeathState.h"

#include "AbilitySystem/Tasks/GTDeath.h"
#include "Character/Bot/ASBotCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "General/ASGameMode.h"
#include "Items/Gun.h"
#include "Kismet/GameplayStatics.h"

UDeathState::UDeathState() :
	DeathDecayDuration(5.f)
{
	StateID = EMovementState::Death;
	NextState = EMovementState::NONE;
	TargetCameraHeight = 5.f;
}

void UDeathState::OnEnterState_Implementation()
{
	Super::OnEnterState_Implementation();
	if (!Character)
		return;
	
	FVector ThrowDirection = (Character->GetActorForwardVector() + FVector::UpVector).GetSafeNormal();
	if (Character->GetEquippedGun())
	{
		if (AASBotCharacter* BotCharacter = Cast<AASBotCharacter>(Character))
		{
			Character->GetEquippedGun()->Reload();
			if (BotCharacter->GetTarget())
			{
				FVector ApproxTargetLocation = (BotCharacter->GetTargetPosition() + BotCharacter->GetTarget()->GetVelocity() * .5f
					- Character->GetActorLocation()).GetSafeNormal();
				ThrowDirection = (ApproxTargetLocation + FVector::UpVector * .65f ).GetSafeNormal();
			}
		}
		Character->GetEquippedGun()->Throw(ThrowDirection * 350.f);
	}

	if (auto ASC = Character->GetAbilitySystemComponent())
	{
		ASC->CancelAllAbilities();
	}
	Character->GetCharacterMovement()->DisableMovement();
	Character->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (USkeletalMeshComponent* Mesh = Character->GetVisibleMesh())
	{
		Mesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		Mesh->SetCollisionProfileName(FName("Ragdoll"));
		Mesh->SetSimulatePhysics(true);
		Mesh->WakeAllRigidBodies();
		Mesh->bBlendPhysics = true;
	}
	TArray<UMaterialInstanceDynamic*> MaterialInstances;
	for (const auto& Material : DecayMaterials )
	{
		if (UMaterialInstanceDynamic* NewMID = UMaterialInstanceDynamic::Create(Material, Character))
		{
			MaterialInstances.Add(NewMID);
		}
	}

	
	UGTDeath* DeathTask = UGTDeath::Create(Character->GetGameplayTasksComponent(), Character, MaterialInstances, DeathDecayDuration);
	DeathTask->ReadyForActivation();

}

