#include "AEZProjectile.h"
#include "EchoZone/Weapon/DataAsset/UEZAmmoDataAsset.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

AEZProjectile::AEZProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	RootComponent = CollisionComponent;
	CollisionComponent->InitSphereRadius(2.0f);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Block);

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMesh->SetupAttachment(CollisionComponent);
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEZProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void AEZProjectile::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CurrentLifetime += DeltaSeconds;
	if (CurrentLifetime >= MaxLifetime)
	{
		Destroy();
		return;
	}

	UpdateBallistics(DeltaSeconds);
	MoveWithSweep(DeltaSeconds);
}

void AEZProjectile::InitProjectile(const UEZAmmoDataAsset* AmmoData, const FVector& ShotDirection)
{
	if (!AmmoData)
	{
		Destroy();
		return;
	}

	const FVector Direction = ShotDirection.GetSafeNormal();

	CurrentVelocity = Direction * AmmoData->MuzzleVelocity;
	GravityScale = AmmoData->GravityScale;
	DragCoefficient = AmmoData->DragCoefficient;
	Damage = AmmoData->Damage;
	Penetration = AmmoData->Penetration;
	MaxLifetime = AmmoData->MaxLifetime;

	SetActorRotation(Direction.Rotation());
}

void AEZProjectile::UpdateBallistics(float DeltaSeconds)
{
	const float GravityZ = GetWorld() ? GetWorld()->GetGravityZ() : -980.0f;

	CurrentVelocity += FVector(0.0f, 0.0f, GravityZ * GravityScale) * DeltaSeconds;

	const float Speed = CurrentVelocity.Size();
	if (Speed > KINDA_SMALL_NUMBER)
	{
		const FVector DragForce = -CurrentVelocity.GetSafeNormal() * Speed * DragCoefficient;
		CurrentVelocity += DragForce * DeltaSeconds;
	}
}

void AEZProjectile::MoveWithSweep(float DeltaSeconds)
{
	const FVector TraceStart = GetActorLocation();
	const FVector DeltaMove = CurrentVelocity * DeltaSeconds;
	const FVector TraceEnd = TraceStart + DeltaMove;

	FHitResult Hit;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(GetOwner());
	QueryParams.AddIgnoredActor(GetInstigator());

	const bool bHit = GetWorld()->SweepSingleByChannel(
		Hit,
		TraceStart,
		TraceEnd,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeSphere(CollisionComponent->GetScaledSphereRadius()),
		QueryParams
	);

	if (bDrawDebugTrajectory)
	{
		DrawDebugLine(GetWorld(), TraceStart, TraceEnd, bHit ? FColor::Red : FColor::Green, false, 2.0f, 0, 1.0f);
	}

	if (bHit)
	{
		SetActorLocation(Hit.ImpactPoint);
		ProcessHit(Hit, TraceStart, TraceEnd);
		return;
	}

	SetActorLocation(TraceEnd);

	if (!CurrentVelocity.IsNearlyZero())
	{
		SetActorRotation(CurrentVelocity.GetSafeNormal().Rotation());
	}
}

void AEZProjectile::ProcessHit(const FHitResult& Hit, const FVector& TraceStart, const FVector& TraceEnd)
{
	AActor* HitActor = Hit.GetActor();
	if (HitActor)
	{
		UGameplayStatics::ApplyPointDamage(
			HitActor,
			Damage,
			CurrentVelocity.GetSafeNormal(),
			Hit,
			GetInstigatorController(),
			this,
			nullptr
		);
	}

	if (bDrawDebugTrajectory)
	{
		DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 6.0f, 8, FColor::Yellow, false, 3.0f);
	}

	Destroy();
}

float AEZProjectile::GetSpeed() const
{
	return CurrentVelocity.Size();
}