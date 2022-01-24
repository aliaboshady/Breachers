#include "BreachersGameState.h"
#include "Breachers/GameModes/DeathMatchGameMode.h"
#include "Net/UnrealNetwork.h"

void ABreachersGameState::BeginPlay()
{
	Super::BeginPlay();
	OneSecondTimespan = FTimespan(0, 0, 1);
	
	if(ABreachersGameModeBase* GM = Cast<ABreachersGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		CountDownTimeSpan = FTimespan(0, GM->GetRoundTimeInMinutes(), 0);
		GetWorldTimerManager().SetTimer(CountDownTimerHandle, this, &ABreachersGameState::Multicast_DecrementCountdownTime, 1, true);
	}
}

void ABreachersGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABreachersGameState, CountDownTimeSpan);
}

void ABreachersGameState::EndOfRound()
{
}

void ABreachersGameState::EndOfMatch()
{
	if(ABreachersGameModeBase* GM = Cast<ABreachersGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		GM->EndOfMatch();
	}
}

bool ABreachersGameState::IsRoundTimeIsFinished()
{
	return CountDownTimeSpan.GetHours() <= 0 && CountDownTimeSpan.GetMinutes() <= 0 && CountDownTimeSpan.GetSeconds() <= 0;
}

void ABreachersGameState::Multicast_DecrementCountdownTime_Implementation()
{
	if(!IsRoundTimeIsFinished())
	{
		CountDownTimeSpan -= OneSecondTimespan;
		OnCountDownChange.Broadcast(CountDownTimeSpan);
	}
	
	if(IsRoundTimeIsFinished())
	{
		GetWorldTimerManager().ClearTimer(CountDownTimerHandle);
		EndOfRound();
	}
}
