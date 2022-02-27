#pragma once
#include "CoreMinimal.h"
#include "ThrowableWeapon.h"
#include "Grenade.generated.h"

UCLASS()
class BREACHERS_API AGrenade : public AThrowableWeapon
{
	GENERATED_BODY()

public:
	AGrenade();
	virtual void OnThrow() override;

	virtual void OnActivate() override;

	UFUNCTION(Server, Reliable)
	void Server_ExplosionDamage();

protected:
	UPROPERTY(EditAnywhere)
	float DamageInnerRadius;
	
	UPROPERTY(EditAnywhere)
	float DamageOuterRadius;
};
