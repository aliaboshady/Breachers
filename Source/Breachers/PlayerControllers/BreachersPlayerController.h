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

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSpawn);

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
	void OnPlayerSpawn();
	void UpdateKillfeed(FName KillerName, bool bKillerIsAttacker, UTexture2D* WeaponIcon, FName KilledName, bool bKilledIsAttacker);
	void SetCanMove(bool bCanMovePlayer);
	void SetCanShoot(bool bCanShootPlayer);
	
	UFUNCTION(Server, Reliable)
	void Server_ChangeTeam(ETeam NewTeam);

	FORCEINLINE TEnumAsByte<ETeam> GetPlayerTeam(){return NextTeamRespawn;}
	FORCEINLINE bool HasChosenTeam(){return bHasChosenTeam;}

	UFUNCTION(Client, Reliable)
	void Client_ShowPlayerUI();
	
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

	UPROPERTY()
	FOnSpawn OnSpawn;
	
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
	void Server_SpawnAttacker();

	UFUNCTION(Server, Reliable)
	void Server_SpawnDefender();

	UFUNCTION(Server, Reliable)
	void Server_SetNextTeam(ETeam NextTeam);

	UFUNCTION(Client, Reliable)
	void Client_SetNextTeam(ETeam NextTeam);

	UFUNCTION(Client, Reliable)
	void Client_ShowTeamSelectionMenu();

	UFUNCTION(Client, Reliable)
	void Client_CreatePauseMenuWidget();

	UFUNCTION(Client, Reliable)
	void Client_CreateScoreBoardWidget();

	UFUNCTION(Client, Reliable)
	void Client_CreateKillfeedWidget();

	UFUNCTION(Client, Reliable)
	void Client_CreateCountDownWidget();

	UFUNCTION(Client, Reliable)
	void Client_UpdateKillfeed(FName KillerName, bool bKillerIsAttacker, UTexture2D* WeaponIcon, FName KilledName, bool bKilledIsAttacker);
	
	UFUNCTION(Server, Reliable)
	void Server_SetCanMove(bool bCanMovePlayer);
	
	UFUNCTION(Client, Reliable)
	void Client_SetCanMove(bool bCanMovePlayer);

	UFUNCTION(Server, Reliable)
	void Server_SetCanShoot(bool bCanShootPlayer);
	
	UFUNCTION(Client, Reliable)
	void Client_SetCanShoot(bool bCanShootPlayer);

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

	UPROPERTY(Replicated)
	TEnumAsByte<ETeam> NextTeamRespawn;

	UPROPERTY(Replicated)
	bool bHasChosenTeam;

	bool bCanMove;
	bool bCanShoot;
};
