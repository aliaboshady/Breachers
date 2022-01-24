#include "BreachersPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Breachers/Characters/CharacterBase.h"
#include "Breachers/Components/BuyMenu.h"
#include "Breachers/Components/HealthSystem.h"
#include "Breachers/Components/MoneySystem.h"
#include "Breachers/Components/WeaponSystem.h"
#include "Breachers/GameModes/BreachersGameModeBase.h"
#include "Breachers/PlayerStates/BreachersPlayerState.h"
#include "Camera/CameraActor.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ABreachersPlayerController::ABreachersPlayerController()
{
	MoneySystem = CreateDefaultSubobject<UMoneySystem>(TEXT("Money System"));
	BuyMenu = CreateDefaultSubobject<UBuyMenu>(TEXT("Buy Menu"));
	bCanOpenCloseScoreBoard = true;
}

void ABreachersPlayerController::BeginPlay()
{
	Super::BeginPlay();
	PossessStartCamera();
	BreachersGameModeBase = Cast<ABreachersGameModeBase>(GetWorld()->GetAuthGameMode());
	BreachersPlayerState = Cast<ABreachersPlayerState>(PlayerState);

	Client_CreatePauseMenuWidget();
	Client_CreateScoreBoardWidget();

	if(CountDownTimerWidgetClass && IsLocalPlayerController())
	{
		if(UUserWidget* CountDownTimerWidget = CreateWidget(this, CountDownTimerWidgetClass))
		{
			CountDownTimerWidget->AddToViewport();
		}
	}
}

void ABreachersPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABreachersPlayerController, CharacterPlayer);
}

void ABreachersPlayerController::Client_CreatePauseMenuWidget_Implementation()
{
	if(PauseMenuWidgetClass) PauseMenuWidget = CreateWidget(this, PauseMenuWidgetClass);
}
void ABreachersPlayerController::Client_CreateScoreBoardWidget_Implementation()
{
	if(ScoreBoardWidgetClass) ScoreBoardWidget = CreateWidget(this, ScoreBoardWidgetClass);
}

void ABreachersPlayerController::PossessStartCamera()
{
	ACameraActor* StartCamera = Cast<ACameraActor>(UGameplayStatics::GetActorOfClass(GetWorld(), ACameraActor::StaticClass()));
	if(StartCamera) SetViewTargetWithBlend(StartCamera, 0);
}

void ABreachersPlayerController::SetInputUI(bool bIsUI)
{
	if(bIsUI)
	{
		SetInputMode(FInputModeUIOnly());
		bShowMouseCursor = true;
	}
	else
	{
		SetInputMode(FInputModeGameOnly());
		bShowMouseCursor = false;
	}
}

void ABreachersPlayerController::ShowTeamSelectionMenu()
{
	Client_ShowTeamSelectionMenu();
}

void ABreachersPlayerController::Client_ShowTeamSelectionMenu_Implementation()
{
	if(!TeamSelectWidgetClass) return;
	TeamSelectWidget = CreateWidget(this, TeamSelectWidgetClass);
	if(TeamSelectWidget) TeamSelectWidget->AddToViewport();
	SetInputUI(true);
}

void ABreachersPlayerController::SelectAttacker()
{
	Server_SpawnAttacker();
	OnSelectCharacter();
}

void ABreachersPlayerController::SelectDefender()
{
	Server_SpawnDefender();
	OnSelectCharacter();
}

void ABreachersPlayerController::OnSelectCharacter()
{
	if(TeamSelectWidget) TeamSelectWidget->RemoveFromParent();
	SetInputUI(false);
	Client_ShowPlayerUI();
}

void ABreachersPlayerController::Client_ShowPlayerUI_Implementation()
{
	if(!PlayerUIWidgetClass) return;
	UUserWidget* UIWidget = CreateWidget(this, PlayerUIWidgetClass);
	if(UIWidget) UIWidget->AddToViewport();
}

void ABreachersPlayerController::Server_SpawnAttacker_Implementation()
{
	if(BreachersGameModeBase)
	{
		BreachersGameModeBase->RequestAttackerSpawn(this);
	}
}

void ABreachersPlayerController::Server_SpawnDefender_Implementation()
{
	if(BreachersGameModeBase)
	{
		BreachersGameModeBase->RequestDefenderSpawn(this);
	}
}

void ABreachersPlayerController::OnDie()
{
	if(BreachersGameModeBase) BreachersGameModeBase->OnPlayerDied(this);
	//if(BreachersPlayerState)
}

void ABreachersPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	CharacterPlayer = Cast<ACharacterBase>(InPawn);
	if(CharacterPlayer && CharacterPlayer->HealthSystem)
	{
		CharacterPlayer->HealthSystem->OnDie.AddDynamic(this, &ABreachersPlayerController::OnDie);
	}
}

void ABreachersPlayerController::EnableShooting(bool bEnableShooting)
{
	Server_EnableShooting(bEnableShooting);
}

void ABreachersPlayerController::Server_EnableShooting_Implementation(bool bEnableShooting)
{
	CharacterPlayer->WeaponSystem->EnableShooting(bEnableShooting);
}

void ABreachersPlayerController::ShowHidePauseMenu()
{
	if(!PauseMenuWidget || !CharacterPlayer) return;
	if(bPauseMenuOpen)
	{
		PauseMenuWidget->RemoveFromViewport();
		CharacterPlayer->EnableInput(this);
		SetInputUI(false);
	}
	else
	{
		PauseMenuWidget->AddToViewport();
		CharacterPlayer->DisableInput(this);
		SetInputUI(true);
	}
	bPauseMenuOpen = !bPauseMenuOpen;
}

void ABreachersPlayerController::OpenScoreBoard()
{
	if(!bCanOpenCloseScoreBoard) return;
	if(ScoreBoardWidget) ScoreBoardWidget->AddToViewport();
}

void ABreachersPlayerController::CloseScoreBoard()
{
	if(!bCanOpenCloseScoreBoard) return;
	if(ScoreBoardWidget) ScoreBoardWidget->RemoveFromViewport();
}