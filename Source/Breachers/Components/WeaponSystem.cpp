#include "WeaponSystem.h"
#include "Breachers/Characters/CharacterBase.h"
#include "Breachers/Weapons/WeaponBase.h"
#include "Net/UnrealNetwork.h"

UWeaponSystem::UWeaponSystem()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UWeaponSystem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UWeaponSystem, CurrentWeapon);
	DOREPLIFETIME(UWeaponSystem, PrimaryWeapon);
	DOREPLIFETIME(UWeaponSystem, SecondaryWeapon);
	DOREPLIFETIME(UWeaponSystem, MeleeWeapon);
}

void UWeaponSystem::BeginPlay()
{
	Super::BeginPlay();
	CharacterPlayer = Cast<ACharacterBase>(GetOwner());
	if(MeleeWeaponClass)
	{
		Server_SpawnWeapon(MeleeWeaponClass);
	}
}

void UWeaponSystem::Server_SpawnWeapon_Implementation(TSubclassOf<AWeaponBase> WeaponClass)
{
	AWeaponBase* SpawnedMeleeWeapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass, FActorSpawnParameters());
	if(SpawnedMeleeWeapon)
	{
		TakeWeapon(SpawnedMeleeWeapon);
		EquipWeapon(SpawnedMeleeWeapon);
	}
}

bool UWeaponSystem::CanTakeWeapon(AWeaponBase* Weapon)
{
	return (Weapon->WeaponInfo.WeaponType == Primary && PrimaryWeapon == nullptr) ||
		(Weapon->WeaponInfo.WeaponType == Secondary && SecondaryWeapon == nullptr);
}

void UWeaponSystem::Server_TakeWeapon_Implementation(AWeaponBase* Weapon)
{
	if(Weapon->WeaponInfo.WeaponType == Primary && PrimaryWeapon == nullptr)
	{
		PrimaryWeapon = Weapon;
		PrimaryWeapon->SetOwner(CharacterPlayer);
		PrimaryWeapon->SetInstigator(CharacterPlayer);
		Weapon->SetHidden(true);
		//EquipWeapon(Weapon);
	}
	else if(Weapon->WeaponInfo.WeaponType == Secondary && SecondaryWeapon == nullptr)
	{
		SecondaryWeapon = Weapon;
		SecondaryWeapon->SetOwner(CharacterPlayer);
		SecondaryWeapon->SetInstigator(CharacterPlayer);
		Weapon->SetHidden(true);
	}
	else if(Weapon->WeaponInfo.WeaponType == Melee && MeleeWeapon == nullptr)
	{
		MeleeWeapon = Weapon;
		MeleeWeapon->SetOwner(CharacterPlayer);
		MeleeWeapon->SetInstigator(CharacterPlayer);
	}
}

void UWeaponSystem::TakeWeapon(AWeaponBase* Weapon)
{
	Server_TakeWeapon(Weapon);
}

void UWeaponSystem::Server_EquipWeapon_Implementation(AWeaponBase* Weapon)
{
	CurrentWeapon = Weapon;
	Client_EquipWeaponVisualsFP(Weapon);
	Multicast_EquipWeaponVisualsTP(Weapon);
}

void UWeaponSystem::EquipWeapon(AWeaponBase* Weapon)
{
	Server_EquipWeapon(Weapon);
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
		Weapon->SetHidden(false);
	}
}

FAttachmentTransformRules UWeaponSystem::CreateAttachRules()
{
	FAttachmentTransformRules AttachRules = FAttachmentTransformRules( EAttachmentRule::KeepRelative, true );;
	AttachRules.LocationRule = EAttachmentRule::SnapToTarget;
	AttachRules.RotationRule = EAttachmentRule::SnapToTarget;
	AttachRules.ScaleRule = EAttachmentRule::SnapToTarget;
	return AttachRules;
}
