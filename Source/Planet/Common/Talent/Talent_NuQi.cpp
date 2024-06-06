
#include "Talent_NuQi.h"

int32 FTalent_NuQi::GetCurrentValue() const
{
	return CurrentValue;
}

int32 FTalent_NuQi::GetMaxValue() const
{
	return MaxValue;
}

void FTalent_NuQi::SetCurrentValue(int32 NewVal)
{
	if (CurrentValue != NewVal)
	{
		NewVal = FMath::Clamp(CurrentValue, 0, MaxValue);

		CallbackContainerHelper.ValueChanged(CurrentValue, NewVal);

		CurrentValue = NewVal;
	}
}

void FTalent_NuQi::AddCurrentValue(int32 Value)
{
	SetCurrentValue(GetCurrentValue() + Value);
}

