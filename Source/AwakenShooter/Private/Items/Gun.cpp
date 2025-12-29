// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Gun.h"

#include "AbilitySystemComponent.h"
#include "AwakenShooter.h"
#include "AbilitySystem/GameplayTags.h"
#include "AbilitySystem/GunAttributeSet.h"
#include "Character/ASCharacter.h"

AGun::AGun()
{
	PrimaryActorTick.bCanEverTick = true;
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
	Mesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	Mesh->SetCollisionObjectType(ECC_WorldDynamic);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Overlap);

	Mesh->SetSimulatePhysics(false);

	GunAbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("GunAbilitySystemComponent"));
	GunAbilitySystemComponent->SetIsReplicated(false); // <- No Multiplayer :(

	GunAttributeSet = CreateDefaultSubobject<UGunAttributeSet>(TEXT("GunAttributeSet"));
	
	SetActorTickInterval(.25f);
}

void AGun::InitializeAttributes()
{
	if (!GunAttributeSet || !AttributeDefaults)
	{
		UE_LOG(LogAwakenShooter, Error, TEXT("'%s' Failed to find an AttributeSet!"), *GetNameSafe(this));
		return;
	}
	GunAbilitySystemComponent->ApplyGameplayEffectToSelf(
			AttributeDefaults->GetDefaultObject<UGameplayEffect>(), 1.f, GunAbilitySystemComponent->MakeEffectContext());}

void AGun::BeginPlay()
{
	Super::BeginPlay();
	InitializeAttributes();
	
	if (MainActionAbility)
	{
		GunAbilitySystemComponent->K2_GiveAbility(MainActionAbility);
	}
	if (SecondaryActionAbility)
	{
		GunAbilitySystemComponent->K2_GiveAbility(SecondaryActionAbility);
	}
	if (!IsOnGround())
	{
		Drop();
	}
	if (GunAbilitySystemComponent)
	{
		GunAbilitySystemComponent->AddGameplayCue(FASGameplayTags::GameplayCue_Gun_MuzzleFlare0);
	}
}

void AGun::Reload()
{
	GunAbilitySystemComponent->BP_ApplyGameplayEffectToSelf(ReloadEffect, 0.f, GunAbilitySystemComponent->MakeEffectContext());
}

void AGun::Throw(const FVector& ForceVector)
{
	if (!Mesh)
	{
		return;
	}
	
	Drop();
	Mesh->AddImpulse(ForceVector*30.f);
}

void AGun::Drop()
{
	if (GunHolder)
	{
		RemoveEffects(GunHolder->GetAbilitySystemComponent());
		GunHolder = nullptr;
	}
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	Mesh->SetSimulatePhysics(true);
	Mesh->WakeRigidBody();
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);
}

void AGun::MainAction()
{
	if (!MainActionAbility)
	{
		return;
	}
	GunAbilitySystemComponent->TryActivateAbilityByClass(MainActionAbility);
}

void AGun::SecondaryAction()
{
	if (!SecondaryActionAbility)
	{
		return;
	}
	GunAbilitySystemComponent->TryActivateAbilityByClass(SecondaryActionAbility);
}

void AGun::Interact(AASCharacter* Interactor)
{
	if (!Interactor)
	{
		return;
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
	if (!Mesh)
		return;
	
	if (Mesh->IsSimulatingPhysics() && !Mesh->RigidBodyIsAwake())
	{
		Mesh->SetSimulatePhysics(false);
		SetActorTickEnabled(false);
	}
}

void AGun::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

}

void AGun::SetGunHolder(AASCharacter* NewHolder)
{
	if (GunHolder == NewHolder)
	{
		return;
	}
	if (GunHolder)
	{
		Drop();
	}
	if (!NewHolder)
	{
		return;
	}
	GunHolder = NewHolder;
	GetMeshComponent()->SetSimulatePhysics(false);
	GetMeshComponent()->SetPhysicsLinearVelocity(FVector::ZeroVector);
	GetMeshComponent()->SetPhysicsAngularVelocityInRadians(FVector::ZeroVector);
	GetMeshComponent()->PutRigidBodyToSleep();
	SetActorEnableCollision(false);
	ApplyEffects(NewHolder->GetAbilitySystemComponent());
	AttachToComponent(
		NewHolder->GetFirstPersonMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("HandGrip_R"));
}

void AGun::ApplyEffects(UAbilitySystemComponent* ASC)
{
	if (!ASC)
	{
		return;
	}
	
	for (const auto& Effect : GunPassiveEffects)
	{
		EffectHandles.Add(ASC->BP_ApplyGameplayEffectToSelf(Effect, 1.f, ASC->MakeEffectContext()));
	}
}

void AGun::RemoveEffects(UAbilitySystemComponent* ASC)
{
	if (!ASC)
	{
		return;
	}
	
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
