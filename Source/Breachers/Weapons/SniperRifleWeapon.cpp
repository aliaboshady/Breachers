#include "SniperRifleWeapon.h"

#include "Breachers/Characters/CharacterBase.h"

ASniperRifleWeapon::ASniperRifleWeapon()
{
	bIsScoped = false;
}

void ASniperRifleWeapon::OnPrimaryFire()
{
	Client_Unscope();
	Super::OnPrimaryFire();
}

void ASniperRifleWeapon::OnSecondaryFire()
{
	if(bIsFiring || bIsReloading || bIsEquipping) return;
	
	bIsScoped = !bIsScoped;
	if(CharacterPlayer)
	{
		if(bIsScoped) Client_Scope();
		else Client_Unscope();
	}
}

void ASniperRifleWeapon::OnReload()
{
	Client_Unscope();
	Super::OnReload();
}

void ASniperRifleWeapon::Client_Scope_Implementation()
{
	CharacterPlayer->GetArmsMeshFP()->SetRelativeTransform(ArmsFP_Location_Scoped);
}

void ASniperRifleWeapon::Client_Unscope_Implementation()
{
	CharacterPlayer->GetArmsMeshFP()->SetRelativeTransform(WeaponInfo.ArmsTransformFP);
}
