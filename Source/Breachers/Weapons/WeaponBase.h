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
	void Reload();
	void FinishReload();
	FORCEINLINE int32 GetCurrentAmmoInClip() const {return CurrentAmmoInClip;}
	FORCEINLINE int32 GetCurrentTotalAmmo() const {return CurrentTotalAmmo;}

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* Mesh_FP;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* Mesh_TP;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	FWeaponInfo WeaponInfo;

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	FVector RecoilShot(float Spread) const;
	void SetupWeaponInfo();
	
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

	UFUNCTION(Client, Reliable)
	void Client_OnReloadEffects();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnReloadEffects();

	UPROPERTY(Replicated)
	ACharacterBase* CharacterPlayer;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* SphereComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName DataTableKey;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UDataTable* WeaponInfoDataTable;

	UFUNCTION()
	void OnOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	
	UPROPERTY(Replicated)
	int32 CurrentTotalAmmo;
	
	UPROPERTY(Replicated)
	int32 CurrentAmmoInClip;
};
