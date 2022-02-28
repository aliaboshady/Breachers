#include "Molotov.h"

#include "Breachers/Characters/CharacterBase.h"
#include "Breachers/Interactables/Fire.h"

AMolotov::AMolotov()
{
	ActivationSlope = 5;
}

void AMolotov::OnActivate()
{
	Server_SpawnFire();
	Super::OnActivate();
}

void AMolotov::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                     FVector NormalImpulse, const FHitResult& Hit)
{
	Super::OnHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);
	float Normal_X = NormalImpulse.X;
	float Normal_Y = NormalImpulse.Y;
	float sum = Normal_X + Normal_Y;
	if(sum > -ActivationSlope && sum <= ActivationSlope) OnActivate();
}

void AMolotov::Server_SpawnFire_Implementation()
{
	if(!FireClass) return;
	
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	if(AFire* Fire = GetWorld()->SpawnActor<AFire>(FireClass, GetActorLocation(), FRotator(0), SpawnParameters))
	{
		Fire->SetLifeSpan(ActivationDuration);
		Fire->SetDamage(WeaponInfo.DamageInfo.HeadDamage);
		Fire->SetInstigator(CharacterPlayer);
		Fire->SetOwner(this);
	}
}
