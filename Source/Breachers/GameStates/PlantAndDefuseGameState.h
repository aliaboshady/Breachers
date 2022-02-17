#pragma once
#include "CoreMinimal.h"
#include "BreachersGameState.h"
#include "PlantAndDefuseGameState.generated.h"

UENUM(BlueprintType)
enum ERoundState
{
	BombUnplanted,
	BombPlanted,
	BombDefused,
	BombExploded
};

UENUM(BlueprintType)
enum EPhase
{
	BuyPhase,
	MainPhase,
	EndPhase
};

UCLASS()
class BREACHERS_API APlantAndDefuseGameState : public ABreachersGameState
{
	GENERATED_BODY()

public:
	APlantAndDefuseGameState();
	void StartCountDownTimer();
	FORCEINLINE EPhase GetCurrentGamePhase(){return CurrentGamePhase;}
	FORCEINLINE ERoundState GetCurrentRoundState(){return CurrentRoundState;}
	FORCEINLINE void SetCurrentRoundState(ERoundState NewRoundState){CurrentRoundState = NewRoundState;}
	
	void OnPlayerDied();
	void OnPlantBomb();
	void OnDefuseBomb();
	void OnBombExploded();
	
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void Multicast_DecrementCountdownTime_Implementation() override;
	void SetBombDetonateTimer();
	void CheckPlayersCount();
	void OnFullTeamKilled(bool bAttackersWon);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ChangeCurrentGamePhase(EPhase NewGamePhase);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ChangeCurrentRoundState(ERoundState NewRoundState);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetWinnerTeam(bool bDidAttackersWin);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetBombPlantedTimer();
	
	void StartBuyPhase();
	void EndOfBuyPhase();

	void StartMainPhase();
	void EndOfMainPhase();

	void StartEndPhase();
	
	virtual void BeginPlay() override;
	virtual void EndOfRound() override;

	UPROPERTY(Replicated)
	TEnumAsByte<EPhase> CurrentGamePhase;

	UPROPERTY(Replicated)
	TEnumAsByte<ERoundState> CurrentRoundState;

	UPROPERTY(Replicated)
	bool bAttackersWin;

	int32 BombDetonateTime;
};
