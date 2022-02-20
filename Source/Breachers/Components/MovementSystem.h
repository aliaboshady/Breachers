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
	void SetSpeedsOfWeapon(float WeaponRunSpeed, float WeaponWalkSpeed, float WeaponCrouchSpeed);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CrouchSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float WalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RunSpeed;

	void SetMouseSensitivityFactor(float NewFactor);
	void SetCanMove(bool bCanMovePlayer);
	void SetIsPlantingOrDefusing(bool bPlantingOrDefusing);

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable)
	void Server_SetSpeedsOfWeapon(float WeaponRunSpeed, float WeaponWalkSpeed, float WeaponCrouchSpeed);

	UFUNCTION(Client, Reliable)
	void Client_SetSpeedsOfWeapon(float WeaponRunSpeed, float WeaponWalkSpeed, float WeaponCrouchSpeed);

	void PR_Client_SetSpeedsOfWeapon(float WeaponRunSpeed, float WeaponWalkSpeed, float WeaponCrouchSpeed);
	
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

	UFUNCTION(Client, Reliable)
	void Client_OnDie(AController* InstigatedBy, AActor* DamageCauser);

	UPROPERTY()
	ACharacterBase* CharacterPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float JumpMoveFactor;

	UPROPERTY(Replicated)
	bool bCanMove;
	
	UPROPERTY(Replicated)
	bool bIsPlantingOrDefusing;
	
	bool bCanTakeInput;
	bool bIsWalking;
	float MouseTurnValue;
	float MouseSensitivityFactor;
};
