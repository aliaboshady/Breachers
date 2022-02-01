#pragma once
#include "CoreMinimal.h"
#include "Breachers/Characters/CharacterBase.h"
#include "GameFramework/PlayerController.h"
#include "BreachersPlayerController.generated.h"

class UBuyMenu;
class UKillfeed;
class UScoreBoard;
class UMoneySystem;
class ACharacterBase;
class ABreachersPlayerState;
class ABreachersGameModeBase;
class ABreachersGameState;

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
	void ToggleChangeTeamMenu();
	void OnKill();
	void UpdateKillfeed(FName KillerName, UTexture2D* WeaponIcon, FName KilledName);

	UFUNCTION(Client, Reliable)
	void Client_ClearAllWidgets();
	
	UFUNCTION(Client, Reliable)
	void Client_DisableScoreBoard();
	
	UFUNCTION(Client, Reliable)
	void Client_OpenScoreBoard();

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
	void KillPlayer();

	UFUNCTION(Server, Reliable)
	void Server_EnableShooting(bool bEnableShooting);

	UFUNCTION()
	void OnDie(AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION(BlueprintCallable)
	void SelectAttacker();
	UFUNCTION(BlueprintCallable)
	void SelectDefender();

	void OnSelectCharacter();

	UFUNCTION(Server, Reliable)
	void Server_ChangeTeam(ETeam NewTeam);

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

	UFUNCTION(Client, Reliable)
	void Client_CreateKillfeedWidget();

	UFUNCTION(Client, Reliable)
	void Client_UpdateKillfeed(FName KillerName, UTexture2D* WeaponIcon, FName KilledName);

	UPROPERTY()
	ABreachersGameModeBase* BreachersGameModeBase;
	
	UPROPERTY()
	ABreachersGameState* BreachersGameState;
	
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
	TSubclassOf<UScoreBoard> ScoreBoardWidgetClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UKillfeed> KillfeedWidgetClass;
	
	UPROPERTY()
	UUserWidget* TeamSelectWidget;

	UPROPERTY()
	UUserWidget* PauseMenuWidget;
	
	UPROPERTY()
	UScoreBoard* ScoreBoardWidget;

	UPROPERTY()
	UKillfeed* KillfeedWidget;

	bool bPauseMenuOpen;
	bool bCanOpenCloseScoreBoard;
	TEnumAsByte<ETeam> NextTeamRespawn;
};
