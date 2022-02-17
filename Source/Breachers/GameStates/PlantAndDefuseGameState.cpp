#include "PlantAndDefuseGameState.h"
#include "Breachers/GameModes/PlantAndDefuseGameMode.h"
#include "Breachers/PlayerStates/BreachersPlayerState.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

APlantAndDefuseGameState::APlantAndDefuseGameState()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APlantAndDefuseGameState::BeginPlay()
{
	CurrentGamePhase = BuyPhase;
	CurrentRoundState = BombUnplanted;
	bAttackersWin = false;
	OneSecondTimespan = FTimespan(0, 0, 1);
	SetBombDetonateTimer();
	if(HasAuthority()) StartCountDownTimer();
}

void APlantAndDefuseGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APlantAndDefuseGameState, CurrentGamePhase);
	DOREPLIFETIME(APlantAndDefuseGameState, CurrentRoundState);
	DOREPLIFETIME(APlantAndDefuseGameState, bAttackersWin);
}

void APlantAndDefuseGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	FString GamePhase = UEnum::GetValueAsName(CurrentGamePhase).ToString();
	FString RoundState = UEnum::GetValueAsName(CurrentRoundState).ToString();
	//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("%s - %s"), *GamePhase, *RoundState));
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

void APlantAndDefuseGameState::Multicast_SetWinnerTeam_Implementation(bool bDidAttackersWin)
{
	bAttackersWin = bDidAttackersWin;
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
		if(CurrentRoundState == BombPlanted) OnBombExploded();
		PDGM->StartEndPhase(bAttackersWin);
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
	if(CurrentGamePhase == EndPhase) return;
	Multicast_ChangeCurrentRoundState(BombPlanted);
	Multicast_SetBombPlantedTimer();
}

void APlantAndDefuseGameState::OnDefuseBomb()
{
	Multicast_ChangeCurrentRoundState(BombDefused);
	bAttackersWin = false;
	Multicast_ChangeCurrentGamePhase(EndPhase);
	StartEndPhase();
}

void APlantAndDefuseGameState::OnBombExploded()
{
	Multicast_ChangeCurrentRoundState(BombExploded);
	bAttackersWin = true;
	Multicast_ChangeCurrentGamePhase(EndPhase);
	StartEndPhase();
}

void APlantAndDefuseGameState::OnPlayerDied()
{
	FTimerHandle DeadHandle;
	GetWorldTimerManager().SetTimer(DeadHandle, this, &APlantAndDefuseGameState::CheckPlayersCount, 1, false, 0.05);
}

void APlantAndDefuseGameState::CheckPlayersCount()
{
	if(CurrentGamePhase == EndPhase) return;
	
	int32 AttackersCount = 0;
	int32 DefendersCount = 0;
	
	for (APlayerState* Player : PlayerArray)
	{
		if(ABreachersPlayerState* BPS = Cast<ABreachersPlayerState>(Player))
		{
			if(BPS->GetIsDead()) continue;
			if(BPS->GetPawn()->ActorHasTag(TAG_Attacker)) AttackersCount++;
			else if(BPS->GetPawn()->ActorHasTag(TAG_Defender)) DefendersCount++;
		}
	}

	if(DefendersCount == 0 && (CurrentRoundState == BombUnplanted || CurrentRoundState == BombPlanted))
	{
		OnFullTeamKilled(true);
	}
	else if(AttackersCount == 0 && CurrentRoundState == BombUnplanted)
	{
		OnFullTeamKilled(false);
	}
}

void APlantAndDefuseGameState::OnFullTeamKilled(bool bAttackersWon)
{
	bAttackersWin = bAttackersWon;
	Multicast_ChangeCurrentGamePhase(EndPhase);
	StartEndPhase();
}
