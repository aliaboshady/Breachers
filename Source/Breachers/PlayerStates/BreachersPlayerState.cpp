#include "BreachersPlayerState.h"

#include "Breachers/GameStates/BreachersGameState.h"
#include "Net/UnrealNetwork.h"

ABreachersPlayerState::ABreachersPlayerState()
{
	Deaths = 0;
	Kills = 0;
}

void ABreachersPlayerState::BeginPlay()
{
	Super::BeginPlay();
	BreachersGameState = Cast<ABreachersGameState>(GetWorld()->GetGameState());
}

void ABreachersPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABreachersPlayerState, Kills);
	DOREPLIFETIME(ABreachersPlayerState, Deaths);
}

void ABreachersPlayerState::OnDie(AController* InstigatedBy, AActor* DamageCauser)
{
	if(BreachersGameState) BreachersGameState->PlayerOnDied(InstigatedBy, DamageCauser, this);
	++Deaths;
}

void ABreachersPlayerState::OnKill()
{
	++Kills;
}
