#include "PlantAndDefuseGameState.h"
#include "Breachers/GameModes/PlantAndDefuseGameMode.h"
#include "Breachers/PlayerStates/BreachersPlayerState.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Sound/SoundCue.h"

APlantAndDefuseGameState::APlantAndDefuseGameState()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APlantAndDefuseGameState::BeginPlay()
{
	AttackersScore = 0;
	DefendersScore = 0;
	CurrentGamePhase = BuyPhase;
	CurrentRoundState = BombUnplanted;
	bAttackersWin = false;
	bCanSetWinnerTeam = true;
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
	DOREPLIFETIME(APlantAndDefuseGameState, AttackersScore);
	DOREPLIFETIME(APlantAndDefuseGameState, DefendersScore);
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
	OnRoundStateChange.Broadcast(CurrentRoundState);
}

void APlantAndDefuseGameState::Multicast_SetBombPlantedTimer_Implementation()
{
	CountDownTimeSpan = FTimespan(0, 0, BombDetonateTime);
}

void APlantAndDefuseGameState::Server_SetWinnerTeam_Implementation(bool bDidAttackersWin)
{
	if(bCanSetWinnerTeam)
	{
		Multicast_SetWinnerTeam(bDidAttackersWin);
		Server_SetCanSetWinnerTeam(false);
	}
}

void APlantAndDefuseGameState::Multicast_SetWinnerTeam_Implementation(bool bDidAttackersWin)
{
	bAttackersWin = bDidAttackersWin;
	if(bDidAttackersWin) Multicast_IncreaseAttackersScore();
	else Multicast_IncreaseDefendersScore();
	OnTeamsScoreChange.Broadcast(AttackersScore, DefendersScore);
}

void APlantAndDefuseGameState::StartBuyPhase()
{
	ShowTeamsCountUI();
	Server_SetCanSetWinnerTeam(true);
	Multicast_ChangeCurrentRoundState(BombUnplanted);
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
	if(TimeRanOutBothTeamsAlive())
	{
		Server_SetWinnerTeam(false);
		NetMulticast_PlaySound(DefendersWinAnnouncement);
	}
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
	NetMulticast_PlaySound(BombPlantedSound);
}

void APlantAndDefuseGameState::OnDefuseBomb()
{
	Multicast_ChangeCurrentRoundState(BombDefused);
	Server_SetWinnerTeam(false);
	Multicast_ChangeCurrentGamePhase(EndPhase);
	StartEndPhase();
	NetMulticast_PlaySound(BombDefusedSound);

	float BombDefusedSoundDuration = 0;
	if(BombDefusedSound) BombDefusedSoundDuration = BombDefusedSound->GetDuration();
	FTimerHandle AnnouncementHandle;
	FTimerDelegate AnnouncementDelegate;
	AnnouncementDelegate.BindUFunction(this, FName(TEXT("NetMulticast_PlaySound")), DefendersWinAnnouncement);
	GetWorldTimerManager().SetTimer(AnnouncementHandle, AnnouncementDelegate, 1, false, BombDefusedSoundDuration);
}

void APlantAndDefuseGameState::OnBombExploded()
{
	Multicast_ChangeCurrentRoundState(BombExploded);
	Server_SetWinnerTeam(true);
	Multicast_ChangeCurrentGamePhase(EndPhase);
	StartEndPhase();
	NetMulticast_PlaySound(AttackersWinAnnouncement);
	if(APlantAndDefuseGameMode* PDGM = Cast<APlantAndDefuseGameMode>(GetWorld()->GetAuthGameMode()))
	{
		PDGM->OnBombExploded();
	}
}

void APlantAndDefuseGameState::OnPlayerDied()
{
	FTimerHandle DeadHandle;
	GetWorldTimerManager().SetTimer(DeadHandle, this, &APlantAndDefuseGameState::CheckPlayersCount, 1, false, 0.05);
}

void APlantAndDefuseGameState::CheckPlayersCount()
{
	int32 AttackersCount;
	int32 DefendersCount;
	GetTeamsCount(AttackersCount, DefendersCount);

	if(CurrentGamePhase == EndPhase) return;
	
	if(DefendersCount == 0 && (CurrentRoundState == BombUnplanted || CurrentRoundState == BombPlanted))
	{
		OnFullTeamKilled(true);
	}
	else if(AttackersCount == 0 && CurrentRoundState == BombUnplanted)
	{
		OnFullTeamKilled(false);
	}
}

void APlantAndDefuseGameState::GetTeamsCount(int32& AttackersCount, int32& DefendersCount)
{
	AttackersCount = 0;
	DefendersCount = 0;
	
	for (APlayerState* Player : PlayerArray)
	{
		if(ABreachersPlayerState* BPS = Cast<ABreachersPlayerState>(Player))
		{
			if(BPS->GetIsDead()) continue;

			if(APawn* PlayerPawn = BPS->GetPawn())
			{
				if(PlayerPawn->ActorHasTag(TAG_Attacker)) AttackersCount++;
				else if(PlayerPawn->ActorHasTag(TAG_Defender)) DefendersCount++;
			}
		}
	}

	Multicast_OnCountDownChange(AttackersCount, DefendersCount);
}

void APlantAndDefuseGameState::ShowTeamsCountUI()
{
	Multicast_ShowTeamsCountUI();
}

void APlantAndDefuseGameState::Multicast_ShowTeamsCountUI_Implementation()
{
	int32 AttackersCount;
	int32 DefendersCount;
	GetTeamsCount(AttackersCount, DefendersCount);
}

void APlantAndDefuseGameState::Multicast_OnCountDownChange_Implementation(int32 AttackersCount, int32 DefendersCount)
{
	OnTeamsNumberChange.Broadcast(AttackersCount, DefendersCount);
}

void APlantAndDefuseGameState::OnFullTeamKilled(bool bAttackersWon)
{
	Server_SetWinnerTeam(bAttackersWon);
	Multicast_ChangeCurrentGamePhase(EndPhase);
	StartEndPhase();
	if(bAttackersWin) NetMulticast_PlaySound(AttackersWinAnnouncement);
	else NetMulticast_PlaySound(DefendersWinAnnouncement);
}

void APlantAndDefuseGameState::Multicast_IncreaseAttackersScore_Implementation()
{
	AttackersScore++;
}

void APlantAndDefuseGameState::Multicast_IncreaseDefendersScore_Implementation()
{
	DefendersScore++;
}

void APlantAndDefuseGameState::NetMulticast_PlaySound_Implementation(USoundCue* Sound)
{
	if(Sound) UGameplayStatics::PlaySound2D(GetWorld(), Sound);
}

bool APlantAndDefuseGameState::TimeRanOutBothTeamsAlive()
{
	int32 AttackersCount;
	int32 DefendersCount;
	GetTeamsCount(AttackersCount, DefendersCount);
	return AttackersCount > 0 && DefendersCount > 0 && CurrentRoundState == BombUnplanted;
}

void APlantAndDefuseGameState::Server_SetCanSetWinnerTeam_Implementation(bool bCanSetWinner)
{
	Multicast_SetCanSetWinnerTeam(bCanSetWinner);
}

void APlantAndDefuseGameState::Multicast_SetCanSetWinnerTeam_Implementation(bool bCanSetWinner)
{
	bCanSetWinnerTeam = bCanSetWinner;
}