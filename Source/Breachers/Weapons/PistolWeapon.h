#pragma once
#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "PistolWeapon.generated.h"

UCLASS()
class BREACHERS_API APistolWeapon : public AWeaponBase
{
	GENERATED_BODY()

public:
	virtual void OnSecondaryFire() override;
};
