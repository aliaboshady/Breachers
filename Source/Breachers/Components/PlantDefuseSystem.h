#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlantDefuseSystem.generated.h"

class ACharacterBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BREACHERS_API UPlantDefuseSystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPlantDefuseSystem();
	void SetPlayerInputComponent(UInputComponent* PlayerInputComponent);

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void GetCharacterTag();
	void ToPlantOrDefuse();

	UFUNCTION(Server, Reliable)
	void Server_Plant();
	
	UFUNCTION(Server, Reliable)
	void Server_Defuse();

	UPROPERTY(Replicated)
	ACharacterBase* CharacterPlayer;
	
	UPROPERTY(Replicated)
	bool bIsPlanter;

	UPROPERTY(Replicated)
	bool bIsInSite;

	UPROPERTY(Replicated)
	bool bIsNearBomb;
};
