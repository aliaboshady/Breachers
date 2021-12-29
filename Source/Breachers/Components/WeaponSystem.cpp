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

void UWeaponSystem::EquipKnife()
{
	if(MeleeWeapon) EquipWeapon(MeleeWeapon);
}

void UWeaponSystem::BeginPlay()
{
	Super::BeginPlay();
	CharacterPlayer = Cast<ACharacterBase>(GetOwner());

	if(CharacterPlayer->HasAuthority()) Server_SpawnWeapon(MeleeWeaponClass);
	
	FTimerHandle EquipKnifeHandle;
	GetWorld()->GetTimerManager().SetTimer(EquipKnifeHandle, this, &UWeaponSystem::EquipKnife, 1, false, 0.1);
}

void UWeaponSystem::Server_SpawnWeapon_Implementation(TSubclassOf<AWeaponBase> WeaponClass)
{
	GetWorld()->SpawnActor<AWeaponBase>(WeaponClass, CharacterPlayer->GetActorTransform(), FActorSpawnParameters());
}

bool UWeaponSystem::CanTakeWeapon(AWeaponBase* Weapon)
{
	return (Weapon->WeaponInfo.WeaponType == Primary && PrimaryWeapon == nullptr) ||
		(Weapon->WeaponInfo.WeaponType == Secondary && SecondaryWeapon == nullptr) ||
		(Weapon->WeaponInfo.WeaponType == Melee && MeleeWeapon == nullptr);
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
	//EquipWeapon(Weapon);
}

void UWeaponSystem::TakeWeapon(AWeaponBase* Weapon)
{
	Server_TakeWeapon(Weapon);
}

void UWeaponSystem::Server_EquipWeapon_Implementation(AWeaponBase* Weapon)
{
	CurrentWeapon = Weapon;
	Multicast_EquipWeaponVisualsTP(Weapon);
	Client_EquipWeaponVisualsFP(Weapon);
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

void UWeaponSystem::Multicast_HideWeapon_Implementation(AWeaponBase* Weapon, bool bHidden)
{
	Weapon->Mesh_TP->CastShadow = !bHidden;
	Weapon->SetHidden(bHidden);
}