// Fill out your copyright notice in the Description page of Project Settings.


#include "AEZTestInteractActor.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"

// Sets default values
AEZTestInteractActor::AEZTestInteractActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;
}

void AEZTestInteractActor::Interact_Implementation(AActor* Interactor)
{
	bWasUsed = !bWasUsed;

	if (GEngine)
	{
		const FString Msg = FString::Printf(TEXT("Interacted with %s bu %s. Used state: %s"), *GetName(), *GetNameSafe(Interactor), bWasUsed ? TEXT("True") : TEXT("False"));
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, Msg);
	}
}

bool AEZTestInteractActor::CanInteract_Implementation(AActor* Interactor) const
{
	return bCanCurrentlyInteact;
}

FText AEZTestInteractActor::GetInteractText_Implementation() const
{
	return InteractText;
}


