#include "HealthSystem.h"
#include "Breachers/Characters/CharacterBase.h"
#include "Net/UnrealNetwork.h"

UHealthSystem::UHealthSystem()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicated(true);
	MaxHealth = 100;
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
}

void UHealthSystem::OnTakePointDamage(AActor* DamagedActor, float Damage, AController* InstigatedBy,
	FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection,
	const UDamageType* DamageType, AActor* DamageCauser)
{
	CurrentHealth = FMath::Clamp(CurrentHealth - static_cast<int32>(Damage), 0, MaxHealth);
}