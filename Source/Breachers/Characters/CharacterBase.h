#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterBase.generated.h"

#define TAG_Player "Player"
#define TIME_PickWeaponAfterDrop 1

#define COLLISION_CharacterPlayerTP "CharacterPlayerTP"
#define COLLISION_ArmsFP "ArmsFP"
#define COLLISION_Ragdoll "Ragdoll"

#define SURFACE_Head  SurfaceType1
#define SURFACE_Torso SurfaceType2
#define SURFACE_Arms  SurfaceType3
#define SURFACE_Legs  SurfaceType4

#define INPUT_MoveForward	"MoveForward"
#define INPUT_MoveRight		"MoveRight"
#define INPUT_LookUp		"LookUp"
#define INPUT_Turn			"Turn"
#define INPUT_Jump			"Jump"
#define INPUT_Crouch		"Crouch"
#define INPUT_Walk			"Walk"

class AWeaponBase;
class UWeaponSystem;
class UHealthSystem;
class UMovementSystem;
class UCameraComponent;
class USpringArmComponent;
class ABreachersPlayerController;

UCLASS()
class BREACHERS_API ACharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	ACharacterBase();
	USkeletalMeshComponent* GetArmsMeshFP() const;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	FVector GetCameraLocation() const;
	FVector GetCameraDirection() const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWeaponSystem* WeaponSystem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UHealthSystem* HealthSystem;

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void SwitchToDeathCamera();
	void DeathCameraAnimation();
	FTimerHandle DeathTimerHandle;
	float DeathAnimationRate;
	void StopRagdollMovement() const;

	UFUNCTION(Server, Reliable)
	void Server_OnDie();
	
	UFUNCTION(Client, Reliable)
	void Client_OnDie_Visuals();
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnDie_Ragdoll();

	UPROPERTY(VisibleAnywhere)
	UMovementSystem* MovementSystem;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* DeathCameraComponent;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* DeathSpringArm;
	
	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* Arms_FP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DeathAnimationSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DeathAnimationDistance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DeathAnimationPitch;
	
	float StopRagdollMovementAfterTime;
};