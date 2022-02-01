#include "PlantAndDefuseGameMode.h"

#include "Kismet/KismetSystemLibrary.h"

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
