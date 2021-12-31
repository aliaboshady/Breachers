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
	void EquipWeapon(AWeaponBase* Weapon);
	bool CanTakeWeapon(AWeaponBase* Weapon);
	void TakeWeapon(AWeaponBase* Weapon);
	void EquipPrimary();
	void EquipSecondary();
	void EquipMelee();
	void StartFire();
	void StopFire();
	AWeaponBase* GetCurrentWeapon();

protected:
	virtual void BeginPlay() override;
	FAttachmentTransformRules CreateAttachRules();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void EquipKnife();
	void UnequipWeapon(AWeaponBase* Weapon);
	
	UFUNCTION(Server, Reliable)
	void Server_SpawnWeapon(TSubclassOf<AWeaponBase> WeaponClass);

	UFUNCTION(Client, Reliable)
	void Client_EquipWeaponVisualsFP(AWeaponBase* Weapon);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_EquipWeaponVisualsTP(AWeaponBase* Weapon);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_HideWeapon(AWeaponBase* Weapon, bool bHidden);

	void Fire();
	void ResetCanFire();

	UPROPERTY()
	ACharacterBase* CharacterPlayer;

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

	FTimerHandle StartFireTimer;
	bool bCanFire;
};