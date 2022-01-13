#include "SniperRifleWeapon.h"
#include "Breachers/Characters/CharacterBase.h"
#include "Kismet/KismetMathLibrary.h"

ASniperRifleWeapon::ASniperRifleWeapon()
{
	ScopingSpeed = 6;
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
	ScopeTimeAlpha += DeltaSeconds * ScopingSpeed;
	
	if(bWantsToScope) Client_ScopeHandle(OldTransform, NewTransform, ScopeTimeAlpha);
	Server_HandleTickDisabling(ScopeTimeAlpha);
}

void ASniperRifleWeapon::OnPrimaryFire()
{
	Server_ForceUnscope();
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

void ASniperRifleWeapon::Client_ScopeHandle_Implementation(FTransform Transform1, FTransform Transform2, float Alpha)
{
	if(!CharacterPlayer) return;
	
	const FTransform CurrentTransform = UKismetMathLibrary::TLerp(Transform1, Transform2, Alpha);
	CharacterPlayer->GetArmsMeshFP()->SetRelativeTransform(CurrentTransform);
}

void ASniperRifleWeapon::ResetCanScope()
{
	bCanScope = true;
	bCanFire = true;
}

void ASniperRifleWeapon::Server_ForceUnscope_Implementation()
{
	SetActorTickEnabled(false);
	ScopeTimeAlpha = 0;
	bWantsToScope = false;
	bCanScope = false;
	OldTransform = WeaponInfo.ArmsTransformFP;
	NewTransform = ArmsFP_Transform_Scoped;
	Client_ForceUnscope();
	GetWorldTimerManager().SetTimer(ScopeTimer, this, &ASniperRifleWeapon::ResetCanScope, 1, false, ScopingCooldown);
}

void ASniperRifleWeapon::Client_ForceUnscope_Implementation()
{
	CharacterPlayer->GetArmsMeshFP()->SetRelativeTransform(OldTransform);
}
