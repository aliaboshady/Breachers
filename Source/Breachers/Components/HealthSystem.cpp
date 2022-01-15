#include "HealthSystem.h"

#include "MoneySystem.h"
#include "Breachers/Characters/CharacterBase.h"
#include "Breachers/Weapons/WeaponBase.h"
#include "Components/CapsuleComponent.h"
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
	CurrentHealth = FMath::Clamp(CurrentHealth - static_cast<int32>(Damage), 0, MaxHealth);
	if(CurrentHealth <= 0 && !bIsDead)
	{
		AWeaponBase* KillerWeapon = Cast<AWeaponBase>(DamageCauser);
		ACharacterBase* KillerCharacter = Cast<ACharacterBase>(InstigatedBy->GetPawn());
		if(KillerCharacter && KillerWeapon)
		{
			KillerCharacter->MoneySystem->AddToCurrentMoney(KillerWeapon->WeaponInfo.KillRewardMoney);
		}
		bIsDead = true;
		Server_KillPlayer();
	}
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
