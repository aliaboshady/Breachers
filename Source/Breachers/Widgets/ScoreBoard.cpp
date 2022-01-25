#include "ScoreBoard.h"
#include "Breachers/GameStates/BreachersGameState.h"

void UScoreBoard::OnAddToScreen(ABreachersGameState* GameState)
{
	AddPlayersScoreRows(GameState);
}
