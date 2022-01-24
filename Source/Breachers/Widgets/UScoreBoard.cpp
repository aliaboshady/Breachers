#include "UScoreBoard.h"
#include "Breachers/GameStates/BreachersGameState.h"

void UUScoreBoard::OnAddToScreen(ABreachersGameState* GameState)
{
	AddPlayersScoreRows(GameState);
}
