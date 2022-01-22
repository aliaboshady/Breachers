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

void ADeathMatchGameState::OnRep_CountDownTimeSpan()
{
	if(bUnlimitedTime) return;
	Super::OnRep_CountDownTimeSpan();
}
