#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

class ACharacterBase;
class USphereComponent;

UCLASS()
class BREACHERS_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeaponBase();
	void GetEquiped(ACharacterBase* Player);

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* Mesh_FP;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* Mesh_TP;

protected:
	virtual void BeginPlay() override;

	// UPROPERTY(EditAnywhere)
	// FWeaponInfo WeaponInfo;

	UPROPERTY(EditAnywhere)
	bool bIsRifle;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* SphereComponent;

	UFUNCTION()
	void OnOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
};
