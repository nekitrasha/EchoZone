// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UEZInteractableInterface.h"
#include "AEZDoorActor.generated.h"

class UStaticMeshComponent;

UCLASS()
class ECHOZONE_API AEZDoorActor : public AActor,public IEZInteractableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEZDoorActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* DoorFrame;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* DoorMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Door")
	float OpenAngle = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Door")
	float InterpSpeed = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Door")
	bool bLocked = false;

	UPROPERTY(BlueprintReadOnly, Category = "Door")
	bool bIsOpen = false;

	float ClosedYaw = 0.0f;
	float TargetYaw = 0.0f;

public:
	UFUNCTION(BlueprintCallable, Category = "Door")
	void SetOpen(bool bOpen);

public:
	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual bool CanInteract_Implementation(AActor* Interactor) const override;
	virtual FText GetInteractText_Implementation() const override;

protected:
	void UpdateDoorRotation(float DeltaTime);
};
