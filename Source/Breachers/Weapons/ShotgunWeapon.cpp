#include "ShotgunWeapon.h"

void AShotgunWeapon::FireSpread()
{
	CurrentAmmoInClip += WeaponInfo.BulletsPerSpread - 1;
	Super::FireSpread();
}
