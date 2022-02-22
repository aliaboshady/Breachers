#include "Bomb.h"

#include "Breachers/Characters/CharacterBase.h"
#include "Breachers/Components/PlantDefuseSystem.h"
#include "Breachers/GameModes/PlantAndDefuseGameMode.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"

ABomb::ABomb()
{
	DefuseArea = CreateDefaultSubobject<USphereComponent>("DefuseArea");
	DefuseArea->InitSphereRadius(130);
	DefuseArea->SetupAttachment(RootComponent);

	bIsBeginDefused = false;
	BombState = BombUnplanted;
	TickSegmentIndex = 0;
	ExplosionVolume = 10;
}

void ABomb::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABomb, bIsBeginDefused);
	DOREPLIFETIME(ABomb, BombState);
	DOREPLIFETIME(ABomb, Planter);
}

void ABomb::BeginPlay()
{
	Super::BeginPlay();
	DefuseArea->OnComponentBeginOverlap.AddDynamic(this, &ABomb::OnPlayerEnterDefuseArea);
	DefuseArea->OnComponentEndOverlap.AddDynamic(this, &ABomb::OnPlayerExitDefuseArea);
	NormalBlendSpace_TP = WeaponInfo.WeaponAnimations.BlendSpace_ArmsTP;
	Server_SetTickingSegments();
}

void ABomb::OnPlayerEnterDefuseArea(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(UPlantDefuseSystem* PDSystem = OtherActor->FindComponentByClass<UPlantDefuseSystem>())
	{
		PDSystem->SetBombToDefuse(this);
	}
}

void ABomb::OnPlayerExitDefuseArea(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(UPlantDefuseSystem* PDSystem = OtherActor->FindComponentByClass<UPlantDefuseSystem>())
	{
		PDSystem->UnsetBombToDefuse();
	}
}

void ABomb::SetIsBeingDefused(bool bIsDefusing)
{
	Multicast_SetIsBeingDefused(bIsDefusing);
}
void ABomb::Multicast_SetIsBeingDefused_Implementation(bool bIsDefusing)
{
	bIsBeginDefused = bIsDefusing;
}

void ABomb::OnPlanted(ACharacterBase* PlanterCharacter)
{
	Planter = PlanterCharacter;
	if(Mesh_TP)
	{
		Mesh_TP->SetEnableGravity(false);
		Mesh_TP->SetSimulatePhysics(false);
		Mesh_TP->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	if(SphereComponent)
	{
		SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SphereComponent->SetCollisionProfileName(COLLISION_NoCollision, false);
	}
	SetBombState(BombPlanted);
	Server_StartTickSegment();
}

void ABomb::OnDefused()
{
	Server_ForceEndTick();
}

void ABomb::OnExploded()
{
	Server_ForceEndTick();
	Multicast_ExplosionEffects();
	Server_ExplosionDamage();
}

void ABomb::OnStartDefuse()
{
	NetMulticast_PlayBombSound(WeaponInfo.WeaponEffects.ImpactSound);
}

void ABomb::OnStartPlant(int32 PlantTime)
{
	NetMulticast_PlayBombSound(WeaponInfo.WeaponEffects.MuzzleFireSound);
	NetMulticast_PlayPlantAnimationAfterTime(PlantTime);
}

void ABomb::OnStopPlant()
{
	GetWorld()->GetTimerManager().ClearTimer(PlantOrDefuseAnimationTimerHandle);
	Multicast_SetAimOffsetToNormal();
	CancelAllAnimations();
}

void ABomb::Multicast_SetAimOffsetToPlanting_Implementation()
{
	if(PlantBlendSpace_TP) WeaponInfo.WeaponAnimations.BlendSpace_ArmsTP = PlantBlendSpace_TP;
}

void ABomb::Multicast_SetAimOffsetToNormal_Implementation()
{
	if(NormalBlendSpace_TP) WeaponInfo.WeaponAnimations.BlendSpace_ArmsTP = NormalBlendSpace_TP;
}

void ABomb::SetBombState(ERoundState NewBombState)
{
	Multicast_SetBombState(NewBombState);
}

void ABomb::Multicast_SetBombState_Implementation(ERoundState NewBombState)
{
	BombState = NewBombState;
}

void ABomb::NetMulticast_PlayPlantAnimationAfterTime_Implementation(int32 PlantTime)
{
	float Delay = 0;
	if(Bomb) Delay = WeaponInfo.ReloadInfo.EquipTime;
	
	FTimerDelegate AnimationDelegate;
	AnimationDelegate.BindUFunction(this, FName(TEXT("PlantAnimation")), PlantTime);
	GetWorld()->GetTimerManager().SetTimer(PlantOrDefuseAnimationTimerHandle, AnimationDelegate, 1, false, Delay + 0.01);
}

void ABomb::PlantAnimation(int32 PlantTime)
{
	Multicast_SetAimOffsetToPlanting();
	UAnimMontage* PlantMontage_FP = WeaponInfo.WeaponAnimations.FireAnim_ArmsFP;
	UAnimMontage* PlantMontage_TP= WeaponInfo.WeaponAnimations.FireAnim_ArmsTP;
	if(!CharacterPlayer || !PlantMontage_FP || !PlantMontage_TP) return;
	PlayAnimationWithTime(PlantMontage_FP, CharacterPlayer->GetArmsMeshFP(), PlantTime);
	PlayAnimationWithTime(PlantMontage_TP, CharacterPlayer->GetMesh(), PlantTime);
}

void ABomb::NetMulticast_PlayBombSound_Implementation(USoundCue* BombSound)
{
	if(BombSound) UGameplayStatics::PlaySoundAtLocation(GetWorld(), BombSound, GetActorLocation(), GetActorRotation());
}

void ABomb::Server_SetTickingSegments_Implementation()
{
	int32 DetonateTime = 0;
	int32 TotalSegments = 0;
	if(APlantAndDefuseGameMode* PDGM = Cast<APlantAndDefuseGameMode>(GetWorld()->GetAuthGameMode()))
	{
		DetonateTime = PDGM->GetDetonateTimeInSeconds();
	}

	for (FBombTickingSegments Segment : BombTickingSegments)
	{
		TotalSegments += Segment.SegmentSize;
	}

	for (int i = 0; i < BombTickingSegments.Num(); i++)
	{
		float SegmentSizeInSeconds = (float(BombTickingSegments[i].SegmentSize) / float(TotalSegments)) * DetonateTime;
		if(i == 0) SegmentSizeInSeconds = FMath::Clamp(--SegmentSizeInSeconds, 0.0f, 100000.0f);
		BombTickingSegments[i].SegmentSizeInSeconds = SegmentSizeInSeconds;
	}
}

void ABomb::Server_StartTickSegment_Implementation()
{
	float Rate = 1 / float(BombTickingSegments[TickSegmentIndex].BeatsPerSecond);
	int32 SegmentSize = BombTickingSegments[TickSegmentIndex].SegmentSizeInSeconds;
	GetWorldTimerManager().SetTimer(TickSegmentTimerHandle, this, &ABomb::Multicast_PlayTickSound, Rate, true);
	GetWorldTimerManager().SetTimer(StopTickTimerHandle, this, &ABomb::Server_EndTickSegment, 1, false, SegmentSize);
}

void ABomb::Server_EndTickSegment_Implementation()
{
	GetWorldTimerManager().ClearTimer(TickSegmentTimerHandle);
	Multicast_IncrementTickSegmentIndex();
	if(TickSegmentIndex >= BombTickingSegments.Num())
	{
		GetWorldTimerManager().ClearTimer(StopTickTimerHandle);
		return;
	}
	Server_StartTickSegment();
}

void ABomb::Multicast_PlayTickSound_Implementation()
{
	float Volume = BombTickingSegments[TickSegmentIndex].VolumeMultiplier;
	if(BombTickSound) UGameplayStatics::PlaySoundAtLocation(GetWorld(), BombTickSound, GetActorLocation(), GetActorRotation(), Volume);
}

void ABomb::Multicast_IncrementTickSegmentIndex_Implementation()
{
	TickSegmentIndex++;
}

void ABomb::Server_ForceEndTick_Implementation()
{
	GetWorldTimerManager().ClearTimer(TickSegmentTimerHandle);
	GetWorldTimerManager().ClearTimer(StopTickTimerHandle);
}

void ABomb::Multicast_ExplosionEffects_Implementation()
{
	UParticleSystem* ExplosionEffect = WeaponInfo.WeaponEffects.MuzzleFlashEffect;
	if(ExplosionEffect) UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
	if(ExplosionSound) UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSound, GetActorLocation(), GetActorRotation(), ExplosionVolume);
}

void ABomb::Server_ExplosionDamage_Implementation()
{
	TArray<AActor*> ActorsToIgnore;
	UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(), WeaponInfo.DamageInfo.HeadDamage, 1,
		GetActorLocation(), DamageInnerRadius, DamageOuterRadius, 1, UDamageType::StaticClass(), ActorsToIgnore,
		this, Planter->GetController(), TRACE_BombExplosion
	);
}