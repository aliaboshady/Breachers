#pragma once
#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "AssaultRifleWeapon.generated.h"

UCLASS()
class BREACHERS_API AAssaultRifleWeapon : public AWeaponBase
{
	GENERATED_BODY()

public:
	virtual void OnSecondaryFire() override;
};
