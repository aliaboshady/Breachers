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
	bool HasPrimaryWeapon() const;
	bool HasSecondaryWeapon() const;
	void EquipWeapon(AWeaponBase* Weapon);

protected:
	virtual void BeginPlay() override;
	FAttachmentTransformRules CreateAttachRules();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable)
	void Server_EquipWeapon(AWeaponBase* Weapon);

	UFUNCTION(Client, Reliable)
	void Client_EquipWeaponVisualsFP(AWeaponBase* Weapon);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_EquipWeaponVisualsTP(AWeaponBase* Weapon);

	UPROPERTY(Replicated)
	AWeaponBase* CurrentWeapon;

	UPROPERTY(Replicated)
	AWeaponBase* PrimaryWeapon;

	UPROPERTY(Replicated)
	AWeaponBase* SecondaryWeapon;

	UPROPERTY(Replicated)
	ACharacterBase* CharacterPlayer;
};
