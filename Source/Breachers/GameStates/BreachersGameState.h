#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "BreachersGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCountDownChange, FTimespan , CountDownTimeSpan);
UCLASS()
class BREACHERS_API ABreachersGameState : public AGameStateBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void EndOfRound();
	virtual void EndOfMatch();
	bool IsRoundTimeIsFinished();
	
	UPROPERTY(BlueprintAssignable)
	FOnCountDownChange OnCountDownChange;

	UFUNCTION(NetMulticast, Reliable)
	virtual void Multicast_DecrementCountdownTime();
	
	UPROPERTY(Replicated)
	FTimespan CountDownTimeSpan;
	FTimespan OneSecondTimespan;
	FTimerHandle CountDownTimerHandle;
};
