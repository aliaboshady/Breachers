#include "BreachersGameModeBase.h"
#include "BreachersPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Breachers/Characters/CharacterBase.h"
#include "Breachers/Components/BuyMenu.h"

void ABreachersGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	GetPlayersStarts();
}

void ABreachersGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

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
		if(AttackerClass) SpawnCharacter(AttackerClass, SpawnTransform, Controller);
	}
	
}

void ABreachersGameModeBase::RequestDefenderSpawn(AController* Controller)
{
	if(DefenderSpawns.Num() > 0)
	{
		const int32 RandInd = FMath::RandRange(0, DefenderSpawns.Num() - 1);
		const FTransform SpawnTransform = DefenderSpawns[RandInd]->GetActorTransform();
		if(DefenderClass) SpawnCharacter(DefenderClass, SpawnTransform, Controller);
	}
}

void ABreachersGameModeBase::GetPlayersStarts()
{
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "AttackerSpawn", AttackerSpawns);
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "DefenderSpawn", DefenderSpawns);
}

void ABreachersGameModeBase::SpawnCharacter(TSubclassOf<ACharacterBase> CharacterClass, FTransform SpawnTransform, AController* Controller) const
{
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	if(ACharacterBase* Character = GetWorld()->SpawnActor<ACharacterBase>(CharacterClass, SpawnTransform, SpawnParameters))
	{
		Controller->Possess(Character);
		if(Character->BuyMenu) Character->BuyMenu->EnableBuying(true);
	}
}