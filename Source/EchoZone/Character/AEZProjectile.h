// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AEZProjectile.generated.h"

class UStaticMeshComponent;
class UProjectileMovementComponent;
class USphereComponent;

UCLASS()
class ECHOZONE_API AEZProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEZProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditDefaultOnly, BlueprintReadOnly, Category = "Projectile")
	float Damage = 25.0f;

	UPROPERTY(EditDefaultOnly, BlueprintReadOnly, Category = "Projectile")
	float LifeSeconds = 5.0f;

	UPROPERTY(EditDefaultOnly, BlueprintReadOnly, Category = "Projectile")
	bool bDestroyOnHit = true;

	UFUNCTION()
	void OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

public:	
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void SetDamage(float NewDamage);

	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void SetVelocityDirection(const FVector& Direction);

};
