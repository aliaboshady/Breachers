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
	virtual void OnUnquip() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void Client_Recoil(FVector CalculatedRecoil) override;
	virtual FVector RecoilShot(float Spread);
	void ResetCanScope();
	virtual void OnRestartRound() override;

	virtual void OnDie(AController* InstigatedBy, AActor* DamageCauser) override;
	virtual void OnPlayerUnpossessed() override;

	UFUNCTION(Client, Reliable)
	void Client_ChangeAddedRecoil(float AddedRecoil);

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
	
	UFUNCTION(Client, Reliable)
	void Client_CreatScopeWidget();

	UFUNCTION(Client, Reliable)
	void Client_ShowScopeWidget(bool bShowScope);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ScopeZoomMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0.01", UIMin = "0.01"));
	float ScopingTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ScopingCooldown;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AddedRecoilUnscoped;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FTransform ArmsFP_Transform_Scoped;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimSequenceBase* ScopedIdlePose_ArmsTP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAimOffsetBlendSpace1D* ScopedBlendSpace_ArmsTP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UUserWidget> ScopeWidgetClass;

	UPROPERTY()
	UAnimSequenceBase* UnscopedIdlePose_ArmsTP;

	UPROPERTY()
	UAimOffsetBlendSpace1D* UnscopedBlendSpace_ArmsTP;

	UPROPERTY()
	UUserWidget* ScopeWidget;
	
	FTransform OldTransform;
	FTransform NewTransform;

	FTimerHandle ScopeTimer;
	bool bCanScope;
	bool bWantsToScope;
	float ScopeTimeAlpha;
	float DefaultScopingZoom;
	float CurrentAddedRecoil;
	bool bIsInScope;
};
