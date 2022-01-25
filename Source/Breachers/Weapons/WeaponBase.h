#pragma once
#include "CoreMinimal.h"
#include "WeaponInfo.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

#define COLLISION_Weapon "Weapon"
#define COLLISION_WeaponOverlapSphere "WeaponOverlapSphere"
#define TRACE_WeaponPick ECC_GameTraceChannel1
#define TAG_Weapon "Weapon"
#define TAG_Primary "Primary"
#define TAG_Secondary "Primary"
#define TAG_Melee "Melee"

class ABulletTracer;
class ACharacterBase;
class USphereComponent;

UCLASS()
class BREACHERS_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeaponBase();
	virtual void OnTaken();
	virtual void OnDrop(ACharacterBase* DropperCharacter);
	virtual void OnPrimaryFire();
	virtual void OnSecondaryFire();
	virtual void OnStopFire();
	virtual void OnReload();
	void OnFinishReload();
	void OnCancelReload();
	virtual void OnEquip();
	virtual void OnUnquip();
	void OnCancelEquip();
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
	virtual FVector RecoilShot(float Spread);
	void SetupWeaponInfo();
	void PlatAnimationWithTime(UAnimMontage* AnimationMontage, USkeletalMeshComponent* Mesh, float Time);
	void CancelAllAnimations() const;

	void DecreaseRecoilTimePerShot();
	void WaitRecoilTimePerShot();

	UFUNCTION()
	virtual void OnDie(AController* InstigatedBy, AActor* DamageCauser);
	
	UFUNCTION(Client, Reliable)
	virtual void Client_Recoil(FVector CalculatedRecoil);

	UFUNCTION(Client, Reliable)
	void Client_ResetRecoil();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnDropEnableOverlap();
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnTaken();

	UFUNCTION(Server, Reliable)
	void Server_SpawnBulletTracer(FHitResult OutHit);
	
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
	void Client_OnReloadAnimations();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnReloadAnimations();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnCancelReloadAnimations();

	UFUNCTION(Client, Reliable)
	void Client_OnEquipAnimations();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnEquipAnimations();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnCancelEquipAnimations();

	void SpawnBulletTracer(FHitResult OutHit, FVector Start, bool bIsClient);
	
	UFUNCTION(Client, Reliable)
	void Client_SpawnBulletTracer(FHitResult OutHit);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SpawnBulletTracer(FHitResult OutHit);
	
	virtual void FireSpread();
	void ResetCanFire();
	void ResetIsFiring();

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
	
	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 CurrentTotalAmmo;
	
	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 CurrentAmmoInClip;

	UPROPERTY()
	ACharacterBase* PreviousOwner;

	FTimerHandle StartFireTimer;
	FTimerHandle ReloadTimer;
	FTimerHandle EquipTimer;
	FTimerHandle RecoilTimeHandle;
	FTimerHandle RecoilTimeWaitHandle;
	bool bCanFire;
	bool bIsFiring;
	bool bIsReloading;
	bool bIsEquipping;

	float RecoilTimePerShot;
};