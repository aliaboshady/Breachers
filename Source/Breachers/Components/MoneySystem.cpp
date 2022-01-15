#include "MoneySystem.h"
#include "Net/UnrealNetwork.h"

UMoneySystem::UMoneySystem()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicated(true);

	StartUpMoney = 800;
	MaxMoney = 9000;
}

void UMoneySystem::BeginPlay()
{
	Super::BeginPlay();
	CurrentMoney = StartUpMoney;
}

void UMoneySystem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UMoneySystem, CharacterPlayer);
	DOREPLIFETIME(UMoneySystem, CurrentMoney);
}

void UMoneySystem::Server_AddToCurrentMoney_Implementation(int32 AddedMoney)
{
	CurrentMoney = FMath::Clamp(CurrentMoney + AddedMoney, 0, MaxMoney);
}

void UMoneySystem::AddToCurrentMoney(int32 AddedMoney)
{
	Server_AddToCurrentMoney(AddedMoney);
}
