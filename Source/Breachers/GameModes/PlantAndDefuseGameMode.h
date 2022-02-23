#pragma once
#include "CoreMinimal.h"
#include "BreachersGameModeBase.h"
#include "Breachers/Widgets/GamePhaseBanner.h"
#include "PlantAndDefuseGameMode.generated.h"

class ABomb;
class ABreachersPlayerState;

UCLASS()
class BREACHERS_API APlantAndDefuseGameMode : public ABreachersGameModeBase
{
	GENERATED_BODY()

public:
	APlantAndDefuseGameMode();
	virtual void OnPlayerDied(ABreachersPlayerController* Controller, ETeam NextTeamRespawn) override;
	void EndOfRound(bool bAttackersWon);
	FORCEINLINE int32 GetBuyPhaseTimeInSeconds(){return BuyPhaseTimeInSeconds;}
	FORCEINLINE int32 GetEndPhaseTimeInSeconds(){return EndPhaseTimeInSeconds;}
	FORCEINLINE int32 GetPlantTimeInSeconds(){return PlantTimeInSeconds;}
	FORCEINLINE int32 GetDefuseTimeInSeconds(){return DefuseTimeInSeconds;}
	FORCEINLINE int32 GetDetonateTimeInSeconds(){return DetonateTimeInSeconds;}

	void StartBuyPhase();
	void StartMainPhase();
	void StartEndPhase(bool bAttackersWin);

	void PlantBomb(ABomb* Bomb, ACharacterBase* CharacterPlayer);
	void DefuseBomb();

protected:
	virtual void BeginPlay() override;
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	void ShuffleSpawnPoints(TArray<AActor*>& SpawnArray);
	virtual FTransform GetSpawnTransform(TArray<AActor*> &SpawnPoints, ETeam NextTeamRespawn = Attacker) override;
	void PinBomb(ABomb* Bomb, ACharacterBase* CharacterPlayer);
	void GetBombSpawnLocations();
	void SpawnBombAtRandomLocation();
	void RemoveOldBombFromMap();
	
	UFUNCTION()
	void SpawnPlayerWithSelectedTeam(APlayerController* NewPlayer);

	void RestartRound(bool bAttackersWon);
	void RespawnALlPlayers(bool bAttackersWon);
	void RemoveAllUnpossessedBodies();
	void RemoveAllUnownedWeapons();
	void RestartCountDownTimer();
	void DestroyAllParticleSystems();
	void BuyPhasePlayerConstrains(bool bIsInBuyPhase);
	void SetPhaseBanner(EPhaseBanner PhaseBanner);
	void CheckHalfTime();
	void CheckEndOfGame();
	void SwitchTeams(ABreachersPlayerController* BPC, ETeam PlayerTeam);
	void AddMoneyToPlayer(ETeam PlayerTeam, bool bAttackersWon, ABreachersPlayerController* BPC);
	void RespawnPlayer(ETeam PlayerTeam, ABreachersPlayerController* BPC, ABreachersPlayerState* BPS, bool bForceRespawn = false);
	
	UFUNCTION(BlueprintCallable)
	void SetUnlimitedMoney(bool bIsUnlimitedMoney);

	UFUNCTION(BlueprintCallable)
	void SetUnlimitedRounds(bool bIsUnlimitedRounds);

	UFUNCTION(BlueprintCallable)
	void SetRoundTime(int32 RoundTime);

	UFUNCTION(BlueprintCallable)
	void SetRoundNumber(int32 RoundsCount);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 PlantTimeInSeconds;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 DefuseTimeInSeconds;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 DetonateTimeInSeconds;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 BuyPhaseTimeInSeconds;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 EndPhaseTimeInSeconds;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bUnlimitedRounds;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 RoundsNumber;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 WinMoney;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 LoseMoney;

	int32 AttackerSpawnIndex;
	int32 DefenderSpawnIndex;
	bool bShouldSwitchTeams;
	bool bHasSwitchedTeams;

	UPROPERTY()
	TArray<AActor*> BombSpawns;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ABomb> BombClass;
};
