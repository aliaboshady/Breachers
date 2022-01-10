#include "MeleeWeapon.h"

AMeleeWeapon::AMeleeWeapon()
{
	PrimaryAttackTime = 0.5;
	SecondaryAttackTime = 1;
	PrimaryTimeBetweenHits = 1;
	SecondaryTimeBetweenHits = 2;
	WeaponInfo.TraceLength = 100;
}

void AMeleeWeapon::OnFire(bool bShouldDecreaseBullets)
{
	WeaponInfo.FireAnimationTime = PrimaryAttackTime;
	WeaponInfo.TimeBetweenShots = PrimaryTimeBetweenHits;
	if(PrimaryAttackAnim_ArmsFP) WeaponInfo.WeaponAnimations.FireAnim_ArmsFP = PrimaryAttackAnim_ArmsFP;
	if(PrimaryAttackAnim_ArmsTP) WeaponInfo.WeaponAnimations.FireAnim_ArmsTP = PrimaryAttackAnim_ArmsTP;
	Super::OnFire(false);
}

void AMeleeWeapon::OnSecondaryFire(bool bShouldDecreaseBullets)
{
	WeaponInfo.FireAnimationTime = SecondaryAttackTime;
	WeaponInfo.TimeBetweenShots = SecondaryTimeBetweenHits;
	if(SecondaryAttackAnim_ArmsFP) WeaponInfo.WeaponAnimations.FireAnim_ArmsFP = SecondaryAttackAnim_ArmsFP;
	if(SecondaryAttackAnim_ArmsTP) WeaponInfo.WeaponAnimations.FireAnim_ArmsTP = SecondaryAttackAnim_ArmsTP;
	Super::OnSecondaryFire(false);
}
