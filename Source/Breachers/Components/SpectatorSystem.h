#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpectatorSystem.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BREACHERS_API USpectatorSystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	USpectatorSystem();

protected:
	virtual void BeginPlay() override;
};
