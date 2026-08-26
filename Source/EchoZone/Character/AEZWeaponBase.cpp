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

	MuzzlePoint = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzlePont"));
	MuzzlePoint->SetupAttachment(WeaponMesh);
	MuzzlePoint->SetRelativeLocation(FVector(50.0f, 0.0f, 0.0f));

	AmmoInMagazine = MagazineSize;
	CurrentSpreadAngle = BaseSpreadAngle;
	CurrentRecoilMultiplier = 1.0f;
}

// Called when the game starts or when spawned
void AEZWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	AmmoInMagazine = MagazineSize;
	CurrentSpreadAngle = BaseSpreadAngle;
	CurrentRecoilMultiplier = 1.0f;
}

void AEZWeaponBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	const float TargetSpread = GetEffectiveBaseSpread();
	CurrentSpreadAngle = FMath::FInterpTo(CurrentSpreadAngle, TargetSpread, DeltaSeconds, SpreadRecoverySpeed);

	CurrentRecoilMultiplier = FMath::FInterpTo(CurrentRecoilMultiplier, 1.0f, DeltaSeconds, RecoilRecoverySpeed);
}

bool AEZWeaponBase::CanFire() const
{
	return !bIsReloading && ProjectileClass && AmmoData && AmmoInMagazine > 0 && (bCanFireWhileSprinting || !IsOwnerSprinting());
}

bool AEZWeaponBase::CanReload() const
{
	return !bIsReloading && AmmoInMagazine < MagazineSize && ReserveAmmo > 0;
}

void AEZWeaponBase::StartFire()
{
	bTriggerHeld = true;

	if (!CanFire())
	{
		return;
	}

	FireShot();

	if (FireMode == EEZFireMode::FullAuto)
	{
		const float TimeBetweenShot = 60.0f / FMath::Max(FireRate, 1.0f);

		GetWorldTimerManager().SetTimer(AutoFireTimerHandle, this, &AEZWeaponBase::HandleAutoFire, TimeBetweenShot, true, TimeBetweenShot);
	}	
}

void AEZWeaponBase::StopFire()
{
	bTriggerHeld = false;
	GetWorldTimerManager().ClearTimer(AutoFireTimerHandle);
}

void AEZWeaponBase::HandleAutoFire()
{
	if (!bTriggerHeld || !CanFire())
	{
		StopFire();
		return;
	}
	FireShot();
}

void AEZWeaponBase::FireShot()
{
	if (!CanFire() || !GetWorld() || !MuzzlePoint)
	{
		return;
	}

	AmmoInMagazine--;

	const FVector SpawnLocation = MuzzlePoint->GetComponentLocation();
	const FVector ShotDirection = GetShotDirection(SpawnLocation);
	const FRotator SpawnRotation = ShotDirection.Rotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	SpawnParams.Instigator = Cast<APawn>(GetOwner());
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AEZProjectile* Projectile = GetWorld()->SpawnActor<AEZProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);

	if (Projectile)
	{
		Projectile->InitProjectile(AmmoData, ShotDirection);
	}

	CurrentSpreadAngle = FMath::Clamp(CurrentSpreadAngle + SpreadPerShot, GetEffectiveBaseSpread(), MaxSpreadAngle);
	CurrentRecoilMultiplier = FMath::Clamp(CurrentRecoilMultiplier + RecoilKickPerShot, 1.0f, MaxRecoilMultiplier);
	ApplyRecoil();

	if (bDrawDebugShot)
	{
		const FVector AimPoint = GetCameraAimPoint();
		DrawDebugLine(GetWorld(), SpawnLocation, SpawnLocation + ShotDirection * 3000.0f, FColor::Green, false, 1.0f, 0, 1.5f);
		DrawDebugSphere(GetWorld(), AimPoint, 6.0f, 8, FColor::Red, false, 1.0f);
	}
}

void AEZWeaponBase::Reload()
{
	if (!CanReload())
	{
		return;
	}

	bIsReloading = true;
	StopFire();

	GetWorldTimerManager().SetTimer(ReloadTimerHandle, this, &AEZWeaponBase::FinishReload, ReloadTime, false);
}

void AEZWeaponBase::FinishReload()
{
	const int32 MissingAmmo = MagazineSize - AmmoInMagazine;
	const int32 AmmoToLoad = FMath::Min(MissingAmmo, ReserveAmmo);

	AmmoInMagazine += AmmoToLoad;
	ReserveAmmo -= AmmoToLoad;

	bIsReloading = false;
}

void AEZWeaponBase::StartAim()
{
	bIsAiming = true;
}

void AEZWeaponBase::StopAim()
{
	bIsAiming = false;
}

float AEZWeaponBase::GetAimAlpha() const
{
	const float Ergo = GetErgonomicsNormalized();
	return FMath::Lerp(0.75, 1.25, Ergo);
}

FVector AEZWeaponBase::GetAimDirection() const
{
	if (const APawn* OwnerPawn = Cast<APawn>(GetOwner()))
	{
		if (const AController* Controller = OwnerPawn->GetController())
		{
			FVector ViewLocation;
			FRotator ViewRotation;
			Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);
			return ViewRotation.Vector();
		}
	}

	return MuzzlePoint->GetForwardVector();
}

FVector AEZWeaponBase::GetShotDirection(const FVector& FromLocation) const
{
	const FVector AimPoint = GetCameraAimPoint();
	FVector Direction = (AimPoint - FromLocation).GetSafeNormal();

	const float HalfRad = FMath::DegreesToRadians(CurrentSpreadAngle);
	Direction = FMath::VRandCone(Direction, HalfRad, HalfRad).GetSafeNormal();

	return Direction;
}

FVector AEZWeaponBase::GetCameraAimPoint() const
{
	const APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn || !GetWorld())
	{
		return MuzzlePoint ? MuzzlePoint->GetComponentLocation() + MuzzlePoint->GetForwardVector() * 50000.0f : FVector::ZeroVector;
	}

	const AController* Controller = OwnerPawn->GetController();
	if (!Controller)
	{
		return MuzzlePoint ? MuzzlePoint->GetComponentLocation() + MuzzlePoint->GetForwardVector() * 50000.0f : FVector::ZeroVector;
	}

	FVector ViewLocation;
	FRotator ViewRotation;

	Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);

	const FVector TraceStart = ViewLocation;
	const FVector TraceEnd = TraceStart + ViewRotation.Vector() * 50000.0f;
	
	FHitResult Hit;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());
	QueryParams.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
	{
		return Hit.ImpactPoint;
	}
	return TraceEnd;
}

void AEZWeaponBase::ApplyRecoil()
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn)
	{
		return;
	}

	const float PitchRecoil = GetEffectiveVerticalRecoil() * CurrentRecoilMultiplier;
	const float YawRecoil = FMath::FRandRange(-GetEffectiveHorizontalRecoil(), GetEffectiveHorizontalRecoil()) * CurrentRecoilMultiplier;

	OwnerPawn->AddControllerPitchInput(-PitchRecoil);
	OwnerPawn->AddControllerYawInput(YawRecoil);
}

float AEZWeaponBase::GetErgonomicsNormalized() const
{
	return FMath::Clamp(Ergonomics / 100.0f, 0.0f, 1.0f);
}

float AEZWeaponBase::GetMovementSpreadMultiplier() const
{
	const APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn)
	{
		return 1.0f;
	}

	const ACharacter* OwnerCharacter = Cast<ACharacter>(OwnerPawn);
	if (!OwnerCharacter)
	{
		return 1.0f;
	}

	const UCharacterMovementComponent* MoveComp = OwnerCharacter->GetCharacterMovement();
	if (!MoveComp)
	{
		return 1.0f;
	}

	float Multiplier = 1.0f;

	if (MoveComp->IsFalling())
	{
		Multiplier *= InAirSpreadMultiplier;
	}

	if (OwnerCharacter->bIsCrouched)
	{
		Multiplier *= CrouchSpreadMultiplier;
	}

	const FVector HorizontalVelocity = FVector(OwnerCharacter->GetVelocity().X, OwnerCharacter->GetVelocity().Y, 0.0f);
	if (!HorizontalVelocity.IsNearlyZero())
	{
		Multiplier *= MovingSpreadMultiplier;
	}

	if (bIsAiming)
	{
		Multiplier *= ADS_SpreadMultiplier;
	}

	return Multiplier;
}

float AEZWeaponBase::GetEffectiveBaseSpread() const
{
	const float Ergo = GetErgonomicsNormalized();

	const float ErgoSpreadMultiplier = FMath::Lerp(1.25f, 0.8f, Ergo);
	return BaseSpreadAngle * ErgoSpreadMultiplier * GetMovementSpreadMultiplier();
}

float AEZWeaponBase::GetEffectiveVerticalRecoil() const
{
	const float Ergo = GetErgonomicsNormalized();
	return VerticalRecoil * FMath::Lerp(1.25, 0.75, Ergo);
}

float AEZWeaponBase::GetEffectiveHorizontalRecoil() const
{
	const float Ergo = GetErgonomicsNormalized();
	return HorizontalRecoil * FMath::Lerp(1.25f, 0.75, Ergo);
}

bool AEZWeaponBase::IsOwnerSprinting() const
{
	const APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn)
	{
		return false;
	}

	const ACharacter* OwnerCharacter = Cast<ACharacter>(OwnerPawn);
	if (!OwnerCharacter)
	{
		return false;
	}

	const UCharacterMovementComponent* MoveComp = OwnerCharacter->GetCharacterMovement();
	if (!MoveComp)
	{
		return false;
	}

	const float CurrentSpeed2D = OwnerCharacter->GetVelocity().Size2D();
	const float MaxWalkSpeed = MoveComp->MaxWalkSpeed;

	if (CurrentSpeed2D < 10.0f)
	{
		return false;
	}

	return CurrentSpeed2D > (MaxWalkSpeed * 0.9f);
}
