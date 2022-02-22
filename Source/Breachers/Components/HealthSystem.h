#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthSystem.generated.h"

class ACharacterBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDie, AController* , InstigatedBy, AActor* , DamageCauser);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BREACHERS_API UHealthSystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	UHealthSystem();

	UFUNCTION(Server, Reliable)
	void Server_KillPlayer(AController* InstigatedBy, AActor* DamageCauser);

	void Reset();

	FORCEINLINE bool GetIsDead(){return bIsDead;}

	UPROPERTY()
	FOnDie OnDie;

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void RewardKiller(AController* InstigatedBy, AActor* DamageCauser);
	void GetOwnerTag();
	
	UFUNCTION()
	void OnTakePointDamage(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation, class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const class UDamageType* DamageType, AActor* DamageCauser);

	UFUNCTION()
	void OnTakeRadialDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, FVector Origin, FHitResult HitInfo, AController* InstigatedBy, AActor* DamageCauser );

	UFUNCTION()
	void OnTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ShowBlood(FVector HitLocation);

	UFUNCTION()
	void OnRep_IsDead();
	
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
