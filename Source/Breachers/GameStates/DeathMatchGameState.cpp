#include "DeathMatchGameState.h"
#include "Breachers/GameModes/DeathMatchGameMode.h"
#include "Net/UnrealNetwork.h"

void ADeathMatchGameState::BeginPlay()
{
	Super::BeginPlay();
	OneSecondTimespan = FTimespan(0, 0, 1);
	
	if(ADeathMatchGameMode* DeathMatchGM = Cast<ADeathMatchGameMode>(GetWorld()->GetAuthGameMode()))
	{
		CountDownTimeSpan = FTimespan(0, DeathMatchGM->GetMatchTimeInMinutes(), 0);
		GetWorldTimerManager().SetTimer(CountDownTimerHandle, this, &ADeathMatchGameState::OnRep_CountDownTimeSpan, 1, true);
	}
}

void ADeathMatchGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ADeathMatchGameState, CountDownTimeSpan);
}

bool ADeathMatchGameState::IsTimeIsFinished()
{
	return CountDownTimeSpan.GetHours() <= 0 && CountDownTimeSpan.GetMinutes() <= 0 && CountDownTimeSpan.GetSeconds() <= 0;
}

void ADeathMatchGameState::OnRep_CountDownTimeSpan()
{
	if(HasAuthority()) UE_LOG(LogTemp, Warning, TEXT("%d : %d : %d"), CountDownTimeSpan.GetHours(), CountDownTimeSpan.GetMinutes(), CountDownTimeSpan.GetSeconds());
	if(!IsTimeIsFinished())
	{
		CountDownTimeSpan -= OneSecondTimespan;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Time is finished"));
		GetWorldTimerManager().ClearTimer(CountDownTimerHandle);
	}
}
