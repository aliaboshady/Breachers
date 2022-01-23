#include "DeathMatchGameState.h"
#include "Breachers/GameModes/DeathMatchGameMode.h"

void ADeathMatchGameState::BeginPlay()
{
	Super::BeginPlay();
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
