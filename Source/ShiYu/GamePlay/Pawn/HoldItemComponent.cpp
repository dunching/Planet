
#include "HoldItemComponent.h"

void FHoldItemsData::AddItem(const FItemAry& TempItemAry)
{
	ItemAry.ItemsAry.Append(TempItemAry.ItemsAry);

 	OnChange(TempItemAry, EItemChangeType::kAdd);
}

void FHoldItemsData::RemoveItem(const FItemAry& TempItemAry)
{
	FItemAry RemoveItemMap;
	for (auto Iter : TempItemAry.ItemsAry)
	{
		for (auto SecondIter : ItemAry.ItemsAry)
		{
			if (Iter.ItemType.ID == SecondIter.ItemType.ID)
			{
				Iter.Num = FMath::Clamp(Iter.Num, 0, SecondIter.Num);

				RemoveItemMap.ItemsAry.Add(Iter);

				SecondIter.Num -= Iter.Num;

			}
		}
	}

	for (int32 Index = ItemAry.ItemsAry.Num() - 1; Index >= 0; Index--)
	{
		if (ItemAry.ItemsAry[Index].Num <= 0)
		{
			ItemAry.ItemsAry.RemoveAt(Index);
		}
	}

	OnChange(RemoveItemMap, EItemChangeType::kSub);
}

const FItemAry& FHoldItemsData::GetItemsAry() const
{
	return ItemAry;
}

const FCreatingMap& FHoldItemsData::GetCreateMap() const
{
	return CreateMap;
}

const FCanbeCreatinedSet& FHoldItemsData::GetCanCreateMap() const
{
	return CanCreateSet;
}

bool FHoldItemsData::CanCreateItem(FItemNum ItemsType) const
{
// 	if (CreateMap.Contains(ItemsType))
// 	{
// 		auto CreatePloy = CreateMap[ItemsType];
// 		for (auto Iter : CreatePloy)
// 		{
// 			if (ItemAry.Contains(Iter.Key))
// 			{
// 				return Iter.Value <= ItemAry[Iter.Key];
// 			}
// 		}
// 	}

	return false;
}

void FHoldItemsData::CreatedItem(FItemNum ItemsType)
{
// 	if (CanCreateItem(ItemsType))
// 	{
// 		if (CreateMap.Contains(ItemsType))
// 		{
// 			auto CreatePloy = CreateMap[ItemsType];
// 			for (auto& Iter : CreatePloy)
// 			{
// 				if (ItemAry.Contains(Iter.Key))
// 				{
// 					ItemAry[Iter.Key] -= Iter.Value;
// 				}
// 			}
// 			OnChange(CreatePloy, EItemChangeType::kSub);
// 		}
// 	}
}

void FHoldItemsData::UnCreatedItem(FItemNum ItemsType)
{
// 	if (CreateMap.Contains(ItemsType))
// 	{
// 		auto CreatePloy = CreateMap[ItemsType];
// 		for (auto& Iter : CreatePloy)
// 		{
// 			if (ItemAry.Contains(Iter.Key))
// 			{
// 				ItemAry[Iter.Key] += Iter.Value;
// 			}
// 			else
// 			{
// 				ItemAry.Add(Iter.Key, Iter.Value);
// 			}
// 		}
// 		OnChange(CreatePloy, EItemChangeType::kAdd);
// 	}
}

int32 FHoldItemsData::GetItemCount(FItemNum ItemsType)const
{
// 	if (ItemAry.Contains(ItemsType))
// 	{
// 		return ItemAry[ItemsType];
// 	}
// 	else
	{
		return 0;
	}
}

void FHoldItemsData::SetItemChangeCB(
	EOnItemChangeNotityObjType OnItemChangeNotityObjType,
	const FOnItemChangeNotifyCB& OnItemChangeNotifyCB
)
{
	OnItemChangeNotifyCBMap.Add(OnItemChangeNotityObjType, OnItemChangeNotifyCB);
}

void FHoldItemsData::RemoveItemChangeCB(
	EOnItemChangeNotityObjType OnItemChangeNotityObjType
)
{
	if (OnItemChangeNotifyCBMap.Contains(OnItemChangeNotityObjType))
	{
		OnItemChangeNotifyCBMap.Remove(OnItemChangeNotityObjType);
	}
}

void FHoldItemsData::ClearItemChangeCB()
{
	OnItemChangeNotifyCBMap.Empty();
}

void FHoldItemsData::InitCreateMap(const FCreatingMap& NewCreateMap)
{
	CreateMap = NewCreateMap;
}

void FHoldItemsData::OnChange(const FItemAry& ChangeItemMap, EItemChangeType ItemChangeType)
{
	for (auto Iter : OnItemChangeNotifyCBMap)
	{
		if (Iter.Value)
		{
			Iter.Value(*this, ItemChangeType);
		}
	}
}

void FHoldItemsData::InitCanBeCreatedSet(const FCanbeCreatinedSet& NewCanBeCreatedSet)
{
	CanCreateSet = NewCanBeCreatedSet;
}

void FHoldItemsData::InitItemMap(const FItemAry& NewItemMap)
{
	AddItem(NewItemMap);
}

UHoldItemComponent::UHoldItemComponent(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
	SPHoldItemPropertyPtr = MakeShared<FHoldItemsData>();
}

void UHoldItemComponent::BeginPlay()
{
	Super::BeginPlay();
}

FName UHoldItemComponent::ComponentName = TEXT("UHoldItemComponent");
