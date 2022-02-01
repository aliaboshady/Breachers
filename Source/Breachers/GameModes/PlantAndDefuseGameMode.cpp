#include "PlantAndDefuseGameMode.h"
#include "Breachers/Components/WeaponSystem.h"
#include "Breachers/GameStates/BreachersGameState.h"
#include "Breachers/GameStates/PlantAndDefuseGameState.h"
#include "Breachers/PlayerControllers/BreachersPlayerController.h"
#include "Breachers/Weapons/WeaponBase.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"

void APlantAndDefuseGameMode::OnPlayerDied(ABreachersPlayerController* Controller, ETeam NextTeamRespawn)
{
	Super::OnPlayerDied(Controller, NextTeamRespawn);
}

void APlantAndDefuseGameMode::EndOfRound()
{
	RestartRound();
}

void APlantAndDefuseGameMode::RestartRound()
{
	RespawnALlPlayers();
	RemoveAllUnpossessedBodies();
	RemoveAllUnownedWeapons();
	RestartCountDownTimer();
}

void APlantAndDefuseGameMode::RespawnALlPlayers()
{
	if(ABreachersGameState* BGS = GetGameState<ABreachersGameState>())
	{
		for (APlayerState* PlayerState : BGS->PlayerArray)
		{
			if(ABreachersPlayerController* BPC = Cast<ABreachersPlayerController>(PlayerState->GetOwner()))
			{
				if(BPC->CharacterPlayer) BPC->CharacterPlayer->WeaponSystem->OnRestartRound();
				
				ETeam PlayerTeam = BPC->GetPlayerTeam();
				if(PlayerTeam == Attacker) RequestAttackerSpawn(BPC);
				else RequestDefenderSpawn(BPC);
			}
		}
	}
}

void APlantAndDefuseGameMode::RemoveAllUnpossessedBodies()
{
	TArray<AActor*> AllPlayers;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), TAG_Player, AllPlayers);
	if(AllPlayers.Num() == 0) return;

	for (AActor* Player : AllPlayers)
	{
		if(!Player->GetInstigatorController()) Player->Destroy();
	}
}

void APlantAndDefuseGameMode::RemoveAllUnownedWeapons()
{
	TArray<AActor*> AllWeapons;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), TAG_Weapon, AllWeapons);
	if(AllWeapons.Num() == 0) return;

	for (AActor* Weapon : AllWeapons)
	{
		if(!Weapon->GetOwner() || !Weapon->GetOwner()->GetInstigatorController()) Weapon->Destroy();
	}
}

void APlantAndDefuseGameMode::RestartCountDownTimer()
{
	if(APlantAndDefuseGameState* PDGS = GetGameState<APlantAndDefuseGameState>())
	{
		PDGS->StartCountDownTimer();
	}
}

void APlantAndDefuseGameMode::SetUnlimitedMoney(bool bIsUnlimitedMoney)
{
	bUnlimitedMoney = bIsUnlimitedMoney;
}

void APlantAndDefuseGameMode::SetUnlimitedRounds(bool bIsUnlimitedRounds)
{
	bUnlimitedRounds = bIsUnlimitedRounds;
}

void APlantAndDefuseGameMode::SetRoundTime(int32 RoundTime)
{
	RoundTimeInMinutes = RoundTime;
}

void APlantAndDefuseGameMode::SetRoundNumber(int32 RoundsCount)
{
	RoundsNumber = RoundsCount;
}
