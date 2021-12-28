#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponSystem.generated.h"

class AWeaponBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BREACHERS_API UWeaponSystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	UWeaponSystem();
	bool HasPrimaryWeapon() const;
	bool HasSecondaryWeapon() const;

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	AWeaponBase* CurrentWeapon;

	UPROPERTY()
	AWeaponBase* PrimaryWeapon;

	UPROPERTY()
	AWeaponBase* SecondaryWeapon;
};
