#include "HealthSystem.h"
#include "MoneySystem.h"
#include "Breachers/Characters/CharacterBase.h"
#include "Breachers/GameModes/BreachersGameModeBase.h"
#include "Breachers/GameModes/PlantAndDefuseGameMode.h"
#include "Breachers/PlayerControllers/BreachersPlayerController.h"
#include "Breachers/Weapons/WeaponBase.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

UHealthSystem::UHealthSystem()
{
	PrimaryComponentTick.bCanEverTick = false;
	MaxHealth = 100;
	bIsDead = false;
}

void UHealthSystem::BeginPlay()
{
	Super::BeginPlay();
	SetIsReplicated(true);
	CharacterPlayer = Cast<ACharacterBase>(GetOwner());
	if(CharacterPlayer && CharacterPlayer->HasAuthority())
	{
		CharacterPlayer->OnTakePointDamage.AddDynamic(this, &UHealthSystem::OnTakePointDamage);
		CharacterPlayer->OnTakeRadialDamage.AddDynamic(this, &UHealthSystem::OnTakeRadialDamage);
		CharacterPlayer->OnTakeAnyDamage.AddDynamic(this, &UHealthSystem::OnTakeAnyDamage);
	}

	CurrentHealth = MaxHealth;

	FTimerHandle TagHandle;
	GetWorld()->GetTimerManager().SetTimer(TagHandle, this, &UHealthSystem::GetOwnerTag, 1, false, 0.1);

	if(ABreachersGameModeBase* BreachersGM = Cast<ABreachersGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		bFriendlyFireOn = BreachersGM->IsFirendlyFireOn();
	}
}

void UHealthSystem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UHealthSystem, CurrentHealth);
	DOREPLIFETIME(UHealthSystem, CharacterPlayer);
	DOREPLIFETIME(UHealthSystem, bIsDead);
}

void UHealthSystem::Reset()
{
	bIsDead = false;
	CurrentHealth = MaxHealth;
}

void UHealthSystem::OnTakePointDamage(AActor* DamagedActor, float Damage, AController* InstigatedBy,
                                      FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection,
                                      const UDamageType* DamageType, AActor* DamageCauser)
{
	int32 DamageDealt = static_cast<int32>(Damage);
	if(DamageDealt > 0) Multicast_ShowBlood(HitLocation);
	if(CurrentHealth - DamageDealt <= 0) CharacterPlayer->PushOnDeath(DamageCauser, CharacterPlayer->GetActorLocation() - ShotFromDirection);
}

inline void UHealthSystem::OnTakeRadialDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	FVector Origin, FHitResult HitInfo, AController* InstigatedBy, AActor* DamageCauser)
{
	int32 DamageDealt = static_cast<int32>(Damage);
	if(DamageDealt > 0) Multicast_ShowBlood(CharacterPlayer->GetActorLocation());
	if(CurrentHealth - DamageDealt <= 0) CharacterPlayer->PushOnDeath(DamageCauser, CharacterPlayer->GetActorLocation() - Origin);
}

inline void UHealthSystem::OnTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* InstigatedBy, AActor* DamageCauser)
{
	bool bInstigatorIsTeammate = InstigatedBy->GetCharacter()->ActorHasTag(OwnerTeamTag);
	if(!bFriendlyFireOn && bInstigatorIsTeammate) return;
	
	int32 DamageDealt = static_cast<int32>(Damage);
	CurrentHealth = FMath::Clamp(CurrentHealth - DamageDealt, 0, MaxHealth);
	if(CurrentHealth <= 0 && !bIsDead)
	{
		bIsDead = true;
		Server_KillPlayer(InstigatedBy, DamageCauser);

		APlantAndDefuseGameMode* PDGM = Cast<APlantAndDefuseGameMode>(GetWorld()->GetAuthGameMode());
		if(PDGM && bInstigatorIsTeammate) return;
		
		RewardKiller(InstigatedBy, DamageCauser);
	}
}

void UHealthSystem::Multicast_ShowBlood_Implementation(FVector HitLocation)
{
	if(BloodEffect) UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BloodEffect, HitLocation);
}

void UHealthSystem::OnRep_IsDead()
{
	bIsDead = true;
}

void UHealthSystem::Server_KillPlayer_Implementation(AController* InstigatedBy, AActor* DamageCauser)
{
	OnRep_IsDead();
	OnDie.Broadcast(InstigatedBy, DamageCauser);
}

void UHealthSystem::RewardKiller(AController* InstigatedBy, AActor* DamageCauser)
{
	AWeaponBase* KillerWeapon = Cast<AWeaponBase>(DamageCauser);
	ACharacterBase* KillerCharacter = Cast<ACharacterBase>(InstigatedBy->GetPawn());
	if(KillerCharacter && KillerWeapon)
	{
		KillerCharacter->GetBreacherPC()->MoneySystem->AddToCurrentMoney(KillerWeapon->WeaponInfo.KillRewardMoney);
		KillerCharacter->GetBreacherPC()->OnKill();
	}
}

void UHealthSystem::GetOwnerTag()
{
	OwnerTeamTag = GetOwner()->ActorHasTag(FName(TAG_Attacker)) ? TAG_Attacker : TAG_Defender;
}
