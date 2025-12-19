// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Gun.h"


AGun::AGun()
{
	PrimaryActorTick.bCanEverTick = false;
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
}


