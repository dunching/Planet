
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

void FBaseProperty::SetCurrentValue(int32 NewValue)
{
	if (CurrentValue != NewValue)
	{
		const auto OldValue = CurrentValue;
		CurrentValue = NewValue;
		if (bIsSaveUpdate)
		{
		}
		else
		{
			CallbackContainerHelper.ValueChanged(OldValue, CurrentValue);
		}
	}
}

void FBaseProperty::AddCurrentValue(int32 val)
{
	SetCurrentValue(GetCurrentValue() + val);
}
