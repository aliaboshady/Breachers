#pragma once
#include "CoreMinimal.h"
#include "BreachersGameModeBase.h"
#include "PlantAndDefuseGameMode.generated.h"

UCLASS()
class BREACHERS_API APlantAndDefuseGameMode : public ABreachersGameModeBase
{
	GENERATED_BODY()

public:
	virtual void OnPlayerDied(ABreachersPlayerController* Controller, ETeam NextTeamRespawn) override;
	void EndOfRound();

protected:
	virtual void BeginPlay() override;
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	void ShuffleSpawnPoints(TArray<AActor*>& SpawnArray);
	virtual FTransform GetSpawnTransform(TArray<AActor*> &SpawnPoints, ETeam NextTeamRespawn = Attacker) override;
	
	UFUNCTION()
	void SpawnPlayerWithSelectedTeam(APlayerController* NewPlayer);
	
	void RestartRound();
	void RespawnALlPlayers();
	void RemoveAllUnpossessedBodies();
	void RemoveAllUnownedWeapons();
	void RestartCountDownTimer();
	
	UFUNCTION(BlueprintCallable)
	void SetUnlimitedMoney(bool bIsUnlimitedMoney);

	UFUNCTION(BlueprintCallable)
	void SetUnlimitedRounds(bool bIsUnlimitedRounds);

	UFUNCTION(BlueprintCallable)
	void SetRoundTime(int32 RoundTime);

	UFUNCTION(BlueprintCallable)
	void SetRoundNumber(int32 RoundsCount);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bUnlimitedRounds;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 RoundsNumber;

	int32 AttackerSpawnIndex;
	int32 DefenderSpawnIndex;
};
