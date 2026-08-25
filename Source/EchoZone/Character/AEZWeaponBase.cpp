// Fill out your copyright notice in the Description page of Project Settings.


#include "AEZWeaponBase.h"
#include "AEZProjectile.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/World.h"

// Sets default values
AEZWeaponBase::AEZWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootComponent = RootScene;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(RootScene);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	MuzzlePiont = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzlePont"));
	MuzzlePiont->SetupAttachment(WeaponMesh);
	MuzzlePiont->SetRelativeLocation(FVector(50.0f, 0.0f, 0.0f));

	AmmoInMagazine = MagazineSize;

}

// Called when the game starts or when spawned
void AEZWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	AmmoInMagazine = MagazineSize;
	
}

bool AEZWeaponBase::CanFire() const
{
	return bCanFire && !bIsReloading && AmmoInMagazine > 0 && ProjectileClass != nullptr;
}

bool AEZWeaponBase::CanReload() const
{
	return !bIsReloading && AmmoInMagazine < MagazineSize;
}

void AEZWeaponBase::StartFire()
{
	if (!CanFire())
	{
		return;
	}

	FireShot();

	bCanFire = false;
	const float TimeBetweenShots = 60.0f / FMath::Max(FireRate, 1.0f);
	GetWorldTimerManager().SetTimer(FireCooldownTimerHandle, this, &AEZWeaponBase::ResetFire, TimeBetweenShots, false);
}

void AEZWeaponBase::FireShot()
{
	if (!ProjectileClass || AmmoInMagazine <= 0 || !GetWorld())
	{
		return;
	}

	AmmoInMagazine--;

	const FVector SpawnLocation = MuzzlePiont->GetComponentLocation();
	const FRotator SpawnRotation = MuzzlePiont->GetComponentRotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	SpawnParams.Instigator = Cast<APawn>(GetOwner());
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AEZProjectile* Projectile = GetWorld()->SpawnActor<AEZProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);

	if (Projectile)
	{
		Projectile->SetDamage(Damage);
		Projectile->SetVelocityDirection(MuzzlePiont->GetForwardVector());
	}
}

void AEZWeaponBase::Reload()
{
	if (!CanReload())
	{
		return;
	}

	bIsReloading = true;
	GetWorldTimerManager().SetTimer(ReloadTimerHandle, this, &AEZWeaponBase::FinishReload, ReloadTime, false);
}

void AEZWeaponBase::FinishReload()
{
	AmmoInMagazine = MagazineSize;
	bIsReloading = false;
}

void AEZWeaponBase::ResetFire()
{
	bCanFire = true;
}