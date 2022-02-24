#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpectatorSystem.generated.h"

class ACharacterBase;
class APlantAndDefusePlayerController;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BREACHERS_API USpectatorSystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	USpectatorSystem();
	void SpectateNext();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void GetValidPlayersToSpectate(TArray<ACharacterBase*>& ValidPlayers);

	UPROPERTY(Replicated)
	APlantAndDefusePlayerController* PC;
};
