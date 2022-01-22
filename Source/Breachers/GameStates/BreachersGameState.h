#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "BreachersGameState.generated.h"

UCLASS()
class BREACHERS_API ABreachersGameState : public AGameStateBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	bool IsRoundTimeIsFinished();
	
	UFUNCTION()
	void OnRep_CountDownTimeSpan();
	
	UPROPERTY(ReplicatedUsing=OnRep_CountDownTimeSpan)
	FTimespan CountDownTimeSpan;
	FTimespan OneSecondTimespan;
	FTimerHandle CountDownTimerHandle;
};
