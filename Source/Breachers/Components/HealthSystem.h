#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthSystem.generated.h"

class ACharacterBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDie);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BREACHERS_API UHealthSystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	UHealthSystem();

	UPROPERTY()
	FOnDie OnDie;

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void RewardKiller(AController* InstigatedBy, AActor* DamageCauser);
	void GetOwnerTag();
	
	UFUNCTION()
	void OnTakePointDamage(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation, class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const class UDamageType* DamageType, AActor* DamageCauser);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ShowBlood(FVector HitLocation);
	
	UFUNCTION(Server, Reliable)
	void Server_KillPlayer();

	UFUNCTION()
	void OnRep_IsDead() const;
	
	UPROPERTY(Replicated)
	ACharacterBase* CharacterPlayer;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MaxHealth;

	UPROPERTY(EditAnywhere)
	UParticleSystem* BloodEffect;

	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 CurrentHealth;

	UPROPERTY(ReplicatedUsing=OnRep_IsDead)
	bool bIsDead;

	bool bFriendlyFireOn;
	FName OwnerTeamTag;
};