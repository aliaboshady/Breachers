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
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	bool IsTimeIsFinished();
	
	UFUNCTION()
	void OnRep_CountDownTimeSpan();
	
	UPROPERTY(ReplicatedUsing=OnRep_CountDownTimeSpan)
	FTimespan CountDownTimeSpan;
	FTimespan OneSecondTimespan;
	FTimerHandle CountDownTimerHandle;

	bool bUnlimitedTime;
};
