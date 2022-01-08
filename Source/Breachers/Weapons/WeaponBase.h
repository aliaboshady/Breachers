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
	void OnTaken();
	void OnFire();

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* Mesh_FP;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* Mesh_TP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	FWeaponInfo WeaponInfo;

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	FVector RecoilShot(float Spread) const;
	
	int32 GetSurfaceDamage(FHitResult OutHit) const;
	
	UFUNCTION(Client, Reliable)
	void Client_OnFire();

	UFUNCTION(Server, Reliable)
	void Server_ProcessShot(FHitResult OutHit);

	UFUNCTION(Server, Reliable)
	void Server_OnFireEffects(FHitResult OutHit);

	UFUNCTION(Client, Reliable)
	void Client_OnFireEffects();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnFireEffects(FHitResult OutHit);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SpawnBulletHoleDecal(FHitResult OutHit);

	UPROPERTY(Replicated)
	ACharacterBase* CharacterPlayer;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* SphereComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TraceLength;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BulletRadius;

	UFUNCTION()
	void OnOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
};
