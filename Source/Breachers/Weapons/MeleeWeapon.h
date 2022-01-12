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

protected:
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
};
