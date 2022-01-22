#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "BreachersGameState.generated.h"

UCLASS()
class BREACHERS_API ABreachersGameState : public AGameStateBase
{
	GENERATED_BODY()

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
