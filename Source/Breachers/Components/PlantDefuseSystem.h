#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlantDefuseSystem.generated.h"

class ABomb;
class ACharacterBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BREACHERS_API UPlantDefuseSystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPlantDefuseSystem();
	void SetPlayerInputComponent(UInputComponent* PlayerInputComponent);
	void SetBombToDefuse(ABomb* BombToDefuse);
	void UnsetBombToDefuse();
	void OnPlayerEnterSite();
	void OnPlayerExitSite();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void GetCharacterTag();
	void StopPlantOrDefuse();
	bool IsStraightLineToBomb();
	void StartPlant(int32 PlantTime);
	void StartDefuse(int32 DefuseTime);
	void SetPlayerConstraints(bool bPlantingOrDefusing);

	UFUNCTION(Server, Reliable)
	void Server_StartPlantOrDefuse();

	UFUNCTION(Server, Reliable)
	void Server_Plant();
	
	UFUNCTION(Server, Reliable)
	void Server_Defuse();

	UFUNCTION(Server, Reliable)
	void Server_OnPlayerEnterSite();
	UFUNCTION(Client, Reliable)
	void Client_OnPlayerEnterSite();

	UFUNCTION(Server, Reliable)
	void Server_OnPlayerExitSite();
	UFUNCTION(Client, Reliable)
	void Client_OnPlayerExitSite();

	UFUNCTION(Server, Reliable)
	void Server_SetBombToDefuse(ABomb* BombToDefuse);
	UFUNCTION(Client, Reliable)
	void Client_SetBombToDefuse(ABomb* BombToDefuse);

	UFUNCTION(Server, Reliable)
	void Server_UnsetBombToDefuse();
	UFUNCTION(Client, Reliable)
	void Client_UnsetBombToDefuse();

	UPROPERTY(Replicated)
	ACharacterBase* CharacterPlayer;

	UPROPERTY(Replicated)
	ABomb* Bomb;

	UPROPERTY(Replicated)
	bool bIsPlanting;
	
	UPROPERTY(Replicated)
	bool bIsDefusing;
	
	UPROPERTY(Replicated)
	bool bIsPlanter;

	UPROPERTY(Replicated)
	bool bIsInSite;

	UPROPERTY(Replicated)
	bool bIsNearBomb;

	FTimerHandle PlantOrDefuseTimerHandle;
};
