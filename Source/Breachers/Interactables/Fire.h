#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Fire.generated.h"

class USoundCue;
class UBoxComponent;

UCLASS()
class BREACHERS_API AFire : public AActor
{
	GENERATED_BODY()
	
public:	
	AFire();
	void SetDamage(float NewDamage);

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION(Server, Reliable)
	void Server_DamagePlayers();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayerBurningSound();

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* BoxCollision;
	
	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* FireParticles;

	UPROPERTY(VisibleAnywhere)
	UAudioComponent* FireSound;

	UPROPERTY(EditAnywhere)
	USoundCue* PlayerBurningSound;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.01"))
	float Rate;

	float Damage;
};
