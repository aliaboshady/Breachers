#pragma once
#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "MeleeWeapon.generated.h"

UCLASS()
class BREACHERS_API AMeleeWeapon : public AWeaponBase
{
	GENERATED_BODY()

public:
	AMeleeWeapon();
	virtual void OnPrimaryFire() override;
	virtual void OnSecondaryFire() override;
	void OnMeleeHit();

protected:
	
	UFUNCTION(Server, Reliable)
	void Server_ProcessMeleeHit(FHitResult OutHit);
	
	UFUNCTION(Server, Reliable)
	void Server_OnMeleeHit();

	UFUNCTION(Client, Reliable)
	void Client_OnMeleeHit();
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnMeleeHit_Effects(FHitResult OutHit);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_MeleeAnimation(bool bIsPrimary);

	void ChangeAnimation(bool bIsPrimary);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float PrimaryAttackTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SecondaryAttackTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float PrimaryTimeBetweenHits;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SecondaryTimeBetweenHits;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* PrimaryAttackAnim_ArmsFP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* PrimaryAttackAnim_ArmsTP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* SecondaryAttackAnim_ArmsFP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* SecondaryAttackAnim_ArmsTP;

	void ResetHasHitSomething();
	bool bHasHitSomething;
};
