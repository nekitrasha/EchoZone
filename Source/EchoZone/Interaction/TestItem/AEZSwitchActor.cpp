// Fill out your copyright notice in the Description page of Project Settings.


#include "AEZSwitchActor.h"
#include "Components/StaticMeshComponent.h"
#include "AEZDoorActor.h"

// Sets default values
AEZSwitchActor::AEZSwitchActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;
}

void AEZSwitchActor::Interact_Implementation(AActor* Interactor)
{
	bIsOn = !bIsOn;

	if (LinkedDoor)
	{
		LinkedDoor->SetOpen(bIsOn);
	}
}

bool AEZSwitchActor::CanInteract_Implementation(AActor* Interactor) const
{
	return true;
}

FText AEZSwitchActor::GetInteractText_Implementation() const
{
	return bIsOn ? FText::FromString(TEXT("Turn off")) : FText::FromString(TEXT("Turn on"));
}

