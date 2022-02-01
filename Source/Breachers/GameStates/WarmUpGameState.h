#pragma once
#include "CoreMinimal.h"
#include "DeathMatchGameState.h"
#include "WarmUpGameState.generated.h"

UCLASS()
class BREACHERS_API AWarmUpGameState : public ADeathMatchGameState
{
	GENERATED_BODY()

protected:
	virtual void EndOfRound() override;
};
