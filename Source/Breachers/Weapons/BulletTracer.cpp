#include "BulletTracer.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

ABulletTracer::ABulletTracer()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	TracerParticle = CreateDefaultSubobject<UParticleSystemComponent>("TracerParticle_FP");
	TracerParticle->SetOwnerNoSee(true);
	RootComponent = TracerParticle;
	
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->MaxSpeed = 3000;
	ProjectileMovement->InitialSpeed = 3000;
	ProjectileMovement->ProjectileGravityScale = 0;
}

void ABulletTracer::BeginPlay()
{
	Super::BeginPlay();
	SetReplicateMovement(true);
}

float ABulletTracer::GetBulletSpeed()
{
	return ProjectileMovement->InitialSpeed;
}
