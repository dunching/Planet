
#include "Weapon_Base.h"

#include "PropertyEntrys.h"

#include "SceneElement.h"
#include "CharacterBase.h"
#include "BaseFeatureComponent.h"
#include "HumanCharacter.h"

void AWeapon_Base::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AWeapon_Base::AttachToCharacter(ACharacterBase* CharacterPtr)
{
	Super::AttachToCharacter(CharacterPtr);
}

void AWeapon_Base::SetWeaponUnit/*_Implementation*/(const FWeaponProxy& WeaponProxy)
{
	WeaponUnitPtr = DynamicCastSharedPtr<FWeaponProxy>(WeaponProxy.GetThisSPtr());
	if (WeaponUnitPtr)
	{
		AttachToCharacter(WeaponUnitPtr->GetAllocationCharacter());
	}
}
