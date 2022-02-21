#pragma once
#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "Breachers/GameStates/PlantAndDefuseGameState.h"
#include "Bomb.generated.h"

UCLASS()
class BREACHERS_API ABomb : public AWeaponBase
{
	GENERATED_BODY()

public:
	ABomb();
	void SetIsBeingDefused(bool bIsDefusing);
	FORCEINLINE bool GetIsBeingDefused(){return bIsBeginDefused;}
	FORCEINLINE ERoundState GetBombState(){return BombState;}
	void OnStartPlant(int32 PlantTime);
	void SetBombState(ERoundState NewBombState);
	void OnStopPlant();
	void OnPlanted();
	
protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnPlayerEnterDefuseArea(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void OnPlayerExitDefuseArea(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void PlantAnimation(int32 PlantTime);
	
	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_PlayPlantAnimationAfterTime(int32 PlantTime);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetIsBeingDefused(bool bIsDefusing);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetAimOffsetToPlanting();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetAimOffsetToNormal();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetBombState(ERoundState NewBombState);
	
	UPROPERTY(VisibleAnywhere)
	USphereComponent* DefuseArea;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAimOffsetBlendSpace1D* PlantBlendSpace_TP;

	UPROPERTY()
	UAimOffsetBlendSpace1D* NormalBlendSpace_TP;

	UPROPERTY(Replicated)
	bool bIsBeginDefused;
	
	UPROPERTY(Replicated)
	TEnumAsByte<ERoundState> BombState;

	FTimerHandle PlantOrDefuseAnimationTimerHandle;
};
