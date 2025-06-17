#include "ItemProxy_Material.h"

#include "HumanCharacter_AI.h"
#include "CharacterAttributesComponent.h"
#include "DataTableCollection.h"
#include "AllocationSkills.h"
#include "Skill_Base.h"
#include "Skill_Consumable_Generic.h"
#include "CharacterAbilitySystemComponent.h"
#include "SceneProxyTable.h"
#include "GameplayTagsLibrary.h"
#include "GroupManagger.h"
#include "InventoryComponent.h"
#include "Tools.h"

FMaterialProxy::FMaterialProxy()
{
}

bool FMaterialProxy::NetSerialize(
	FArchive& Ar,
	class UPackageMap* Map,
	bool& bOutSuccess
	)
{
	Super::NetSerialize(Ar, Map, bOutSuccess);
	NetSerialize_Unique(Ar, Map, bOutSuccess);

	return true;
}

void FMaterialProxy::InitialProxy(
	const FGameplayTag& InProxyType
	)
{
	Super::InitialProxy(InProxyType);
}

void FMaterialProxy::UpdateByRemote(
	const TSharedPtr<FMaterialProxy>& RemoteSPtr
	)
{
	Super::UpdateByRemote(RemoteSPtr);
	UpdateByRemote_Unique(RemoteSPtr);
}

bool FMaterialProxy::Active()
{
	return true;
}

UItemProxy_Description_Material* FMaterialProxy::GetTableRowProxy_Material() const
{
	auto TableRowPtr = GetTableRowProxy();
	auto ItemProxy_Description_SkillPtr = Cast<UItemProxy_Description_Material>(
		 TableRowPtr->ItemProxy_Description.LoadSynchronous()
		);
	return ItemProxy_Description_SkillPtr;
}

void FMaterialProxy::UpdateData()
{
#if UE_EDITOR || UE_SERVER
	if (GetInventoryComponentBase()->GetNetMode() == NM_DedicatedServer)
	{
		if (GetNum() > 0)
		{
			GetInventoryComponentBase()->UpdateProxy(GetID());
		}
		else
		{
			if (bIsRemoveWhenZero)
			{
				GetInventoryComponentBase()->RemoveProxy(GetID());
			}
			else
			{
				GetInventoryComponentBase()->UpdateProxy(GetID());
			}
		}
	}
#endif
}

void FMaterialProxy::ModifyNum(
	int32 Value
	)
{
	IProxy_Unique::ModifyNum(Value);

	UpdateData();
}

bool FExperienceMaterialProxy::NetSerialize(
	FArchive& Ar,
	class UPackageMap* Map,
	bool& bOutSuccess
	)
{
	
	return Super::NetSerialize(Ar, Map, bOutSuccess);
}

TObjectPtr<FExperienceMaterialProxy::FItemProxy_Description> FExperienceMaterialProxy::GetTableRowProxy_ExperienceMaterial() const
{
	auto TableRowPtr = GetTableRowProxy();
	auto ItemProxy_Description_SkillPtr = Cast<FItemProxy_Description>(
		 TableRowPtr->ItemProxy_Description.LoadSynchronous()
		);
	return ItemProxy_Description_SkillPtr;
}

int32 FExperienceMaterialProxy::GetExperienceValue() const
{
	return GetTableRowProxy_ExperienceMaterial()->ExperienceValue;
}
