#include "KnifeAttackAnimNotify.h"
#include "Breachers/Characters/CharacterBase.h"
#include "Breachers/Components/WeaponSystem.h"
#include "Breachers/Weapons/MeleeWeapon.h"

void UKnifeAttackAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	if(ACharacterBase* CharacterPlayer = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		if(AMeleeWeapon* MeleeWeapon = Cast<AMeleeWeapon>(CharacterPlayer->WeaponSystem->GetCurrentWeapon()))
		{
			MeleeWeapon->OnMeleeHit();
		}
	}
}
