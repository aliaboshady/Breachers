#include "ShotgunWeapon.h"

void AShotgunWeapon::FireSpread()
{
	CurrentAmmoInClip += WeaponInfo.ShotInfo.BulletsPerSpread - 1;
	Super::FireSpread();
}
