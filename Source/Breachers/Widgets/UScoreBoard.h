#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UScoreBoard.generated.h"

class ABreachersGameState;

UCLASS()
class BREACHERS_API UUScoreBoard : public UUserWidget
{
	GENERATED_BODY()

public:
	void OnAddToScreen(ABreachersGameState* GameState);

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void AddPlayersScoreRows(ABreachersGameState* GameState);
};
