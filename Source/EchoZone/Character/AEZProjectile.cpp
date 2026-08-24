// Fill out your copyright notice in the Description page of Project Settings.


#include "AEZProjectile.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEZProjectile::AEZProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComponent->InitSphereRadius(8.0f);
	CollisionComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	RootComponent = CollisionComponent;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 3000.0f;
	ProjectileMovement->MaxSpeed = 3000.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;

	InitialLifeSpan = 5.0f;

}

// Called when the game starts or when spawned
void AEZProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	CollisionComponent->OnComponentHit.AddDynamic(this, &AEZProjectile::OnProjectileHit);
	SetLifeSpan(LifeSeconds);
}

void AEZProjectile::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor || OtherActor == this || OtherActor == GetOwner())
	{
		return;
	}

	UGameplayStatics::ApplyPointDamage(OtherActor, Damage, GetActorForwardVector(), Hit, GetInstigatorController(), this, nullptr);

	if (bDestroyOnHit)
	{
		Destroy();
	}
}

void AEZProjectile::SetDamage(float NewDamage)
{
	Damage = NewDamage;
}

void AEZProjectile::SetVelocityDirection(const FVector& Direction)
{
	if (!ProjectileMovement)
	{
		return;
	}

	const FVector SafeDirection = Direction.GetSafeNormal();
	ProjectileMovement->Velocity = SafeDirection * ProjectileMovement->InitialSpeed;
}

