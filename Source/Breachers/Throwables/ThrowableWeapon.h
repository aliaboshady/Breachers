#pragma once
#include "CoreMinimal.h"
#include "Breachers/Weapons/WeaponBase.h"
#include "ThrowableWeapon.generated.h"

UCLASS()
class BREACHERS_API AThrowableWeapon : public AWeaponBase
{
	GENERATED_BODY()

public:
	virtual void OnPrimaryFire() override;

protected:
	void EquipPreviousWeaponAfterThrow();
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayThrowAnimation();
};
