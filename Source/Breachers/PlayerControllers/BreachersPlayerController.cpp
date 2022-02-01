#include "BreachersPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Breachers/Characters/CharacterBase.h"
#include "Breachers/Components/BuyMenu.h"
#include "Breachers/Components/HealthSystem.h"
#include "Breachers/Components/MoneySystem.h"
#include "Breachers/Components/WeaponSystem.h"
#include "Breachers/GameModes/BreachersGameModeBase.h"
#include "Breachers/GameStates/BreachersGameState.h"
#include "Breachers/PlayerStates/BreachersPlayerState.h"
#include "Breachers/Widgets/ScoreBoard.h"
#include "Breachers/Widgets/Killfeed.h"
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
	BreachersGameState = Cast<ABreachersGameState>(GetWorld()->GetGameState());
	BreachersPlayerState = Cast<ABreachersPlayerState>(PlayerState);

	Client_CreatePauseMenuWidget();
	Client_CreateScoreBoardWidget();
	Client_CreateKillfeedWidget();

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

void ABreachersPlayerController::Client_ClearAllWidgets_Implementation()
{
	UWidgetLayoutLibrary::RemoveAllWidgets(this);
}

void ABreachersPlayerController::Client_CreatePauseMenuWidget_Implementation()
{
	if(PauseMenuWidgetClass) PauseMenuWidget = CreateWidget(this, PauseMenuWidgetClass);
}
void ABreachersPlayerController::Client_CreateScoreBoardWidget_Implementation()
{
	if(ScoreBoardWidgetClass) ScoreBoardWidget = CreateWidget<UScoreBoard>(this, ScoreBoardWidgetClass);
}
void ABreachersPlayerController::Client_CreateKillfeedWidget_Implementation()
{
	if(KillfeedWidgetClass)
	{
		KillfeedWidget = CreateWidget<UKillfeed>(this, KillfeedWidgetClass);
		if(KillfeedWidget) KillfeedWidget->AddToViewport();
	}
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

void ABreachersPlayerController::KillPlayer()
{
	if(ACharacterBase* PlayerCharacter = Cast<ACharacterBase>(GetPawn()))
	{
		PlayerCharacter->HealthSystem->Server_KillPlayer(this, nullptr);
	}
}

void ABreachersPlayerController::ShowTeamSelectionMenu()
{
	Client_ClearAllWidgets();
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
	Server_ChangeTeam(Attacker);
	OnSelectCharacter();
	
	if(GetPawn())
	{
		KillPlayer();
	}
	else
	{
		Server_SpawnAttacker();
	}
}

void ABreachersPlayerController::SelectDefender()
{
	Server_ChangeTeam(Defender);
	OnSelectCharacter();
	
	if(GetPawn())
	{
		KillPlayer();
	}
	else
	{
		Server_SpawnDefender();
	}
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

void ABreachersPlayerController::OnDie(AController* InstigatedBy, AActor* DamageCauser)
{
	if(BreachersGameModeBase) BreachersGameModeBase->OnPlayerDied(this, NextTeamRespawn);
	if(BreachersPlayerState) BreachersPlayerState->OnDie(InstigatedBy, DamageCauser);
}

void ABreachersPlayerController::OnKill()
{
	if(BreachersPlayerState) BreachersPlayerState->OnKill();
}

void ABreachersPlayerController::OnPlayerSpawn()
{
	OnSpawn.Broadcast();
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
	if(!bCanOpenCloseScoreBoard || !ScoreBoardWidget) return;
	ScoreBoardWidget->AddToViewport();
	ScoreBoardWidget->OnAddToScreen(BreachersGameState);
}

void ABreachersPlayerController::CloseScoreBoard()
{
	if(!bCanOpenCloseScoreBoard) return;
	if(ScoreBoardWidget) ScoreBoardWidget->RemoveFromViewport();
}

void ABreachersPlayerController::ToggleChangeTeamMenu()
{
	if(!TeamSelectWidget) return;
	if(!TeamSelectWidget->IsVisible())
	{
		TeamSelectWidget->AddToViewport();
		SetInputMode(FInputModeGameAndUI());
		bShowMouseCursor = true;
	}
	else
	{
		TeamSelectWidget->RemoveFromViewport();
		SetInputMode(FInputModeGameOnly());
		bShowMouseCursor = false;
	}
}

void ABreachersPlayerController::Client_OpenScoreBoard_Implementation()
{
	OpenScoreBoard();
}

void ABreachersPlayerController::Client_DisableScoreBoard_Implementation()
{
	bCanOpenCloseScoreBoard = false;
}

void ABreachersPlayerController::UpdateKillfeed(FName KillerName, UTexture2D* WeaponIcon, FName KilledName)
{
	Client_UpdateKillfeed(KillerName, WeaponIcon, KilledName);
}

void ABreachersPlayerController::Client_UpdateKillfeed_Implementation(FName KillerName, UTexture2D* WeaponIcon, FName KilledName)
{
	if(KillfeedWidget) KillfeedWidget->AddKillfeedRow(KillerName, WeaponIcon, KilledName);
}

void ABreachersPlayerController::Server_ChangeTeam_Implementation(ETeam NewTeam)
{
	NextTeamRespawn = NewTeam;
}