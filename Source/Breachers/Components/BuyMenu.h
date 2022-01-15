#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuyMenu.generated.h"

#define INPUT_OpenBuyMenu "BuyMenu"

class AWeaponBase;
class ACharacterBase;
class ABreachersPlayerController;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BREACHERS_API UBuyMenu : public UActorComponent
{
	GENERATED_BODY()

public:	
	UBuyMenu();
	void SetPlayerInputComponent(UInputComponent* PlayerInputComponent);
	void EnableBuying(bool bEnableBuying);

	UFUNCTION(BlueprintCallable)
	void BuyWeapon(int32 Price, TSubclassOf<AWeaponBase> WeaponClass);

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void ShowHideBuyMenu();
	void SetInputUI(bool bIsUI);

	UFUNCTION(Server, Reliable)
	void Server_BuyWeapon(int32 Price, TSubclassOf<AWeaponBase> WeaponClass);
	

	UFUNCTION(Client, Reliable)
	void Client_CreateBuyMenu();

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> BuyMenuWidgetClass;

	UPROPERTY(Replicated)
	bool bCanBuy;

	UPROPERTY(Replicated)
	ACharacterBase* CharacterPlayer;
	
	UPROPERTY(Replicated)
	ABreachersPlayerController* PC;
	
	bool bBuyMenuOpen;

	UPROPERTY()
	UUserWidget* BuyMenuWidget;
};