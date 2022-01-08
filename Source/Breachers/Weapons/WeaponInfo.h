#pragma once
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "WeaponInfo.generated.h"

#define SOCKET_Muzzle "MuzzleFlash"

class USoundCue;

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
struct FWeaponAnimation
{
	GENERATED_BODY()
	
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

USTRUCT(BlueprintType)
struct FWeaponEffect
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UParticleSystem* MuzzleFlashEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UParticleSystem* ImpactEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USoundCue* MuzzleFireSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USoundCue* ImpactSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UMaterialInterface* BulletHoleDecal;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DecalSize;
};

USTRUCT(BlueprintType)
struct FWeaponInfo : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<EWeaponType> WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<EFireMode> WeaponFireMode;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 HeadDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 TorsoDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 ArmsDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 LegsDamage;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RecoilFactor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BulletsPerSpread;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MaxAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TimeBetweenShots;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TraceLength;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BulletRadius;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FWeaponAnimation WeaponAnimations;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FWeaponEffect WeaponEffects;
};
