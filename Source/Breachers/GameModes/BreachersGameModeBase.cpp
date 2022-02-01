#include "BreachersGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Breachers/Characters/CharacterBase.h"
#include "Breachers/Components/BuyMenu.h"
#include "Breachers/GameInstance/MainGameInstance.h"
#include "Breachers/GameStates/BreachersGameState.h"
#include "Breachers/PlayerControllers/BreachersPlayerController.h"
#include "GameFramework/PlayerState.h"

ABreachersGameModeBase::ABreachersGameModeBase()
{
	bFriendlyFireOn = false;
	bUnlimitedMoney = false;
	RoundTimeInMinutes = 10;
	SlowDilationFactor = 0.1;
	SlowDilationTime = 10;
	bDestroyWeaponsOnDie = false;
	bInfiniteAmmo = false;
}

void ABreachersGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	GetPlayersStarts();
	BreachersGameState = GetGameState<ABreachersGameState>();
}

void ABreachersGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);

	if(ABreachersPlayerController* CharacterPC = Cast<ABreachersPlayerController>(NewPlayer))
	{
		CharacterPC->ShowTeamSelectionMenu();
	}
}

void ABreachersGameModeBase::RequestAttackerSpawn(AController* Controller)
{
	if(AttackerSpawns.Num() > 0)
	{
		const int32 RandInd = FMath::RandRange(0, AttackerSpawns.Num() - 1);
		const FTransform SpawnTransform = AttackerSpawns[RandInd]->GetActorTransform();
		if(AttackerClass) SpawnCharacter(AttackerClass, SpawnTransform, Controller, TAG_Attacker);
	}
}

void ABreachersGameModeBase::RequestDefenderSpawn(AController* Controller)
{
	if(DefenderSpawns.Num() > 0)
	{
		const int32 RandInd = FMath::RandRange(0, DefenderSpawns.Num() - 1);
		const FTransform SpawnTransform = DefenderSpawns[RandInd]->GetActorTransform();
		if(DefenderClass) SpawnCharacter(DefenderClass, SpawnTransform, Controller, TAG_Defender);
	}
}

void ABreachersGameModeBase::GetPlayersStarts()
{
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "AttackerSpawn", AttackerSpawns);
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "DefenderSpawn", DefenderSpawns);
}

void ABreachersGameModeBase::SpawnCharacter(TSubclassOf<ACharacterBase> CharacterClass, FTransform SpawnTransform, AController* Controller, FString Tag) const
{
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	if(ACharacterBase* Character = GetWorld()->SpawnActor<ACharacterBase>(CharacterClass, SpawnTransform, SpawnParameters))
	{
		Controller->Possess(Character);
		Character->Tags.Add(FName(Tag));
		if(ABreachersPlayerController* PC = Cast<ABreachersPlayerController>(Controller))
		{
			PC->OnPlayerSpawn();
			PC->BuyMenu->EnableBuying(true);
		}
	}
}

void ABreachersGameModeBase::EndOfMatch()
{
	OpenEndMatchScoreBoard();
	SlowDownTime();
	
	FTimerHandle EndServerTime;
	GetWorldTimerManager().SetTimer(EndServerTime, this, &ABreachersGameModeBase::EndServer, 1, false, SlowDilationTime * SlowDilationFactor);
}

void ABreachersGameModeBase::EndServer()
{
	SetNormalTimeSpeed();
	if(UMainGameInstance* GI = Cast<UMainGameInstance>(GetGameInstance()))
	{
		GI->DestroyGameSession();
	}
}

void ABreachersGameModeBase::SlowDownTime()
{
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), SlowDilationFactor);
}

void ABreachersGameModeBase::SetNormalTimeSpeed()
{
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1);
}

void ABreachersGameModeBase::OnPlayerDied(ABreachersPlayerController* Controller, ETeam NextTeamRespawn)
{
	
}

void ABreachersGameModeBase::OpenEndMatchScoreBoard()
{
	for (auto PlayerState : BreachersGameState->PlayerArray)
	{
		if(ABreachersPlayerController* BPC = Cast<ABreachersPlayerController>(PlayerState->GetOwner()))
		{
			BPC->Client_OpenScoreBoard();
			BPC->Client_DisableScoreBoard();
		}
	}
}
