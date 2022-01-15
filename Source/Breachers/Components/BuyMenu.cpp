#include "BuyMenu.h"
#include "Blueprint/UserWidget.h"
#include "Breachers/Characters/CharacterBase.h"
#include "Breachers/Core/BreachersPlayerController.h"
#include "Net/UnrealNetwork.h"

UBuyMenu::UBuyMenu()
{
	PrimaryComponentTick.bCanEverTick = false;
	bCanBuy = false;
	SetIsReplicated(true);
}

void UBuyMenu::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UBuyMenu, bCanBuy);
	DOREPLIFETIME(UBuyMenu, CharacterPlayer);
	DOREPLIFETIME(UBuyMenu, PC);
}

void UBuyMenu::BeginPlay()
{
	Super::BeginPlay();
	CharacterPlayer = Cast<ACharacterBase>(GetOwner());

	FTimerHandle BuyMenuHandle;
	GetWorld()->GetTimerManager().SetTimer(BuyMenuHandle, this, &UBuyMenu::Client_CreateBuyMenu, 1, false, 0.2);
}

void UBuyMenu::SetPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if(PlayerInputComponent)
	{	
		PlayerInputComponent->BindAction(INPUT_OpenBuyMenu, IE_Pressed, this, &UBuyMenu::ShowHideBuyMenu);
	}
}

void UBuyMenu::Client_CreateBuyMenu_Implementation()
{
	if(BuyMenuWidgetClass && CharacterPlayer)
	{
		PC = Cast<ABreachersPlayerController>(CharacterPlayer->GetController());
		if(PC)
		{
			BuyMenuWidget = CreateWidget(PC, BuyMenuWidgetClass);
		}
	}
}

void UBuyMenu::ShowHideBuyMenu()
{
	if(!bCanBuy || !BuyMenuWidget || !PC) return;
	
	if(!bBuyMenuOpen) BuyMenuWidget->AddToViewport();
	else BuyMenuWidget->RemoveFromViewport();
	bBuyMenuOpen = !bBuyMenuOpen;
	SetInputUI(bBuyMenuOpen);
}

void UBuyMenu::SetInputUI(bool bIsUI)
{
	if(!PC) return;
	
	if(bIsUI) PC->SetInputMode(FInputModeGameAndUI());
	else PC->SetInputMode(FInputModeGameOnly());
	PC->bShowMouseCursor = bIsUI;
	PC->EnableShooting(!bIsUI);
}

void UBuyMenu::EnableBuying(bool bEnableBuying)
{
	bCanBuy = bEnableBuying;
}
