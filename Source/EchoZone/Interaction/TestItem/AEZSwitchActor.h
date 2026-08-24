// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EchoZone/Interaction/UEZInteractableInterface.h"
#include "AEZSwitchActor.generated.h"

class UStaticMeshComponent;
class AEZDoorActor;

UCLASS()
class ECHOZONE_API AEZSwitchActor : public AActor, public IEZInteractableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEZSwitchActor();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Switch")
	AEZDoorActor* LinkedDoor = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Switch")
	bool bIsOn = false;

public:	
	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual bool CanInteract_Implementation(AActor* Interactor) const override;
	virtual FText GetInteractText_Implementation() const override;
};
