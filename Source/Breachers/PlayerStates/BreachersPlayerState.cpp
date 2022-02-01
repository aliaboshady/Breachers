#include "BreachersPlayerState.h"
#include "Breachers/GameStates/BreachersGameState.h"
#include "Breachers/PlayerControllers/BreachersPlayerController.h"
#include "Net/UnrealNetwork.h"

ABreachersPlayerState::ABreachersPlayerState()
{
	Deaths = 0;
	Kills = 0;
	bIsDead = false;
}

void ABreachersPlayerState::BeginPlay()
{
	Super::BeginPlay();
	BreachersGameState = Cast<ABreachersGameState>(GetWorld()->GetGameState());
	
	if (ABreachersPlayerController* BPC = Cast<ABreachersPlayerController>(GetOwner()))
	{
		BPC->OnSpawn.AddDynamic(this, &ABreachersPlayerState::OnSpawn);
	}
}

void ABreachersPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABreachersPlayerState, Kills);
	DOREPLIFETIME(ABreachersPlayerState, Deaths);
	DOREPLIFETIME(ABreachersPlayerState, bIsDead);
}

void ABreachersPlayerState::OnSpawn()
{
	bIsDead = false;
}

void ABreachersPlayerState::OnDie(AController* InstigatedBy, AActor* DamageCauser)
{
	if(BreachersGameState) BreachersGameState->PlayerOnDied(InstigatedBy, DamageCauser, this);
	++Deaths;
	bIsDead = true;
}

void ABreachersPlayerState::OnKill()
{
	++Kills;
}
