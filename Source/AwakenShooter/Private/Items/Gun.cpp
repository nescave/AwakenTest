// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Gun.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AwakenShooter.h"
#include "AbilitySystem/GameplayTags.h"
#include "AbilitySystem/GunAttributeSet.h"
#include "Character/ASCharacter.h"
#include "General/ProjectGlobals.h"

AGun::AGun() :
	SocketName(FName("HandGrip_Rifle")),
	DefaultGunPosition(2.f, 1.f,-194.f),
	bThrown(false)
{
	PrimaryActorTick.bCanEverTick = true;
	
	WorldMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WorldMesh"));
	SetRootComponent(WorldMesh);

	WorldMesh->SetCollisionProfileName(ASCollisionProfile::Gun_ProfileName);
	WorldMesh->SetSimulatePhysics(false);

	FPSMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FPSMesh"));
	FPSMesh->SetupAttachment(WorldMesh);
	FPSMesh->SetFirstPersonPrimitiveType(EFirstPersonPrimitiveType::FirstPerson);
	FPSMesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	SetFPSMode(false);
	
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
		AttributeDefaults->GetDefaultObject<UGameplayEffect>(), 1.f, GunAbilitySystemComponent->MakeEffectContext());
}

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
		GunAbilitySystemComponent->AddGameplayCue(FASGameplayTags::GameplayCue::Gun::MuzzleFlare0);
		GunAbilitySystemComponent->AddGameplayCue(FASGameplayTags::GameplayCue::Gun::Melee);
		GunAbilitySystemComponent->AddGameplayCue(FASGameplayTags::GameplayCue::Hit::MeleeImpact);
		GunAbilitySystemComponent->AddGameplayCue(FASGameplayTags::GameplayCue::Hit::BulletImpact);
	}
}

void AGun::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	FPSMesh->SetStaticMesh(WorldMesh->GetStaticMesh());
}

void AGun::Reload()
{
	GunAbilitySystemComponent->BP_ApplyGameplayEffectToSelf(ReloadEffect, 0.f, GunAbilitySystemComponent->MakeEffectContext());
	OnGunReloaded.Broadcast();
}

void AGun::Throw(const FVector& ThrowVector)
{
	if (!WorldMesh)
	{
		return;
	}
	if (GunHolder && GunHolder->IsPlayerControlled())
	{
		Thrower = GunHolder;
		bThrown = true;
		SetIgnoreEnemiesCollision(false);
		WorldMesh->OnComponentHit.AddDynamic(this, &AGun::OnHitOther);
	}
	Drop(false);
	WorldMesh->SetPhysicsAngularVelocityInRadians(FVector::ZeroVector);
	WorldMesh->SetPhysicsLinearVelocity(ThrowVector);
}

void AGun::Drop(bool bStopAllMovement)
{
	if (GunHolder)
	{
		RemoveEffects(GunHolder->GetAbilitySystemComponent());
		GunHolder->SetEquippedGun(nullptr);
		GunHolder = nullptr;
	}
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	WorldMesh->SetSimulatePhysics(true);
	WorldMesh->WakeRigidBody();
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);
	SetFPSMode(false);
	if (bStopAllMovement)
	{
		WorldMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
		WorldMesh->SetPhysicsAngularVelocityInRadians(FVector::ZeroVector);
	}
}

bool AGun::MainAction()
{
	if (!MainActionAbility)
	{
		return false;
	}
	return GunAbilitySystemComponent->TryActivateAbilityByClass(MainActionAbility);
}

bool AGun::SecondaryAction()
{
	if (!SecondaryActionAbility)
	{
		return false;
	}
	return GunAbilitySystemComponent->TryActivateAbilityByClass(SecondaryActionAbility);
}

void AGun::SetIgnoreEnemiesCollision(bool bIgnoreCharacters)
{
	if (bIgnoreCharacters)
	{
		WorldMesh->SetCollisionResponseToChannel(ASTraceChannel::Enemy, ECR_Block);
	}
	else
	{
		WorldMesh->SetCollisionResponseToChannel(ASTraceChannel::Enemy, ECR_Block);
	}
}

void AGun::SetFPSMode(bool bFPSVisible)
{
	WorldMesh->SetVisibility(!bFPSVisible);
	FPSMesh->SetVisibility(bFPSVisible);
}

float AGun::GetBaseDamage() const
{
	return GunAttributeSet->GetDamage();
}

float AGun::GetBaseFireRate() const
{
	return GunAttributeSet->GetFireRate();
}

float AGun::GetBaseRecoilVertical() const
{
	return GunAttributeSet->GetRecoilVertical();
}

float AGun::GetBaseRecoilHorizontal() const
{
	return GunAttributeSet->GetRecoilHorizontal();
}

float AGun::GetBaseAmmo() const
{
	return GunAttributeSet->GetAmmo();
}

float AGun::GetBaseMaxAmmo() const
{
	return GunAttributeSet->GetMaxAmmo();
}

float AGun::GetBaseAccuracy() const
{
	return GunAttributeSet->GetAccuracy();
}

float AGun::GetBaseShotsPerRound() const
{
	return GunAttributeSet->GetShotsPerRound();
}

float AGun::GetBaseMaxSpread() const
{
	return GunAttributeSet->GetMaxSpread();
}

void AGun::OnHitOther(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor == Thrower)
		return;

	AASCharacter* OtherCharacter = Cast<AASCharacter>(OtherActor);
	if (!OtherCharacter)
		return;

	if (GetVelocity().Size() < 100.f)
		return;

	if (ThrowHitEffect)
	{
		FGameplayEffectContextHandle EffectContext = GetGunAbilitySystemComponent()->MakeEffectContext();
		EffectContext.AddSourceObject(this);
		EffectContext.AddHitResult(Hit);
		FGameplayEventData EventData;
		EventData.EventTag = FASGameplayTags::Event::Hit;
		EventData.Instigator = Thrower;
		EventData.Target = OtherActor;
		EventData.ContextHandle = EffectContext;
		EventData.EventMagnitude = GetVelocity().Size() * .15f;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OtherCharacter, FASGameplayTags::Event::Hit, EventData);

		FGameplayEffectSpec Spec = FGameplayEffectSpec(ThrowHitEffect.GetDefaultObject(), EffectContext);
		const float Damage = FMath::Clamp(EventData.EventMagnitude, 0.f, 50.f);
		Spec.SetSetByCallerMagnitude(FASGameplayTags::Data::Damage, Damage);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Damage: ") + FString::SanitizeFloat(Damage));
		OtherCharacter->Stun(EventData.EventMagnitude * .5f);
		GetGunAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(Spec, OtherCharacter->GetAbilitySystemComponent());

		WorldMesh->OnComponentHit.RemoveDynamic(this, &AGun::OnHitOther);
	}
}

void AGun::Interact_Implementation(AASCharacter* Interactor)
{
	if (!Interactor)
	{
		return;
	}
	Interactor->TryActivateAbilityByTag(FASGameplayTags::Ability::EquipGun);
}

void AGun::SetHighlighted_Implementation(float HighlightValue)
{
	WorldMesh->SetCustomPrimitiveDataFloat(0, HighlightValue);
}

bool AGun::IsInteractable_Implementation() const
{
	if (!GunHolder && !bThrown)
		return true;
	return false;
}

void AGun::TryClearRigidbody()
{
	if (WorldMesh->IsSimulatingPhysics() && !WorldMesh->RigidBodyIsAwake())
	{
		WorldMesh->SetSimulatePhysics(false);
		SetActorTickEnabled(false);
	}
	if (IsOnGround())
	{
		bThrown = false;
		Thrower = nullptr;
		WorldMesh->OnComponentHit.RemoveAll(this);
		SetIgnoreEnemiesCollision(false);
	}
}

void AGun::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (!WorldMesh)
		return;
	
	TryClearRigidbody();
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
	if (GunHolder->IsPlayerControlled())
		SetFPSMode(true);
	
	GetMeshComponent()->SetSimulatePhysics(false);
	GetMeshComponent()->SetPhysicsLinearVelocity(FVector::ZeroVector);
	GetMeshComponent()->SetPhysicsAngularVelocityInRadians(FVector::ZeroVector);
	GetMeshComponent()->PutRigidBodyToSleep();
	SetActorEnableCollision(false);
	ApplyEffects(NewHolder->GetAbilitySystemComponent());
	AttachToComponent(NewHolder->GetVisibleMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, GetSocketName());
	WorldMesh->OnComponentHit.RemoveAll(this);
}

UAnimMontage* AGun::GetFireMontage() const
{
	if (GetGunAttributes()->GetAmmo() < 0.f)
		return DryFireMontage;

	return FireMontage;
}

bool AGun::IsAmmoFull() const
{
	return GetGunAttributes()->GetAmmo() == GetGunAttributes()->GetMaxAmmo();
}

bool AGun::IsAmmoEmpty() const
{
	return GetGunAttributes()->GetAmmo() == 0.f;
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
	if (GetWorld()->LineTraceSingleByChannel(HitResult, GetActorLocation(), GetActorLocation()
		- FVector::UpVector * 25.f, ECC_Visibility, Params))
	{
		return true;
	}
	return false;
}
