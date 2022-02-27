#include "Grenade.h"
#include "Breachers/Characters/CharacterBase.h"
#include "Kismet/GameplayStatics.h"

AGrenade::AGrenade()
{
	DamageInnerRadius = 100;
	DamageOuterRadius = 300;
}

void AGrenade::OnThrow()
{
	Super::OnThrow();
	FTimerHandle ActivateTimer;
	GetWorldTimerManager().SetTimer(ActivateTimer, this, &AGrenade::OnActivate, 1, false, TimeToActivate);
}

void AGrenade::OnActivate()
{
	Server_ExplosionDamage();
	Super::OnActivate();
}

void AGrenade::Server_ExplosionDamage_Implementation()
{
	if(!ThrowerCharacter) return;
	
	TArray<AActor*> ActorsToIgnore;
	UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(), WeaponInfo.DamageInfo.HeadDamage, 1,
		GetActorLocation(), DamageInnerRadius, DamageOuterRadius, 1, UDamageType::StaticClass(), ActorsToIgnore,
		this, ThrowerCharacter->GetController(), ECC_Visibility
	);
}