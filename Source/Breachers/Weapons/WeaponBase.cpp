#include "WeaponBase.h"
#include "DrawDebugHelpers.h"
#include "Breachers/Characters/CharacterBase.h"
#include "Breachers/Components/WeaponSystem.h"
#include "Breachers/Core/BreachersPlayerController.h"
#include "Components/DecalComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Sound/SoundCue.h"

AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	Tags.Add(TAG_Weapon);

	Mesh_TP = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh_TP"));
	RootComponent = Mesh_TP;
	Mesh_TP->CastShadow = true;
	Mesh_TP->bOwnerNoSee = true;
	Mesh_TP->bCastHiddenShadow = true;
	Mesh_TP->SetSimulatePhysics(true);
	Mesh_TP->SetCollisionProfileName(COLLISION_Weapon, true);
	Mesh_TP->CanCharacterStepUpOn = ECB_No;

	Mesh_FP = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh_FP"));
	Mesh_FP->SetupAttachment(RootComponent);
	Mesh_FP->CastShadow = false;
	Mesh_FP->bOnlyOwnerSee = true;
	
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Component"));
	SphereComponent->SetSphereRadius(40);
	SphereComponent->SetupAttachment(RootComponent);
	SphereComponent->SetRelativeLocation(FVector(0, 20, 5));
	SphereComponent->SetCollisionProfileName(COLLISION_WeaponOverlapSphere, true);

	bCanFire = true;
	bIsFiring = false;
	bIsReloading = false;
	bIsEquipping = false;
	RecoilTimePerShot = 0;
}

void AWeaponBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWeaponBase, WeaponInfo);
	DOREPLIFETIME(AWeaponBase, CharacterPlayer);
	DOREPLIFETIME(AWeaponBase, CurrentTotalAmmo);
	DOREPLIFETIME(AWeaponBase, CurrentAmmoInClip);
}

void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	SetReplicateMovement(true);
	SetActorTickEnabled(false);
	SetupWeaponInfo();
	
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AWeaponBase::OnOverlapped);
	CurrentTotalAmmo = WeaponInfo.ReloadInfo.MaxTotalAmmo - WeaponInfo.ReloadInfo.MaxAmmoInClip;
	CurrentAmmoInClip = WeaponInfo.ReloadInfo.MaxAmmoInClip;

	if(WeaponInfo.WeaponType == Primary) Tags.Add(TAG_Primary);
	else if(WeaponInfo.WeaponType == Secondary) Tags.Add(TAG_Secondary);
	else Tags.Add(TAG_Melee);
}

void AWeaponBase::SetupWeaponInfo()
{
	if(WeaponInfoDataTable)
	{
		const FString ContextString(DataTableKey.ToString());
		const FWeaponInfo* WeaponInfoFromDT = WeaponInfoDataTable->FindRow<FWeaponInfo>(DataTableKey, ContextString, true);
		if(WeaponInfoFromDT) WeaponInfo = *WeaponInfoFromDT;
	}
}

void AWeaponBase::OnOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!HasAuthority()) return;
	
	if(OtherActor->ActorHasTag(TAG_Player))
	{
		if(const ACharacterBase* Player = Cast<ACharacterBase>(OtherActor))
		{
			if(PreviousOwner && PreviousOwner == Player) return;
			
			if(Player->WeaponSystem->CanTakeWeapon(this))
			{
				Player->WeaponSystem->TakeWeapon(this);
				OnTaken();
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////// Fire //////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void AWeaponBase::OnPrimaryFire()
{
	if(!bCanFire || !CharacterPlayer || bIsReloading || bIsEquipping || CurrentAmmoInClip <= 0) return;
	
	bCanFire = false;
	bIsFiring = true;
	FTimerHandle IsFiringTimer;
	GetWorld()->GetTimerManager().SetTimer(IsFiringTimer, this, &AWeaponBase::ResetIsFiring, 1, false, WeaponInfo.ShotInfo.FireAnimationTime);

	if(WeaponInfo.WeaponFireMode == Auto)
	{
		GetWorld()->GetTimerManager().SetTimer(StartFireTimer, this, &AWeaponBase::Client_OnFire, WeaponInfo.RecoilInfo.TimeBetweenShots + 0.01, true, 0);
	}
	else if(WeaponInfo.WeaponFireMode == Spread)
	{
		FireSpread();
	}
	else
	{
		Client_OnFire();
	}
}

void AWeaponBase::OnStopFire()
{
	GetWorld()->GetTimerManager().ClearTimer(StartFireTimer);
	Client_ResetRecoil();
}

void AWeaponBase::OnSecondaryFire()
{
	
}

void AWeaponBase::FireSpread()
{
	for (int32 i = 0; i < WeaponInfo.ShotInfo.BulletsPerSpread; i++)
	{
		Client_OnFire();
	}
	bCanFire = false;
	FTimerHandle ResetTimer;
	GetWorld()->GetTimerManager().SetTimer(ResetTimer, this, &AWeaponBase::ResetCanFire, 1, false, WeaponInfo.RecoilInfo.TimeBetweenShots);
}

void AWeaponBase::ResetCanFire()
{
	bCanFire = true;
}

void AWeaponBase::ResetIsFiring()
{
	bIsFiring = false;
}

void AWeaponBase::Client_OnFire_Implementation()
{
	if(CurrentAmmoInClip <= 0) return;
	
	const FVector RecoilVector = RecoilShot(WeaponInfo.RecoilInfo.RecoilFactor);
	const FVector Start = CharacterPlayer->GetCameraLocation();
	FVector End = CharacterPlayer->GetCameraDirection() * WeaponInfo.ShotInfo.TraceLength + Start;
	End += RecoilVector * WeaponInfo.RecoilInfo.RecoilFactor;
	
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(CharacterPlayer);
	
	FHitResult OutHit;
	UKismetSystemLibrary::SphereTraceSingle(GetWorld(), Start, End, WeaponInfo.ShotInfo.BulletRadius, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, ActorsToIgnore, EDrawDebugTrace::None, OutHit, true);

	Client_Recoil(RecoilVector);
	Server_ProcessShot(OutHit);
}

void AWeaponBase::Server_ProcessShot_Implementation(FHitResult OutHit)
{
	if(CurrentAmmoInClip <= 0) return;
	CurrentAmmoInClip = FMath::Clamp(--CurrentAmmoInClip, 0, WeaponInfo.ReloadInfo.MaxTotalAmmo);
	FTimerHandle ResetTimer;
	GetWorld()->GetTimerManager().SetTimer(ResetTimer, this, &AWeaponBase::ResetCanFire, 1, false, WeaponInfo.RecoilInfo.TimeBetweenShots);
	
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
		}
	}
	Server_SpawnBulletTracer(OutHit);
	Server_OnFireEffects(OutHit);
}

void AWeaponBase::Multicast_SpawnBulletHoleDecal_Implementation(FHitResult OutHit)
{
	if(WeaponInfo.WeaponEffects.BulletHoleDecal)
	{
		FRotator DecalRotation = UKismetMathLibrary::MakeRotFromX(OutHit.ImpactNormal);
		DecalRotation.Roll += 180;
		DecalRotation.Pitch += 180;
		UDecalComponent* SpawnedDecal = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), WeaponInfo.WeaponEffects.BulletHoleDecal, FVector(WeaponInfo.WeaponEffects.DecalSize), OutHit.ImpactPoint, DecalRotation);
		SpawnedDecal->SetFadeScreenSize(0.0001);
	}
}

int32 AWeaponBase::GetSurfaceDamage(FHitResult OutHit) const
{
	const EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(OutHit.PhysMaterial.Get());
	switch (SurfaceType)
	{
	case SURFACE_Head:
		return WeaponInfo.DamageInfo.HeadDamage;
		
	case SURFACE_Torso:
		return WeaponInfo.DamageInfo.TorsoDamage;
		
	case SURFACE_Arms:
		return WeaponInfo.DamageInfo.ArmsDamage;
		
	case SURFACE_Legs:
		return WeaponInfo.DamageInfo.LegsDamage;
		
	default: return 0;
	}
}

void AWeaponBase::Server_OnFireEffects_Implementation(FHitResult OutHit)
{
	Client_OnFireEffects();
	Multicast_OnFireEffects(OutHit);
}

void AWeaponBase::Client_OnFireEffects_Implementation()
{
	if(WeaponInfo.WeaponEffects.MuzzleFlashEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(WeaponInfo.WeaponEffects.MuzzleFlashEffect, Mesh_FP, SOCKET_Muzzle);
	}
	
	if(WeaponInfo.WeaponEffects.MuzzleFireSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), WeaponInfo.WeaponEffects.MuzzleFireSound);
	}

	PlatAnimationWithTime(WeaponInfo.WeaponAnimations.FireAnim_Weapon, Mesh_FP, WeaponInfo.ShotInfo.FireAnimationTime);
	PlatAnimationWithTime(WeaponInfo.WeaponAnimations.FireAnim_ArmsFP, CharacterPlayer->GetArmsMeshFP(), WeaponInfo.ShotInfo.FireAnimationTime);
}

void AWeaponBase::Multicast_OnFireEffects_Implementation(FHitResult OutHit)
{
	if(WeaponInfo.WeaponEffects.MuzzleFlashEffect)
	{
		UParticleSystemComponent* SpawnedEffect = UGameplayStatics::SpawnEmitterAttached(WeaponInfo.WeaponEffects.MuzzleFlashEffect, Mesh_TP, SOCKET_Muzzle);
		SpawnedEffect->SetOwnerNoSee(true);
	}

	if(OutHit.bBlockingHit && WeaponInfo.WeaponEffects.ImpactEffect && !OutHit.GetActor()->ActorHasTag(TAG_Player))
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), WeaponInfo.WeaponEffects.ImpactEffect, OutHit.ImpactPoint);
	}

	if(WeaponInfo.WeaponEffects.MuzzleFireSound)
	{
		 if(CharacterPlayer && !CharacterPlayer->IsLocallyControlled())
		 {
		 	UGameplayStatics::SpawnSoundAttached(WeaponInfo.WeaponEffects.MuzzleFireSound, Mesh_TP, SOCKET_Muzzle);
		 }
	}

	if(OutHit.bBlockingHit && WeaponInfo.WeaponEffects.ImpactSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), WeaponInfo.WeaponEffects.ImpactSound, OutHit.ImpactPoint);
	}

	PlatAnimationWithTime(WeaponInfo.WeaponAnimations.FireAnim_Weapon, Mesh_TP, WeaponInfo.ShotInfo.FireAnimationTime);
	PlatAnimationWithTime(WeaponInfo.WeaponAnimations.FireAnim_ArmsTP, CharacterPlayer->GetMesh(), WeaponInfo.ShotInfo.FireAnimationTime);
}

void AWeaponBase::SpawnBulletTracer(FHitResult OutHit, FVector Start, bool bIsClient)
{
	FVector End = OutHit.TraceEnd;
	if(OutHit.bBlockingHit) End = OutHit.ImpactPoint;
	FVector Direction = (End - Start).GetSafeNormal();
	
	TSubclassOf<ABulletTracer> TracerClass = WeaponInfo.WeaponEffects.BulletTracer;
	if(TracerClass)
	{
		ABulletTracer* BulletTracer = GetWorld()->SpawnActor<ABulletTracer>(TracerClass, Start, Direction.Rotation(), FActorSpawnParameters());
		float TravelDistance = FVector::Distance(Start, End);
		float BulletLifetime = TravelDistance / BulletTracer->GetBulletSpeed();
		BulletTracer->SetLifeSpan(BulletLifetime);

		if(CharacterPlayer)
		{
			if(!bIsClient) BulletTracer->SetOwner(CharacterPlayer);
			BulletTracer->TracerParticle->SetWorldLocation(Start);
		}
	}
}

void AWeaponBase::Server_SpawnBulletTracer_Implementation(FHitResult OutHit)
{
	Client_SpawnBulletTracer(OutHit);
	Multicast_SpawnBulletTracer(OutHit);
}

void AWeaponBase::Client_SpawnBulletTracer_Implementation(FHitResult OutHit)
{
	SpawnBulletTracer(OutHit, Mesh_FP->GetSocketLocation(SOCKET_Muzzle), true);
}

void AWeaponBase::Multicast_SpawnBulletTracer_Implementation(FHitResult OutHit)
{
	SpawnBulletTracer(OutHit, Mesh_TP->GetSocketLocation(SOCKET_Muzzle), false);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////// Reload //////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void AWeaponBase::OnReload()
{
	if(CurrentTotalAmmo <= 0 || CurrentAmmoInClip >= WeaponInfo.ReloadInfo.MaxAmmoInClip || bIsEquipping || bIsReloading || bIsFiring) return;
	OnStopFire();
	
	GetWorld()->GetTimerManager().SetTimer(ReloadTimer, this, &AWeaponBase::OnFinishReload, 1, false, WeaponInfo.ReloadInfo.ReloadTime);
	bIsReloading = true;
	
	Client_OnReloadAnimations();
	Multicast_OnReloadAnimations();
}

void AWeaponBase::OnFinishReload()
{
	bIsReloading = false;
	const int32 NeededAmmo = WeaponInfo.ReloadInfo.MaxAmmoInClip - CurrentAmmoInClip;

	if(CurrentTotalAmmo - NeededAmmo >= 0)
	{
		CurrentTotalAmmo = FMath::Clamp(CurrentTotalAmmo - NeededAmmo, 0, WeaponInfo.ReloadInfo.MaxTotalAmmo);
		CurrentAmmoInClip += NeededAmmo;
	}
	else
	{
		CurrentAmmoInClip += CurrentTotalAmmo;
		CurrentTotalAmmo = 0;
	}
}

void AWeaponBase::OnCancelReload()
{
	GetWorld()->GetTimerManager().ClearTimer(ReloadTimer);
	bIsReloading = false;
	Multicast_OnCancelReloadAnimations();
}

void AWeaponBase::Client_OnReloadAnimations_Implementation()
{
	PlatAnimationWithTime(WeaponInfo.WeaponAnimations.ReloadAnim_WeaponFP, Mesh_FP, WeaponInfo.ReloadInfo.ReloadTime);
	PlatAnimationWithTime(WeaponInfo.WeaponAnimations.ReloadAnim_ArmsFP, CharacterPlayer->GetArmsMeshFP(), WeaponInfo.ReloadInfo.ReloadTime);
}

void AWeaponBase::Multicast_OnReloadAnimations_Implementation()
{
	PlatAnimationWithTime(WeaponInfo.WeaponAnimations.ReloadAnim_WeaponTP, Mesh_TP, WeaponInfo.ReloadInfo.ReloadTime);
	PlatAnimationWithTime(WeaponInfo.WeaponAnimations.ReloadAnim_ArmsTP, CharacterPlayer->GetMesh(), WeaponInfo.ReloadInfo.ReloadTime);
}

void AWeaponBase::Multicast_OnCancelReloadAnimations_Implementation()
{
	CancelAllAnimations();
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////// Equip/Take Weapons ///////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void AWeaponBase::OnTaken()
{
	CharacterPlayer = Cast<ACharacterBase>(GetOwner());
	if(Mesh_TP)
	{
		Mesh_TP->SetEnableGravity(false);
		Mesh_TP->SetSimulatePhysics(false);
		Mesh_TP->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	Multicast_OnTaken();
}

void AWeaponBase::Multicast_OnTaken_Implementation()
{
	if(SphereComponent) SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWeaponBase::OnDrop(ACharacterBase* DropperCharacter)
{
	PreviousOwner = DropperCharacter;
	
	Mesh_TP->SetEnableGravity(true);
	Mesh_TP->SetSimulatePhysics(true);
	Mesh_TP->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	ABreachersPlayerController* DropperCharacterPC = DropperCharacter->GetBreacherPC();
	FVector ThrowDirection;
	if(DropperCharacterPC) ThrowDirection = DropperCharacterPC->GetControlRotation().Vector();
	else ThrowDirection = DropperCharacter->GetCameraDirection();
	
	const FVector Force = ThrowDirection * DropperCharacter->WeaponSystem->WeaponThrowForce;
	Mesh_TP->AddImpulse(Force);
	
	FTimerHandle OverlapHandle;
	GetWorldTimerManager().SetTimer(OverlapHandle, this, &AWeaponBase::Multicast_OnDropEnableOverlap, 1, false, TIME_PickWeaponAfterDrop);
}

void AWeaponBase::Multicast_OnDropEnableOverlap_Implementation()
{
	PreviousOwner = nullptr;
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AWeaponBase::OnEquip()
{
	bIsEquipping = true;
	GetWorld()->GetTimerManager().SetTimer(EquipTimer, this, &AWeaponBase::OnCancelEquip, 1, false, WeaponInfo.ReloadInfo.EquipTime);
	Client_OnEquipAnimations();
	Multicast_OnEquipAnimations();
}

void AWeaponBase::OnUnquip()
{
}

void AWeaponBase::OnCancelEquip()
{
	if(bIsEquipping)
	{
		Multicast_OnCancelEquipAnimations();
		bIsEquipping = false;
	}
}

void AWeaponBase::Client_OnEquipAnimations_Implementation()
{
	PlatAnimationWithTime(WeaponInfo.WeaponAnimations.EquipAnim_Weapon, Mesh_FP, WeaponInfo.ReloadInfo.EquipTime);
	PlatAnimationWithTime(WeaponInfo.WeaponAnimations.EquipAnim_ArmsFP, CharacterPlayer->GetArmsMeshFP(), WeaponInfo.ReloadInfo.EquipTime);
}

void AWeaponBase::Multicast_OnEquipAnimations_Implementation()
{
	PlatAnimationWithTime(WeaponInfo.WeaponAnimations.EquipAnim_Weapon, Mesh_TP, WeaponInfo.ReloadInfo.EquipTime);
	PlatAnimationWithTime(WeaponInfo.WeaponAnimations.EquipAnim_ArmsTP, CharacterPlayer->GetMesh(), WeaponInfo.ReloadInfo.EquipTime);
}

void AWeaponBase::Multicast_OnCancelEquipAnimations_Implementation()
{
	CancelAllAnimations();
}

void AWeaponBase::PlatAnimationWithTime(UAnimMontage* AnimationMontage, USkeletalMeshComponent* Mesh, float Time)
{
	if(!AnimationMontage || !Mesh || !Mesh->GetAnimInstance()) return;
	const float MontageLength = Mesh->GetAnimInstance()->Montage_Play(AnimationMontage);
	
	float Rate;
	if(Time == 0) Rate = 1;
	else Rate = MontageLength / (Time + 0.01);
	
	Mesh->GetAnimInstance()->Montage_SetPlayRate(AnimationMontage, Rate);
}

void AWeaponBase::CancelAllAnimations() const
{
	if(!CharacterPlayer) return;
	UAnimInstance* AnimIn_Mesh_FP = Mesh_FP->GetAnimInstance();
	UAnimInstance* AnimIn_Mesh_TP = Mesh_TP->GetAnimInstance();
	UAnimInstance* AnimIn_CharacterMesh_FP = CharacterPlayer->GetArmsMeshFP()->GetAnimInstance();
	UAnimInstance* AnimIn_CharacterMesh_TP = CharacterPlayer->GetMesh()->GetAnimInstance();
	
	if(AnimIn_Mesh_FP) AnimIn_Mesh_FP->StopAllMontages(0);
	if(AnimIn_Mesh_TP) AnimIn_Mesh_TP->StopAllMontages(0);
	if(AnimIn_CharacterMesh_FP) AnimIn_CharacterMesh_FP->StopAllMontages(0);
	if(AnimIn_CharacterMesh_TP) AnimIn_CharacterMesh_TP->StopAllMontages(0);
}

FVector AWeaponBase::RecoilShot(float Spread)
{
	RecoilTimePerShot += WeaponInfo.RecoilInfo.TimeBetweenShots;
	UCurveVector* RecoilPattern = WeaponInfo.RecoilInfo.RecoilPattern;
	FVector RecoilVector;
	
	if(RecoilPattern)
	{
		RecoilVector = RecoilPattern->GetVectorValue(RecoilTimePerShot);
	}
	
	FVector NewLocation = FVector(RecoilVector.Z, 0, RecoilVector.Y);
	return NewLocation;
}

void AWeaponBase::Client_Recoil_Implementation(FVector CalculatedRecoil)
{
	if(!CharacterPlayer) return;
	FRotator NewRotation = CharacterPlayer->GetControlRotation();
	NewRotation.Pitch += CalculatedRecoil.Z;
	NewRotation.Yaw += CalculatedRecoil.Y;
	CharacterPlayer->GetBreacherPC()->SetControlRotation(NewRotation);
}

void AWeaponBase::Client_ResetRecoil_Implementation()
{
	RecoilTimePerShot = 0;
}