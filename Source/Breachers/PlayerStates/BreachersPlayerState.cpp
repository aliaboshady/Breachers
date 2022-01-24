#include "BreachersPlayerState.h"
#include "Net/UnrealNetwork.h"

ABreachersPlayerState::ABreachersPlayerState()
{
	Deaths = 0;
	Kills = 0;
}

void ABreachersPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABreachersPlayerState, Kills);
	DOREPLIFETIME(ABreachersPlayerState, Deaths);
}

void ABreachersPlayerState::OnDie()
{
	++Deaths;
}

void ABreachersPlayerState::OnKill()
{
	++Kills;
}
