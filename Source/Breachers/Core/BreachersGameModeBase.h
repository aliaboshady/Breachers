#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BreachersGameModeBase.generated.h"

class APlayerStart;
class ACharacterBase;

UCLASS()
class BREACHERS_API ABreachersGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	void RequestAttackerSpawn(AController* Controller);
	void RequestDefenderSpawn(AController* Controller);

protected:
	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	void GetPlayersStarts();
	void SpawnCharacter(TSubclassOf<ACharacterBase> CharacterClass, FTransform SpawnTransform, AController* Controller) const;

	UPROPERTY()
	TArray<AActor*> AttackerSpawns;
	
	UPROPERTY()
	TArray<AActor*> DefenderSpawns;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ACharacterBase> AttackerClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ACharacterBase> DefenderClass;
};
