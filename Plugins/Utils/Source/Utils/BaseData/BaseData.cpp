
#include "BaseData.h"

FBaseProperty::FBaseProperty()
{
}

FBaseProperty::FBaseProperty(int32 Value)
{
	SetCurrentValue(Value);
}

bool FBaseProperty::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	Ar << CurrentValue;

	return true;
}

int32 FBaseProperty::GetCurrentValue() const
{
	return CurrentValue;
}

void FBaseProperty::SetCurrentValue(int32 val)
{
	if (CurrentValue != val)
	{
		if (bIsSaveUpdate)
		{
		}
		else
		{
			CallbackContainerHelper.ValueChanged(CurrentValue, val);
		}

		CurrentValue = val;
	}
}

void FBaseProperty::AddCurrentValue(int32 val)
{
	SetCurrentValue(GetCurrentValue() + val);
}
