#include "PlanetWeapon_Base.h"

#include "CharacterBase.h"
#include "InventoryComponent.h"
#include "ModifyItemProxyStrategy.h"
#include "PlanetPlayerController.h"

void APlanetWeapon_Base::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void APlanetWeapon_Base::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void APlanetWeapon_Base::AttachToCharacter(ACharacter* CharacterPtr)
{
	Super::AttachToCharacter(CharacterPtr);
}

void APlanetWeapon_Base::AttachToCharacterBase(
	ACharacterBase* CharacterPtr
	)
{
}

void APlanetWeapon_Base::SetWeaponProxyImp/*_Implementation*/(ACharacterBase* AllocationCharacterPtr)
{
	AttachToCharacterBase(AllocationCharacterPtr);
}

void APlanetWeapon_Base::SetWeaponProxy(const FGuid& InWeaponProxy_ID)
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
			InventoryComponentPtr = OwnerPCPtr->GetInventoryComponent();
		}
	}

	if (InventoryComponentPtr)
	{
		WeaponProxyPtr = InventoryComponentPtr->FindProxy<FModifyItemProxyStrategy_Weapon>(InWeaponProxy_ID);
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

TSharedPtr<FWeaponProxy> APlanetWeapon_Base::GetWeaponProxy() const
{
	return WeaponProxyPtr;
}
