#pragma once
#include "CoreMinimal.h"
#include "BreachersGameModeBase.h"
#include "DeathMatchGameMode.generated.h"

UCLASS()
class BREACHERS_API ADeathMatchGameMode : public ABreachersGameModeBase
{
	GENERATED_BODY()

public:
	ADeathMatchGameMode();
	virtual void OnPlayerDied(ABreachersPlayerController* Controller) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RespawnTime;
};
