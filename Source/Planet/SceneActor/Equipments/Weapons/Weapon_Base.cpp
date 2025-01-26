#include "Weapon_Base.h"

#include "PropertyEntrys.h"
#include "Net/UnrealNetwork.h"

#include "ItemProxy_Minimal.h"
#include "CharacterBase.h"
#include "CharacterAbilitySystemComponent.h"
#include "HumanCharacter.h"
#include "InventoryComponent.h"
#include "PlanetPlayerController.h"

void AWeapon_Base::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AWeapon_Base::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AWeapon_Base::AttachToCharacter(ACharacterBase* CharacterPtr)
{
	Super::AttachToCharacter(CharacterPtr);
}

void AWeapon_Base::SetWeaponProxyImp/*_Implementation*/(ACharacterBase* AllocationCharacterPtr)
{
	AttachToCharacter(AllocationCharacterPtr);
}

void AWeapon_Base::SetWeaponProxy(const FGuid& InWeaponProxy_ID)
{
	UInventoryComponent* InventoryComponentPtr = nullptr;

	// 这里设置了 Owner。第一次掉取到的还是PC？
	auto OwnerPtr = GetOwner<ACharacterBase>();
	if (OwnerPtr)
	{
		InventoryComponentPtr = OwnerPtr->GetInventoryComponent();
	}
	else
	{
		auto OwnerPCPtr = GetOwner<APlanetPlayerController>();
		if (OwnerPCPtr)
		{
			InventoryComponentPtr = OwnerPCPtr->GetHoldingItemsComponent();
		}
	}

	if (InventoryComponentPtr)
	{
		WeaponProxyPtr = InventoryComponentPtr->FindProxy_Weapon(InWeaponProxy_ID);
		if (WeaponProxyPtr)
		{
#if UE_EDITOR || UE_SERVER
			if (GetNetMode() == NM_DedicatedServer)
			{
				auto AllocationCharacterPtr = WeaponProxyPtr->GetAllocationCharacter();
				SetWeaponProxyImp(AllocationCharacterPtr);
			}
#endif
		}
	}
}

TSharedPtr<FWeaponProxy> AWeapon_Base::GetWeaponProxy() const
{
	return WeaponProxyPtr;
}
