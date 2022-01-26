#include "DeathMatchGameMode.h"
#include "Breachers/Characters/CharacterBase.h"
#include "Breachers/PlayerControllers/BreachersPlayerController.h"

ADeathMatchGameMode::ADeathMatchGameMode()
{
	bUnlimitedTime = false;
	RespawnTime = 2;
	bDestroyWeaponsOnDie = true;
	bInfiniteAmmo = true;
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

void ADeathMatchGameMode::RequestAttackerSpawn(AController* Controller)
{
	DestroyCorpseAndWeapons(Controller);
	Super::RequestAttackerSpawn(Controller);
}

void ADeathMatchGameMode::RequestDefenderSpawn(AController* Controller)
{
	DestroyCorpseAndWeapons(Controller);
	Super::RequestDefenderSpawn(Controller);
}

void ADeathMatchGameMode::DestroyCorpseAndWeapons(AController* Controller)
{
	if(!Controller) return;

	if(ACharacterBase* Character = Cast<ACharacterBase>(Controller->GetPawn()))
	{
		Character->Destroy();
	}
}
