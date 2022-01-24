#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BreachersGameModeBase.generated.h"

class APlayerStart;
class ACharacterBase;
class ABreachersPlayerController;

UCLASS()
class BREACHERS_API ABreachersGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ABreachersGameModeBase();
	FORCEINLINE int32 GetStartUpMoney(){return StartUpMoney;}
	FORCEINLINE int32 GetMaxMoney(){return MaxMoney;}
	FORCEINLINE bool GetIsUnlimitedMoney(){return bUnlimitedMoney;}
	FORCEINLINE int32 GetRoundTimeInMinutes(){return RoundTimeInMinutes;}
	void EndOfMatch();

	UFUNCTION()
	void RequestAttackerSpawn(AController* Controller);
	
	UFUNCTION()
	void RequestDefenderSpawn(AController* Controller);
	
	virtual void OnPlayerDied(ABreachersPlayerController* Controller);

	FORCEINLINE bool IsFirendlyFireOn(){return bFriendlyFireOn;}

protected:
	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	void GetPlayersStarts();
	void SpawnCharacter(TSubclassOf<ACharacterBase> CharacterClass, FTransform SpawnTransform, AController* Controller, FString Tag) const;
	void EndServer();
	void SlowDownTime();
	void SetNormalTimeSpeed();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bFriendlyFireOn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bUnlimitedMoney;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 StartUpMoney;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MaxMoney;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 RoundTimeInMinutes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SlowDilationFactor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SlowDilationTime;

	UPROPERTY()
	TArray<AActor*> AttackerSpawns;
	
	UPROPERTY()
	TArray<AActor*> DefenderSpawns;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ACharacterBase> AttackerClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ACharacterBase> DefenderClass;
};
