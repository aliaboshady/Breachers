#include "ShotgunWeapon.h"

void AShotgunWeapon::FireSpread()
{
	CurrentAmmoInClip += WeaponInfo.ShotInfo.BulletsPerSpread - 1;
	Super::FireSpread();
}

FVector AShotgunWeapon::RecoilShot(float Spread)
{
	FVector NewLocation = FVector(0);
	NewLocation.X += FMath::RandRange(-Spread, Spread);
	NewLocation.Y += FMath::RandRange(-Spread, Spread);
	NewLocation.Z += FMath::RandRange(-Spread, Spread);
	return NewLocation / WeaponInfo.RecoilInfo.RecoilFactor;
}

void AShotgunWeapon::Client_Recoil(FVector CalculatedRecoil)
{
	
}
