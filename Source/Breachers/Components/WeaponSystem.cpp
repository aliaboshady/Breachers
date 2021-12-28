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
}

void UWeaponSystem::BeginPlay()
{
	Super::BeginPlay();
	CharacterPlayer = Cast<ACharacterBase>(GetOwner());
}

bool UWeaponSystem::HasPrimaryWeapon() const
{
	return PrimaryWeapon != nullptr;
}

bool UWeaponSystem::HasSecondaryWeapon() const
{
	return SecondaryWeapon != nullptr;
}

void UWeaponSystem::EquipWeapon(AWeaponBase* Weapon)
{
	Server_EquipWeapon(Weapon);
	Client_EquipWeaponVisualsFP(Weapon);
	Multicast_EquipWeaponVisualsTP(Weapon);
}

void UWeaponSystem::Server_EquipWeapon_Implementation(AWeaponBase* Weapon)
{
	if(Weapon->WeaponInfo.WeaponType == Primary)
	{
		PrimaryWeapon = Weapon;
	}
	else if(Weapon->WeaponInfo.WeaponType == Secondary)
	{
		SecondaryWeapon = Weapon;
	}
}

void UWeaponSystem::Client_EquipWeaponVisualsFP_Implementation(AWeaponBase* Weapon)
{
	if(CharacterPlayer)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found Player FP"));
		USkeletalMeshComponent* ArmsFP = CharacterPlayer->GetArmsMeshFP();
		Weapon->Mesh_FP->AttachToComponent(ArmsFP, CreateAttachRules(), "WeaponSocket");
	}
}

void UWeaponSystem::Multicast_EquipWeaponVisualsTP_Implementation(AWeaponBase* Weapon)
{
	if(CharacterPlayer)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found Player TP"));
		USkeletalMeshComponent* ArmsTP = CharacterPlayer->GetMesh();
		Weapon->Mesh_TP->AttachToComponent(ArmsTP, CreateAttachRules(), "WeaponSocket");
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
