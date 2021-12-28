#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterBase.generated.h"

class AWeaponBase;
class UWeaponSystem;
class UMovementSystem;
class UCameraComponent;

UCLASS()
class BREACHERS_API ACharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	ACharacterBase();
	USkeletalMeshComponent* GetArmsMeshFP();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void EquipWeapon(AWeaponBase* Weapon);

	UPROPERTY(VisibleAnywhere)
	UWeaponSystem* WeaponSystem;

	bool CanTakeWeapon(AWeaponBase* Weapon);
	void TakeWeapon(AWeaponBase* Weapon);

	UFUNCTION(Server, Reliable)
	void Server_TakeWeapon(AWeaponBase* Weapon);

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void MoveForward(float Value);
	void MoveRight(float Value);
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

	UPROPERTY(VisibleAnywhere)
	UMovementSystem* MovementSystem;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* Arms_FP;

	UPROPERTY(EditAnywhere)
	float CrouchSpeed;
	
	UPROPERTY(EditAnywhere)
	float WalkSpeed;

	UPROPERTY(EditAnywhere)
	float RunSpeed;
};
