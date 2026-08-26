// Fill out your copyright notice in the Description page of Project Settings.


#include "AEZProjectile.h"
#include "EchoZone/Weapon/DataAsset/UEZAmmoDataAsset.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

// Sets default values
AEZProjectile::AEZProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComponent->InitSphereRadius(6.0f);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionComponent->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Block);
	RootComponent = CollisionComponent;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.0f;

	InitialLifeSpan = 5.0f;

}

// Called when the game starts or when spawned
void AEZProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	CollisionComponent->OnComponentHit.AddDynamic(this, &AEZProjectile::OnProjectileHit);
	
	if (AActor* OwnerActor = GetOwner())
	{
		CollisionComponent->IgnoreActorWhenMoving(OwnerActor, true);
	}
}

void AEZProjectile::InitProjectile(const UEZAmmoDataAsset* InAmmoData, const FVector& Direction)
{
	if (!InAmmoData || !ProjectileMovement)
	{
		return;
	}

	AmmoData = InAmmoData;

	ProjectileMovement->InitialSpeed = AmmoData->InitialSpeed;
	ProjectileMovement->MaxSpeed = AmmoData->MaxSpeed;
	ProjectileMovement->ProjectileGravityScale = AmmoData->GravityScale;
	ProjectileMovement->Velocity = Direction.GetSafeNormal() * AmmoData->InitialSpeed;

	SetActorRotation(Direction.Rotation());
	SetLifeSpan(AmmoData->LifeSeconds);
}

void AEZProjectile::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor || OtherActor == this || OtherActor == GetOwner() || !AmmoData)
	{
		return;
	}

	UGameplayStatics::ApplyPointDamage(OtherActor, AmmoData->Damage, GetActorForwardVector(), Hit, GetInstigatorController(), this, nullptr);

	if (bDrawDebugImpact)
	{
		DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 8.0f, 8, FColor::Red, false, 1.5f);
	}

	if (bDestroyOnHit)
	{
		Destroy();
	}
}


