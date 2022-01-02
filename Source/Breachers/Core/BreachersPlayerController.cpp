#include "BreachersPlayerController.h"
#include "BreachersGameModeBase.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraActor.h"
#include "Kismet/GameplayStatics.h"

void ABreachersPlayerController::BeginPlay()
{
	Super::BeginPlay();
	PossessStartCamera();
	BreachersGameModeBase = Cast<ABreachersGameModeBase>(GetWorld()->GetAuthGameMode());
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
