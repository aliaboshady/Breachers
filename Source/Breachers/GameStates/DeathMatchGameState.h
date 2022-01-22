#pragma once
#include "CoreMinimal.h"
#include "BreachersGameState.h"
#include "DeathMatchGameState.generated.h"

UCLASS()
class BREACHERS_API ADeathMatchGameState : public ABreachersGameState
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void Multicast_DecrementCountdownTime() override;
	
	bool bUnlimitedTime;
};
