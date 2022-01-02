#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterBase.generated.h"

#define SURFACE_Head  SurfaceType1
#define SURFACE_Torso SurfaceType2
#define SURFACE_Arms  SurfaceType3
#define SURFACE_Legs  SurfaceType4

class AWeaponBase;
class UWeaponSystem;
class UHealthSystem;
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
	FVector GetCameraLocation();
	FVector GetCameraDirection();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWeaponSystem* WeaponSystem;

	bool CanTakeWeapon(AWeaponBase* Weapon);
	void TakeWeapon(AWeaponBase* Weapon);

	UFUNCTION(Server, Reliable)
	void Server_TakeWeapon(AWeaponBase* Weapon);

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void EquipPrimary();
	void EquipSecondary();
	void EquipMelee();

	UFUNCTION(Server, Reliable)
	void Server_StartFire();
	void StartFire();

	UFUNCTION(Server, Reliable)
	void Server_StopFire();
	void StopFire();

	UPROPERTY(VisibleAnywhere)
	UMovementSystem* MovementSystem;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* Arms_FP;
	
	UPROPERTY(VisibleAnywhere)
	UHealthSystem* HealthSystem;
};
