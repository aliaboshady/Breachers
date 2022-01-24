#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BreachersPlayerState.generated.h"

UCLASS()
class BREACHERS_API ABreachersPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ABreachersPlayerState();
	void OnDie();
	void OnKill();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(Replicated)
	int32 Deaths;

	UPROPERTY(Replicated)
	int32 Kills;
};
