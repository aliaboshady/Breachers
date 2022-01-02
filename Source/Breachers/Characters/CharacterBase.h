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
	USkeletalMeshComponent* GetArmsMeshFP() const;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	FVector GetCameraLocation() const;
	FVector GetCameraDirection() const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWeaponSystem* WeaponSystem;

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(VisibleAnywhere)
	UMovementSystem* MovementSystem;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComponent;
	
	UPROPERTY(VisibleAnywhere)
	UHealthSystem* HealthSystem;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* Arms_FP;
};
