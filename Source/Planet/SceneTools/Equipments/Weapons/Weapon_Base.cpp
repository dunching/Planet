
#include "Weapon_Base.h"
#include "PropertyEntrys.h"
#include "SceneElement.h"
#include "CharacterBase.h"
#include "BaseFeatureGAComponent.h"
#include "HumanCharacter.h"

void AWeapon_Base::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (auto OwnerCharacterPtr = GetOwner<FOwnerPawnType>())
	{
		OwnerCharacterPtr->GetInteractiveBaseGAComponent()->ClearData2Self(GetAllData(), WeaponUnitPtr->GetUnitType());
	}

	Super::EndPlay(EndPlayReason);
}

void AWeapon_Base::AttachToCharacter(ACharacterBase* CharacterPtr)
{
	Super::AttachToCharacter(CharacterPtr);

	if (WeaponUnitPtr)
	{
		auto PropertyEntrysPtr= WeaponUnitPtr->GetMainPropertyEntry();
		if (PropertyEntrysPtr)
		{
			TMap<ECharacterPropertyType, FBaseProperty> ModifyPropertyMap;

			ModifyPropertyMap.Add(PropertyEntrysPtr->CharacterPropertyType, PropertyEntrysPtr->Value);

			CharacterPtr->GetInteractiveBaseGAComponent()->SendEvent2Self(
				ModifyPropertyMap, WeaponUnitPtr->GetUnitType()
			);
		}
	}
}

