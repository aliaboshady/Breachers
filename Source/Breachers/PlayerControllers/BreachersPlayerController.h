#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BreachersPlayerController.generated.h"

class UBuyMenu;
class UMoneySystem;
class ACharacterBase;
class ABreachersPlayerState;
class ABreachersGameModeBase;

UCLASS()
class BREACHERS_API ABreachersPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ABreachersPlayerController();
	void ShowTeamSelectionMenu();
	void EnableShooting(bool bEnableShooting);
	void OpenScoreBoard();
	void CloseScoreBoard();

	UFUNCTION(BlueprintCallable)
	void ShowHidePauseMenu();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UMoneySystem* MoneySystem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBuyMenu* BuyMenu;

	UPROPERTY(Replicated, BlueprintReadOnly)
	ACharacterBase* CharacterPlayer;
	
protected:
	virtual void BeginPlay() override;
	void PossessStartCamera();
	virtual void OnPossess(APawn* InPawn) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void SetInputUI(bool bIsUI = true);

	UFUNCTION(Server, Reliable)
	void Server_EnableShooting(bool bEnableShooting);

	UFUNCTION()
	void OnDie();

	UFUNCTION(BlueprintCallable)
	void SelectAttacker();
	UFUNCTION(BlueprintCallable)
	void SelectDefender();

	void OnSelectCharacter();

	UFUNCTION(Server, Reliable)
	void Server_SpawnAttacker();

	UFUNCTION(Server, Reliable)
	void Server_SpawnDefender();

	UFUNCTION(Client, Reliable)
	void Client_ShowTeamSelectionMenu();
	
	UFUNCTION(Client, Reliable)
	void Client_ShowPlayerUI();

	UFUNCTION(Client, Reliable)
	void Client_CreatePauseMenuWidget();

	UFUNCTION(Client, Reliable)
	void Client_CreateScoreBoardWidget();

	UPROPERTY()
	ABreachersGameModeBase* BreachersGameModeBase;
	
	UPROPERTY()
	ABreachersPlayerState* BreachersPlayerState;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> TeamSelectWidgetClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> PlayerUIWidgetClass;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> PauseMenuWidgetClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> CountDownTimerWidgetClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> ScoreBoardWidgetClass;
	
	UPROPERTY()
	UUserWidget* TeamSelectWidget;

	UPROPERTY()
	UUserWidget* PauseMenuWidget;
	
	UPROPERTY()
	UUserWidget* ScoreBoardWidget;

	bool bPauseMenuOpen;
	bool bCanOpenCloseScoreBoard;
};