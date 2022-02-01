#pragma once
#include "CoreMinimal.h"
#include "BreachersGameState.h"
#include "PlantAndDefuseGameState.generated.h"

UCLASS()
class BREACHERS_API APlantAndDefuseGameState : public ABreachersGameState
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
};
