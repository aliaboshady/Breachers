#pragma once
#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "Bomb.generated.h"

UCLASS()
class BREACHERS_API ABomb : public AWeaponBase
{
	GENERATED_BODY()

public:
	ABomb();
	
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnPlayerEnterDefuseArea(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void OnPlayerExitDefuseArea(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UPROPERTY(VisibleAnywhere)
	USphereComponent* DefuseArea;
};
