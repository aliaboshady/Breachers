#pragma once
#include "CoreMinimal.h"
#include "WeaponInfo.generated.h"

UENUM(BlueprintType)
enum EWeaponType
{
	Primary,
	Secondary,
	Melee
};

UENUM(BlueprintType)
enum EFireMode
{
	Single,
	Auto,
	Spread,
	Burst,
};

USTRUCT(BlueprintType)
struct FWeaponInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<EWeaponType> WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<EFireMode> WeaponFireMode;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MaxAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* EquipAnim;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* HideAnim;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* FireAnim;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* ReloadAnim;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* IdleAnim;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* WalkAnim;
};
