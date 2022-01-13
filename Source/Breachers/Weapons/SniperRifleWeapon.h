#pragma once
#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "SniperRifleWeapon.generated.h"

UCLASS()
class BREACHERS_API ASniperRifleWeapon : public AWeaponBase
{
	GENERATED_BODY()

public:
	ASniperRifleWeapon();

protected:
	virtual void OnPrimaryFire() override;
	virtual void OnSecondaryFire() override;
	virtual void OnReload() override;
	virtual void OnEquip() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	void ResetCanScope();

	UFUNCTION(Server, Reliable)
	void Server_HandleTickDisabling(float Alpha);

	UFUNCTION(Server, Reliable)
	void Server_ForceUnscope();

	UFUNCTION(Client, Reliable)
	void Client_ForceUnscope_FP();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ForceUnscope_TP();
	
	UFUNCTION(Client, Reliable)
	void Client_ScopeHandle_FP(FTransform Transform1, FTransform Transform2, float Alpha);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ScopeHandle_TP();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.01", UIMin = "0.01"));
	float ScopingTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ScopingCooldown;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FTransform ArmsFP_Transform_Scoped;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimSequenceBase* ScopedIdlePose_ArmsTP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAimOffsetBlendSpace1D* ScopedBlendSpace_ArmsTP;

	UPROPERTY()
	UAnimSequenceBase* UnscopedIdlePose_ArmsTP;

	UPROPERTY()
	UAimOffsetBlendSpace1D* UnscopedBlendSpace_ArmsTP;
	
	FTransform OldTransform;
	FTransform NewTransform;

	FTimerHandle ScopeTimer;
	bool bCanScope;
	bool bWantsToScope;
	bool bIsInScope;
	float ScopeTimeAlpha;
};
