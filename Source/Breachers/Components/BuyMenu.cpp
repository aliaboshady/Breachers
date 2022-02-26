#include "BuyMenu.h"
#include "Breachers/Weapons/WeaponBase.h"
#include "MoneySystem.h"
#include "WeaponSystem.h"
#include "Blueprint/UserWidget.h"
#include "Breachers/Characters/CharacterBase.h"
#include "Breachers/GameModes/DeathMatchGameMode.h"
#include "Breachers/PlayerControllers/BreachersPlayerController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

UBuyMenu::UBuyMenu()
{
	PrimaryComponentTick.bCanEverTick = false;
	bCanBuy = false;
}

void UBuyMenu::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UBuyMenu, bCanBuy);
	DOREPLIFETIME(UBuyMenu, PC);
}

void UBuyMenu::BeginPlay()
{
	Super::BeginPlay();
	SetIsReplicated(true);
	FTimerHandle BuyMenuHandle;
	GetWorld()->GetTimerManager().SetTimer(BuyMenuHandle, this, &UBuyMenu::GetPC, 1, false, 0.2);
}

void UBuyMenu::SetPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if(PlayerInputComponent)
	{	
		PlayerInputComponent->BindAction(INPUT_OpenBuyMenu, IE_Pressed, this, &UBuyMenu::ShowHideBuyMenu);
	}
}

void UBuyMenu::GetPC()
{
	PC = Cast<ABreachersPlayerController>(GetOwner());
	Client_CreateBuyMenu();
}

bool UBuyMenu::HasThisThrowable(FString WeaponName)
{
	if(!PC || !PC->CharacterPlayer) return false;
	return PC->CharacterPlayer->WeaponSystem->HasThisThrowable(WeaponName);
}

void UBuyMenu::Client_CreateBuyMenu_Implementation()
{
	if(BuyMenuWidgetClass && PC) BuyMenuWidget = CreateWidget(PC, BuyMenuWidgetClass);
}

void UBuyMenu::ShowHideBuyMenu()
{
	Client_ShowHideBuyMenu();
}

void UBuyMenu::Client_ShowHideBuyMenu_Implementation()
{
	if(!bCanBuy || !BuyMenuWidget || !PC) return;
	
	if(!bBuyMenuOpen) BuyMenuWidget->AddToViewport();
	else BuyMenuWidget->RemoveFromViewport();
	bBuyMenuOpen = !bBuyMenuOpen;
	SetInputUI(bBuyMenuOpen);
}

void UBuyMenu::CloseBuyMenu()
{
	Client_CloseBuyMenu();
}
void UBuyMenu::Client_CloseBuyMenu_Implementation()
{
	if(bBuyMenuOpen) ShowHideBuyMenu();
}

void UBuyMenu::SetInputUI(bool bIsUI)
{
	if(!PC) return;
	
	if(bIsUI) PC->SetInputMode(FInputModeGameAndUI());
	else PC->SetInputMode(FInputModeGameOnly());
	PC->bShowMouseCursor = bIsUI;
	if(Cast<ADeathMatchGameMode>(GetWorld()->GetAuthGameMode())) PC->EnableShooting(!bIsUI);
}

void UBuyMenu::EnableBuying(bool bEnableBuying)
{
	bCanBuy = bEnableBuying;
}

void UBuyMenu::BuyWeapon(int32 Price, TSubclassOf<AWeaponBase> WeaponClass, EWeaponType WeaponType)
{
	Server_BuyWeapon(Price, WeaponClass, WeaponType);
}

void UBuyMenu::Server_BuyWeapon_Implementation(int32 Price, TSubclassOf<AWeaponBase> WeaponClass, EWeaponType WeaponType)
{
	if(WeaponClass && PC)
	{
		PC->MoneySystem->AddToCurrentMoney(-Price);
		PC->CharacterPlayer->WeaponSystem->SpawnWeapon(WeaponClass, WeaponType);
	}
}
