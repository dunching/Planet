
#include "CharacterAttibutes.h"

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
		CallbackContainerHelper.ValueChanged(CurrentValue, val);

		CurrentValue = val;
	}
}

void FBaseProperty::AddCurrentValue(int32 val)
{
	SetCurrentValue(GetCurrentValue() + val);
}

void FBasePropertySet::AddCurrentValue(int32 val)
{
	const auto NewVal = CurrentValue.GetCurrentValue() + val;

	CurrentValue.SetCurrentValue(FMath::Clamp(NewVal, MinValue.GetCurrentValue(), MaxValue.GetCurrentValue()));
}

int32 FBasePropertySet::GetCurrentValue() const
{
	return CurrentValue.GetCurrentValue();
}

FBaseProperty& FBasePropertySet::GetCurrentProperty()
{
	return CurrentValue;
}

int32 FBasePropertySet::GetMaxValue() const
{
	return MaxValue.GetCurrentValue();
}

FBaseProperty& FBasePropertySet::GetMaxProperty()
{
	return MaxValue;
}

FCharacterAttributes::~FCharacterAttributes()
{

}

void FCharacterAttributes::ProcessGAEVent(const FGameplayAbilityTargetData_GAEvent& GAEvent)
{
	HP.AddCurrentValue(GAEvent.Data.TreatmentVolume - GAEvent.Data.ADDamage);
}

FScopeCharacterAttributes::FScopeCharacterAttributes(FCharacterAttributes& CharacterAttributes)
{

}
