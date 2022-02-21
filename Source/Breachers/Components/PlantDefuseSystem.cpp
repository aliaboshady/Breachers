#include "PlantDefuseSystem.h"

#include "MovementSystem.h"
#include "WeaponSystem.h"
#include "Breachers/Characters/CharacterBase.h"
#include "Breachers/GameModes/PlantAndDefuseGameMode.h"
#include "Breachers/Weapons/Bomb.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

UPlantDefuseSystem::UPlantDefuseSystem()
{
	PrimaryComponentTick.bCanEverTick = false;
	bIsInSite = false;
	bIsNearBomb = false;
	bIsPlanting = false;
	bIsDefusing = false;
}

void UPlantDefuseSystem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UPlantDefuseSystem, Bomb);
	DOREPLIFETIME(UPlantDefuseSystem, CharacterPlayer);
	DOREPLIFETIME(UPlantDefuseSystem, bIsPlanting);
	DOREPLIFETIME(UPlantDefuseSystem, bIsDefusing);
	DOREPLIFETIME(UPlantDefuseSystem, bIsPlanter);
	DOREPLIFETIME(UPlantDefuseSystem, bIsInSite);
	DOREPLIFETIME(UPlantDefuseSystem, bIsNearBomb);
}

void UPlantDefuseSystem::BeginPlay()
{
	Super::BeginPlay();
	SetIsReplicated(true);
	CharacterPlayer = Cast<ACharacterBase>(GetOwner());

	FTimerHandle PlayerTagTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(PlayerTagTimerHandle, this, &UPlantDefuseSystem::GetCharacterTag, 1, false, 1);
}

void UPlantDefuseSystem::GetCharacterTag()
{
	if(CharacterPlayer) bIsPlanter = CharacterPlayer->ActorHasTag(TAG_Attacker) ? true : false;
}

void UPlantDefuseSystem::SetPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if(PlayerInputComponent && CharacterPlayer)
	{
		PlayerInputComponent->BindAction("PlantDefuse", IE_Pressed, this, &UPlantDefuseSystem::Server_StartPlantOrDefuse);
		PlayerInputComponent->BindAction("PlantDefuse", IE_Released, this, &UPlantDefuseSystem::Server_StopPlantOrDefuse);
	}
}

void UPlantDefuseSystem::Server_StartPlantOrDefuse_Implementation()
{
	if(!CharacterPlayer) return;

	int32 PlantTime = 0;
	int32 DefuseTime = 0;
	
	if(APlantAndDefuseGameMode* PDGM = Cast<APlantAndDefuseGameMode>(GetWorld()->GetAuthGameMode()))
	{
		PlantTime = PDGM->GetPlantTimeInSeconds();
		DefuseTime = PDGM->GetDefuseTimeInSeconds();
	}
	
	if(bIsPlanter) StartPlant(PlantTime);
	else StartDefuse(DefuseTime);
}

void UPlantDefuseSystem::StartPlant(int32 PlantTime)
{
	if(!bIsInSite || !CharacterPlayer->WeaponSystem->HasBomb()) return;
	Multicast_SetIsPlanting(true);
	Multicast_StartPlantDefuseEffects(PlantTime);
	GetWorld()->GetTimerManager().SetTimer(PlantOrDefuseTimerHandle, this, &UPlantDefuseSystem::Server_Plant, 1, false, PlantTime);
}

void UPlantDefuseSystem::StartDefuse(int32 DefuseTime)
{
	if(!Bomb || Bomb->GetIsBeingDefused() || !IsStraightLineToBomb()) return;
	CharacterPlayer->WeaponSystem->EquipDefuser();
	Multicast_SetIsDefusing(true);
	Multicast_StartPlantDefuseEffects();
	Bomb->SetIsBeingDefused(true);
	GetWorld()->GetTimerManager().SetTimer(PlantOrDefuseTimerHandle, this, &UPlantDefuseSystem::Server_Defuse, 1, false, DefuseTime);
}

void UPlantDefuseSystem::Server_StopPlantOrDefuse_Implementation()
{
	if(bIsDefusing || bIsPlanting) CharacterPlayer->WeaponSystem->EquipPreviousWeapon();
	Multicast_SetIsDefusing(false);
	Multicast_SetIsPlanting(false);
	if(Bomb) Bomb->SetIsBeingDefused(false);
	Multicast_StopPlantDefuseEffects();
	GetWorld()->GetTimerManager().ClearTimer(PlantOrDefuseTimerHandle);
}

void UPlantDefuseSystem::Server_Plant_Implementation()
{
	if(!Bomb || !CharacterPlayer) return;
	Multicast_SetIsPlanting(false);
	Multicast_StopPlantDefuseEffects();
	if(APlantAndDefuseGameMode* PDGM = Cast<APlantAndDefuseGameMode>(GetWorld()->GetAuthGameMode()))
	{
		PDGM->PlantBomb(Bomb, CharacterPlayer);
	}
}

void UPlantDefuseSystem::Server_Defuse_Implementation()
{
	if(bIsDefusing) CharacterPlayer->WeaponSystem->EquipPreviousWeapon();
	Bomb->SetIsBeingDefused(false);
	Multicast_SetIsDefusing(false);
	SetPlayerConstraints(false);
	if(APlantAndDefuseGameMode* PDGM = Cast<APlantAndDefuseGameMode>(GetWorld()->GetAuthGameMode()))
	{
		PDGM->DefuseBomb();
	}
}

bool UPlantDefuseSystem::IsStraightLineToBomb()
{
	if(!Bomb || !CharacterPlayer) return false;
	
	FVector Start = CharacterPlayer->GetActorLocation();
	FVector End = Bomb->GetActorLocation();
	
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(CharacterPlayer);
	
	FHitResult OutHit;
	UKismetSystemLibrary::LineTraceSingle(GetWorld(), Start, End, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, ActorsToIgnore, EDrawDebugTrace::None, OutHit, true);
	if(OutHit.bBlockingHit && OutHit.Actor->ActorHasTag(TAG_Bomb))
	{
		return true;
	}
	
	return false;
}

void UPlantDefuseSystem::OnPlayerEnterSite()
{
	Server_OnPlayerEnterSite();
}
void UPlantDefuseSystem::Server_OnPlayerEnterSite_Implementation()
{
	bIsInSite = true;
	Client_OnPlayerEnterSite();
}
void UPlantDefuseSystem::Client_OnPlayerEnterSite_Implementation()
{
	bIsInSite = true;
}

void UPlantDefuseSystem::OnPlayerExitSite()
{
	Server_OnPlayerExitSite();
}

void UPlantDefuseSystem::Server_OnPlayerExitSite_Implementation()
{
	bIsInSite = false;
	Client_OnPlayerExitSite();
}
void UPlantDefuseSystem::Client_OnPlayerExitSite_Implementation()
{
	bIsInSite = false;
}

void UPlantDefuseSystem::SetBombToDefuse(ABomb* BombToDefuse)
{
	Server_SetBombToDefuse(BombToDefuse);
}
void UPlantDefuseSystem::Server_SetBombToDefuse_Implementation(ABomb* BombToDefuse)
{
	Bomb = BombToDefuse;
	Client_SetBombToDefuse(BombToDefuse);
}
void UPlantDefuseSystem::Client_SetBombToDefuse_Implementation(ABomb* BombToDefuse)
{
	Bomb = BombToDefuse;
}

void UPlantDefuseSystem::UnsetBombToDefuse()
{
	Server_UnsetBombToDefuse();
}
void UPlantDefuseSystem::Server_UnsetBombToDefuse_Implementation()
{
	Bomb = nullptr;
	Client_UnsetBombToDefuse();
}
void UPlantDefuseSystem::Client_UnsetBombToDefuse_Implementation()
{
	Bomb = nullptr;
}

void UPlantDefuseSystem::SetPlayerConstraints(bool bPlantingOrDefusing)
{
	if(CharacterPlayer)
	{
		CharacterPlayer->MovementSystem->SetIsPlantingOrDefusing(bPlantingOrDefusing);
		CharacterPlayer->WeaponSystem->SetIsPlantingOrDefusing(bPlantingOrDefusing);
	}
}

void UPlantDefuseSystem::Multicast_SetIsPlanting_Implementation(bool bPlanting)
{
	bIsPlanting = bPlanting;
}

void UPlantDefuseSystem::Multicast_SetIsDefusing_Implementation(bool bDefusing)
{
	bIsDefusing = bDefusing;
}

void UPlantDefuseSystem::Multicast_StartPlantDefuseEffects_Implementation(int32 PlantTime)
{
	SetPlayerConstraints(true);
	CharacterPlayer->MovementSystem->StartCrouch();
	PlayPlantAnimationAfterTime(PlantTime);
}

void UPlantDefuseSystem::Multicast_StopPlantDefuseEffects_Implementation()
{
	SetPlayerConstraints(false);
	CharacterPlayer->MovementSystem->StopCrouch();
	if(bIsPlanter && Bomb && bIsPlanting) Bomb->OnStopPlant();
	GetWorld()->GetTimerManager().ClearTimer(PlantOrDefuseAnimationTimerHandle);
}

void UPlantDefuseSystem::PlantAnimation(int32 PlantTime)
{
	if(bIsPlanter && Bomb) Bomb->OnStartPlant(PlantTime);
}

void UPlantDefuseSystem::PlayPlantAnimationAfterTime(int32 PlantTime)
{
	float Delay = 0;
	if(Bomb) Delay = Bomb->WeaponInfo.ReloadInfo.EquipTime;
	
	FTimerDelegate AnimationDelegate;
	AnimationDelegate.BindUFunction(this, FName(TEXT("PlantAnimation")), PlantTime);
	GetWorld()->GetTimerManager().SetTimer(PlantOrDefuseAnimationTimerHandle, AnimationDelegate, 1, false, Delay + 0.01);
}