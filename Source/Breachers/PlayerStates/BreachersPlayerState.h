#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BreachersPlayerState.generated.h"

class ABreachersGameState;

UCLASS()
class BREACHERS_API ABreachersPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ABreachersPlayerState();
	void OnDie(AController* InstigatedBy, AActor* DamageCauser);
	void OnKill();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE int32 GetKillsCount(){return Kills;}
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE int32 GetDeathsCount(){return Deaths;}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE bool GetIsDead(){return bIsDead;}

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnSpawn();

	UPROPERTY(Replicated)
	int32 Deaths;

	UPROPERTY(Replicated)
	int32 Kills;

	UPROPERTY(Replicated)
	bool bIsDead;

	UPROPERTY()
	ABreachersGameState* BreachersGameState;
};
