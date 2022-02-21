#pragma once
#include "CoreMinimal.h"
#include "BreachersGameState.h"
#include "PlantAndDefuseGameState.generated.h"

class USoundCue;

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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTeamsNumberChange, int32, AttackersCount, int32 , DefendersCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTeamsScoreChange, int32, AttackersScore, int32 , DefendersScore);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRoundState, ERoundState, BombState);

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
	void ShowTeamsCountUI();
	
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Multicast_DecrementCountdownTime_Implementation() override;
	void SetBombDetonateTimer();
	void CheckPlayersCount();
	void OnFullTeamKilled(bool bAttackersWon);
	void GetTeamsCount(int32& AttackersCount, int32& DefendersCount);

	UPROPERTY(BlueprintAssignable)
	FOnTeamsNumberChange OnTeamsNumberChange;
	
	UPROPERTY(BlueprintAssignable)
	FOnTeamsScoreChange OnTeamsScoreChange;
	
	UPROPERTY(BlueprintAssignable)
	FOnRoundState OnRoundStateChange;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnCountDownChange(int32 AttackersCount, int32 DefendersCount);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ShowTeamsCountUI();
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ChangeCurrentGamePhase(EPhase NewGamePhase);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ChangeCurrentRoundState(ERoundState NewRoundState);
	
	UFUNCTION(Server, Reliable)
	void Server_SetWinnerTeam(bool bDidAttackersWin);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetWinnerTeam(bool bDidAttackersWin);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetBombPlantedTimer();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_IncreaseAttackersScore();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_IncreaseDefendersScore();

	UFUNCTION(Server, Reliable)
	void Server_SetCanSetWinnerTeam(bool bCanSetWinner);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetCanSetWinnerTeam(bool bCanSetWinner);

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_PlaySound(USoundCue* Sound);

	bool TimeRanOutBothTeamsAlive();
	
	void StartBuyPhase();
	void EndOfBuyPhase();

	void StartMainPhase();
	void EndOfMainPhase();

	void StartEndPhase();
	
	virtual void BeginPlay() override;
	virtual void EndOfRound() override;

	UPROPERTY(EditAnywhere)
	USoundCue* BombPlantedSound;
	
	UPROPERTY(EditAnywhere)
	USoundCue* BombDefusedSound;

	UPROPERTY(Replicated)
	TEnumAsByte<EPhase> CurrentGamePhase;

	UPROPERTY(Replicated)
	TEnumAsByte<ERoundState> CurrentRoundState;

	UPROPERTY(Replicated)
	bool bAttackersWin;

	UPROPERTY(Replicated)
	bool bCanSetWinnerTeam;

	int32 BombDetonateTime;

	UPROPERTY(Replicated)
	int32 AttackersScore;
	
	UPROPERTY(Replicated)
	int32 DefendersScore;
};
