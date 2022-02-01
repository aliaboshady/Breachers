#include "PlantAndDefuseGameMode.h"
#include "Breachers/GameStates/BreachersGameState.h"
#include "Breachers/PlayerControllers/BreachersPlayerController.h"
#include "GameFramework/PlayerState.h"

void APlantAndDefuseGameMode::OnPlayerDied(ABreachersPlayerController* Controller, ETeam NextTeamRespawn)
{
	Super::OnPlayerDied(Controller, NextTeamRespawn);
}

void APlantAndDefuseGameMode::EndOfRound()
{
	if(ABreachersGameState* BGS = GetGameState<ABreachersGameState>())
	{
		for (APlayerState* PlayerState : BGS->PlayerArray)
		{
			if(ABreachersPlayerController* BPC = Cast<ABreachersPlayerController>(PlayerState->GetOwner()))
			{
				ETeam PlayerTeam = BPC->GetPlayerTeam();
				if(PlayerTeam == Attacker) RequestAttackerSpawn(BPC);
				else RequestDefenderSpawn(BPC);
			}
		}
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
