#include "PlantDefuseSystem.h"

#include "WeaponSystem.h"
#include "Breachers/Characters/CharacterBase.h"
#include "Breachers/GameModes/PlantAndDefuseGameMode.h"
#include "Net/UnrealNetwork.h"

UPlantDefuseSystem::UPlantDefuseSystem()
{
	PrimaryComponentTick.bCanEverTick = false;
	bIsInSite = false;
	bIsNearBomb = false;
}

void UPlantDefuseSystem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UPlantDefuseSystem, CharacterPlayer);
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
		PlayerInputComponent->BindAction("PlantDefuse", IE_Pressed, this, &UPlantDefuseSystem::ToPlantOrDefuse);
	}
}

void UPlantDefuseSystem::ToPlantOrDefuse()
{
	if(!CharacterPlayer) return;
	if(bIsPlanter)
	{
		if(bIsInSite /*&& CharacterPlayer->WeaponSystem->HasBomb()*/) Server_Plant();
	}
	else Server_Defuse();
}

void UPlantDefuseSystem::Server_Plant_Implementation()
{
	if(APlantAndDefuseGameMode* PDGM = Cast<APlantAndDefuseGameMode>(GetWorld()->GetAuthGameMode()))
	{
		PDGM->PlantBomb();
	}
}

void UPlantDefuseSystem::Server_Defuse_Implementation()
{
	if(APlantAndDefuseGameMode* PDGM = Cast<APlantAndDefuseGameMode>(GetWorld()->GetAuthGameMode()))
	{
		PDGM->DefuseBomb();
	}
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