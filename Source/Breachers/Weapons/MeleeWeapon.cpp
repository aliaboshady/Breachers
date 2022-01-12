#include "MeleeWeapon.h"

AMeleeWeapon::AMeleeWeapon()
{
	PrimaryAttackTime = 0.5;
	SecondaryAttackTime = 1;
	PrimaryTimeBetweenHits = 1;
	SecondaryTimeBetweenHits = 2;
}

void AMeleeWeapon::OnPrimaryFire()
{
	WeaponInfo.FireAnimationTime = PrimaryAttackTime;
	WeaponInfo.TimeBetweenShots = PrimaryTimeBetweenHits;
	if(PrimaryAttackAnim_ArmsFP) WeaponInfo.WeaponAnimations.FireAnim_ArmsFP = PrimaryAttackAnim_ArmsFP;
	if(PrimaryAttackAnim_ArmsTP) WeaponInfo.WeaponAnimations.FireAnim_ArmsTP = PrimaryAttackAnim_ArmsTP;
	CurrentAmmoInClip++;
	Super::OnPrimaryFire();
}

void AMeleeWeapon::OnSecondaryFire()
{
	if(!bCanFire) return;
	
	WeaponInfo.FireAnimationTime = SecondaryAttackTime;
	WeaponInfo.TimeBetweenShots = SecondaryTimeBetweenHits;
	if(SecondaryAttackAnim_ArmsFP) WeaponInfo.WeaponAnimations.FireAnim_ArmsFP = SecondaryAttackAnim_ArmsFP;
	if(SecondaryAttackAnim_ArmsTP) WeaponInfo.WeaponAnimations.FireAnim_ArmsTP = SecondaryAttackAnim_ArmsTP;

	bCanFire = false;
	CurrentAmmoInClip++;
	Client_OnFire();
	Super::OnSecondaryFire();
}
