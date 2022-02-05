#pragma once
#include "CoreMinimal.h"
#include "Breachers/Characters/CharacterBase.h"
#include "Engine/GameInstance.h"
#include "MainGameInstance.generated.h"

UCLASS()
class BREACHERS_API UMainGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void DestroyGameSession();

	void ChooseNextTeam(ETeam Team);
	FORCEINLINE bool HasChosenTeam(){return bHasChosenTeam;}
	FORCEINLINE ETeam GetNextTeam(){return NextTeamRespawn;}

protected:
	TEnumAsByte<ETeam> NextTeamRespawn;
	bool bHasChosenTeam = false;
};
