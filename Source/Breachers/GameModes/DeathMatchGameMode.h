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
	FORCEINLINE int32 GetMatchTimeInMinutes(){return MatchTimeInMinutes;}

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bUnlimitedTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MatchTimeInMinutes;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RespawnTime;
};
