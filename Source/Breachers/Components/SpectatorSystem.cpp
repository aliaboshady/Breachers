#include "SpectatorSystem.h"
#include "Breachers/Characters/CharacterBase.h"
#include "Breachers/GameStates/PlantAndDefuseGameState.h"
#include "Breachers/PlayerControllers/PlantAndDefusePlayerController.h"
#include "Breachers/PlayerStates/BreachersPlayerState.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

USpectatorSystem::USpectatorSystem()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USpectatorSystem::BeginPlay()
{
	Super::BeginPlay();
	SetIsReplicated(true);
	PC = Cast<APlantAndDefusePlayerController>(GetOwner());
}

void USpectatorSystem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(USpectatorSystem, PC);
}

void USpectatorSystem::GetValidPlayersToSpectate(TArray<ACharacterBase*>& ValidPlayers)
{
	if(!PC || !PC->CharacterPlayer) return;
	
	if(APlantAndDefuseGameState* PDGS = Cast<APlantAndDefuseGameState>(GetWorld()->GetGameState()))
	{
		TArray<APlayerState*> PlayerStates = PDGS->PlayerArray;
		for (APlayerState* PlayerState : PlayerStates)
		{
			ABreachersPlayerState* BPS = Cast<ABreachersPlayerState>(PlayerState);
			ACharacterBase* CharacterPlayer = Cast<ACharacterBase>(PlayerState->GetPawn());
			if(!CharacterPlayer || CharacterPlayer == PC->CharacterPlayer || !BPS || BPS->GetIsDead()) continue;

			if(CharacterPlayer->ActorHasTag(TAG_Attacker) && PC->CharacterPlayer->ActorHasTag(TAG_Attacker))
			{
				ValidPlayers.Add(CharacterPlayer);
			}
			else if(CharacterPlayer->ActorHasTag(TAG_Defender) && PC->CharacterPlayer->ActorHasTag(TAG_Defender))
			{
				ValidPlayers.Add(CharacterPlayer);
			}
		}
	}
}

void USpectatorSystem::SpectateNext()
{
	TArray<ACharacterBase*> ValidPlayerCharacters;
	GetValidPlayersToSpectate(ValidPlayerCharacters);
	if(ValidPlayerCharacters.Num() > 0)
	{
		UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("%d"), ValidPlayerCharacters.Num()));
	}
}
