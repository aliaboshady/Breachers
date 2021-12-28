#include "WeaponBase.h"
#include "Breachers/Characters/CharacterBase.h"
#include "Breachers/Components/WeaponSystem.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"

AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

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
		if(ACharacterBase* Player = Cast<ACharacterBase>(OtherActor))
		{
			if(!Player->WeaponSystem->HasPrimaryWeapon() || !Player->WeaponSystem->HasSecondaryWeapon())
			{
				Player->EquipWeapon(this);
				GetEquiped(Player);
			}
		}
	}
}

void AWeaponBase::GetEquiped(ACharacterBase* Player)
{
	SetOwner(Player);
	SetInstigator(Player);
	
	if(Mesh_TP)
	{
		Mesh_TP->SetEnableGravity(false);
		Mesh_TP->SetSimulatePhysics(false);
		Mesh_TP->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	if(SphereComponent) SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
