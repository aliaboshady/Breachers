#pragma once
#include "CoreMinimal.h"
#include "Breachers/Weapons/WeaponBase.h"
#include "ThrowableWeapon.generated.h"

UCLASS()
class BREACHERS_API AThrowableWeapon : public AWeaponBase
{
	GENERATED_BODY()

public:
	AThrowableWeapon();
	virtual void OnPrimaryFire() override;
	virtual void OnSecondaryFire() override;
	virtual void OnThrow();
	float GetCurrentThrowForce();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void EquipPreviousWeaponAfterThrow();
	void OnFire(float ThrowForce);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetThrowForce(float NewThrowForce);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayThrowAnimation();

	UPROPERTY(EditAnywhere)
	float PrimaryThrowForce;

	UPROPERTY(EditAnywhere)
	float SecondaryThrowForce;

	UPROPERTY(Replicated)
	float CurrentThrowForce;
};
