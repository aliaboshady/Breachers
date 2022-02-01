#pragma once
#include "CoreMinimal.h"
#include "BreachersGameModeBase.h"
#include "PlantAndDefuseGameMode.generated.h"

UCLASS()
class BREACHERS_API APlantAndDefuseGameMode : public ABreachersGameModeBase
{
	GENERATED_BODY()

protected:
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
};
