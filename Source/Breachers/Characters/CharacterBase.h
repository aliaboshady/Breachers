#pragma once
#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "CharacterBase.generated.h"

#define TAG_Player "Player"
#define TAG_Attacker "Attacker"
#define TAG_Defender "Defender"
#define TIME_PickWeaponAfterDrop 1

#define COLLISION_CharacterPlayerTP "CharacterPlayerTP"
#define COLLISION_ArmsFP "ArmsFP"
#define COLLISION_Ragdoll "Ragdoll"

#define SURFACE_Head  SurfaceType1
#define SURFACE_Torso SurfaceType2
#define SURFACE_Arms  SurfaceType3
#define SURFACE_Legs  SurfaceType4
#define SURFACE_Rock  SurfaceType10
#define SURFACE_Tile  SurfaceType11

#define INPUT_MoveForward	"MoveForward"
#define INPUT_MoveRight		"MoveRight"
#define INPUT_LookUp		"LookUp"
#define INPUT_Turn			"Turn"
#define INPUT_Jump			"Jump"
#define INPUT_Crouch		"Crouch"
#define INPUT_Walk			"Walk"
#define INPUT_Pause			"PauseMenu"
#define INPUT_ScoreBoard	"ScoreBoard"

UENUM(BlueprintType)
enum ETeam
{
	Attacker,
	Defender
};

class UBuyMenu;
class AWeaponBase;
class UMoneySystem;
class UWeaponSystem;
class UHealthSystem;
class UMovementSystem;
class UCameraComponent;
class USpringArmComponent;
class ABreachersPlayerController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLand);

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
	FORCEINLINE void SetCameraFOV(float FOV){CameraComponent->FieldOfView = FOV;}
	FORCEINLINE ABreachersPlayerController* GetBreacherPC(){return PC;}
	void PushOnDeath(AActor* DamageCauser, FVector PushDirection);
	FHitResult GetSurfaceType();

	UPROPERTY()
	FOnLand OnLand;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWeaponSystem* WeaponSystem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UHealthSystem* HealthSystem;
	
	UPROPERTY(VisibleAnywhere)
	UMovementSystem* MovementSystem;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<ETeam> Team;

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PossessedBy(AController* NewController) override;
	void SwitchToDeathCamera();
	void DeathCameraAnimation();
	FTimerHandle DeathTimerHandle;
	float DeathAnimationRate;
	void StopRagdollMovement() const;
	void ShowHideBuyMenu();
	void ShowHidePauseMenu();
	virtual void Landed(const FHitResult& Hit) override;
	void OpenScoreBoard();
	void CloseScoreBoard();

	void OnDie();
	
	UFUNCTION(Server, Reliable)
	void Server_OnDie(AController* InstigatedBy, AActor* DamageCauser);
	
	UFUNCTION(Client, Reliable)
	void Client_OnDie_Visuals();
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnDie_Ragdoll();
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PushOnDeath();

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* DeathCameraComponent;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* DeathSpringArm;
	
	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* Arms_FP;

	UPROPERTY(Replicated)
	ABreachersPlayerController* PC;

	UPROPERTY(Replicated)
	AActor* KillerWeapon;
	
	UPROPERTY(Replicated)
	FVector KillForceDirection;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DeathAnimationSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DeathAnimationDistance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DeathAnimationPitch;
	
	float StopRagdollMovementAfterTime;
};