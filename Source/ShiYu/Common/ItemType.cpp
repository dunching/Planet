
#include "ItemType.h"

#include <type_traits>

bool CheckItemNumIsValid(const FItemNum& NewItemBase)
{
	if (NewItemBase.Num > 0)
	{
		switch (NewItemBase.ItemType.Enumtype)
		{
		case EEnumtype::kEquipment:
		{
			if (std::get<EEquipmentType>(NewItemBase.ItemType.ItemType) != EEquipmentType::kNone)
			{
				return true;
			}
		}
		case EEnumtype::kBuilding:
		{
			if (std::get<EBuildingType>(NewItemBase.ItemType.ItemType) != EBuildingType::kNone)
			{
				return true;
			}
		}
		case EEnumtype::kRawMaterialType:
		{
			if (std::get<ERawMaterialType>(NewItemBase.ItemType.ItemType) != ERawMaterialType::kNone)
			{
				return true;
			}
		}
		}
	}

	return false;
}

FItemType::FItemType():
	ID(FMath::RandHelper(MAX_int32))
{
	ItemType = EEquipmentType::kNone;
}
