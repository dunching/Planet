
#include "Weapon_Base.h"

#include "PropertyEntrys.h"

#include "SceneElement.h"
#include "CharacterBase.h"
#include "BaseFeatureGAComponent.h"
#include "HumanCharacter.h"

void AWeapon_Base::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		if (auto OwnerCharacterPtr = GetOwner<FOwnerPawnType>())
		{
			OwnerCharacterPtr->GetInteractiveBaseGAComponent()->ClearData2Self(GetAllData(), WeaponUnitPtr->GetUnitType());
		}
	}
#endif

	Super::EndPlay(EndPlayReason);
}

void AWeapon_Base::AttachToCharacter(ACharacterBase* CharacterPtr)
{
	Super::AttachToCharacter(CharacterPtr);

#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		if (WeaponUnitPtr)
		{
			auto PropertyEntrysPtr = WeaponUnitPtr->GetMainPropertyEntry();
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
#endif
}

void AWeapon_Base::SetWeaponUnit_Implementation(const FWeaponProxy& WeaponProxy)
{
	WeaponUnitPtr = DynamicCastSharedPtr<FWeaponProxy>(WeaponProxy.GetThisSPtr());
	if (WeaponUnitPtr)
	{
		AttachToCharacter(WeaponUnitPtr->OwnerCharacterUnitPtr.Pin()->ProxyCharacterPtr.Get());
	}
}
