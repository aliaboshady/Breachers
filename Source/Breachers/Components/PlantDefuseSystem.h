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
	void Plant();
	void Defuse();

	UPROPERTY(Replicated)
	ACharacterBase* CharacterPlayer;
	
	UPROPERTY(Replicated)
	bool bIsPlanter;
};
