#pragma once
#include "CoreMinimal.h"
#include "DeathMatchGameState.h"
#include "WarmUpGameState.generated.h"

UCLASS()
class BREACHERS_API AWarmUpGameState : public ADeathMatchGameState
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetUnlimitedMoney_NextGM(bool bIsUnlimitedMoney);

	UFUNCTION(BlueprintCallable)
	void SetUnlimitedRounds_NextGM(bool bIsUnlimitedRounds);

	UFUNCTION(BlueprintCallable)
	void SetRoundTimeInMinutes_NextGM(int32 RoundTime);

	UFUNCTION(BlueprintCallable)
	void SetRoundNumberPerHalf_NextGM(int32 RoundNumberPerHalf);

protected:
	virtual void BeginPlay() override;
	virtual void EndOfRound() override;
	
	bool bUnlimitedMoney_NextGM;
	bool bUnlimitedRounds_NextGM;
	int32 RoundTimeInMinutes_NextGM;
	int32 RoundNumberPerHalf_NextGM;
};
