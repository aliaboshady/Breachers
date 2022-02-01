#include "WarmUpGameState.h"

void AWarmUpGameState::BeginPlay()
{
	Super::BeginPlay();
	bUnlimitedTime = false;
}

void AWarmUpGameState::EndOfRound()
{
	Super::EndOfRound();
}

void AWarmUpGameState::SetUnlimitedMoney_NextGM(bool bIsUnlimitedMoney)
{
	bUnlimitedMoney_NextGM = bIsUnlimitedMoney;
}

void AWarmUpGameState::SetUnlimitedRounds_NextGM(bool bIsUnlimitedRounds)
{
	bUnlimitedRounds_NextGM = bIsUnlimitedRounds;
}

void AWarmUpGameState::SetRoundTimeInMinutes_NextGM(int32 RoundTime)
{
	RoundTimeInMinutes_NextGM = RoundTime;
}

void AWarmUpGameState::SetRoundNumberPerHalf_NextGM(int32 RoundNumberPerHalf)
{
	RoundNumberPerHalf_NextGM = RoundNumberPerHalf;
}
