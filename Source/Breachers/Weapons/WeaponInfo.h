#pragma once
#include "CoreMinimal.h"
#include "BulletTracer.h"
#include "Animation/AimOffsetBlendSpace1D.h"
#include "Curves/CurveVector.h"
#include "Engine/DataTable.h"
#include "WeaponInfo.generated.h"

#define SOCKET_Muzzle "MuzzleFlash"

class USoundCue;

UENUM(BlueprintType)
enum EWeaponType
{
	Primary,
	Secondary,
	Melee,
	Bomb
};

UENUM(BlueprintType)
enum EFireMode
{
	Single,
	Auto,
	Spread,
	Burst
};


USTRUCT(BlueprintType)
struct FWeaponAnimation
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimSequenceBase* IdlePose_ArmsFP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimSequenceBase* IdlePose_ArmsTP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAimOffsetBlendSpace1D* BlendSpace_ArmsTP;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* EquipAnim_Weapon;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* EquipAnim_ArmsFP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* EquipAnim_ArmsTP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* FireAnim_Weapon;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* FireAnim_ArmsFP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* FireAnim_ArmsTP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* ReloadAnim_WeaponFP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* ReloadAnim_WeaponTP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* ReloadAnim_ArmsFP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* ReloadAnim_ArmsTP;
};

USTRUCT(BlueprintType)
struct FWeaponEffect
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<ABulletTracer> BulletTracer;
	
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
struct FDamageInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float KillPushForce;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 HeadDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 TorsoDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 ArmsDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 LegsDamage;
};

USTRUCT(BlueprintType)
struct FRecoilInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TimeBetweenShots;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RecoilFactor;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RecoilRecoverySpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RecoilRecoveryDelay;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxAccurateSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UCurveVector* RecoilPattern;
};

USTRUCT(BlueprintType)
struct FCarrySpeed
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CarryWeaponRunSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CarryWeaponWalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CarryWeaponCrouchSpeed;
};

USTRUCT(BlueprintType)
struct FReloadInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MaxTotalAmmo;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MaxAmmoInClip;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ReloadTime;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float EquipTime;

};

USTRUCT(BlueprintType)
struct FShotInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BulletsPerSpread;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float FireAnimationTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TraceLength;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BulletRadius;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DefaultFOV;
};

USTRUCT(BlueprintType)
struct FWeaponInfo : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString WeaponName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* WeaponIcon;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<EWeaponType> WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<EFireMode> WeaponFireMode;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsDroppable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 WeaponPrice;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 KillRewardMoney;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FDamageInfo DamageInfo;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FRecoilInfo RecoilInfo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FTransform ArmsTransformFP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FCarrySpeed CarrySpeeds;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FReloadInfo ReloadInfo;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FShotInfo ShotInfo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FWeaponAnimation WeaponAnimations;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FWeaponEffect WeaponEffects;
};
