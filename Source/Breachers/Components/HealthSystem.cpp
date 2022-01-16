#include "HealthSystem.h"

#include "MoneySystem.h"
#include "Breachers/Characters/CharacterBase.h"
#include "Breachers/Core/BreachersPlayerController.h"
#include "Breachers/Weapons/WeaponBase.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

UHealthSystem::UHealthSystem()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicated(true);
	MaxHealth = 100;
	bIsDead = false;
}

void UHealthSystem::BeginPlay()
{
	Super::BeginPlay();
	CharacterPlayer = Cast<ACharacterBase>(GetOwner());
	if(CharacterPlayer && CharacterPlayer->HasAuthority())
	{
		CharacterPlayer->OnTakePointDamage.AddDynamic(this, &UHealthSystem::OnTakePointDamage);
	}

	CurrentHealth = MaxHealth;
}

void UHealthSystem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UHealthSystem, CurrentHealth);
	DOREPLIFETIME(UHealthSystem, CharacterPlayer);
	DOREPLIFETIME(UHealthSystem, bIsDead);
}

void UHealthSystem::OnTakePointDamage(AActor* DamagedActor, float Damage, AController* InstigatedBy,
                                      FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection,
                                      const UDamageType* DamageType, AActor* DamageCauser)
{
	int32 DamageDealt = static_cast<int32>(Damage);
	if(DamageDealt > 0) Multicast_ShowBlood(HitLocation);
	CurrentHealth = FMath::Clamp(CurrentHealth - DamageDealt, 0, MaxHealth);
	if(CurrentHealth <= 0 && !bIsDead)
	{
		bIsDead = true;
		Server_KillPlayer();
		RewardKiller(InstigatedBy, DamageCauser);
		CharacterPlayer->PushOnDeath(DamageCauser, CharacterPlayer->GetActorLocation() - ShotFromDirection);
	}
}

void UHealthSystem::Multicast_ShowBlood_Implementation(FVector HitLocation)
{
	if(BloodEffect) UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BloodEffect, HitLocation);
}

void UHealthSystem::OnRep_IsDead() const
{
	if(!CharacterPlayer) return;
	OnDie.Broadcast();
}

void UHealthSystem::Server_KillPlayer_Implementation()
{
	OnRep_IsDead();
}

void UHealthSystem::RewardKiller(AController* InstigatedBy, AActor* DamageCauser)
{
	AWeaponBase* KillerWeapon = Cast<AWeaponBase>(DamageCauser);
	ACharacterBase* KillerCharacter = Cast<ACharacterBase>(InstigatedBy->GetPawn());
	if(KillerCharacter && KillerWeapon)
	{
		KillerCharacter->GetBreacherPC()->MoneySystem->AddToCurrentMoney(KillerWeapon->WeaponInfo.KillRewardMoney);
	}
}