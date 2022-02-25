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
