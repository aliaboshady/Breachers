#pragma once
#include "CoreMinimal.h"
#include "BreachersGameState.h"
#include "DeathMatchGameState.generated.h"

UCLASS()
class BREACHERS_API ADeathMatchGameState : public ABreachersGameState
{
	GENERATED_BODY()

protected:
	
	bool bUnlimitedTime;
};
