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
	FORCEINLINE bool GetIsUnlimitedTime(){return bUnlimitedTime;}

protected:
	virtual void RequestAttackerSpawn(AController* Controller) override;
	virtual void RequestDefenderSpawn(AController* Controller) override;
	void DestroyCorpseAndWeapons(AController* Controller);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bUnlimitedTime;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RespawnTime;
};
