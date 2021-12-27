#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterBase.generated.h"

class UMovementSystem;
class UCameraComponent;

UCLASS()
class BREACHERS_API ACharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	ACharacterBase();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	UMovementSystem* MovementSystem;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void StartCrouch();
	void StopCrouch();

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* Arms_FP;
};
