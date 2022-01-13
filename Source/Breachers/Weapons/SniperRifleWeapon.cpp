#include "SniperRifleWeapon.h"
#include "Breachers/Characters/CharacterBase.h"
#include "Kismet/KismetMathLibrary.h"

ASniperRifleWeapon::ASniperRifleWeapon()
{
	ScopingSpeed = 6;
	ScopingCooldown = 0.5;
	bIsInScope = false;
	bIsScoping = false;
	bCanScope = true;
}

void ASniperRifleWeapon::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	ScopeTimeAlpha += DeltaSeconds * ScopingSpeed;
	
	if(bIsInScope) Client_ScopeHandle(OldTransform, NewTransform, ScopeTimeAlpha);
	else Client_ScopeHandle(OldTransform, NewTransform, ScopeTimeAlpha);
	Server_HandleTickDisabling(ScopeTimeAlpha);
}

void ASniperRifleWeapon::OnPrimaryFire()
{
	Server_ForceUnscope();
	Super::OnPrimaryFire();
}

void ASniperRifleWeapon::OnSecondaryFire()
{
	if(bIsFiring || bIsReloading || bIsEquipping || bIsScoping || !bCanScope) return;
	SetActorTickEnabled(true);
	ScopeTimeAlpha = 0;
	bIsScoping = true;
	bCanScope = false;
	
	if(bIsInScope)
	{
		OldTransform = ArmsFP_Transform_Scoped;
		NewTransform = WeaponInfo.ArmsTransformFP;
	}
	else
	{
		OldTransform = WeaponInfo.ArmsTransformFP;
		NewTransform = ArmsFP_Transform_Scoped;
	}
}

void ASniperRifleWeapon::OnReload()
{
	Server_ForceUnscope();
	Super::OnReload();
}

void ASniperRifleWeapon::Server_HandleTickDisabling_Implementation(float Alpha)
{
	if(Alpha >= 1)
	{
		SetActorTickEnabled(false);
		bIsScoping = false;
		bIsInScope = !bIsInScope;
		GetWorldTimerManager().SetTimer(ScopeTimer, this, &ASniperRifleWeapon::ResetCanScope, 1, false, ScopingCooldown);
	}
}

void ASniperRifleWeapon::Client_ScopeHandle_Implementation(FTransform Transform1, FTransform Transform2, float Alpha)
{
	if(!CharacterPlayer) return;
	const FTransform CurrentTransform = UKismetMathLibrary::TLerp(Transform1, Transform2, Alpha);
	CharacterPlayer->GetArmsMeshFP()->SetRelativeTransform(CurrentTransform);
}


void ASniperRifleWeapon::ResetCanScope()
{
	bCanScope = true;
}

void ASniperRifleWeapon::Server_ForceUnscope_Implementation()
{
	if(bIsFiring || bIsReloading || bIsEquipping || bIsScoping || !bCanScope) return;
	SetActorTickEnabled(true);
	ScopeTimeAlpha = 0;
	bIsScoping = true;
	bCanScope = false;
	
	bIsInScope = true;
	OldTransform = ArmsFP_Transform_Scoped;
	NewTransform = WeaponInfo.ArmsTransformFP;
}
