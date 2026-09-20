// Fill out your copyright notice in the Description page of Project Settings.


#include "AEZMedkitPickup.h"
#include "Components/StaticMeshComponent.h"
#include "EchoZone/Character/Health/Components/UEZHealthComponent.h"

// Sets default values
AEZMedkitPickup::AEZMedkitPickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;
}

void AEZMedkitPickup::Interact_Implementation(AActor* Interactor)
{
	/*if (!Interactor)
	{
		return;
	}

	EZHealthComponent* HealthComponent = Interactor->FindComponentByClass<UEZHealthComponent>();
	if (!HealthComponent || HealthComponent->IsFullHealth())
	{
		return;
	}

	//HealthComponent->Heal(HealthAmount);*/
	Destroy();
}

bool AEZMedkitPickup::CanInteract_Implementation(AActor* Interactor) const
{
	return false;
	/*if (!Interactor)
	{
		return false;
	}

	UEZHealthComponent* HealthComponent = Interactor->FindComponentByClass<UEZHealthComponent>();
	if (!HealthComponent)
	{
		return false;
	}

	//return !HealthComponent->IsFullHealth();*/
}

FText AEZMedkitPickup::GetInteractText_Implementation() const
{
	return FText::FromString(TEXT("Pick up Medkit"));
}


