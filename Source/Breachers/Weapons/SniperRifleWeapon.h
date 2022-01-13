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
	virtual void Tick(float DeltaSeconds) override;
	void ResetCanScope();

	UFUNCTION(Server, Reliable)
	void Server_HandleTickDisabling(float Alpha);

	UFUNCTION(Server, Reliable)
	void Server_ForceUnscope();
	
	UFUNCTION(Client, Reliable)
	void Client_ScopeHandle(FTransform Transform1, FTransform Transform2, float Alpha);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ScopingSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ScopingCooldown;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FTransform ArmsFP_Transform_Scoped;

	FTransform OldTransform;
	FTransform NewTransform;

	FTimerHandle ScopeTimer;
	bool bCanScope;
	bool bIsInScope;
	bool bIsScoping;
	float ScopeTimeAlpha;
};
