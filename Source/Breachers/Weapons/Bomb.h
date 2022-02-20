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
	void SetIsBeingDefused(bool bIsDefusing);
	FORCEINLINE bool GetIsBeingDefused(){return bIsBeginDefused;}
	void OnStartPlant(int32 PlantTime);
	void OnStopPlant();
	void OnPlanted();
	FORCEINLINE float GetArmsPositionOffset(){return ArmsPositionOffsetWhilePlanting;}
	
protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnPlayerEnterDefuseArea(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void OnPlayerExitDefuseArea(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(Server, Reliable)
	void Server_SetIsBeingDefused(bool bIsDefusing);
	
	UFUNCTION(Client, Reliable)
	void Client_SetIsBeingDefused(bool bIsDefusing);
	
	UPROPERTY(VisibleAnywhere)
	USphereComponent* DefuseArea;

	UPROPERTY(EditAnywhere)
	UAnimMontage* PlantMontage;

	UPROPERTY(EditAnywhere)
	float ArmsPositionOffsetWhilePlanting;

	UPROPERTY(Replicated)
	bool bIsBeginDefused;
};
