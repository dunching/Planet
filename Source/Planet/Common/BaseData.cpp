
#include "BaseData.h"

FBaseProperty::FBaseProperty()
{
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
