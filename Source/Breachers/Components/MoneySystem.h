#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MoneySystem.generated.h"

class ACharacterBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BREACHERS_API UMoneySystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	UMoneySystem();
	void AddToCurrentMoney(int32 AddedMoney);
	void ResetCurrentMoney();

	UFUNCTION(BlueprintCallable)
	FORCEINLINE int32 GetCurrentMoney(){return CurrentMoney;}

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable)
	void Server_AddToCurrentMoney(int32 AddedMoney);

	void SetMoney();
	
	UFUNCTION(Server, Reliable)
	void Server_SetMoney();

	UFUNCTION(Client, Reliable)
	void Client_SetMoney();

	bool bUnlimitedMoney;
	int32 StartUpMoney;
	int32 MaxMoney;
	
	UPROPERTY(Replicated)
	int32 CurrentMoney;
};
