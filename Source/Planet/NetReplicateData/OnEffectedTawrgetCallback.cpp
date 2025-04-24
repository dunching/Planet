
#include "OnEffectedTawrgetCallback.h"

#include "ConversationComponent.h"
#include "TeamMatesHelperComponent.h"
#include "CharacterBase.h"

void FCDItem_FASI::PreReplicatedRemove(const FCD_FASI_Container& InArraySerializer)
{
	// if (SkillCooldownHelper->SkillProxy_ID.IsValid())
	// {
	// 	InArraySerializer.CDCaculatorComponentPtr->Separate_Map.Remove(SkillCooldownHelper->SkillProxy_ID);
	// }
	// else if (SkillCooldownHelper->SkillType.IsValid())
	// {
	// 	InArraySerializer.CDCaculatorComponentPtr->Common_Map.Remove(SkillCooldownHelper->SkillType);
	// }
}

void FCDItem_FASI::PostReplicatedAdd(const FCD_FASI_Container& InArraySerializer)
{
	// if (SkillCooldownHelper->SkillProxy_ID.IsValid())
	// {
	// 	InArraySerializer.CDCaculatorComponentPtr->Separate_Map.Add(SkillCooldownHelper->SkillProxy_ID, SkillCooldownHelper);
	// }
	// else if (SkillCooldownHelper->SkillType.IsValid())
	// {
	// 	InArraySerializer.CDCaculatorComponentPtr->Common_Map.Add(SkillCooldownHelper->SkillType, SkillCooldownHelper);
	// }
}

void FCDItem_FASI::PostReplicatedChange(const FCD_FASI_Container& InArraySerializer)
{
	// if (SkillCooldownHelper->SkillProxy_ID.IsValid())
	// {
	// 	if (InArraySerializer.CDCaculatorComponentPtr->Separate_Map.Contains(SkillCooldownHelper->SkillProxy_ID))
	// 	{
	// 	}
	// }
	// else if (SkillCooldownHelper->SkillType.IsValid())
	// {
	// 	if (InArraySerializer.CDCaculatorComponentPtr->Common_Map.Contains(SkillCooldownHelper->SkillType))
	// 	{
	// 	}
	// }
}

bool FCDItem_FASI::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	// if (Ar.IsSaving())
	// {
	// 	Ar << SkillCooldownHelper->SkillProxy_ID;
	// 	Ar << SkillCooldownHelper->SkillType;
	// 	Ar << SkillCooldownHelper->CooldownTime;
	// 	Ar << SkillCooldownHelper->CooldownConsumeTime;
	// }
	// else if (Ar.IsLoading())
	// {
	// 	if (!SkillCooldownHelper)
	// 	{
	// 		SkillCooldownHelper = MakeShared<FSkillCooldownHelper>();
	// 	}
	// 	Ar << SkillCooldownHelper->SkillProxy_ID;
	// 	Ar << SkillCooldownHelper->SkillType;
	// 	Ar << SkillCooldownHelper->CooldownTime;
	// 	Ar << SkillCooldownHelper->CooldownConsumeTime;
	// }

	return true;
}

bool FCD_FASI_Container::NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
{
	const auto Result =
		FFastArraySerializer::FastArrayDeltaSerialize<FItemType, FContainerType>(Items, DeltaParms, *this);

	return Result;
}

void FCD_FASI_Container::AddItem(const TSharedPtr<FSkillCooldownHelper>& SkillCooldownHelper)
{
#if UE_EDITOR || UE_SERVER
	if (CDCaculatorComponentPtr->GetNetMode() == NM_DedicatedServer)
	{
		if (SkillCooldownHelper)
		{
			FItemType Item;

			Item.SkillCooldownHelper = SkillCooldownHelper;

			auto& Ref = Items.Add_GetRef(Item);

			MarkItemDirty(Ref);
		}
	}
#endif
}

void FCD_FASI_Container::UpdateItem(const TSharedPtr<FSkillCooldownHelper>& SkillCooldownHelper)
{
#if UE_EDITOR || UE_SERVER
	if (CDCaculatorComponentPtr->GetNetMode() == NM_DedicatedServer)
	{
		if (SkillCooldownHelper)
		{
			for (int32 Index = 0; Index < Items.Num(); Index++)
			{
				if (Items[Index].SkillCooldownHelper == SkillCooldownHelper)
				{
					// 注意：ProxySPtr 这个指针已经在外部进行了修改，在这部我们不必再对 Items[Index] 进行修改
					MarkItemDirty(Items[Index]);
					return;
				}
			}

			AddItem(SkillCooldownHelper);
		}
	}
#endif
}

void FCD_FASI_Container::RemoveItem(const TSharedPtr<FSkillCooldownHelper>& SkillCooldownHelper)
{
#if UE_EDITOR || UE_SERVER
	if (CDCaculatorComponentPtr->GetNetMode() == NM_DedicatedServer)
	{
		for (int32 Index = 0; Index < Items.Num(); ++Index)
		{
			if (Items[Index].SkillCooldownHelper == SkillCooldownHelper)
			{
				Items.RemoveAt(Index);

				MarkArrayDirty();
				return;
			}
		}
	}
#endif
}

bool FOnEffectedTawrgetCallback::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	Ar << InstigatorCharacterPtr;
	Ar << TargetCharacterPtr;
	Ar << bIsDeath;

	return true;
}
