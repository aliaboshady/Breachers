#include "ThrowableWeapon.h"
#include "Breachers/Characters/CharacterBase.h"
#include "Breachers/Components/WeaponSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

AThrowableWeapon::AThrowableWeapon()
{
	PrimaryThrowForce = 2000;
	SecondaryThrowForce = 400;
	TimeToActivate = 1;
	ActivationDuration = 0;
}

void AThrowableWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AThrowableWeapon, CurrentThrowForce);
	DOREPLIFETIME(AThrowableWeapon, ThrowerCharacter);
}

void AThrowableWeapon::OnPrimaryFire()
{
	OnFire(PrimaryThrowForce);
}

void AThrowableWeapon::OnSecondaryFire()
{
	OnFire(SecondaryThrowForce);
}

void AThrowableWeapon::OnFire(float ThrowForce)
{
	if(!bCanFire || !CharacterPlayer || bIsReloading || bIsEquipping || CurrentAmmoInClip <= 0) return;
	Multicast_SetThrowForce(ThrowForce);
	Multicast_PlayThrowAnimation();
	bCanFire = false;
	bIsFiring = true;
	CharacterPlayer->WeaponSystem->SetIsThrowing(true);

	FTimerHandle IsFiringTimer;
	GetWorld()->GetTimerManager().SetTimer(IsFiringTimer, this, &AThrowableWeapon::ResetIsFiring, 1, false, WeaponInfo.ShotInfo.FireAnimationTime);

	FTimerHandle ResetTimer;
	GetWorld()->GetTimerManager().SetTimer(ResetTimer, this, &AThrowableWeapon::ResetCanFire, 1, false, WeaponInfo.ShotInfo.FireAnimationTime);
	
	FTimerHandle EquipPreviousWeaponTimer;
	GetWorld()->GetTimerManager().SetTimer(EquipPreviousWeaponTimer, this, &AThrowableWeapon::EquipPreviousWeaponAfterThrow, 1, false, WeaponInfo.ShotInfo.FireAnimationTime);
}

void AThrowableWeapon::Multicast_SetThrowForce_Implementation(float NewThrowForce)
{
	CurrentThrowForce = NewThrowForce;
}

void AThrowableWeapon::OnThrow()
{
	SetToNotBePickedUp();
	WeaponInfo.bIsDroppable = true;
}

float AThrowableWeapon::GetCurrentThrowForce()
{
	return CurrentThrowForce;
}

void AThrowableWeapon::SetThrowerCharacter(ACharacterBase* NewThrowerCharacter)
{
	ThrowerCharacter = NewThrowerCharacter;
}

void AThrowableWeapon::EquipPreviousWeaponAfterThrow()
{
	if(CharacterPlayer)
	{
		CharacterPlayer->WeaponSystem->SetIsThrowing(false);
		CharacterPlayer->WeaponSystem->EquipBestValidWeapon();
	}
}

void AThrowableWeapon::Multicast_PlayThrowAnimation_Implementation()
{
	UAnimMontage* FireAnim_Weapon_TP = WeaponInfo.WeaponAnimations.FireAnim_ArmsTP;
	UAnimMontage* FireAnim_Weapon_FP = WeaponInfo.WeaponAnimations.FireAnim_ArmsFP;
	if(!CharacterPlayer || !FireAnim_Weapon_TP || !FireAnim_Weapon_FP) return;
	PlayAnimationWithTime(FireAnim_Weapon_TP, CharacterPlayer->GetMesh(), WeaponInfo.ShotInfo.FireAnimationTime);
	PlayAnimationWithTime(FireAnim_Weapon_FP, CharacterPlayer->GetArmsMeshFP(), WeaponInfo.ShotInfo.FireAnimationTime);
}

void AThrowableWeapon::OnActivate()
{
	Multicast_ActivationEffects();
	Destroy();
}

void AThrowableWeapon::Multicast_ActivationEffects_Implementation()
{
	UParticleSystem* ExplosionParticles = WeaponInfo.WeaponEffects.MuzzleFlashEffect;
	if(ExplosionParticles) UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionParticles, GetActorTransform());

	USoundCue* ExplosionSound = WeaponInfo.WeaponEffects.MuzzleFireSound;
	if(ExplosionSound) UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSound, GetActorLocation());
}