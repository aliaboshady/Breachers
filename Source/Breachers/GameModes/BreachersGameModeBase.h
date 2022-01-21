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

	UFUNCTION()
	void RequestAttackerSpawn(AController* Controller);
	
	UFUNCTION()
	void RequestDefenderSpawn(AController* Controller);
	
	void OnPlayerDied(ABreachersPlayerController* Controller);

protected:
	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	void GetPlayersStarts();
	void SpawnCharacter(TSubclassOf<ACharacterBase> CharacterClass, FTransform SpawnTransform, AController* Controller) const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RespawnTime;

	UPROPERTY()
	TArray<AActor*> AttackerSpawns;
	
	UPROPERTY()
	TArray<AActor*> DefenderSpawns;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ACharacterBase> AttackerClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ACharacterBase> DefenderClass;
};
