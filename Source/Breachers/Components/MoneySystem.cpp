#include "MoneySystem.h"
#include "Net/UnrealNetwork.h"
#include "Breachers/GameModes/BreachersGameModeBase.h"

UMoneySystem::UMoneySystem()
{
	PrimaryComponentTick.bCanEverTick = false;

	StartUpMoney = 800;
	MaxMoney = 9000;
}

void UMoneySystem::BeginPlay()
{
	Super::BeginPlay();
	SetIsReplicated(true);

	FTimerHandle SetMoneyTimer;
	GetWorld()->GetTimerManager().SetTimer(SetMoneyTimer, this, &UMoneySystem::Server_SetMoney, 1, false, 0.25);
}

void UMoneySystem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UMoneySystem, CurrentMoney);
}

void UMoneySystem::Server_AddToCurrentMoney_Implementation(int32 AddedMoney)
{
	CurrentMoney = FMath::Clamp(CurrentMoney + AddedMoney, 0, MaxMoney);
}

void UMoneySystem::AddToCurrentMoney(int32 AddedMoney)
{
	if(bUnlimitedMoney) return;
	Server_AddToCurrentMoney(AddedMoney);
}

void UMoneySystem::ResetCurrentMoney()
{
	CurrentMoney = StartUpMoney;
}

void UMoneySystem::SetMoney()
{
	ABreachersGameModeBase* BreachersGM = Cast<ABreachersGameModeBase>(GetWorld()->GetAuthGameMode());
	if(!BreachersGM) return;
	
	bUnlimitedMoney = BreachersGM->GetIsUnlimitedMoney();
	StartUpMoney = BreachersGM->GetStartUpMoney();
	MaxMoney = BreachersGM->GetMaxMoney();
	CurrentMoney = StartUpMoney;
}

void UMoneySystem::Server_SetMoney_Implementation()
{
	SetMoney();
	Client_SetMoney();
}

void UMoneySystem::Client_SetMoney_Implementation()
{
	SetMoney();
}