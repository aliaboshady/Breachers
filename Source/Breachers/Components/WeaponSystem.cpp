#include "WeaponSystem.h"
#include "Breachers/Characters/CharacterBase.h"
#include "Breachers/Weapons/WeaponBase.h"
#include "Net/UnrealNetwork.h"

UWeaponSystem::UWeaponSystem()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicated(true);
	WeaponThrowForce = 40000;
	bShootingEnabled = true;
}

void UWeaponSystem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UWeaponSystem, LastTakenWeapon);
	DOREPLIFETIME(UWeaponSystem, PreviousWeapon);
	DOREPLIFETIME(UWeaponSystem, CurrentWeapon);
	DOREPLIFETIME(UWeaponSystem, PrimaryWeapon);
	DOREPLIFETIME(UWeaponSystem, SecondaryWeapon);
	DOREPLIFETIME(UWeaponSystem, MeleeWeapon);
	DOREPLIFETIME(UWeaponSystem, CharacterPlayer);
	DOREPLIFETIME(UWeaponSystem, bShootingEnabled);
}

void UWeaponSystem::BeginPlay()
{
	Super::BeginPlay();
	CharacterPlayer = Cast<ACharacterBase>(GetOwner());

	if(CharacterPlayer->HasAuthority()) Server_SpawnStartWeapons();
}

void UWeaponSystem::SetPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if(PlayerInputComponent && CharacterPlayer)
	{
		PlayerInputComponent->BindAction("EquipPrimary", IE_Pressed, this, &UWeaponSystem::Server_EquipPrimary);
		PlayerInputComponent->BindAction("EquipSecondary", IE_Pressed, this, &UWeaponSystem::Server_EquipSecondary);
		PlayerInputComponent->BindAction("EquipMelee", IE_Pressed, this, &UWeaponSystem::Server_EquipMelee);
	
		PlayerInputComponent->BindAction("PrimaryFire", IE_Pressed, this, &UWeaponSystem::Server_StartPrimaryFire);
		PlayerInputComponent->BindAction("PrimaryFire", IE_Released, this, &UWeaponSystem::Server_StopPrimaryFire);

		PlayerInputComponent->BindAction("SecondaryFire", IE_Pressed, this, &UWeaponSystem::Server_SecondaryFire);
		PlayerInputComponent->BindAction("PreviousWeapon", IE_Pressed, this, &UWeaponSystem::Server_EquipPreviousWeapon);
		PlayerInputComponent->BindAction("DropWeapon", IE_Pressed, this, &UWeaponSystem::Server_DropWeapon);

		PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &UWeaponSystem::Server_Reload);
	}
}

void UWeaponSystem::Server_SpawnStartWeapons_Implementation()
{
	if(MeleeWeaponClass) GetWorld()->SpawnActor<AWeaponBase>(MeleeWeaponClass, CharacterPlayer->GetActorTransform(), FActorSpawnParameters());
	if(PistolWeaponClass) GetWorld()->SpawnActor<AWeaponBase>(PistolWeaponClass, CharacterPlayer->GetActorTransform(), FActorSpawnParameters());

	FTimerHandle SpawnedWeaponHandle;
	GetWorld()->GetTimerManager().SetTimer(SpawnedWeaponHandle, this, &UWeaponSystem::EquipStartUpWeapons, 1, false, 0.05);
}

void UWeaponSystem::SpawnWeapon(TSubclassOf<AWeaponBase> WeaponClass)
{
	Server_SpawnWeapon(WeaponClass);
}

void UWeaponSystem::Server_SpawnWeapon_Implementation(TSubclassOf<AWeaponBase> WeaponClass)
{
	if(WeaponClass)
	{
		AWeaponBase* SpawnedWeapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass, CharacterPlayer->GetActorTransform(), FActorSpawnParameters());
		FTimerHandle SpawnedWeaponHandle;

		if(!LastTakenWeapon || !PrimaryWeapon || !SecondaryWeapon) return;

		EWeaponType WeaponType = SpawnedWeapon->WeaponInfo.WeaponType;
		if(WeaponType == Primary && LastTakenWeapon == PrimaryWeapon)
		{
			GetWorld()->GetTimerManager().SetTimer(SpawnedWeaponHandle, this, &UWeaponSystem::Server_EquipPrimary, 1, false, 0.05);
		}
		else if(WeaponType == Secondary && LastTakenWeapon == SecondaryWeapon)
		{
			GetWorld()->GetTimerManager().SetTimer(SpawnedWeaponHandle, this, &UWeaponSystem::Server_EquipSecondary, 1, false, 0.05);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////// Equip/Take Weapons ///////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UWeaponSystem::Server_TakeWeapon_Implementation(AWeaponBase* Weapon)
{
	if(Weapon->WeaponInfo.WeaponType == Primary && PrimaryWeapon == nullptr)
	{
		PrimaryWeapon = Weapon;
	}
	else if(Weapon->WeaponInfo.WeaponType == Secondary && SecondaryWeapon == nullptr)
	{
		SecondaryWeapon = Weapon;
	}
	else if(Weapon->WeaponInfo.WeaponType == Melee && MeleeWeapon == nullptr)
	{
		MeleeWeapon = Weapon;
	}

	LastTakenWeapon = Weapon;
	Multicast_HideWeapon(Weapon, true);
	Weapon->SetOwner(CharacterPlayer);
	Weapon->SetInstigator(CharacterPlayer);
}
void UWeaponSystem::TakeWeapon(AWeaponBase* Weapon)
{
	Server_TakeWeapon(Weapon);
}

void UWeaponSystem::Server_DropWeapon_Implementation()
{
	if(!CurrentWeapon || !CurrentWeapon->WeaponInfo.bIsDroppable || !CharacterPlayer) return;
	
	if(CurrentWeapon->WeaponInfo.WeaponType == Primary)
	{
		PrimaryWeapon = nullptr;
	}
	else if(CurrentWeapon->WeaponInfo.WeaponType == Secondary)
	{
		SecondaryWeapon = nullptr;
	}

	Multicast_DropWeaponVisualsTP(CurrentWeapon);
	CurrentWeapon->OnDrop(CharacterPlayer);
	CurrentWeapon->SetOwner(nullptr);
	CurrentWeapon->SetInstigator(nullptr);
	CurrentWeapon = nullptr;

	if(PreviousWeapon) EquipWeapon(PreviousWeapon);
	else if(MeleeWeapon) EquipWeapon(MeleeWeapon);
	PreviousWeapon = nullptr;
}
void UWeaponSystem::DropWeapon()
{
	Server_DropWeapon();
}

void UWeaponSystem::EquipWeapon(AWeaponBase* Weapon)
{
	Server_CancelReload();
	if(CurrentWeapon)
	{
		PreviousWeapon = CurrentWeapon;
		UnequipWeapon(CurrentWeapon);
		CurrentWeapon->OnCancelEquip();
		CurrentWeapon->OnStopFire();
	}
	CurrentWeapon = Weapon;
	CurrentWeapon->OnEquip();
	Server_StopPrimaryFire();

	Multicast_EquipWeaponVisualsTP(Weapon);
	Client_EquipWeaponVisualsFP(Weapon);
}

void UWeaponSystem::Server_EquipPreviousWeapon_Implementation()
{
	if(PreviousWeapon) EquipWeapon(PreviousWeapon);
}

void UWeaponSystem::UnequipWeapon(AWeaponBase* Weapon)
{
	Weapon->OnUnquip();
	Multicast_HideWeapon(Weapon, true);
}

void UWeaponSystem::EquipStartUpWeapons()
{
	if(MeleeWeapon)TakeWeapon(MeleeWeapon);
	if(SecondaryWeapon) EquipWeapon(SecondaryWeapon);
}

bool UWeaponSystem::CanTakeWeapon(AWeaponBase* Weapon)
{
	bool bCanTakeWeaponP = Weapon->WeaponInfo.WeaponType == Primary && PrimaryWeapon == nullptr;
	bool bCanTakeWeaponS = Weapon->WeaponInfo.WeaponType == Secondary && SecondaryWeapon == nullptr;
	bool bCanTakeWeaponM = Weapon->WeaponInfo.WeaponType == Melee && MeleeWeapon == nullptr;
	return bCanTakeWeaponP || bCanTakeWeaponS || bCanTakeWeaponM;
}

void UWeaponSystem::Server_EquipPrimary_Implementation()
{
	if(!PrimaryWeapon || CurrentWeapon == PrimaryWeapon) return;
	EquipWeapon(PrimaryWeapon);
}

void UWeaponSystem::Server_EquipSecondary_Implementation()
{
	if(!SecondaryWeapon || CurrentWeapon == SecondaryWeapon) return;
	EquipWeapon(SecondaryWeapon);
}

void UWeaponSystem::Server_EquipMelee_Implementation()
{
	if(!MeleeWeapon || CurrentWeapon == MeleeWeapon) return;
	EquipWeapon(MeleeWeapon);
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////// Fire //////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void UWeaponSystem::Server_StartPrimaryFire_Implementation()
{
	if(!bShootingEnabled) return;
	if(CurrentWeapon) CurrentWeapon->OnPrimaryFire();
}

void UWeaponSystem::Server_StopPrimaryFire_Implementation()
{
	if(CurrentWeapon) CurrentWeapon->OnStopFire();
}

void UWeaponSystem::Server_SecondaryFire_Implementation()
{
	if(!bShootingEnabled) return;
	if(CurrentWeapon) CurrentWeapon->OnSecondaryFire();
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////// Reload //////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UWeaponSystem::Server_Reload_Implementation()
{
	if(CurrentWeapon) CurrentWeapon->OnReload();
}

void UWeaponSystem::Server_CancelReload_Implementation()
{
	if(CurrentWeapon) CurrentWeapon->OnCancelReload();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////// Visuals /////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UWeaponSystem::Client_EquipWeaponVisualsFP_Implementation(AWeaponBase* Weapon)
{
	if(CharacterPlayer && Weapon)
	{
		USkeletalMeshComponent* ArmsFP = CharacterPlayer->GetArmsMeshFP();
		Weapon->Mesh_FP->AttachToComponent(ArmsFP, CreateAttachRules(), "WeaponSocket");
		Weapon->Mesh_FP->SetHiddenInGame(false);
		ArmsFP->SetRelativeTransform(Weapon->WeaponInfo.ArmsTransformFP);
	}
}
void UWeaponSystem::Multicast_EquipWeaponVisualsTP_Implementation(AWeaponBase* Weapon)
{
	if(CharacterPlayer && Weapon)
	{
		USkeletalMeshComponent* ArmsTP = CharacterPlayer->GetMesh();
		Weapon->Mesh_TP->AttachToComponent(ArmsTP, CreateAttachRules(), "WeaponSocket");
		Weapon->Mesh_TP->CastShadow = true;
		Weapon->Mesh_TP->SetHiddenInGame(false);
	}
}

void UWeaponSystem::Multicast_DropWeaponVisualsTP_Implementation(AWeaponBase* Weapon)
{
	if(CharacterPlayer && Weapon)
	{
		const FDetachmentTransformRules DetachmentTransformRules = FDetachmentTransformRules(EDetachmentRule::KeepWorld, true);
		Weapon->Mesh_TP->DetachFromComponent(DetachmentTransformRules);
	}
}

void UWeaponSystem::Multicast_HideWeapon_Implementation(AWeaponBase* Weapon, bool bHidden)
{
	if(!Weapon) return;
	Weapon->Mesh_TP->CastShadow = !bHidden;
	Weapon->Mesh_TP->SetHiddenInGame(bHidden);
	Weapon->Mesh_FP->SetHiddenInGame(bHidden);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////// Helpers //////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

void UWeaponSystem::EnableShooting(bool bEnableShooting)
{
	bShootingEnabled = bEnableShooting;
}