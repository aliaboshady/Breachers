#include "MainGameInstance.h"

void UMainGameInstance::ChooseNextTeam(ETeam Team)
{
	NextTeamRespawn = Team;
	bHasChosenTeam = true;
}
