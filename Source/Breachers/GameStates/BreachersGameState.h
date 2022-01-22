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
	bool IsRoundTimeIsFinished();
	
	UPROPERTY(BlueprintAssignable)
	FOnCountDownChange OnCountDownChange;
	
	UFUNCTION()
	virtual void OnRep_CountDownTimeSpan();
	
	UPROPERTY(ReplicatedUsing=OnRep_CountDownTimeSpan)
	FTimespan CountDownTimeSpan;
	FTimespan OneSecondTimespan;
	FTimerHandle CountDownTimerHandle;
};
