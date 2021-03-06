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

	UFUNCTION(BlueprintCallable)
	void SetGameModeRef_NextGM(FString GameModeRef);

protected:
	virtual void BeginPlay() override;
	virtual void EndOfRound() override;

	UFUNCTION(BlueprintImplementableEvent)
	void GoToNextGameMode();

	UPROPERTY(BlueprintReadOnly)
	bool bUnlimitedMoney_NextGM;

	UPROPERTY(BlueprintReadOnly)
	bool bUnlimitedRounds_NextGM;

	UPROPERTY(BlueprintReadOnly)
	int32 RoundTimeInMinutes_NextGM;

	UPROPERTY(BlueprintReadOnly)
	int32 RoundNumberPerHalf_NextGM;

	UPROPERTY(BlueprintReadOnly)
	FString GameModeRef_NextGM;
};
