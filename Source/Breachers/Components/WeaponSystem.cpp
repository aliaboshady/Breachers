#include "WeaponSystem.h"
#include "MovementSystem.h"
#include "Breachers/Characters/CharacterBase.h"
#include "Breachers/GameModes/DeathMatchGameMode.h"
#include "Breachers/Weapons/WeaponBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

UWeaponSystem::UWeaponSystem()
{
	PrimaryComponentTick.bCanEverTick = false;
	WeaponThrowForce = 40000;
	WeaponPickupDistance = 200;
	bShootingEnabled = true;
}

void UWeaponSystem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UWeaponSystem, LastBoughtWeapon);
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
	SetIsReplicated(true);
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
		PlayerInputComponent->BindAction("DropWeapon", IE_Pressed, this, &UWeaponSystem::Server_PlayerDropWeapon);

		PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &UWeaponSystem::Server_Reload);
		PlayerInputComponent->BindAction("PickWeapon", IE_Pressed, this, &UWeaponSystem::Client_PickWeapon);
	}
}

void UWeaponSystem::Server_SpawnStartWeapons_Implementation()
{
	if(MeleeWeaponClass && !MeleeWeapon) GetWorld()->SpawnActor<AWeaponBase>(MeleeWeaponClass, CharacterPlayer->GetActorTransform(), FActorSpawnParameters());
	if(PistolWeaponClass && !SecondaryWeapon) GetWorld()->SpawnActor<AWeaponBase>(PistolWeaponClass, CharacterPlayer->GetActorTransform(), FActorSpawnParameters());

	if(CurrentWeapon && CurrentWeapon == PrimaryWeapon) return;
	
	FTimerHandle SpawnedWeaponHandle;
	GetWorld()->GetTimerManager().SetTimer(SpawnedWeaponHandle, this, &UWeaponSystem::EquipStartUpWeapons, 1, false, 0.05);
}

void UWeaponSystem::SpawnWeapon(TSubclassOf<AWeaponBase> WeaponClass, EWeaponType WeaponType)
{
	Server_SpawnWeapon(WeaponClass, WeaponType);
}

void UWeaponSystem::Server_SpawnWeapon_Implementation(TSubclassOf<AWeaponBase> WeaponClass, EWeaponType WeaponType)
{
	if(WeaponClass)
	{
		if(WeaponType == Primary && PrimaryWeapon)
		{
			Server_EquipPrimary();
			DropWeapon();
		}
		if(WeaponType == Secondary &&SecondaryWeapon)
		{
			Server_EquipSecondary();
			DropWeapon();
		}
		
		AWeaponBase* SpawnedWeapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass, CharacterPlayer->GetActorTransform(), FActorSpawnParameters());
		LastBoughtWeapon = SpawnedWeapon;
		
		FTimerHandle SpawnWeaponHandle;
		GetWorld()->GetTimerManager().SetTimer(SpawnWeaponHandle, this, &UWeaponSystem::Server_EquipLastBoughtWeapon, 1, false, 0.1);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////// Equip/Take Weapons ///////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UWeaponSystem::Client_PickWeapon_Implementation()
{
	const FVector Start = CharacterPlayer->GetCameraLocation();
	FVector End = CharacterPlayer->GetCameraDirection() * WeaponPickupDistance + Start;
	
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(CharacterPlayer);
	
	FHitResult OutHit;
	UKismetSystemLibrary::LineTraceSingle(GetWorld(), Start, End, UEngineTypes::ConvertToTraceType(TRACE_WeaponPick), false, ActorsToIgnore, EDrawDebugTrace::None, OutHit, true);

	if(OutHit.bBlockingHit && OutHit.Actor->ActorHasTag(TAG_Weapon))
	{
		if(AWeaponBase* Weapon = Cast<AWeaponBase>(OutHit.Actor))
		{
			Server_PickWeapon(Weapon);
		}
	}
}

void UWeaponSystem::Server_PickWeapon_Implementation(AWeaponBase* Weapon)
{
	TakeWeapon(Weapon);
	Weapon->OnTaken();
	
	FTimerHandle PickupHandle;
	GetWorld()->GetTimerManager().SetTimer(PickupHandle, this, &UWeaponSystem::EquipeLastTakenWeapon, 1, false, 0.05);
}

void UWeaponSystem::Server_TakeWeapon_Implementation(AWeaponBase* Weapon)
{
	if(Weapon->WeaponInfo.WeaponType == Primary)
	{
		if(PrimaryWeapon)
		{
			Server_EquipPrimary();
			DropWeapon();
		}
		PrimaryWeapon = Weapon;
	}
	else if(Weapon->WeaponInfo.WeaponType == Secondary)
	{
		if(SecondaryWeapon)
		{
			Server_EquipSecondary();
			DropWeapon();
		}
		SecondaryWeapon = Weapon;
	}
	else if(Weapon->WeaponInfo.WeaponType == Melee && MeleeWeapon == nullptr)
	{
		MeleeWeapon = Weapon;
	}
	else return;

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
	CurrentWeapon = nullptr;

	if(PreviousWeapon) EquipWeapon(PreviousWeapon);
	else if(MeleeWeapon) EquipWeapon(MeleeWeapon);
	PreviousWeapon = nullptr;
}
void UWeaponSystem::DropWeapon()
{
	Server_DropWeapon();
}

void UWeaponSystem::Server_PlayerDropWeapon_Implementation()
{
	if(ADeathMatchGameMode* DGM = Cast<ADeathMatchGameMode>(GetWorld()->GetAuthGameMode())) return;
	Server_DropWeapon();
}

void UWeaponSystem::DropAllWeapons()
{
	if(PrimaryWeapon)
	{
		Server_EquipPrimary();
		DropWeapon();
	}
	if(SecondaryWeapon)
	{
		Server_EquipSecondary();
		DropWeapon();
	}
	if(MeleeWeapon) UnequipWeapon(MeleeWeapon);
}

void UWeaponSystem::DestroyAllWeapons()
{
	if(PrimaryWeapon)
	{
		PrimaryWeapon->Destroy();
		PrimaryWeapon = nullptr;
	}
	if(SecondaryWeapon)
	{
		SecondaryWeapon->Destroy();
		SecondaryWeapon = nullptr;
	}
	if(MeleeWeapon)
	{
		MeleeWeapon->Destroy();
		MeleeWeapon = nullptr;
	}
}

void UWeaponSystem::OnRestartRound()
{
	Server_CancelReload();
	if(SecondaryWeapon)
	{
		SecondaryWeapon->OnRestartRound();
		Server_EquipSecondary();
	}
	else Server_SpawnStartWeapons();
	
	if(PrimaryWeapon)
	{
		PrimaryWeapon->OnRestartRound();
		Server_EquipPrimary();
	}
}

void UWeaponSystem::Server_EquipWeapon_Implementation(AWeaponBase* Weapon)
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

	float RunSpeed = CurrentWeapon->WeaponInfo.CarrySpeeds.CarryWeaponRunSpeed;
	float WalkSpeed = CurrentWeapon->WeaponInfo.CarrySpeeds.CarryWeaponWalkSpeed;
	float CrouchSpeed = CurrentWeapon->WeaponInfo.CarrySpeeds.CarryWeaponCrouchSpeed;
	CharacterPlayer->MovementSystem->SetSpeedsOfWeapon(RunSpeed, WalkSpeed, CrouchSpeed);
	
	Server_StopPrimaryFire();

	Multicast_EquipWeaponVisualsTP(Weapon);
	Client_EquipWeaponVisualsFP(Weapon);
}
void UWeaponSystem::EquipWeapon(AWeaponBase* Weapon)
{
	Server_EquipWeapon(Weapon);
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
	if(MeleeWeapon)EquipWeapon(MeleeWeapon);
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

void UWeaponSystem::EquipeLastTakenWeapon()
{
	if(LastTakenWeapon) EquipWeapon(LastTakenWeapon);
}

void UWeaponSystem::Server_TakeLastBoughtWeapon_Implementation()
{
	if(LastBoughtWeapon) TakeWeapon(LastBoughtWeapon);
}

void UWeaponSystem::Server_EquipLastBoughtWeapon_Implementation()
{
	if(LastBoughtWeapon) EquipWeapon(LastBoughtWeapon);
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