#include "PlantAndDefuseGameMode.h"

#include "Breachers/Components/BuyMenu.h"
#include "Breachers/Components/HealthSystem.h"
#include "Breachers/Components/MoneySystem.h"
#include "Breachers/Components/MovementSystem.h"
#include "Breachers/Components/WeaponSystem.h"
#include "Breachers/GameInstance/MainGameInstance.h"
#include "Breachers/GameStates/BreachersGameState.h"
#include "Breachers/GameStates/PlantAndDefuseGameState.h"
#include "Breachers/PlayerControllers/BreachersPlayerController.h"
#include "Breachers/PlayerControllers/PlantAndDefusePlayerController.h"
#include "Breachers/PlayerStates/BreachersPlayerState.h"
#include "Breachers/Weapons/Bomb.h"
#include "Breachers/Weapons/WeaponBase.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"

APlantAndDefuseGameMode::APlantAndDefuseGameMode()
{
	PlantTimeInSeconds = 4;
	DefuseTimeInSeconds = 7;
	DetonateTimeInSeconds = 30;
	BuyPhaseTimeInSeconds = 20;
	EndPhaseTimeInSeconds = 5;
}

void APlantAndDefuseGameMode::BeginPlay()
{
	Super::BeginPlay();
	GetBombSpawnLocations();
}

void APlantAndDefuseGameMode::SpawnPlayerWithSelectedTeam(APlayerController* NewPlayer)
{
	if(ABreachersPlayerController* CharacterPC = Cast<ABreachersPlayerController>(NewPlayer))
	{
		if(APlantAndDefuseGameState* PDGS = GetGameState<APlantAndDefuseGameState>())
		{
			if(PDGS->GetCurrentGamePhase() == BuyPhase)
			{
				CharacterPC->SetCanMove(false);
				CharacterPC->SetCanShoot(false);
			}
			else
			{
				CharacterPC->BuyMenu->EnableBuying(false);
				CharacterPC->BuyMenu->CloseBuyMenu();
			}
		}
		
		CharacterPC->Client_ClearAllWidgets();
		CharacterPC->Client_ShowPlayerUI();

		FTimerHandle RespawnHandle;
		FTimerDelegate RespawnDelegate;
	
		if(CharacterPC->HasChosenTeam())
		{			
			ETeam Team = CharacterPC->GetPlayerTeam();
			if(Team == Attacker) RespawnDelegate.BindUFunction(this, FName(TEXT("RequestAttackerSpawn")), CharacterPC);
			else RespawnDelegate.BindUFunction(this, FName(TEXT("RequestDefenderSpawn")), CharacterPC);
		}
		else
		{
			int32 RandomTeam = FMath::RandRange(0, 1);
			if(RandomTeam)
			{
				RespawnDelegate.BindUFunction(this, FName(TEXT("RequestAttackerSpawn")), CharacterPC);
				CharacterPC->Server_ChangeTeam(Attacker);
			}
			else
			{
				RespawnDelegate.BindUFunction(this, FName(TEXT("RequestDefenderSpawn")), CharacterPC);
				CharacterPC->Server_ChangeTeam(Defender);
			}
		}
			
		GetWorldTimerManager().SetTimer(RespawnHandle, RespawnDelegate, 1, false, 0.2);
	}
}

void APlantAndDefuseGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	FTimerHandle SpawnPlayerHandle;
	FTimerDelegate SpawnPlayerDelegate;
	SpawnPlayerDelegate.BindUFunction(this, FName(TEXT("SpawnPlayerWithSelectedTeam")), NewPlayer);
	GetWorldTimerManager().SetTimer(SpawnPlayerHandle, SpawnPlayerDelegate, 1, false, 3);
}

void APlantAndDefuseGameMode::OnPlayerDied(ABreachersPlayerController* Controller, ETeam NextTeamRespawn)
{
	Super::OnPlayerDied(Controller, NextTeamRespawn);
	if(APlantAndDefuseGameState* PDGS = GetGameState<APlantAndDefuseGameState>())
	{
		PDGS->OnPlayerDied();
	}
}

void APlantAndDefuseGameMode::SetPhaseBanner(EPhaseBanner PhaseBanner)
{
	if(ABreachersGameState* BGS = GetGameState<ABreachersGameState>())
	{
		for (APlayerState* PlayerState : BGS->PlayerArray)
		{
			if(ABreachersPlayerState* BPS = Cast<ABreachersPlayerState>(PlayerState))
			{
				if(APlantAndDefusePlayerController* PDPC = Cast<APlantAndDefusePlayerController>(BPS->GetOwner()))
				{
					PDPC->SwitchRoundPhaseBanner(PhaseBanner);
				}
			}
		}
	}
}

void APlantAndDefuseGameMode::BuyPhasePlayerConstrains(bool bIsInBuyPhase)
{
	if(ABreachersGameState* BGS = GetGameState<ABreachersGameState>())
	{
		for (APlayerState* PlayerState : BGS->PlayerArray)
		{
			if(ABreachersPlayerState* BPS = Cast<ABreachersPlayerState>(PlayerState))
			{
				if(ABreachersPlayerController* BPC = Cast<ABreachersPlayerController>(BPS->GetOwner()))
				{
					BPC->BuyMenu->CloseBuyMenu();
					BPC->BuyMenu->EnableBuying(bIsInBuyPhase);
					BPC->SetCanMove(!bIsInBuyPhase);
					BPC->SetCanShoot(!bIsInBuyPhase);
				}
			}
		}
	}
}

void APlantAndDefuseGameMode::StartBuyPhase()
{
	BuyPhasePlayerConstrains(true);
	SetPhaseBanner(BuyPhaseBanner);
	SpawnBombAtRandomLocation();
}

void APlantAndDefuseGameMode::StartMainPhase()
{
	BuyPhasePlayerConstrains(false);
	SetPhaseBanner(MainPhaseBanner);
}

void APlantAndDefuseGameMode::StartEndPhase(bool bAttackersWin)
{
	if(bAttackersWin) SetPhaseBanner(AttackersWinBanner);
	else SetPhaseBanner(DefendersWinBanner);
}
void APlantAndDefuseGameMode::EndOfRound(bool bAttackersWon)
{
	RestartRound(bAttackersWon);
}

void APlantAndDefuseGameMode::RestartRound(bool bAttackersWon)
{
	AttackerSpawnIndex = 0;
	DefenderSpawnIndex = 0;
	ShuffleSpawnPoints(AttackerSpawns);
	ShuffleSpawnPoints(DefenderSpawns);
	RespawnALlPlayers(bAttackersWon);
	RemoveAllUnpossessedBodies();
	RemoveAllUnownedWeapons();
	RestartCountDownTimer();
}

void APlantAndDefuseGameMode::RespawnALlPlayers(bool bAttackersWon)
{
	if(ABreachersGameState* BGS = GetGameState<ABreachersGameState>())
	{
		for (APlayerState* PlayerState : BGS->PlayerArray)
		{
			if(ABreachersPlayerState* BPS = Cast<ABreachersPlayerState>(PlayerState))
			{
				if(ABreachersPlayerController* BPC = Cast<ABreachersPlayerController>(BPS->GetOwner()))
				{
					
					ETeam PlayerTeam = BPC->GetPlayerTeam();
					if(BPC->CharacterPlayer) BPC->CharacterPlayer->WeaponSystem->OnRestartRound();

					if(bAttackersWon)
					{
						if(PlayerTeam == Attacker) BPC->MoneySystem->AddToCurrentMoney(WinMoney);
						else BPC->MoneySystem->AddToCurrentMoney(LoseMoney);
					}
					else
					{
						if(PlayerTeam == Attacker) BPC->MoneySystem->AddToCurrentMoney(LoseMoney);
						else BPC->MoneySystem->AddToCurrentMoney(WinMoney);
					}
					
					if(BPS->GetIsDead())
					{
						if(PlayerTeam == Attacker) RequestAttackerSpawn(BPC);
						else RequestDefenderSpawn(BPC);
					}
					else
					{
						if(PlayerTeam == Attacker) RequestAttackerRepositionToSpawn(BPC);
						else RequestDefenderRepositionToSpawn(BPC);

						if(BPC->CharacterPlayer) BPC->CharacterPlayer->HealthSystem->Reset();
					}
				}
			}
		}
	}
}

void APlantAndDefuseGameMode::RemoveAllUnpossessedBodies()
{
	TArray<AActor*> AllPlayers;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), TAG_Player, AllPlayers);
	if(AllPlayers.Num() == 0) return;

	for (AActor* Player : AllPlayers)
	{
		if(!Player->GetInstigatorController()) Player->Destroy();
	}
}

void APlantAndDefuseGameMode::RemoveAllUnownedWeapons()
{
	RemoveOldBombFromMap();
	
	TArray<AActor*> AllWeapons;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), TAG_Weapon, AllWeapons);
	if(AllWeapons.Num() == 0) return;

	for (AActor* Weapon : AllWeapons)
	{
		if(!Weapon->GetOwner() || !Weapon->GetOwner()->GetInstigatorController()) Weapon->Destroy();
	}
}

void APlantAndDefuseGameMode::RestartCountDownTimer()
{
	if(APlantAndDefuseGameState* PDGS = GetGameState<APlantAndDefuseGameState>())
	{
		PDGS->StartCountDownTimer();
	}
}

void APlantAndDefuseGameMode::SetUnlimitedMoney(bool bIsUnlimitedMoney)
{
	bUnlimitedMoney = bIsUnlimitedMoney;
}

void APlantAndDefuseGameMode::SetUnlimitedRounds(bool bIsUnlimitedRounds)
{
	bUnlimitedRounds = bIsUnlimitedRounds;
}

void APlantAndDefuseGameMode::SetRoundTime(int32 RoundTime)
{
	if(RoundTime > 0) RoundTimeInMinutes = RoundTime;
}

void APlantAndDefuseGameMode::SetRoundNumber(int32 RoundsCount)
{
	if(RoundsCount > 0) RoundsNumber = RoundsCount;
}

void APlantAndDefuseGameMode::ShuffleSpawnPoints(TArray<AActor*>& SpawnArray)
{
	if (SpawnArray.Num() > 0)
	{
		int32 LastIndex = SpawnArray.Num() - 1;
		for (int32 i = 0; i <= LastIndex; ++i)
		{
			int32 Index = FMath::RandRange(i, LastIndex);
			if (i != Index)
			{
				SpawnArray.Swap(i, Index);
			}
		}
	}
}

FTransform APlantAndDefuseGameMode::GetSpawnTransform(TArray<AActor*>& SpawnPoints, ETeam NextTeamRespawn)
{
	if(NextTeamRespawn == Attacker)
	{
		return SpawnPoints[AttackerSpawnIndex++]->GetTransform();
	}
	else
	{
		return SpawnPoints[DefenderSpawnIndex++]->GetTransform();
	}
}

void APlantAndDefuseGameMode::PlantBomb(ABomb* Bomb, ACharacterBase* CharacterPlayer)
{
	PinBomb(Bomb, CharacterPlayer);
	
	if(APlantAndDefuseGameState* PDGS = GetGameState<APlantAndDefuseGameState>())
	{
		if(PDGS->GetCurrentGamePhase() == BuyPhase) return;
		if(PDGS->GetCurrentRoundState() == BombUnplanted) PDGS->OnPlantBomb();
	}
}

void APlantAndDefuseGameMode::DefuseBomb()
{
	if(APlantAndDefuseGameState* PDGS = GetGameState<APlantAndDefuseGameState>())
	{
		if(PDGS->GetCurrentGamePhase() == BuyPhase) return;
		if(PDGS->GetCurrentRoundState() == BombPlanted) PDGS->OnDefuseBomb();
	}
}

void APlantAndDefuseGameMode::OnBombExploded()
{
	if(!BombClass) return;

	if(ABomb* Bomb = Cast<ABomb>(UGameplayStatics::GetActorOfClass(GetWorld(), BombClass)))
	{
		Bomb->OnExploded();
	}
}

void APlantAndDefuseGameMode::PinBomb(ABomb* Bomb, ACharacterBase* CharacterPlayer)
{
	if(!Bomb || !CharacterPlayer) return;
	CharacterPlayer->WeaponSystem->DropWeapon();
	FTransform PlantTransform = CharacterPlayer->GetMesh()->GetComponentTransform();
	PlantTransform.SetRotation(FRotator(PlantTransform.Rotator().Pitch, PlantTransform.Rotator().Yaw - 90, PlantTransform.Rotator().Roll).Quaternion());
	Bomb->OnPlanted(CharacterPlayer);
	Bomb->SetActorTransform(PlantTransform);
}

void APlantAndDefuseGameMode::GetBombSpawnLocations()
{
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "BombSpawn", BombSpawns);
}

void APlantAndDefuseGameMode::SpawnBombAtRandomLocation()
{
	if(!BombClass) return;

	TArray<AActor*> AllBomb;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), TAG_Bomb, AllBomb);
	if(AllBomb.Num() > 0) return;
	
	FTransform SpawnTransform;
	if(BombSpawns.Num() > 0)
	{
		const int32 RandInd = FMath::RandRange(0, BombSpawns.Num() - 1);
		SpawnTransform = BombSpawns[RandInd]->GetActorTransform();
	}
	
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	GetWorld()->SpawnActor<ABomb>(BombClass, SpawnTransform, SpawnParameters);
}

void APlantAndDefuseGameMode::RemoveOldBombFromMap()
{
	TArray<AActor*> AllBomb;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), TAG_Bomb, AllBomb);
	if(AllBomb.Num() == 0) return;

	for (AActor* Bomb : AllBomb)
	{
		if(ACharacterBase* CharacterPlayer = Cast<ACharacterBase>(Bomb->GetOwner()))
		{
			CharacterPlayer->WeaponSystem->DropBomb();
		}
		Bomb->Destroy();
	}
}
