#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuyMenu.generated.h"

#define INPUT_OpenBuyMenu "BuyMenu"

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

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void ShowHideBuyMenu();
	void SetInputUI(bool bIsUI);

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