#include "DeathMatchGameMode.h"
#include "Breachers/Characters/CharacterBase.h"
#include "Breachers/PlayerControllers/BreachersPlayerController.h"

ADeathMatchGameMode::ADeathMatchGameMode()
{
	bUnlimitedTime = false;
	MatchTimeInMinutes = 10;
	RespawnTime = 2;
}

void ADeathMatchGameMode::OnPlayerDied(ABreachersPlayerController* Controller)
{
	FTimerHandle RespawnHandle;
	FTimerDelegate RespawnDelegate;
	
	switch (Controller->CharacterPlayer->Team)
	{
	case Attacker:
		RespawnDelegate.BindUFunction(this, FName(TEXT("RequestAttackerSpawn")), Controller);
		break;
	case Defender:
		RespawnDelegate.BindUFunction(this, FName(TEXT("RequestDefenderSpawn")), Controller);
		break;
	default:;
	}
	GetWorldTimerManager().SetTimer(RespawnHandle, RespawnDelegate, 1, false, RespawnTime);
}
