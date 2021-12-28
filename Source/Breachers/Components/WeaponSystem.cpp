#include "WeaponSystem.h"

UWeaponSystem::UWeaponSystem()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UWeaponSystem::BeginPlay()
{
	Super::BeginPlay();
}

bool UWeaponSystem::HasPrimaryWeapon() const
{
	return PrimaryWeapon != nullptr;
}

bool UWeaponSystem::HasSecondaryWeapon() const
{
	return SecondaryWeapon != nullptr;
}

