#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ScoreBoard.generated.h"

class ABreachersGameState;
class ABreachersPlayerState;

UCLASS()
class BREACHERS_API UScoreBoard : public UUserWidget
{
	GENERATED_BODY()

public:
	void OnAddToScreen(ABreachersGameState* GameState, bool bForced = false);

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void AddPlayersScoreRows(ABreachersGameState* GameState, bool bForced);
};
