// Fill out your copyright notice in the Description page of Project Settings.


#include "AEZWeaponBase.h"
#include "AEZProjectile.h"
#include "EchoZone/Weapon/DataAsset/UEZAmmoDataAsset.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

// Sets default values
AEZWeaponBase::AEZWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootComponent = RootScene;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(RootScene);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	MuzzlePiont = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzlePont"));
	MuzzlePiont->SetupAttachment(WeaponMesh);
	MuzzlePiont->SetRelativeLocation(FVector(50.0f, 0.0f, 0.0f));

	AmmoInMagazine = MagazineSize;
	CurrentSpreadAngle = BaseSpreadAngle;
	CurrentRecoilMultipler = 1.0f;
}

// Called when the game starts or when spawned
void AEZWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	AmmoInMagazine = MagazineSize;
	CurrentSpreadAngle = BaseSpreadAngle;
	CurrentRecoilMultipler = 1.0f;
}

void AEZWeaponBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	const float TargetSpread = GerEffectiveBaseSpread();
	CurrentSpreadAngle = FMath::FInterpTo(CurrentSpreadAngle, TargetSpread, DeltaSeconds, SpreadRecoverySpeed);

	CurrentRecoilMultipler = FMath::FInterpTo(CurrentRecoilMultipler, 1.0f, DeltaSeconds, RecoilRecoverySpeed);
}

bool AEZWeaponBase::CanFire() const
{
	return !bIsReloading && ProjectileClass && AmmoData && AmmoInMagazine > 0 && (bCanFireWhileSprinting || IsOwnerSprinting());
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