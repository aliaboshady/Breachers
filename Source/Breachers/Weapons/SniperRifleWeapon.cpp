#include "SniperRifleWeapon.h"
#include "Breachers/Characters/CharacterBase.h"
#include "Blueprint/UserWidget.h"
#include "Breachers/Components/HealthSystem.h"
#include "Breachers/Core/BreachersPlayerController.h"
#include "Kismet/KismetMathLibrary.h"

ASniperRifleWeapon::ASniperRifleWeapon()
{
	ScopingTime = 1;
	ScopingCooldown = 0.5;
	bWantsToScope = false;
	bCanScope = true;
	bIsInScope = false;
	ScopeZoomMultiplier = 1.5;
	CurrentAddedRecoil = 0;
}

void ASniperRifleWeapon::BeginPlay()
{
	Super::BeginPlay();
	OldTransform = WeaponInfo.ArmsTransformFP;
	NewTransform = ArmsFP_Transform_Scoped;
	UnscopedIdlePose_ArmsTP = WeaponInfo.WeaponAnimations.IdlePose_ArmsTP;
	UnscopedBlendSpace_ArmsTP = WeaponInfo.WeaponAnimations.BlendSpace_ArmsTP;
	DefaultScopingZoom = WeaponInfo.ShotInfo.DefaultFOV;
}

void ASniperRifleWeapon::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	ScopeTimeAlpha += DeltaSeconds * (1 / ScopingTime);
	
	if(bWantsToScope) Client_ScopeHandle_FP(OldTransform, NewTransform, ScopeTimeAlpha);
	Server_HandleTickDisabling(ScopeTimeAlpha);
}

void ASniperRifleWeapon::OnDie()
{
	Server_ForceUnscope();
}

void ASniperRifleWeapon::OnPrimaryFire()
{
	if(bCanFire && CurrentAmmoInClip > 0)
	{
		if(bIsInScope) Client_ChangeAddedRecoil(0);
		else Client_ChangeAddedRecoil(AddedRecoilUnscoped);
		Server_ForceUnscope();
	}
	Super::OnPrimaryFire();
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

void ASniperRifleWeapon::OnUnquip()
{
	Super::OnUnquip();
	Server_ForceUnscope();
}


void ASniperRifleWeapon::OnTaken()
{
	Super::OnTaken();
	if(CharacterPlayer && CharacterPlayer->HealthSystem)
	{
		CharacterPlayer->HealthSystem->OnDie.AddDynamic(this, &ASniperRifleWeapon::OnDie);
	}
}

void ASniperRifleWeapon::OnDrop(ACharacterBase* DropperCharacter)
{
	Super::OnDrop(DropperCharacter);
	if(CharacterPlayer && CharacterPlayer->HealthSystem)
	{
		CharacterPlayer->HealthSystem->OnDie.RemoveDynamic(this, &ASniperRifleWeapon::OnDie);
	}
}

void ASniperRifleWeapon::ResetCanScope()
{
	bCanScope = true;
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

	if(bIsInScope)
	{
		Client_ShowScopeWidget(false);
		Multicast_ForceUnscope_TP();
	}
	else
	{
		Multicast_ScopeHandle_TP();
	}
	bIsInScope = ! bIsInScope;
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
		if(bIsInScope) Client_ShowScopeWidget(true);
	}
}

void ASniperRifleWeapon::Client_ScopeHandle_FP_Implementation(FTransform Transform1, FTransform Transform2, float Alpha)
{
	if(!CharacterPlayer) return;
	const FTransform CurrentTransform = UKismetMathLibrary::TLerp(Transform1, Transform2, Alpha);
	CharacterPlayer->GetArmsMeshFP()->SetRelativeTransform(CurrentTransform);
}

void ASniperRifleWeapon::Multicast_ScopeHandle_TP_Implementation()
{
	WeaponInfo.WeaponAnimations.IdlePose_ArmsTP = ScopedIdlePose_ArmsTP;
	WeaponInfo.WeaponAnimations.BlendSpace_ArmsTP = ScopedBlendSpace_ArmsTP;
}

void ASniperRifleWeapon::Server_ForceUnscope_Implementation()
{
	SetActorTickEnabled(false);
	ScopeTimeAlpha = 0;
	bWantsToScope = false;
	bCanScope = false;
	bIsInScope = false;
	OldTransform = WeaponInfo.ArmsTransformFP;
	NewTransform = ArmsFP_Transform_Scoped;
	Client_ForceUnscope_FP();
	Multicast_ForceUnscope_TP();
	GetWorldTimerManager().SetTimer(ScopeTimer, this, &ASniperRifleWeapon::ResetCanScope, 1, false, ScopingCooldown);
}

void ASniperRifleWeapon::Client_ForceUnscope_FP_Implementation()
{
	CharacterPlayer->GetArmsMeshFP()->SetRelativeTransform(OldTransform);
	Multicast_ForceUnscope_TP();
	Client_ShowScopeWidget(false);
}

void ASniperRifleWeapon::Multicast_ForceUnscope_TP_Implementation()
{
	WeaponInfo.WeaponAnimations.IdlePose_ArmsTP = UnscopedIdlePose_ArmsTP;
	WeaponInfo.WeaponAnimations.BlendSpace_ArmsTP = UnscopedBlendSpace_ArmsTP;
}

void ASniperRifleWeapon::Client_CreatScopeWidget_Implementation()
{
	if(!CharacterPlayer) return;
	if(ABreachersPlayerController* PC = Cast<ABreachersPlayerController>(CharacterPlayer->GetController()))
	{
		if(ScopeWidgetClass) ScopeWidget = CreateWidget(PC, ScopeWidgetClass);
	}
}

void ASniperRifleWeapon::Client_ShowScopeWidget_Implementation(bool bShowScope)
{
	if(!ScopeWidget) Client_CreatScopeWidget();
	if(!ScopeWidget || !CharacterPlayer) return;
	if(DefaultScopingZoom == 0) DefaultScopingZoom = 90;
	
	if(bShowScope)
	{
		ScopeWidget->AddToViewport();
		CharacterPlayer->SetCameraFOV(DefaultScopingZoom / ScopeZoomMultiplier);
	}
	else
	{
		CharacterPlayer->SetCameraFOV(DefaultScopingZoom);
		ScopeWidget->RemoveFromViewport();
	}
	
	Mesh_FP->SetHiddenInGame(bShowScope);
	CharacterPlayer->GetArmsMeshFP()->SetHiddenInGame(bShowScope);
}

void ASniperRifleWeapon::Client_Recoil(FVector MovieSceneBlends){}

FVector ASniperRifleWeapon::RecoilShot(float Spread)
{
	FVector RecoilVector = FVector(0);
	float PlayerSpeed = CharacterPlayer->GetVelocity().Size();
	float Range = CurrentAddedRecoil;
	
	if(PlayerSpeed > WeaponInfo.RecoilInfo.MaxAccurateSpeed)
	{
		Range += WeaponInfo.RecoilInfo.RecoilFactor;
	}
	
	RecoilVector.X += FMath::RandRange(-Range, Range);
	RecoilVector.Y += FMath::RandRange(-Range, Range);
	RecoilVector.Z += FMath::RandRange(-Range, Range);
	return RecoilVector;
}

void ASniperRifleWeapon::Client_ChangeAddedRecoil_Implementation(float AddedRecoil)
{
	CurrentAddedRecoil = AddedRecoil;
}