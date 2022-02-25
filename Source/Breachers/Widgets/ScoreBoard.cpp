#include "ScoreBoard.h"
#include "Breachers/GameStates/BreachersGameState.h"

void UScoreBoard::OnAddToScreen(ABreachersGameState* GameState, bool bForced)
{
	AddPlayersScoreRows(GameState, bForced);
}
