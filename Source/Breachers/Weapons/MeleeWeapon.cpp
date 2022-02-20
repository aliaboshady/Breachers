#include "MeleeWeapon.h"
#include "Breachers/Characters/CharacterBase.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

AMeleeWeapon::AMeleeWeapon()
{
	PrimaryAttackDamage = 25;
	SecondaryAttackDamage = 75;
	PrimaryAttackTime = 0.5;
	SecondaryAttackTime = 1;
	PrimaryTimeBetweenHits = 1;
	SecondaryTimeBetweenHits = 2;
	bHasHitSomething = false;
}

void AMeleeWeapon::OnPrimaryFire()
{
	if(!bCanFire || !CharacterPlayer || bIsEquipping) return;
	
	Multicast_MeleeAnimation(true);
	
	bCanFire = false;
	FTimerHandle ResetTimer;
	GetWorld()->GetTimerManager().SetTimer(ResetTimer, this, &AMeleeWeapon::ResetCanFire, 1, false, WeaponInfo.RecoilInfo.TimeBetweenShots);

	FTimerHandle HitSomethingTimer;
	GetWorld()->GetTimerManager().SetTimer(HitSomethingTimer, this, &AMeleeWeapon::ResetHasHitSomething, 1, false, WeaponInfo.ShotInfo.FireAnimationTime);
}

void AMeleeWeapon::OnSecondaryFire()
{
	if(!bCanFire || !CharacterPlayer || bIsEquipping) return;
	
	Multicast_MeleeAnimation(false);
	
	bCanFire = false;
	FTimerHandle ResetTimer;
	GetWorld()->GetTimerManager().SetTimer(ResetTimer, this, &AMeleeWeapon::ResetCanFire, 1, false, WeaponInfo.RecoilInfo.TimeBetweenShots);

	FTimerHandle HitSomethingTimer;
	GetWorld()->GetTimerManager().SetTimer(HitSomethingTimer, this, &AMeleeWeapon::ResetHasHitSomething, 1, false, WeaponInfo.ShotInfo.FireAnimationTime);
}

void AMeleeWeapon::Multicast_MeleeAnimation_Implementation(bool bIsPrimary)
{
	ChangeProperties(bIsPrimary);
	
	if(WeaponInfo.WeaponEffects.MuzzleFireSound)
	{
		UGameplayStatics::SpawnSoundAttached(WeaponInfo.WeaponEffects.MuzzleFireSound, Mesh_TP, SOCKET_Muzzle);
	}

	PlayAnimationWithTime(WeaponInfo.WeaponAnimations.FireAnim_ArmsFP, CharacterPlayer->GetArmsMeshFP(), WeaponInfo.ShotInfo.FireAnimationTime);
	PlayAnimationWithTime(WeaponInfo.WeaponAnimations.FireAnim_ArmsTP, CharacterPlayer->GetMesh(), WeaponInfo.ShotInfo.FireAnimationTime);
}

void AMeleeWeapon::ChangeProperties(bool bIsPrimary)
{
	if(bIsPrimary)
	{
		WeaponInfo.DamageInfo.ArmsDamage = PrimaryAttackDamage;
		WeaponInfo.DamageInfo.HeadDamage = PrimaryAttackDamage;
		WeaponInfo.DamageInfo.TorsoDamage = PrimaryAttackDamage;
		WeaponInfo.DamageInfo.LegsDamage = PrimaryAttackDamage;
		WeaponInfo.ShotInfo.FireAnimationTime = PrimaryAttackTime;
		WeaponInfo.RecoilInfo.TimeBetweenShots = PrimaryTimeBetweenHits;
		if(PrimaryAttackAnim_ArmsFP) WeaponInfo.WeaponAnimations.FireAnim_ArmsFP = PrimaryAttackAnim_ArmsFP;
		if(PrimaryAttackAnim_ArmsTP) WeaponInfo.WeaponAnimations.FireAnim_ArmsTP = PrimaryAttackAnim_ArmsTP;
		if(PrimaryAttackHoleDecal) WeaponInfo.WeaponEffects.BulletHoleDecal = PrimaryAttackHoleDecal;
	}
	else
	{
		WeaponInfo.DamageInfo.ArmsDamage = SecondaryAttackDamage;
		WeaponInfo.DamageInfo.HeadDamage = SecondaryAttackDamage;
		WeaponInfo.DamageInfo.TorsoDamage = SecondaryAttackDamage;
		WeaponInfo.DamageInfo.LegsDamage = SecondaryAttackDamage;
		WeaponInfo.ShotInfo.FireAnimationTime = SecondaryAttackTime;
		WeaponInfo.RecoilInfo.TimeBetweenShots = SecondaryTimeBetweenHits;
		if(SecondaryAttackAnim_ArmsFP) WeaponInfo.WeaponAnimations.FireAnim_ArmsFP = SecondaryAttackAnim_ArmsFP;
		if(SecondaryAttackAnim_ArmsTP) WeaponInfo.WeaponAnimations.FireAnim_ArmsTP = SecondaryAttackAnim_ArmsTP;
		if(SecondaryAttackHoleDecal) WeaponInfo.WeaponEffects.BulletHoleDecal = SecondaryAttackHoleDecal;
	}
}

void AMeleeWeapon::OnMeleeHit()
{
	Server_OnMeleeHit();
}

void AMeleeWeapon::Server_OnMeleeHit_Implementation()
{
	if(bHasHitSomething) return;
	bHasHitSomething = true;
	Client_OnMeleeHit();
}

void AMeleeWeapon::Client_OnMeleeHit_Implementation()
{
	const FVector Start = CharacterPlayer->GetCameraLocation();
	FVector End = CharacterPlayer->GetCameraDirection() * WeaponInfo.ShotInfo.TraceLength + Start;
	
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(CharacterPlayer);
	
	FHitResult OutHit;
	UKismetSystemLibrary::SphereTraceSingle(GetWorld(), Start, End, WeaponInfo.ShotInfo.BulletRadius, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, ActorsToIgnore, EDrawDebugTrace::None, OutHit, true);

	Server_ProcessMeleeHit(OutHit);
}

void AMeleeWeapon::Server_ProcessMeleeHit_Implementation(FHitResult OutHit)
{
	if(OutHit.bBlockingHit)
	{
		if(OutHit.GetActor()->ActorHasTag(TAG_Player))
		{
			const int32 Damage = GetSurfaceDamage(OutHit);
			UGameplayStatics::ApplyPointDamage(OutHit.GetActor(), Damage, OutHit.TraceStart, OutHit, CharacterPlayer->GetController(), this, UDamageType::StaticClass());
		}
		else
		{
			Multicast_SpawnBulletHoleDecal(OutHit);
			Multicast_OnMeleeHit_Effects(OutHit);
		}
	}
}

void AMeleeWeapon::Multicast_OnMeleeHit_Effects_Implementation(FHitResult OutHit)
{
	if(OutHit.bBlockingHit && WeaponInfo.WeaponEffects.ImpactEffect && !OutHit.GetActor()->ActorHasTag(TAG_Player))
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), WeaponInfo.WeaponEffects.ImpactEffect, OutHit.ImpactPoint);
	}

	if(OutHit.bBlockingHit && WeaponInfo.WeaponEffects.ImpactSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), WeaponInfo.WeaponEffects.ImpactSound, OutHit.ImpactPoint);
	}
}

void AMeleeWeapon::ResetHasHitSomething()
{
	bHasHitSomething = false;
}
