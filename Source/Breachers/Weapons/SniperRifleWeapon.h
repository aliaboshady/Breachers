#pragma once
#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "SniperRifleWeapon.generated.h"

UCLASS()
class BREACHERS_API ASniperRifleWeapon : public AWeaponBase
{
	GENERATED_BODY()

public:
	ASniperRifleWeapon();

protected:
	virtual void OnPrimaryFire() override;
	virtual void OnSecondaryFire() override;
	virtual void OnReload() override;

	UFUNCTION(Client, Reliable)
	void Client_Scope();
	
	UFUNCTION(Client, Reliable)
	void Client_Unscope();

	FTransform ArmsFP_Location_Unscoped;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FTransform ArmsFP_Location_Scoped;
	
	bool bIsScoped;
};
