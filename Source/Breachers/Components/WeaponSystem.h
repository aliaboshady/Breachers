#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponSystem.generated.h"

class AWeaponBase;
class ACharacterBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BREACHERS_API UWeaponSystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	UWeaponSystem();
	void SetPlayerInputComponent(UInputComponent* PlayerInputComponent);
	void EquipWeapon(AWeaponBase* Weapon);
	bool CanTakeWeapon(AWeaponBase* Weapon);
	void TakeWeapon(AWeaponBase* Weapon);
	void DropWeapon();
	AWeaponBase* GetCurrentWeapon();

	UPROPERTY(EditAnywhere)
	float WeaponThrowForce;

protected:
	virtual void BeginPlay() override;
	FAttachmentTransformRules CreateAttachRules();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void EquipSecondaryAtStartUp();
	void UnequipWeapon(AWeaponBase* Weapon);
	void FinishEquip();

	UFUNCTION(Server, Reliable)
	void Server_EquipPreviousWeapon();

	UFUNCTION(Server, Reliable)
	void Server_EquipPrimary();

	UFUNCTION(Server, Reliable)
	void Server_EquipSecondary();

	UFUNCTION(Server, Reliable)
	void Server_EquipMelee();

	UFUNCTION(Server, Reliable)
	void Server_TakeWeapon(AWeaponBase* Weapon);

	UFUNCTION(Server, Reliable)
	void Server_DropWeapon();

	UFUNCTION(Server, Reliable)
	void Server_StartFire();

	UFUNCTION(Server, Reliable)
	void Server_StopFire();
	
	UFUNCTION(Server, Reliable)
	void Server_SecondaryFire();
	
	UFUNCTION(Server, Reliable)
	void Server_SpawnStartWeapons();

	UFUNCTION(Client, Reliable)
	void Client_EquipWeaponVisualsFP(AWeaponBase* Weapon);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_EquipWeaponVisualsTP(AWeaponBase* Weapon);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_DropWeaponVisualsTP(AWeaponBase* Weapon);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_HideWeapon(AWeaponBase* Weapon, bool bHidden);

	UFUNCTION(Server, Reliable)
	void Server_Reload();

	UFUNCTION(Server, Reliable)
	void Server_FinishReload();

	UFUNCTION(Server, Reliable)
	void Server_CancelReload();


	void Fire();
	void FireSpread();
	void ResetCanFire();

	UPROPERTY(Replicated)
	ACharacterBase* CharacterPlayer;

	UPROPERTY(Replicated, BlueprintReadOnly)
	AWeaponBase* PreviousWeapon;
	
	UPROPERTY(Replicated, BlueprintReadOnly)
	AWeaponBase* CurrentWeapon;

	UPROPERTY(Replicated)
	AWeaponBase* PrimaryWeapon;

	UPROPERTY(Replicated)
	AWeaponBase* SecondaryWeapon;

	UPROPERTY(Replicated)
	AWeaponBase* MeleeWeapon;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeaponBase> MeleeWeaponClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeaponBase> PistolWeaponClass;

	FTimerHandle StartFireTimer;
	FTimerHandle ReloadTimer;
	FTimerHandle EquipTimer;
	bool bCanFire;
	bool bIsReloading;
	bool bIsEquipping;
};