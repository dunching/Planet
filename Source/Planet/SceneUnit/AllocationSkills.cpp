
#include "AllocationSkills.h"

#include "AssetRefMap.h"
#include "Planet.h"
#include "CharacterBase.h"
#include "CharacterAttributesComponent.h"
#include "PlanetControllerInterface.h"
#include "HumanCharacter.h"
#include "SceneUnitExtendInfo.h"
#include "GameplayTagsSubSystem.h"

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
			SkillsMap[Socket->Socket]->UnitPtr.Pin()->UnRegisterSkill();
		}
	}

	if (Socket->UnitPtr.IsValid())
	{
		SkillsMap.Add(Socket->Socket, Socket);

		Socket->UnitPtr.Pin()->RegisterSkill();
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
			WeaponsMap[Socket->Socket]->UnitPtr.Pin()->RetractputWeapon();
		}
	}

	if (Socket->UnitPtr.IsValid())
	{
		WeaponsMap.Add(Socket->Socket, Socket);

		Socket->UnitPtr.Pin()->ActiveWeapon();
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
	return false;
}

void FAllocationSkills::Cancel(const TSharedPtr<FSocketBase>& Socket)
{

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
