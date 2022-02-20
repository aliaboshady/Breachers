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
		PlayerInputComponent->BindAction("PlantDefuse", IE_Released, this, &UPlantDefuseSystem::StopPlantOrDefuse);
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
	SetPlayerConstraints(true);
	GetWorld()->GetTimerManager().SetTimer(PlantOrDefuseTimerHandle, this, &UPlantDefuseSystem::Server_Plant, 1, false, PlantTime);
}

void UPlantDefuseSystem::StartDefuse(int32 DefuseTime)
{
	if(!Bomb || Bomb->GetIsBeingDefused() || !IsStraightLineToBomb()) return;
	Bomb->SetIsBeingDefused(true);
	SetPlayerConstraints(true);
	GetWorld()->GetTimerManager().SetTimer(PlantOrDefuseTimerHandle, this, &UPlantDefuseSystem::Server_Defuse, 1, false, DefuseTime);
}

void UPlantDefuseSystem::StopPlantOrDefuse()
{
	if(Bomb) Bomb->SetIsBeingDefused(false);
	SetPlayerConstraints(false);
	GetWorld()->GetTimerManager().ClearTimer(PlantOrDefuseTimerHandle);
}

void UPlantDefuseSystem::Server_Plant_Implementation()
{
	if(!Bomb || !CharacterPlayer) return;
	SetPlayerConstraints(false);
	if(APlantAndDefuseGameMode* PDGM = Cast<APlantAndDefuseGameMode>(GetWorld()->GetAuthGameMode()))
	{
		PDGM->PlantBomb(Bomb, CharacterPlayer);
	}
}

void UPlantDefuseSystem::Server_Defuse_Implementation()
{
	Bomb->SetIsBeingDefused(false);
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