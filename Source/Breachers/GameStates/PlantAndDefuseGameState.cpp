#include "PlantAndDefuseGameState.h"
#include "Breachers/GameModes/PlantAndDefuseGameMode.h"
#include "Net/UnrealNetwork.h"

void APlantAndDefuseGameState::BeginPlay()
{
	CurrentGamePhase = BuyPhase;
	CurrentRoundState = BombUnplanted;
	OneSecondTimespan = FTimespan(0, 0, 1);
	SetBombDetonateTimer();
	if(HasAuthority()) StartCountDownTimer();
}

void APlantAndDefuseGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APlantAndDefuseGameState, CurrentGamePhase);
	DOREPLIFETIME(APlantAndDefuseGameState, CurrentRoundState);
}

void APlantAndDefuseGameState::SetBombDetonateTimer()
{
	if(APlantAndDefuseGameMode* PDGM = Cast<APlantAndDefuseGameMode>(GetWorld()->GetAuthGameMode()))
	{
		BombDetonateTime = PDGM->GetDetonateTimeInSeconds();
	}
}

void APlantAndDefuseGameState::StartCountDownTimer()
{	
	switch (CurrentGamePhase)
	{
	case BuyPhase:
		StartBuyPhase();
		break;

	case MainPhase:
		StartMainPhase();
		break;

	case EndPhase:
		StartEndPhase();
		break;
	}
}

void APlantAndDefuseGameState::Multicast_DecrementCountdownTime_Implementation()
{
	if(!IsRoundTimeIsFinished())
	{
		CountDownTimeSpan -= OneSecondTimespan;
		OnCountDownChange.Broadcast(CountDownTimeSpan);
	}
	
	if(IsRoundTimeIsFinished())
	{
		GetWorldTimerManager().ClearTimer(CountDownTimerHandle);

		switch (CurrentGamePhase)
		{
		case BuyPhase:
			EndOfBuyPhase();
			break;

		case MainPhase:
			EndOfMainPhase();
			break;

		case EndPhase:
			EndOfRound();
			break;
		}
		
		StartCountDownTimer();
	}
}

void APlantAndDefuseGameState::Multicast_ChangeCurrentGamePhase_Implementation(EPhase NewGamePhase)
{
	CurrentGamePhase = NewGamePhase;
}

void APlantAndDefuseGameState::Multicast_ChangeCurrentRoundState_Implementation(ERoundState NewRoundState)
{
	CurrentRoundState = NewRoundState;
}


void APlantAndDefuseGameState::Multicast_SetBombPlantedTimer_Implementation()
{
	CountDownTimeSpan = FTimespan(0, 0, BombDetonateTime);
}

void APlantAndDefuseGameState::StartBuyPhase()
{
	CurrentRoundState = BombUnplanted;
	if(APlantAndDefuseGameMode* PDGM = Cast<APlantAndDefuseGameMode>(GetWorld()->GetAuthGameMode()))
	{
		PDGM->StartBuyPhase();
		GetWorldTimerManager().ClearTimer(CountDownTimerHandle);
		CountDownTimeSpan = FTimespan(0, 0, PDGM->GetBuyPhaseTimeInSeconds());
		GetWorldTimerManager().SetTimer(CountDownTimerHandle, this, &APlantAndDefuseGameState::Multicast_DecrementCountdownTime, 1, true);
	}
}

void APlantAndDefuseGameState::EndOfBuyPhase()
{
	Multicast_ChangeCurrentGamePhase(MainPhase);
}

void APlantAndDefuseGameState::StartMainPhase()
{
	if(APlantAndDefuseGameMode* PDGM = Cast<APlantAndDefuseGameMode>(GetWorld()->GetAuthGameMode()))
	{
		PDGM->StartMainPhase();
		GetWorldTimerManager().ClearTimer(CountDownTimerHandle);
		CountDownTimeSpan = FTimespan(0, PDGM->GetRoundTimeInMinutes(), 0);
		GetWorldTimerManager().SetTimer(CountDownTimerHandle, this, &APlantAndDefuseGameState::Multicast_DecrementCountdownTime, 1, true);
	}
}

void APlantAndDefuseGameState::EndOfMainPhase()
{
	Multicast_ChangeCurrentGamePhase(EndPhase);
}

void APlantAndDefuseGameState::StartEndPhase()
{
	if(APlantAndDefuseGameMode* PDGM = Cast<APlantAndDefuseGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GetWorldTimerManager().ClearTimer(CountDownTimerHandle);
		CountDownTimeSpan = FTimespan(0, 0, PDGM->GetEndPhaseTimeInSeconds());
		GetWorldTimerManager().SetTimer(CountDownTimerHandle, this, &APlantAndDefuseGameState::Multicast_DecrementCountdownTime, 1, true);
	}
}

void APlantAndDefuseGameState::EndOfRound()
{
	Multicast_ChangeCurrentGamePhase(BuyPhase);
	if(APlantAndDefuseGameMode* PDGM = Cast<APlantAndDefuseGameMode>(GetWorld()->GetAuthGameMode()))
	{
		PDGM->EndOfRound();
	}
}

void APlantAndDefuseGameState::OnPlantBomb()
{
	Multicast_ChangeCurrentRoundState(BombPlanted);
	Multicast_SetBombPlantedTimer();
}

void APlantAndDefuseGameState::OnDefuseBomb()
{
	Multicast_ChangeCurrentRoundState(BombDefused);
	Multicast_ChangeCurrentGamePhase(EndPhase);
	StartEndPhase();
}