// Fill out your copyright notice in the Description page of Project Settings.


#include "AEZDoorActor.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AEZDoorActor::AEZDoorActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	DoorFrame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorFrame"));
	DoorFrame->SetupAttachment(SceneRoot);

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorMesh->SetupAttachment(SceneRoot);
}

// Called when the game starts or when spawned
void AEZDoorActor::BeginPlay()
{
	Super::BeginPlay();
	
	ClosedYaw = DoorMesh->GetRelativeRotation().Yaw;
	TargetYaw = ClosedYaw;
}

// Called every frame
void AEZDoorActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateDoorRotation(DeltaTime);
}

void AEZDoorActor::UpdateDoorRotation(float DeltaTime)
{
	FRotator CurrentRotation = DoorMesh->GetRelativeRotation();
	const float NewYaw = FMath::FInterpTo(CurrentRotation.Yaw, TargetYaw, DeltaTime, InterpSpeed);
	CurrentRotation.Yaw = NewYaw;
	DoorMesh->SetRelativeRotation(CurrentRotation);
}

void AEZDoorActor::SetOpen(bool bOpen)
{
	if (bLocked)
	{
		return;
	}

	bIsOpen = bOpen;
	TargetYaw = bIsOpen ? ClosedYaw + OpenAngle : ClosedYaw;
}

void AEZDoorActor::Interact_Implementation(AActor* Interactor)
{
	if (bLocked)
	{
		return;
	}

	bIsOpen = !bIsOpen;
	TargetYaw = bIsOpen ? ClosedYaw + OpenAngle : ClosedYaw;
}

bool AEZDoorActor::CanInteract_Implementation(AActor* Interactor) const
{
	return true;
}

FText AEZDoorActor::GetInteractText_Implementation() const
{
	if (bLocked)
	{
		return FText::FromString(TEXT("Locked"));
	}

	return bIsOpen ? FText::FromString(TEXT("CloseDoor")) : FText::FromString(TEXT("OpenDoor"));
}
