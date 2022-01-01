#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthSystem.generated.h"

class ACharacterBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BREACHERS_API UHealthSystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	UHealthSystem();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION()
	void OnTakePointDamage(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation, class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const class UDamageType* DamageType, AActor* DamageCauser);
	
	UPROPERTY(Replicated)
	ACharacterBase* CharacterPlayer;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MaxHealth;

	UPROPERTY(Replicated)
	int32 CurrentHealth;
};
