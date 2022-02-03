#include "PlantAndDefuseGameMode.h"
#include "Breachers/Components/HealthSystem.h"
#include "Breachers/Components/WeaponSystem.h"
#include "Breachers/GameStates/BreachersGameState.h"
#include "Breachers/GameStates/PlantAndDefuseGameState.h"
#include "Breachers/PlayerControllers/BreachersPlayerController.h"
#include "Breachers/PlayerStates/BreachersPlayerState.h"
#include "Breachers/Weapons/WeaponBase.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"

void APlantAndDefuseGameMode::BeginPlay()
{
	Super::BeginPlay();
	RoundTimeInMinutes = 1;
}

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
			if(ABreachersPlayerState* BPS = Cast<ABreachersPlayerState>(PlayerState))
			{
				if(ABreachersPlayerController* BPC = Cast<ABreachersPlayerController>(BPS->GetOwner()))
				{
					ETeam PlayerTeam = BPC->GetPlayerTeam();
					if(BPC->CharacterPlayer) BPC->CharacterPlayer->WeaponSystem->OnRestartRound();
					
					if(BPS->GetIsDead())
					{
						if(PlayerTeam == Attacker) RequestAttackerSpawn(BPC);
						else RequestDefenderSpawn(BPC);
					}
					else
					{
						if(PlayerTeam == Attacker) RequestAttackerRepositionToSpawn(BPC);
						else RequestDefenderRepositionToSpawn(BPC);

						if(BPC->CharacterPlayer) BPC->CharacterPlayer->HealthSystem->Reset();
					}
				}
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
	if(RoundTime > 0) RoundTimeInMinutes = RoundTime;
}

void APlantAndDefuseGameMode::SetRoundNumber(int32 RoundsCount)
{
	if(RoundsCount > 0) RoundsNumber = RoundsCount;
}
