#pragma once
#include "CoreMinimal.h"
#include "Breachers/Weapons/WeaponInfo.h"
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
	void EnableShooting(bool bEnableShooting);
	AWeaponBase* GetCurrentWeapon();
	void SpawnWeapon(TSubclassOf<AWeaponBase> WeaponClass, EWeaponType WeaponType = Melee);
	void DropAllWeapons();
	void DestroyAllWeapons();
	void OnRestartRound();
	FORCEINLINE bool HasBomb(){return Bomb ? true : false;}
	void SetIsPlantingOrDefusing(bool bPlantingOrDefusing);
	void DropBomb();
	void EquipPreviousWeapon();
	void EquipDefuser();
	AWeaponBase* GetBomb();

	UPROPERTY(EditAnywhere)
	float WeaponThrowForce;

protected:
	virtual void BeginPlay() override;
	FAttachmentTransformRules CreateAttachRules();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void EquipStartUpWeapons();
	void UnequipWeapon(AWeaponBase* Weapon);
	void EquipeLastTakenWeapon();

	UFUNCTION(Server, Reliable)
	void Server_PlayerDropWeapon();

	UFUNCTION(Server, Reliable)
	void Server_EquipPreviousWeapon();

	UFUNCTION(Server, Reliable)
	void Server_EquipWeapon(AWeaponBase* Weapon);
	
	UFUNCTION(Server, Reliable)
	void Server_EquipPrimary();

	UFUNCTION(Server, Reliable)
	void Server_EquipSecondary();

	UFUNCTION(Server, Reliable)
	void Server_EquipMelee();

	UFUNCTION(Server, Reliable)
	void Server_EquipDefuser();
	
	UFUNCTION(Server, Reliable)
	void Server_EquipBomb();

	UFUNCTION(Server, Reliable)
	void Server_EquipGrenade();

	UFUNCTION(Server, Reliable)
	void Server_EquipFlash();

	UFUNCTION(Server, Reliable)
	void Server_EquipSmoke();

	UFUNCTION(Server, Reliable)
	void Server_EquipMolotov();

	UFUNCTION(Server, Reliable)
	void Server_EquipLastBoughtWeapon();

	UFUNCTION(Server, Reliable)
	void Server_TakeLastBoughtWeapon();

	UFUNCTION(Server, Reliable)
	void Server_TakeWeapon(AWeaponBase* Weapon);
	
	UFUNCTION(Server, Reliable)
	void Server_PickWeapon(AWeaponBase* Weapon);

	UFUNCTION(Client, Reliable)
	void Client_PickWeapon();

	UFUNCTION(Server, Reliable)
	void Server_DropWeapon();

	UFUNCTION(Server, Reliable)
	void Server_StartPrimaryFire();

	UFUNCTION(Server, Reliable)
	void Server_StopPrimaryFire();
	
	UFUNCTION(Server, Reliable)
	void Server_SecondaryFire();
	
	UFUNCTION(Server, Reliable)
	void Server_SpawnStartWeapons();

	UFUNCTION(Server, Reliable)
	void Server_SpawnWeapon(TSubclassOf<AWeaponBase> WeaponClass, EWeaponType WeaponType = Melee);

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
	void Server_CancelReload();

	UPROPERTY(Replicated)
	ACharacterBase* CharacterPlayer;

	UPROPERTY(Replicated, BlueprintReadOnly)
	AWeaponBase* LastBoughtWeapon;

	UPROPERTY(Replicated, BlueprintReadOnly)
	AWeaponBase* LastTakenWeapon;

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

	UPROPERTY(Replicated)
	AWeaponBase* Bomb;

	UPROPERTY(Replicated)
	AWeaponBase* DefuseDevice;

	UPROPERTY(Replicated)
	AWeaponBase* Grenade;
	
	UPROPERTY(Replicated)
	AWeaponBase* Flash;
	
	UPROPERTY(Replicated)
	AWeaponBase* Smoke;
	
	UPROPERTY(Replicated)
	AWeaponBase* Molotov;

	UPROPERTY(EditAnywhere)
	float WeaponPickupDistance;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeaponBase> MeleeWeaponClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeaponBase> PistolWeaponClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeaponBase> DefuseDeviceClass;

	UPROPERTY(Replicated)
	bool bShootingEnabled;

	UPROPERTY(Replicated)
	bool bIsPlantingOrDefusing;
};
