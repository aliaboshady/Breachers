#pragma once
#include "CoreMinimal.h"
#include "BreachersGameState.h"
#include "DeathMatchGameState.generated.h"

class ABreachersPlayerState;

UCLASS()
class BREACHERS_API ADeathMatchGameState : public ABreachersGameState
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	TArray<ABreachersPlayerState*> GetSortedPlayersBasedOnKillCount();

protected:
	virtual void BeginPlay() override;
	virtual void Multicast_DecrementCountdownTime() override;
	virtual void EndOfRound() override;
	void GetUnlimitedTimeFromGameMode();

	UPROPERTY(BlueprintReadWrite)
	bool bUnlimitedTime;
};
