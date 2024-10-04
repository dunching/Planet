
#include "Talent_FASI.h"

#include "TalentAllocationComponent.h"


bool FTalentHelper::operator==(const FTalentHelper& RightValue) const
{
	if (PointType == RightValue.PointType)
	{
		switch (PointType)
		{
		case EPointType::kSkill:
		{
			return std::get<EPointSkillType>(Type) == std::get<EPointSkillType>(RightValue.Type);
		}
		break;
		case EPointType::kProperty:
		{
			return std::get<EPointPropertyType>(Type) == std::get<EPointPropertyType>(RightValue.Type);
		}
		break;
		}
	}

	return false;
}

bool FTalentHelper::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	Ar << PointType;
	Ar << Level;
	Ar << TotalLevel;
	Ar << IconSocket;

	if (Ar.IsSaving())
	{
		switch (PointType)
		{
		case EPointType::kSkill:
		{
			Ar << std::get<EPointSkillType>(Type);
		}
		break;
		case EPointType::kProperty:
		{
			Ar << std::get<EPointPropertyType>(Type);
		}
		break;
		}
	}
	else if (Ar.IsLoading())
	{
		switch (PointType)
		{
		case EPointType::kSkill:
		{
			EPointSkillType PointSkillType;
			Ar << PointSkillType;
			Type = PointSkillType;
		}
		break;
		case EPointType::kProperty:
		{
			EPointPropertyType PointPropertyType;
			Ar << PointPropertyType;
			Type = PointPropertyType;
		}
		break;
		}
	}
	return true;
}

void FTalentItem_FASI::PostReplicatedAdd(const FTalent_FASI_Container& InArraySerializer)
{
	InArraySerializer.TalentAllocationComponentPtr->UpdateTalent_Client(TalentHelper);
}

void FTalentItem_FASI::PostReplicatedChange(const FTalent_FASI_Container& InArraySerializer)
{
	InArraySerializer.TalentAllocationComponentPtr->UpdateTalent_Client(TalentHelper);
}

bool FTalentItem_FASI::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	TalentHelper.NetSerialize(Ar, Map, bOutSuccess);

	return true;
}

bool FTalent_FASI_Container::NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
{
	const auto Result =
		FFastArraySerializer::FastArrayDeltaSerialize<FItemType, FContainerType>(Items, DeltaParms, *this);

	return Result;
}

void FTalent_FASI_Container::AddItem(const FTalentHelper& TalentHelper)
{
#if UE_EDITOR || UE_SERVER
	if (TalentAllocationComponentPtr->GetNetMode() == NM_DedicatedServer)
	{
		FItemType Item;

		Item.TalentHelper = TalentHelper;

		auto& Ref = Items.Add_GetRef(Item);

		MarkItemDirty(Ref);
	}
#endif
}

void FTalent_FASI_Container::UpdateItem(const FTalentHelper& TalentHelper)
{
#if UE_EDITOR || UE_SERVER
	if (TalentAllocationComponentPtr->GetNetMode() == NM_DedicatedServer)
	{
		for (int32 Index = 0; Index < Items.Num(); Index++)
		{
			if (Items[Index].TalentHelper.IconSocket == TalentHelper.IconSocket)
			{
				Items[Index].TalentHelper = TalentHelper;
				MarkItemDirty(Items[Index]);
				return;
			}
		}

		AddItem(TalentHelper);
	}
#endif
}
