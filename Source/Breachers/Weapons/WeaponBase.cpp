#include "WeaponBase.h"
#include "Breachers/Characters/CharacterBase.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

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
		if(ACharacterBase* Player = Cast<ACharacterBase>(OtherActor))
		{
			if(Player->CanTakeWeapon(this))
			{
				Player->TakeWeapon(this);
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
	Server_OnFire();
}

void AWeaponBase::Server_OnFire_Implementation()
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
	//Server_ShootInfo(OutHit);
}

FVector AWeaponBase::RecoilShot(float Spread) const
{
	FVector NewLocation = FVector(0);
	NewLocation.X += FMath::RandRange(-Spread, Spread);
	NewLocation.Y += FMath::RandRange(-Spread, Spread);
	NewLocation.Z += FMath::RandRange(-Spread, Spread);
	return NewLocation;
}