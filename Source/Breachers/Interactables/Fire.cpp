#include "Fire.h"

#include "Breachers/Characters/CharacterBase.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

AFire::AFire()
{
	PrimaryActorTick.bCanEverTick = false;
	BoxCollision = CreateDefaultSubobject<UBoxComponent>("BoxCollision");
	BoxCollision->SetBoxExtent(FVector(220, 220, 60));
	RootComponent = BoxCollision;
	
	FireParticles = CreateDefaultSubobject<UParticleSystemComponent>("FireParticles");
	FireParticles->SetupAttachment(RootComponent);
	FireParticles->SetRelativeLocation(FVector(0, 0, -33));

	bReplicates = true;
	Rate = 1;
}

void AFire::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle DamageTimer;
	GetWorldTimerManager().SetTimer(DamageTimer, this, &AFire::Server_DamagePlayers, 1 / Rate, true, 0.1);
}

void AFire::SetDamage(float NewDamage)
{
	Damage = NewDamage;
}

void AFire::Server_DamagePlayers_Implementation()
{
	TArray<AActor*> OverlappingActors;
	BoxCollision->GetOverlappingActors(OverlappingActors, ACharacterBase::StaticClass());

	for (AActor* Player : OverlappingActors)
	{
		UGameplayStatics::ApplyDamage(Player, Damage, GetInstigator()->Controller, GetOwner(), UDamageType::StaticClass());
	}
}
