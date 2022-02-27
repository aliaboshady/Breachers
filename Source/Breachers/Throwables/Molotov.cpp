#include "Molotov.h"

AMolotov::AMolotov()
{
	ActivationSlope = 5;
}

void AMolotov::BeginPlay()
{
	Super::BeginPlay();
	Mesh_TP->OnComponentHit.AddDynamic(this, &AMolotov::OnHit);
}

void AMolotov::OnActivate()
{
	// Do Something
	Super::OnActivate();
}

void AMolotov::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                     FVector NormalImpulse, const FHitResult& Hit)
{
	float Normal_X = NormalImpulse.X;
	float Normal_Y = NormalImpulse.Y;
	float sum = Normal_X + Normal_Y;
	if(sum > -ActivationSlope && sum <= ActivationSlope) OnActivate();
}
