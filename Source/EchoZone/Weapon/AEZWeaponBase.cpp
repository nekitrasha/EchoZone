#include "AEZWeaponBase.h"
#include "AEZProjectile.h"
#include "DataAsset/UEZAmmoDataAsset.h"
#include "DataAsset/UEZWeaponDataAsset.h"
#include "DataAsset/UEZMagazineDataAsset.h"
#include "EchoZone/Character/Component/UEZCharacterMovementComponent.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"

AEZWeaponBase::AEZWeaponBase()
{
	PrimaryActorTick.bCanEverTick = true;

	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootComponent = RootScene;

	WeaponVisualRoot = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponVisualRoot"));
	WeaponVisualRoot->SetupAttachment(RootScene);

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(WeaponVisualRoot);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	MuzzlePoint = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzlePoint"));
	MuzzlePoint->SetupAttachment(WeaponMesh);
	MuzzlePoint->SetRelativeLocation(FVector(50.0f, 0.0f, 0.0f));
}

void AEZWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	InitializeFromData();
}

void AEZWeaponBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateSpread(DeltaSeconds);
	UpdateRecoil(DeltaSeconds);
	UpdateADS(DeltaSeconds);
}

EEZWeaponType AEZWeaponBase::GetWeaponType() const
{
	return WeaponData ? WeaponData->WeaponType : EEZWeaponType::None;
}

EEZAmmoCaliber AEZWeaponBase::GetWeaponCaliber() const
{
	return WeaponData ? WeaponData->Caliber : EEZAmmoCaliber::None;
}

EEZReloadType AEZWeaponBase::GetReloadType() const
{
	return WeaponData ? WeaponData->ReloadType : EEZReloadType::None;
}

EEZWeaponFeedType AEZWeaponBase::GetFeedType() const
{
	return WeaponData ? WeaponData->FeedType : EEZWeaponFeedType::None;
}

bool AEZWeaponBase::IsMagazineCompatible(const UEZMagazineDataAsset* MagazineData) const
{
	return WeaponData && MagazineData && WeaponData->Caliber == MagazineData->Caliber;
}

bool AEZWeaponBase::IsAmmoCompatible(const UEZAmmoDataAsset* AmmoData) const
{
	return WeaponData && AmmoData && WeaponData->Caliber == AmmoData->Caliber;
}

int32 AEZWeaponBase::GetCurrentMagazineAmmo() const
{
	return InsertedMagazine.IsValid() ? InsertedMagazine.CurrentAmmo : 0;
}

int32 AEZWeaponBase::GetAmmoReadyToFire() const
{
	return GetCurrentMagazineAmmo() + (bRoundChambered ? 1 : 0);
}