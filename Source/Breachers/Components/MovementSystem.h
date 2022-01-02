#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MovementSystem.generated.h"

class ACharacterBase;
class UCharacterMovementComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BREACHERS_API UMovementSystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	UMovementSystem();
	void SetPlayerInputComponent(UInputComponent* PlayerInputComponent);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CrouchSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float WalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RunSpeed;

protected:
	virtual void BeginPlay() override;
	
	void MoveForward(float Value);
	void MoveRight(float Value);
	
	void LookUp(float Value);
	void Turn(float Value);

	void Jump();
	void StopJumping();

	void StartCrouch();
	void StopCrouch();

	UFUNCTION(Client, Reliable)
	void Client_StartWalk();
	UFUNCTION(Server, Reliable)
	void Server_StartWalk();
	void StartWalk();

	UFUNCTION(Client, Reliable)
	void Client_StopWalk();
	UFUNCTION(Server, Reliable)
	void Server_StopWalk();
	void StopWalk();

	UPROPERTY()
	ACharacterBase* CharacterPlayer;
};
