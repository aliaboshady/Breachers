#pragma once
#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "SniperRifleWeapon.generated.h"

UCLASS()
class BREACHERS_API ASniperRifleWeapon : public AWeaponBase
{
	GENERATED_BODY()

	virtual void OnSecondaryFire(bool bShouldDecreaseBullets) override;
};
