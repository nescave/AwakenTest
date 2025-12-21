// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Gun.h"

#include "AbilitySystemComponent.h"
#include "EditorCategoryUtils.h"
#include "Camera/CameraComponent.h"
#include "Character/ASCharacter.h"


AGun::AGun()
{
	PrimaryActorTick.bCanEverTick = false;
	Primitive = Cast<UPrimitiveComponent>(RootComponent);
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	SetActorTickInterval(.25f);
}

void AGun::Throw(const FVector& ForceVector)
{
	Primitive->AddForceAtLocation(ForceVector, GetActorLocation());
	SetActorTickEnabled(true);
}

void AGun::Interact(AASCharacter* Interactor)
{
	if (!Interactor)
	{
		return;
	}
	if (AGun* PreviousGun = Interactor->GetEquippedGun())
	{
		PreviousGun->Throw(Interactor->GetCameraComponent()->GetForwardVector() * 1000.f);
	}
	Interactor->EquipGun(this);
}

void AGun::SetHighlighted(float HighlightValue)
{
	Mesh->SetCustomPrimitiveDataFloat(0, HighlightValue);
}

void AGun::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (Primitive->IsSimulatingPhysics() && !Primitive->RigidBodyIsAwake())
	{
		Primitive->SetSimulatePhysics(false);
		SetActorTickEnabled(false);
	}
}

void AGun::ApplyEffects(UAbilitySystemComponent* ASC)
{
	for (const auto& Effect : GunPassiveEffects)
	{
		EffectHandles.Add(ASC->BP_ApplyGameplayEffectToSelf(Effect, 1.f, ASC->MakeEffectContext()));
	}
}

void AGun::RemoveEffects(UAbilitySystemComponent* ASC)
{
	for (const auto& EffectHandle : EffectHandles)
	{
		ASC->RemoveActiveGameplayEffect(EffectHandle);
	}
	EffectHandles.Empty();
}

bool AGun::IsOnGround()
{
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	if (GetWorld()->SweepSingleByChannel(HitResult, GetActorLocation(), GetActorLocation(), 
		GetActorRotation().Quaternion(), ECC_Visibility, 
		FCollisionShape::MakeCapsule(5.f, 15.f),Params))
	{
		return true;
	}
	return false;
}
