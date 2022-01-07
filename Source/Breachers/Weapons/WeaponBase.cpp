#include "WeaponBase.h"
#include "DrawDebugHelpers.h"
#include "Breachers/Characters/CharacterBase.h"
#include "Breachers/Components/WeaponSystem.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Sound/SoundCue.h"

AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	TraceLength = 10000;
	BulletRadius = 2;

	Mesh_TP = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh_TP"));
	RootComponent = Mesh_TP;
	Mesh_TP->CastShadow = true;
	Mesh_TP->bOwnerNoSee = true;
	Mesh_TP->bCastHiddenShadow = true;
	Mesh_TP->SetSimulatePhysics(true);

	Mesh_FP = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh_FP"));
	Mesh_FP->SetupAttachment(RootComponent);
	Mesh_FP->CastShadow = false;
	Mesh_FP->bOnlyOwnerSee = true;
	
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Component"));
	SphereComponent->SetSphereRadius(40);
	SphereComponent->SetupAttachment(RootComponent);
	SphereComponent->SetRelativeLocation(FVector(0, 20, 5));
}

void AWeaponBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWeaponBase, WeaponInfo);
	DOREPLIFETIME(AWeaponBase, CharacterPlayer);
}

void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	SetReplicateMovement(true);
	
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AWeaponBase::OnOverlapped);
}

void AWeaponBase::OnOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!HasAuthority()) return;
	
	if(OtherActor->ActorHasTag(FName("Player")))
	{
		if(const ACharacterBase* Player = Cast<ACharacterBase>(OtherActor))
		{
			if(Player->WeaponSystem->CanTakeWeapon(this))
			{
				Player->WeaponSystem->TakeWeapon(this);
				OnTaken();
			}
		}
	}
}

void AWeaponBase::OnTaken()
{
	CharacterPlayer = Cast<ACharacterBase>(GetOwner());
	if(Mesh_TP)
	{
		Mesh_TP->SetEnableGravity(false);
		Mesh_TP->SetSimulatePhysics(false);
		Mesh_TP->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	if(SphereComponent) SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWeaponBase::OnFire()
{
	Client_OnFire();
}

void AWeaponBase::Client_OnFire_Implementation()
{
	if(!CharacterPlayer) return;
	
	const FVector RecoilVector = RecoilShot(WeaponInfo.RecoilFactor);
	const FVector Start = CharacterPlayer->GetCameraLocation();
	FVector End = CharacterPlayer->GetCameraDirection() * TraceLength + Start;
	End += RecoilVector;
	
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(CharacterPlayer);
	
	FHitResult OutHit;
	UKismetSystemLibrary::SphereTraceSingle(GetWorld(), Start, End, BulletRadius, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, ActorsToIgnore, EDrawDebugTrace::ForDuration, OutHit, true);

	Server_ProcessShot(OutHit);
	Server_OnFireEffects(OutHit);
}

void AWeaponBase::Server_ProcessShot_Implementation(FHitResult OutHit)
{
	if(OutHit.bBlockingHit)
	{
		const int32 Damage = GetSurfaceDamage(OutHit);
		UGameplayStatics::ApplyPointDamage(OutHit.GetActor(), Damage, OutHit.TraceStart, OutHit, CharacterPlayer->GetController(), this, UDamageType::StaticClass());
	}
}

int32 AWeaponBase::GetSurfaceDamage(FHitResult OutHit) const
{
	const EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(OutHit.PhysMaterial.Get());
	switch (SurfaceType)
	{
	case SURFACE_Head:
		return WeaponInfo.HeadDamage;
		
	case SURFACE_Torso:
		return WeaponInfo.TorsoDamage;
		
	case SURFACE_Arms:
		return WeaponInfo.ArmsDamage;
		
	case SURFACE_Legs:
		return WeaponInfo.LegsDamage;
		
	default: return 0;
	}
}

FVector AWeaponBase::RecoilShot(float Spread) const
{
	FVector NewLocation = FVector(0);
	NewLocation.X += FMath::RandRange(-Spread, Spread);
	NewLocation.Y += FMath::RandRange(-Spread, Spread);
	NewLocation.Z += FMath::RandRange(-Spread, Spread);
	return NewLocation;
}

void AWeaponBase::Server_OnFireEffects_Implementation(FHitResult OutHit)
{
	Client_OnFireEffects();
	Multicast_OnFireEffects(OutHit);
}

void AWeaponBase::Client_OnFireEffects_Implementation()
{
	if(WeaponInfo.MuzzleFlashEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(WeaponInfo.MuzzleFlashEffect, Mesh_FP, SOCKET_Muzzle);
	}
	
	if(WeaponInfo.MuzzleFireSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), WeaponInfo.MuzzleFireSound);
	}
}

void AWeaponBase::Multicast_OnFireEffects_Implementation(FHitResult OutHit)
{
	if(WeaponInfo.MuzzleFlashEffect)
	{
		UParticleSystemComponent* SpawnedEffect = UGameplayStatics::SpawnEmitterAttached(WeaponInfo.MuzzleFlashEffect, Mesh_TP, SOCKET_Muzzle);
		SpawnedEffect->SetOwnerNoSee(true);
	}

	if(OutHit.bBlockingHit && WeaponInfo.ImpactEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), WeaponInfo.ImpactEffect, OutHit.ImpactPoint);
	}

	if(WeaponInfo.MuzzleFireSound)
	{
		 if(CharacterPlayer && !CharacterPlayer->IsLocallyControlled())
		 {
		 	UGameplayStatics::SpawnSoundAttached(WeaponInfo.MuzzleFireSound, Mesh_TP, SOCKET_Muzzle);
		 }
	}

	if(OutHit.bBlockingHit && WeaponInfo.ImpactSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), WeaponInfo.ImpactSound, OutHit.ImpactPoint);
	}
}