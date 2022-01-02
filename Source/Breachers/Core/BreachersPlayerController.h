#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BreachersPlayerController.generated.h"

class ABreachersGameModeBase;

UCLASS()
class BREACHERS_API ABreachersPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	void ShowTeamSelectionMenu();
	
protected:
	virtual void BeginPlay() override;
	void PossessStartCamera();
	void SetInputUI(bool bIsUI = true);

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

	UPROPERTY()
	ABreachersGameModeBase* BreachersGameModeBase;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> TeamSelectWidgetClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> PlayerUIWidgetClass;
	
	UPROPERTY()
	UUserWidget* TeamSelectWidget;
};
