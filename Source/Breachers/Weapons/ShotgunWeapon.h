#pragma once
#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "ShotgunWeapon.generated.h"

UCLASS()
class BREACHERS_API AShotgunWeapon : public AWeaponBase
{
	GENERATED_BODY()

protected:
	virtual void FireSpread() override;
};
