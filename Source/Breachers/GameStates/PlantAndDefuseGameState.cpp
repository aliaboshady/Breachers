#include "PlantAndDefuseGameState.h"
#include "Breachers/GameModes/PlantAndDefuseGameMode.h"

void APlantAndDefuseGameState::BeginPlay()
{
	OneSecondTimespan = FTimespan(0, 0, 1);
	
	if(APlantAndDefuseGameMode* GM = Cast<APlantAndDefuseGameMode>(GetWorld()->GetAuthGameMode()))
	{
		CountDownTimeSpan = FTimespan(0, GM->GetRoundTimeInMinutes(), 0);
		GetWorldTimerManager().SetTimer(CountDownTimerHandle, this, &APlantAndDefuseGameState::Multicast_DecrementCountdownTime, 1, true);
	}
}
