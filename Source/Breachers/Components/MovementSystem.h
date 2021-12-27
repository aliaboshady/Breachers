#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MovementSystem.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BREACHERS_API UMovementSystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	UMovementSystem();
	void MoveForward(float Value, APawn* Pawn);
	void MoveRight(float Value, APawn* Pawn);

protected:
	virtual void BeginPlay() override;
};
