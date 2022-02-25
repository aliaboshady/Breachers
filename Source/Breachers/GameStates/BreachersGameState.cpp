#include "BreachersGameState.h"
#include "Breachers/GameModes/DeathMatchGameMode.h"
#include "Breachers/PlayerControllers/BreachersPlayerController.h"
#include "Breachers/PlayerStates/BreachersPlayerState.h"
#include "Breachers/Weapons/WeaponBase.h"
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

void ABreachersGameState::PlayerOnDied(AController* Killer, AActor* DamageCauser, ABreachersPlayerState* KilledPlayerState)
{
	FName KillerName;
	bool bKillerIsAttacker = false;
	UTexture2D* WeaponIcon = nullptr;
	FName KilledName;
	bool bKilledIsAttacker = false;
	
	if(ABreachersPlayerState* KillerBPS = Killer->GetPlayerState<ABreachersPlayerState>())
	{
		KillerName = FName(KillerBPS->GetPlayerName());
		APawn* KillerPawn = KillerBPS->GetPawn();
		if(KillerPawn && KillerPawn->ActorHasTag(TAG_Attacker)) bKillerIsAttacker = true;
	}
	
	if(AWeaponBase* Weapon = Cast<AWeaponBase>(DamageCauser))
    {
		WeaponIcon = Weapon->WeaponInfo.WeaponIcon;
    }

	if(KilledPlayerState)
	{
		KilledName = FName(KilledPlayerState->GetPlayerName());
		APawn* KilledPawn = KilledPlayerState->GetPawn();
		if(KilledPawn && KilledPawn->ActorHasTag(TAG_Attacker)) bKilledIsAttacker = true;
	}

	AddToAllPlayersKillfeed(KillerName, bKillerIsAttacker, WeaponIcon, KilledName, bKilledIsAttacker);
	OnScoreBoardChange();
}

void ABreachersGameState::AddToAllPlayersKillfeed(FName KillerName, bool bKillerIsAttacker, UTexture2D* WeaponIcon, FName KilledName, bool bKilledIsAttacker)
{
	for (auto PlayerState : PlayerArray)
	{
		if(ABreachersPlayerController* BPC = Cast<ABreachersPlayerController>(PlayerState->GetOwner()))
		{
			BPC->UpdateKillfeed(KillerName, bKillerIsAttacker, WeaponIcon, KilledName, bKilledIsAttacker);
		}
	}
}

void ABreachersGameState::OnScoreBoardChange()
{
	for (auto PlayerState : PlayerArray)
	{
		if(ABreachersPlayerController* BPC = Cast<ABreachersPlayerController>(PlayerState->GetOwner()))
		{
			BPC->OnScoreBoardChange();
		}
	}
}