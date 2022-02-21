#include "Bomb.h"

#include "Breachers/Characters/CharacterBase.h"
#include "Breachers/Components/PlantDefuseSystem.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

ABomb::ABomb()
{
	DefuseArea = CreateDefaultSubobject<USphereComponent>("DefuseArea");
	DefuseArea->InitSphereRadius(130);
	DefuseArea->SetupAttachment(RootComponent);

	bIsBeginDefused = false;
	BombState = BombUnplanted;
}

void ABomb::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABomb, bIsBeginDefused);
	DOREPLIFETIME(ABomb, BombState);
}

void ABomb::BeginPlay()
{
	Super::BeginPlay();
	DefuseArea->OnComponentBeginOverlap.AddDynamic(this, &ABomb::OnPlayerEnterDefuseArea);
	DefuseArea->OnComponentEndOverlap.AddDynamic(this, &ABomb::OnPlayerExitDefuseArea);
	NormalBlendSpace_TP = WeaponInfo.WeaponAnimations.BlendSpace_ArmsTP;
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

void ABomb::OnPlanted()
{
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
}

void ABomb::OnStartPlant(int32 PlantTime)
{
	Multicast_SetAimOffsetToPlanting();
	UAnimMontage* PlantMontage_FP = WeaponInfo.WeaponAnimations.FireAnim_ArmsFP;
	UAnimMontage* PlantMontage_TP= WeaponInfo.WeaponAnimations.FireAnim_ArmsTP;
	if(!CharacterPlayer || !PlantMontage_FP || !PlantMontage_TP) return;
	PlayAnimationWithTime(PlantMontage_FP, CharacterPlayer->GetArmsMeshFP(), PlantTime);
	PlayAnimationWithTime(PlantMontage_TP, CharacterPlayer->GetMesh(), PlantTime);
}

void ABomb::OnStopPlant()
{
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