#pragma once
#include "CoreMinimal.h"
#include "WeaponInfo.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	FWeaponInfo WeaponInfo;

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* SphereComponent;

	UFUNCTION()
	void OnOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
};
