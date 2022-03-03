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
	bCanPlayBounceSound = true;
	ImpactSoundPower = 20;
}

void AThrowableWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AThrowableWeapon, CurrentThrowForce);
	DOREPLIFETIME(AThrowableWeapon, ThrowerCharacter);
}

void AThrowableWeapon::BeginPlay()
{
	Super::BeginPlay();
	Mesh_TP->OnComponentHit.AddDynamic(this, &AThrowableWeapon::OnHit);
}

void AThrowableWeapon::OnPrimaryFire()
{
	Multicast_SetThrowAnimation(PrimaryThrowAnimation);
	OnFire(PrimaryThrowForce);
}

void AThrowableWeapon::OnSecondaryFire()
{
	Multicast_SetThrowAnimation(SecondaryThrowAnimation);
	OnFire(SecondaryThrowForce);
}

void AThrowableWeapon::OnFire(float ThrowForce)
{
	if(!bCanFire || !CharacterPlayer || bIsReloading || bIsEquipping || CurrentAmmoInClip <= 0) return;
	Multicast_SetThrowForce(ThrowForce);
	Multicast_PlayThrowAnimation();
	bCanFire = false;
	bIsFiring = true;

	GetWorld()->GetTimerManager().SetTimer(IsFiringTimer, this, &AThrowableWeapon::ResetIsFiring, 1, false, WeaponInfo.ShotInfo.FireAnimationTime);
	GetWorld()->GetTimerManager().SetTimer(ResetTimer, this, &AThrowableWeapon::ResetCanFire, 1, false, WeaponInfo.ShotInfo.FireAnimationTime);
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

void AThrowableWeapon::OnCancelEquip()
{
	Super::OnCancelEquip();
	GetWorld()->GetTimerManager().ClearTimer(IsFiringTimer);
	GetWorld()->GetTimerManager().ClearTimer(ResetTimer);
	bCanFire = true;
	bIsFiring = false;
}

float AThrowableWeapon::GetCurrentThrowForce()
{
	return CurrentThrowForce;
}

void AThrowableWeapon::SetThrowerCharacter(ACharacterBase* NewThrowerCharacter)
{
	ThrowerCharacter = NewThrowerCharacter;
}

void AThrowableWeapon::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if(GetVelocity().Size() >= ImpactSoundPower && bCanPlayBounceSound) Multicast_PlayBounceSound();
}

void AThrowableWeapon::Multicast_SetThrowAnimation_Implementation(UAnimMontage* ThrowAnimation)
{
	if(ThrowAnimation)
	{
		WeaponInfo.WeaponAnimations.FireAnim_ArmsTP = ThrowAnimation;
		WeaponInfo.WeaponAnimations.FireAnim_ArmsFP = ThrowAnimation;
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

void AThrowableWeapon::Multicast_PlayBounceSound_Implementation()
{
	USoundCue* BounceSound = WeaponInfo.WeaponEffects.ImpactSound;
	if(BounceSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), BounceSound, GetActorLocation());
		Multicast_SetCanPlayBounceSound(false);
		FTimerHandle BounceSoundHandle;
		FTimerDelegate BounceSoundDelegate;
		BounceSoundDelegate.BindUFunction(this, FName(TEXT("Multicast_SetCanPlayBounceSound")), true);
		GetWorldTimerManager().SetTimer(BounceSoundHandle, BounceSoundDelegate, 1, false, 0.5);
	}
}

void AThrowableWeapon::Multicast_SetCanPlayBounceSound_Implementation(bool bCanPlaySound)
{
	bCanPlayBounceSound = bCanPlaySound;
}