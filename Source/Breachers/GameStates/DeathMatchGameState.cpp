#include "DeathMatchGameState.h"
#include "Breachers/GameModes/DeathMatchGameMode.h"
#include "Breachers/PlayerStates/BreachersPlayerState.h"

void ADeathMatchGameState::BeginPlay()
{
	Super::BeginPlay();
	bUnlimitedTime = true;

	FTimerHandle UnlimitedTimeHandle;
	GetWorldTimerManager().SetTimer(UnlimitedTimeHandle, this, &ADeathMatchGameState::GetUnlimitedTimeFromGameMode, 1, false, 2);
}

void ADeathMatchGameState::GetUnlimitedTimeFromGameMode()
{
	if(ADeathMatchGameMode* DeathMatchGM = Cast<ADeathMatchGameMode>(GetWorld()->GetAuthGameMode()))
	{
		bUnlimitedTime = DeathMatchGM->GetIsUnlimitedTime();
	}
}

void ADeathMatchGameState::Multicast_DecrementCountdownTime()
{
	UE_LOG(LogTemp, Warning, TEXT("%d"), bUnlimitedTime);
	if(bUnlimitedTime)
	{
		GetWorldTimerManager().ClearTimer(CountDownTimerHandle);
		return;
	}
	Super::Multicast_DecrementCountdownTime();
}

void ADeathMatchGameState::EndOfRound()
{
	EndOfMatch();
}

TArray<ABreachersPlayerState*> ADeathMatchGameState::GetSortedPlayersBasedOnKillCount()
{
	TArray<ABreachersPlayerState*> PlayerStates;

	for (auto PlayerState : PlayerArray)
	{
		if(ABreachersPlayerState* BPS = Cast<ABreachersPlayerState>(PlayerState))
		{
			PlayerStates.Add(BPS);
		}
	}
	
	for (int i = 0; i < PlayerStates.Num(); ++i)
	{
		for (int j = i + 1; j < PlayerStates.Num(); ++j)
		{
			if(PlayerStates[j]->GetKillsCount() > PlayerStates[i]->GetKillsCount())
			{
				ABreachersPlayerState* TempPS = PlayerStates[i];
				PlayerStates[i] = PlayerStates[j];
				PlayerStates[j] = TempPS;
			}
		}
	}
	return PlayerStates;
}
