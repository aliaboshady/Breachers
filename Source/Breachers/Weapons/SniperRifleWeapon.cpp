#include "SniperRifleWeapon.h"
#include "Breachers/Characters/CharacterBase.h"
#include "Kismet/KismetMathLibrary.h"

ASniperRifleWeapon::ASniperRifleWeapon()
{
	ScopingTime = 1;
	ScopingCooldown = 0.5;
	bWantsToScope = false;
	bCanScope = true;
}

void ASniperRifleWeapon::BeginPlay()
{
	Super::BeginPlay();
	OldTransform = WeaponInfo.ArmsTransformFP;
	NewTransform = ArmsFP_Transform_Scoped;
}

void ASniperRifleWeapon::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	ScopeTimeAlpha += DeltaSeconds * (1 / ScopingTime);
	
	if(bWantsToScope) Client_ScopeHandle_FP(OldTransform, NewTransform, ScopeTimeAlpha);
	Server_HandleTickDisabling(ScopeTimeAlpha);
}

void ASniperRifleWeapon::OnPrimaryFire()
{
	if(bCanFire && CurrentAmmoInClip > 0) Server_ForceUnscope();
	Super::OnPrimaryFire();
}

void ASniperRifleWeapon::OnSecondaryFire()
{
	if(bIsFiring || bIsReloading || bIsEquipping || !bCanScope) return;
	SetActorTickEnabled(true);
	ScopeTimeAlpha = 0;
	bWantsToScope = true;
	bCanScope = false;
	bCanFire = false;
	GetWorldTimerManager().SetTimer(StartFireTimer, this, &ASniperRifleWeapon::ResetCanFire, 1, false, ScopingTime);
}

void ASniperRifleWeapon::OnReload()
{
	Server_ForceUnscope();
	Super::OnReload();
}

void ASniperRifleWeapon::OnEquip()
{
	Server_ForceUnscope();
	Super::OnEquip();
}

void ASniperRifleWeapon::Server_HandleTickDisabling_Implementation(float Alpha)
{
	if(Alpha >= 1)
	{
		SetActorTickEnabled(false);
		bWantsToScope = false;
		const FTransform TempTransform = OldTransform;
		OldTransform = NewTransform;
		NewTransform = TempTransform;
		GetWorldTimerManager().SetTimer(ScopeTimer, this, &ASniperRifleWeapon::ResetCanScope, 1, false, ScopingCooldown);
	}
}

void ASniperRifleWeapon::Client_ScopeHandle_FP_Implementation(FTransform Transform1, FTransform Transform2, float Alpha)
{
	if(!CharacterPlayer) return;
	
	const FTransform CurrentTransform = UKismetMathLibrary::TLerp(Transform1, Transform2, Alpha);
	CharacterPlayer->GetArmsMeshFP()->SetRelativeTransform(CurrentTransform);
}

void ASniperRifleWeapon::ResetCanScope()
{
	bCanScope = true;
}

void ASniperRifleWeapon::Multicast_ForceUnscope_TP_Implementation()
{
}

void ASniperRifleWeapon::Multicast_ScopeHandle_TP_Implementation(FTransform Transform1, FTransform Transform2,
	float Alpha)
{
}

void ASniperRifleWeapon::Server_ForceUnscope_Implementation()
{
	SetActorTickEnabled(false);
	ScopeTimeAlpha = 0;
	bWantsToScope = false;
	bCanScope = false;
	OldTransform = WeaponInfo.ArmsTransformFP;
	NewTransform = ArmsFP_Transform_Scoped;
	Client_ForceUnscope_FP();
	GetWorldTimerManager().SetTimer(ScopeTimer, this, &ASniperRifleWeapon::ResetCanScope, 1, false, ScopingCooldown);
}

void ASniperRifleWeapon::Client_ForceUnscope_FP_Implementation()
{
	CharacterPlayer->GetArmsMeshFP()->SetRelativeTransform(OldTransform);
}