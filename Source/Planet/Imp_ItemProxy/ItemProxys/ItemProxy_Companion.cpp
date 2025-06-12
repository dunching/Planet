#include "ItemProxy_Companion.h"

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

FCompanionProxy::FCompanionProxy()
{
}

bool FCompanionProxy::NetSerialize(
	FArchive& Ar,
	class UPackageMap* Map,
	bool& bOutSuccess
	)
{
	Super::NetSerialize(Ar, Map, bOutSuccess);
	NetSerialize_Unique(Ar, Map, bOutSuccess);

	return true;
}

void FCompanionProxy::InitialProxy(
	const FGameplayTag& InProxyType
	)
{
	Super::InitialProxy(InProxyType);
}

void FCompanionProxy::UpdateByRemote(
	const TSharedPtr<FCompanionProxy>& RemoteSPtr
	)
{
	Super::UpdateByRemote(RemoteSPtr);
	UpdateByRemote_Unique(RemoteSPtr);
}

bool FCompanionProxy::Active()
{
#if UE_EDITOR || UE_SERVER
	if (GetInventoryComponent()->GetNetMode() == NM_DedicatedServer)
	{
		auto GameplayAbilityTargetPtr =
			new FGameplayAbilityTargetData_RegisterParam_Consumable;

		// Test
		GameplayAbilityTargetPtr->ProxyID = GetID();

		const auto InputID = FMath::Rand32();
		FGameplayAbilitySpec AbilitySpec(
		                                 GetTableRowProxy_Consumable()->Skill_Consumable_Class,
		                                 1,
		                                 InputID
		                                );

		auto GameplayEventData = MakeShared<FGameplayEventData>();
		GameplayEventData->TargetData.Add(GameplayAbilityTargetPtr);
	}
#endif

	return true;
}

UItemProxy_Description_Consumable* FCompanionProxy::GetTableRowProxy_Consumable() const
{
	auto TableRowPtr = GetTableRowProxy();
	auto ItemProxy_Description_SkillPtr = Cast<UItemProxy_Description_Consumable>(
		 TableRowPtr->ItemProxy_Description.LoadSynchronous()
		);
	return ItemProxy_Description_SkillPtr;
}

void FCompanionProxy::UpdateData()
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

void FCompanionProxy::ModifyNum(
	int32 Value
	)
{
	IProxy_Unique::ModifyNum(Value);

	UpdateData();
}

int32 FCompanionProxy::GetCount() const
{
	return -1;
}

bool FCompanionProxy::GetRemainingCooldown(
	float& RemainingCooldown,
	float& RemainingCooldownPercent
	) const
{
	RemainingCooldown = -1.f;

	return true;
}

bool FCompanionProxy::CheckNotInCooldown() const
{
	return true;
}

void FCompanionProxy::AddCooldownConsumeTime(
	float NewTime
	)
{
}

void FCompanionProxy::FreshUniqueCooldownTime()
{
}

void FCompanionProxy::ApplyCooldown()
{
	// GetAllocationCharacter()->GetConversationComponent()->ApplyCooldown(
	// 	this
	// );
}

void FCompanionProxy::OffsetCooldownTime()
{
}
