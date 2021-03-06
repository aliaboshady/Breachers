#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "BreachersGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCountDownChange, FTimespan , CountDownTimeSpan);

class ABreachersPlayerState;

UCLASS()
class BREACHERS_API ABreachersGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	void PlayerOnDied(AController* Killer, AActor* DamageCauser, ABreachersPlayerState* KilledPlayerState);
	void OnScoreBoardChange();

	UFUNCTION(BlueprintCallable)
	virtual TArray<ABreachersPlayerState*> GetSortedPlayersBasedOnKillCount();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void EndOfRound();
	virtual void EndOfMatch();
	bool IsRoundTimeIsFinished();
	void AddToAllPlayersKillfeed(FName KillerName, bool bKillerIsAttacker, UTexture2D* WeaponIcon, FName KilledName, bool bKilledIsAttacker);
	
	UPROPERTY(BlueprintAssignable)
	FOnCountDownChange OnCountDownChange;

	UFUNCTION(NetMulticast, Reliable)
	virtual void Multicast_DecrementCountdownTime();
	
	UPROPERTY(Replicated, BlueprintReadWrite)
	FTimespan CountDownTimeSpan;
	FTimespan OneSecondTimespan;
	FTimerHandle CountDownTimerHandle;
};
