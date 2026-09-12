// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AEZProjectile.generated.h"

class UStaticMeshComponent;
class USphereComponent;
class UEZAmmoDataAsset;

UCLASS()
class ECHOZONE_API AEZProjectile : public AActor
{
	GENERATED_BODY()
	
public:
	AEZProjectile();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	FVector CurrentVelocity = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	float GravityScale = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	float DragCoefficient = 0.05f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	float Damage = 35.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	float Penetration = 20.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	float MaxLifetime = 6.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	float CurrentLifetime = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Debug")
	bool bDrawDebugTrajectory = false;

public:
	void InitProjectile(const UEZAmmoDataAsset* AmmoData, const FVector& ShotDirection);

protected:
	void UpdateBallistics(float DeltaSeconds);
	void MoveWithSweep(float DeltaSeconds);
	void ProcessHit(const FHitResult& Hit, const FVector& TraceStart, const FVector& TraceEnd);

	float GetSpeed() const;
};
