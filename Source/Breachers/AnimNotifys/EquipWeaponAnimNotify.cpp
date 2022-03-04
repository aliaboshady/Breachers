#include "EquipWeaponAnimNotify.h"
#include "Breachers/Characters/CharacterBase.h"
#include "Breachers/Components/WeaponSystem.h"
#include "Breachers/Weapons/WeaponBase.h"

UEquipWeaponAnimNotify::UEquipWeaponAnimNotify()
{
	bClientOnlySound = true;
}

void UEquipWeaponAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	if(!MeshComp || !MeshComp->GetOwner() || !MeshComp->GetOwner()->GetInstigatorController()) return;

	if(ACharacterBase* CharacterBase = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		AWeaponBase* Weapon = CharacterBase->WeaponSystem->GetCurrentWeapon();
		if(Weapon) Weapon->PlayEquipSound(bClientOnlySound);
	}
}
