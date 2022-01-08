#include "WeaponSystem.h"
#include "Breachers/Characters/CharacterBase.h"
#include "Breachers/Weapons/WeaponBase.h"
#include "Net/UnrealNetwork.h"

UWeaponSystem::UWeaponSystem()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicated(true);
	bCanFire = true;
	bIsReloading = false;
}

void UWeaponSystem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UWeaponSystem, CurrentWeapon);
	DOREPLIFETIME(UWeaponSystem, PrimaryWeapon);
	DOREPLIFETIME(UWeaponSystem, SecondaryWeapon);
	DOREPLIFETIME(UWeaponSystem, MeleeWeapon);
	DOREPLIFETIME(UWeaponSystem, CharacterPlayer);
}

void UWeaponSystem::BeginPlay()
{
	Super::BeginPlay();
	CharacterPlayer = Cast<ACharacterBase>(GetOwner());

	if(CharacterPlayer->HasAuthority()) Server_SpawnWeapon(MeleeWeaponClass);
	
	FTimerHandle EquipKnifeHandle;
	GetWorld()->GetTimerManager().SetTimer(EquipKnifeHandle, this, &UWeaponSystem::EquipKnife, 1, false, 0.1);
}

void UWeaponSystem::SetPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if(PlayerInputComponent && CharacterPlayer)
	{
		PlayerInputComponent->BindAction("EquipPrimary", IE_Pressed, this, &UWeaponSystem::EquipPrimary);
		PlayerInputComponent->BindAction("EquipSecondary", IE_Pressed, this, &UWeaponSystem::EquipSecondary);
		PlayerInputComponent->BindAction("EquipMelee", IE_Pressed, this, &UWeaponSystem::EquipMelee);
	
		PlayerInputComponent->BindAction("PrimaryFire", IE_Pressed, this, &UWeaponSystem::Server_StartFire);
		PlayerInputComponent->BindAction("PrimaryFire", IE_Released, this, &UWeaponSystem::Server_StopFire);

		PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &UWeaponSystem::Server_Reload);
	}
}

void UWeaponSystem::Server_TakeWeapon_Implementation(AWeaponBase* Weapon)
{
	if(Weapon->WeaponInfo.WeaponType == Primary && PrimaryWeapon == nullptr)
	{
		PrimaryWeapon = Weapon;
		Multicast_HideWeapon(Weapon, true);
	}
	else if(Weapon->WeaponInfo.WeaponType == Secondary && SecondaryWeapon == nullptr)
	{
		SecondaryWeapon = Weapon;
		Multicast_HideWeapon(Weapon, true);
	}
	else if(Weapon->WeaponInfo.WeaponType == Melee && MeleeWeapon == nullptr)
	{
		MeleeWeapon = Weapon;
	}

	Weapon->SetOwner(CharacterPlayer);
	Weapon->SetInstigator(CharacterPlayer);
	EquipWeapon(Weapon);
}
void UWeaponSystem::TakeWeapon(AWeaponBase* Weapon)
{
	Server_TakeWeapon(Weapon);
}

void UWeaponSystem::EquipWeapon(AWeaponBase* Weapon)
{
	if(CurrentWeapon) UnequipWeapon(CurrentWeapon);
	CurrentWeapon = Weapon;
	Multicast_EquipWeaponVisualsTP(Weapon);
	Client_EquipWeaponVisualsFP(Weapon);
}

void UWeaponSystem::UnequipWeapon(AWeaponBase* Weapon)
{
	//Multicast_HideWeapon(Weapon, true);
}

void UWeaponSystem::EquipKnife()
{
	if(MeleeWeapon) EquipWeapon(MeleeWeapon);
}

bool UWeaponSystem::CanTakeWeapon(AWeaponBase* Weapon)
{
	bool bCanTakeWeaponP = Weapon->WeaponInfo.WeaponType == Primary && PrimaryWeapon == nullptr;
	bool bCanTakeWeaponS = Weapon->WeaponInfo.WeaponType == Secondary && SecondaryWeapon == nullptr;
	bool bCanTakeWeaponM = Weapon->WeaponInfo.WeaponType == Melee && MeleeWeapon == nullptr;
	return bCanTakeWeaponP || bCanTakeWeaponS || bCanTakeWeaponM;
}

void UWeaponSystem::EquipPrimary()
{
	if(!PrimaryWeapon || CurrentWeapon == PrimaryWeapon) return;

}

void UWeaponSystem::EquipSecondary()
{
	if(!SecondaryWeapon || CurrentWeapon == SecondaryWeapon) return;

}

void UWeaponSystem::EquipMelee()
{
	if(!MeleeWeapon || CurrentWeapon == MeleeWeapon) return;
	
}

void UWeaponSystem::Server_StartFire_Implementation()
{
	if(!CurrentWeapon || CurrentWeapon->GetCurrentAmmoInClip() <= 0 || bIsReloading) return;

	const TEnumAsByte<EFireMode> FireMode = CurrentWeapon->WeaponInfo.WeaponFireMode;
	if(FireMode == Auto)
	{
		GetWorld()->GetTimerManager().SetTimer(StartFireTimer, this, &UWeaponSystem::Fire, CurrentWeapon->WeaponInfo.TimeBetweenShots + 0.01, true, 0);
	}
	if(FireMode == Spread)
	{
		FireSpread();
	}
	else
	{
		Fire();
	}	
}

void UWeaponSystem::Server_StopFire_Implementation()
{
	GetWorld()->GetTimerManager().ClearTimer(StartFireTimer);
}

void UWeaponSystem::Fire()
{
	if(!bCanFire || CurrentWeapon->GetCurrentAmmoInClip() <= 0 || !CurrentWeapon) return;
	CurrentWeapon->OnFire();
	bCanFire = false;
	FTimerHandle ResetTimer;
	GetWorld()->GetTimerManager().SetTimer(ResetTimer, this, &UWeaponSystem::ResetCanFire, 1, false, CurrentWeapon->WeaponInfo.TimeBetweenShots);
}

void UWeaponSystem::FireSpread()
{
	if(!bCanFire || !CurrentWeapon) return;
	
	const int32 ShotsCount = CurrentWeapon->WeaponInfo.BulletsPerSpread;
	for (int32 i = 0; i < ShotsCount; i++)
	{
		CurrentWeapon->OnFire();
	}
	bCanFire = false;
	FTimerHandle ResetTimer;
	GetWorld()->GetTimerManager().SetTimer(ResetTimer, this, &UWeaponSystem::ResetCanFire, 1, false, CurrentWeapon->WeaponInfo.TimeBetweenShots);
}

void UWeaponSystem::ResetCanFire()
{
	bCanFire = true;
}

AWeaponBase* UWeaponSystem::GetCurrentWeapon()
{
	if(CurrentWeapon) return CurrentWeapon;
	return nullptr;
}

FAttachmentTransformRules UWeaponSystem::CreateAttachRules()
{
	FAttachmentTransformRules AttachRules = FAttachmentTransformRules( EAttachmentRule::KeepRelative, true );;
	AttachRules.LocationRule = EAttachmentRule::SnapToTarget;
	AttachRules.RotationRule = EAttachmentRule::SnapToTarget;
	AttachRules.ScaleRule = EAttachmentRule::SnapToTarget;
	return AttachRules;
}

void UWeaponSystem::Server_Reload_Implementation()
{
	if(!CurrentWeapon || CurrentWeapon->GetCurrentTotalAmmo() <= 0) return;

	const bool bCanReload = CurrentWeapon->GetCurrentAmmoInClip() < CurrentWeapon->WeaponInfo.MaxAmmoInClip;
	if(bCanReload && !bIsReloading)
	{
		CurrentWeapon->Reload();
		GetWorld()->GetTimerManager().SetTimer(ReloadTimer, this, &UWeaponSystem::Server_FinishReload, 1, false, CurrentWeapon->WeaponInfo.ReloadTime);
		bIsReloading = true;
	}
}

void UWeaponSystem::Server_FinishReload_Implementation()
{
	if(CurrentWeapon)
	{
		CurrentWeapon->FinishReload();
		bIsReloading = false;
	}
}

void UWeaponSystem::Server_CancelReload_Implementation()
{
	bIsReloading = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////// NETWORK /////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UWeaponSystem::Server_SpawnWeapon_Implementation(TSubclassOf<AWeaponBase> WeaponClass)
{
	GetWorld()->SpawnActor<AWeaponBase>(WeaponClass, CharacterPlayer->GetActorTransform(), FActorSpawnParameters());
}

void UWeaponSystem::Client_EquipWeaponVisualsFP_Implementation(AWeaponBase* Weapon)
{
	if(CharacterPlayer && Weapon)
	{
		USkeletalMeshComponent* ArmsFP = CharacterPlayer->GetArmsMeshFP();
		Weapon->Mesh_FP->AttachToComponent(ArmsFP, CreateAttachRules(), "WeaponSocket");
	}
}
void UWeaponSystem::Multicast_EquipWeaponVisualsTP_Implementation(AWeaponBase* Weapon)
{
	if(CharacterPlayer && Weapon)
	{
		USkeletalMeshComponent* ArmsTP = CharacterPlayer->GetMesh();
		Weapon->Mesh_TP->AttachToComponent(ArmsTP, CreateAttachRules(), "WeaponSocket");
		Weapon->Mesh_TP->CastShadow = true;
		Weapon->SetHidden(false);
	}
}
void UWeaponSystem::Multicast_HideWeapon_Implementation(AWeaponBase* Weapon, bool bHidden)
{
	if(!Weapon) return;
	Weapon->Mesh_TP->CastShadow = !bHidden;
	Weapon->SetHidden(bHidden);
}