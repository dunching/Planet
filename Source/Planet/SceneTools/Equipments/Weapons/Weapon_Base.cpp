
#include "Weapon_Base.h"

#include "PropertyEntrys.h"

#include "ItemProxy.h"
#include "CharacterBase.h"
#include "BaseFeatureComponent.h"
#include "HumanCharacter.h"
#include "HoldingItemsComponent.h"
#include "PlanetPlayerController.h"

void AWeapon_Base::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AWeapon_Base::AttachToCharacter(ACharacterBase* CharacterPtr)
{
	Super::AttachToCharacter(CharacterPtr);
}

void AWeapon_Base::SetWeaponUnit_Implementation(const FGuid& WeaponProxy_ID)
{
	UHoldingItemsComponent* HoldingItemsComponentPtr = nullptr;

	// 这里设置了 Owner。第一次掉取到的还是PC？
	auto OwnerPtr = GetOwner<ACharacterBase>();
	if (OwnerPtr)
	{
		HoldingItemsComponentPtr = OwnerPtr->GetHoldingItemsComponent();
	}
	else
	{
		auto OwnerPCPtr = GetOwner<APlanetPlayerController>();
		if (OwnerPCPtr)
		{
			HoldingItemsComponentPtr = OwnerPCPtr->GetHoldingItemsComponent();
		}
	}

	if (HoldingItemsComponentPtr)
	{
		WeaponProxyPtr = HoldingItemsComponentPtr->FindUnit_Weapon(WeaponProxy_ID);
		if (WeaponProxyPtr)
		{
			AttachToCharacter(WeaponProxyPtr->GetAllocationCharacter());
		}
	}
}
