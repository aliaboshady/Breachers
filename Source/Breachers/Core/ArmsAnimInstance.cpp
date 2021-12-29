#include "ArmsAnimInstance.h"
#include "Breachers/Characters/CharacterBase.h"
#include "Breachers/Components/WeaponSystem.h"
#include "Breachers/Weapons/WeaponBase.h"

void UArmsAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	if(APawn* OwnerPawn = TryGetPawnOwner())
	{
		OwnerCharacter = Cast<ACharacterBase>(OwnerPawn);
	}
}

void UArmsAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	Client_ChangePose();
}

void UArmsAnimInstance::Client_ChangePose_Implementation()
{
	if(!OwnerCharacter) return;
	if(const AWeaponBase* CurrentWeapon = OwnerCharacter->WeaponSystem->GetCurrentWeapon())
	{
		WeaponType = CurrentWeapon->WeaponInfo.WeaponType;
	}
}
