
#include "AllocationSkills.h"

#include "AssetRefMap.h"
#include "Planet.h"
#include "CharacterBase.h"
#include "CharacterAttributesComponent.h"
#include "PlanetControllerInterface.h"
#include "HumanCharacter.h"
#include "SceneUnitExtendInfo.h"
#include "GameplayTagsSubSystem.h"
#include "SceneUnitContainer.h"

void FAllocationSkills::Update(const TSharedPtr<FSkillSocket>& Socket)
{
	if (SkillsMap.Contains(Socket->Socket))
	{
		if (SkillsMap[Socket->Socket] == Socket)
		{
			return;
		}
		else
		{
#if UE_EDITOR || UE_SERVER
			if (OwnerCharacter->ProxyCharacterPtr->GetNetMode() == NM_DedicatedServer)
			{
				SkillsMap[Socket->Socket]->UnitPtr.Pin()->UnRegisterSkill();
			}
#endif
		}
	}

	if (Socket->UnitPtr.IsValid())
	{
		SkillsMap.Add(Socket->Socket, Socket);

#if UE_EDITOR || UE_SERVER
		if (OwnerCharacter->ProxyCharacterPtr->GetNetMode() == NM_DedicatedServer)
		{
			Socket->UnitPtr.Pin()->RegisterSkill();
		}
#endif
	}
	else
	{
		SkillsMap.Remove(Socket->Socket);
	}
}

void FAllocationSkills::Update(const TSharedPtr<FWeaponSocket>& Socket)
{
	if (WeaponsMap.Contains(Socket->Socket))
	{
		if (WeaponsMap[Socket->Socket] == Socket)
		{
			return;
		}
		else
		{
#if UE_EDITOR || UE_SERVER
			if (OwnerCharacter->ProxyCharacterPtr->GetNetMode() == NM_DedicatedServer)
			{
				WeaponsMap[Socket->Socket]->UnitPtr.Pin()->RetractputWeapon();
				WeaponsMap[Socket->Socket]->UnitPtr.Pin()->RemoveWeapons();
			}
#endif
		}
	}

	if (Socket->UnitPtr.IsValid())
	{
		WeaponsMap.Add(Socket->Socket, Socket);

#if UE_EDITOR || UE_SERVER
		if (OwnerCharacter->ProxyCharacterPtr->GetNetMode() == NM_DedicatedServer)
		{
			Socket->UnitPtr.Pin()->EquippingWeapons();
		}
#endif
	}
	else
	{
		WeaponsMap.Remove(Socket->Socket);
	}
}

void FAllocationSkills::Update(const TSharedPtr<FConsumableSocket>& Socket)
{
	ConsumablesMap.Add(Socket->Socket, Socket);
}

bool FAllocationSkills::Active(const TSharedPtr<FSocketBase>& Socket)
{
	return Active(Socket->Socket);
}

bool FAllocationSkills::Active(const FGameplayTag& Socket)
{
	if (SkillsMap.Contains(Socket))
	{
		return SkillsMap[Socket]->UnitPtr.Pin()->Active();
	}
	else if (WeaponsMap.Contains(Socket))
	{
		return WeaponsMap[Socket]->UnitPtr.Pin()->Active();
	}
	else if (ConsumablesMap.Contains(Socket))
	{
		return ConsumablesMap[Socket]->UnitPtr.Pin()->Active();;
	}

	return false;
}

void FAllocationSkills::Cancel(const TSharedPtr<FSocketBase>& Socket)
{
	Cancel(Socket->Socket);
}

void FAllocationSkills::Cancel(const FGameplayTag& Socket)
{
	if (SkillsMap.Contains(Socket))
	{
		SkillsMap[Socket]->UnitPtr.Pin()->Cancel();
	}
	else if (WeaponsMap.Contains(Socket))
	{
		WeaponsMap[Socket]->UnitPtr.Pin()->Cancel();
	}
	else if (ConsumablesMap.Contains(Socket))
	{
		ConsumablesMap[Socket]->UnitPtr.Pin()->Cancel();;
	}
}

TSharedPtr<FSkillSocket> FAllocationSkills::FindSkill(const FGameplayTag& Socket)
{
	auto Iter = SkillsMap.Find(Socket);
	if (Iter)
	{
		return *Iter;
	}
	else
	{
		return nullptr;
	}
}

TSharedPtr<FWeaponSocket> FAllocationSkills::FindWeapon(const FGameplayTag& Socket)
{
	auto Iter = WeaponsMap.Find(Socket);
	if (Iter)
	{
		return *Iter;
	}
	else
	{
		return nullptr;
	}
}

TSharedPtr<FConsumableSocket> FAllocationSkills::FindConsumable(const FGameplayTag& Socket)
{
	auto Iter = ConsumablesMap.Find(Socket);
	if (Iter)
	{
		return *Iter;
	}
	else
	{
		return nullptr;
	}
}

TMap<FGameplayTag, TSharedPtr<FSkillSocket>> FAllocationSkills::GetSkillsMap() const
{
	return SkillsMap;
}

TMap<FGameplayTag, TSharedPtr<FWeaponSocket>> FAllocationSkills::GetWeaponsMap() const
{
	return WeaponsMap;
}

TMap<FGameplayTag, TSharedPtr<FConsumableSocket>> FAllocationSkills::GetConsumablesMap() const
{
	return ConsumablesMap;
}

FSocketBase::~FSocketBase()
{

}

bool FSocketBase::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	if (Ar.IsSaving())
	{
		auto KeyStr = Key.ToString();
		Ar << KeyStr;
		Ar << Socket;

	}
	else if (Ar.IsLoading())
	{
		FString KeyStr;
		Ar << KeyStr;
		Key = FKey(*KeyStr);

		Ar << Socket;
	}

	return true;
}

bool FSkillSocket::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	Super::NetSerialize(Ar, Map, bOutSuccess);

	if (Ar.IsSaving())
	{
		bool bIsValid = UnitPtr.IsValid();
		Ar << bIsValid;
		if (bIsValid)
		{
			UnitPtr.Pin()->NetSerialize(Ar, Map, bOutSuccess);
		}
	}
	else if (Ar.IsLoading())
	{
		bool bIsValid = false;
		Ar << bIsValid;
		if (bIsValid)
		{
			auto TempPtr = MakeShared<FSkillProxy>();
			TempPtr->NetSerialize(Ar, Map, bOutSuccess);

			auto SceneUnitContainer =
				TempPtr->OwnerCharacterUnitPtr.Pin()->ProxyCharacterPtr->GetSceneUnitContainer();

			UnitPtr = DynamicCastSharedPtr<FSkillProxy>(
				SceneUnitContainer->FindUnit_Skill(
					TempPtr->GetID()
				));
		}
	}

	return true;
}

bool FWeaponSocket::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	Super::NetSerialize(Ar, Map, bOutSuccess);

	if (Ar.IsSaving())
	{
		bool bIsValid = UnitPtr.IsValid();
		Ar << bIsValid;
		if (bIsValid)
		{
			UnitPtr.Pin()->NetSerialize(Ar, Map, bOutSuccess);
		}
	}
	else if (Ar.IsLoading())
	{
		bool bIsValid = false;
		Ar << bIsValid;
		if (bIsValid)
		{
			auto TempPtr = MakeShared<FWeaponProxy>();
			TempPtr->NetSerialize(Ar, Map, bOutSuccess);

			auto SceneUnitContainer =
				TempPtr->OwnerCharacterUnitPtr.Pin()->ProxyCharacterPtr->GetSceneUnitContainer();

			UnitPtr = DynamicCastSharedPtr<FWeaponProxy>(
				SceneUnitContainer->FindUnit(
					TempPtr->GetID()
				));
		}
	}

	return true;
}
