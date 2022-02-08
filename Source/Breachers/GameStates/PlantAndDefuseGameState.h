#pragma once
#include "CoreMinimal.h"
#include "BreachersGameState.h"
#include "PlantAndDefuseGameState.generated.h"

UENUM(BlueprintType)
enum ERoundState
{
	BombUnplanted,
	BombPlanted,
	BombDefused,
	BombExploded
};

UENUM(BlueprintType)
enum EPhase
{
	BuyPhase,
	MainPhase,
	EndPhase
};

UCLASS()
class BREACHERS_API APlantAndDefuseGameState : public ABreachersGameState
{
	GENERATED_BODY()

public:
	void StartCountDownTimer();
	FORCEINLINE EPhase GetCurrentGamePhase(){return CurrentGamePhase;}
	FORCEINLINE void SetCurrentRoundState(ERoundState NewRoundState){CurrentRoundState = NewRoundState;}
	
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Multicast_DecrementCountdownTime_Implementation() override;
	
	void StartBuyPhase();
	void EndOfBuyPhase();

	void StartMainPhase();
	void EndOfMainPhase();

	void StartEndPhase();
	
	virtual void BeginPlay() override;
	virtual void EndOfRound() override;

	UPROPERTY(Replicated)
	TEnumAsByte<EPhase> CurrentGamePhase;

	UPROPERTY(Replicated)
	TEnumAsByte<ERoundState> CurrentRoundState;
};
