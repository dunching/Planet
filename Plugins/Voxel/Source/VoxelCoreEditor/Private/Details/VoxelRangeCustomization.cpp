// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelEditorMinimal.h"

template<typename NumericType>
class TVoxelRangeCustomization : public IPropertyTypeCustomization
{
public:
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override
	{
		PrepareSettings(StructPropertyHandle);

		HeaderRow.NameContent()
		[
			StructPropertyHandle->CreatePropertyNameWidget()
		]
		.ValueContent()
		.MinDesiredWidth(251.0f)
		.MaxDesiredWidth(251.0f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			.Padding(FMargin(0.f, 0.f, 3.f, 0.f))
			.AutoWidth()
			[
				SNew(STextBlock)
				.Font(IDetailLayoutBuilder::GetDetailFont())
				.Text(INVTEXT("Min"))
			]
			+ SHorizontalBox::Slot()
			.Padding(FMargin(0.f, 0.f, 3.f, 0.f))
			.VAlign(VAlign_Center)
			[
				SNew(SNumericEntryBox<NumericType>)
				.Value(this, &TVoxelRangeCustomization<NumericType>::OnGetValue, true)
				.MinValue(MinAllowedValue)
				.MinSliderValue(MinAllowedSliderValue)
				.MaxValue_Lambda([this]
				{
					if (bClampToMinMaxLimits)
					{
						return OnGetValue(false);
					}

					return MaxAllowedValue;
				})
				.MaxSliderValue_Lambda([this]
				{
					if (bClampToMinMaxLimits)
					{
						return OnGetValue(false);
					}

					return MaxAllowedSliderValue;
				})
				.OnValueCommitted(this, &TVoxelRangeCustomization<NumericType>::OnValueCommitted, true)
				.OnValueChanged(this, &TVoxelRangeCustomization<NumericType>::OnValueChanged, true)
				.OnBeginSliderMovement(this, &TVoxelRangeCustomization<NumericType>::OnBeginSliderMovement)
				.OnEndSliderMovement(this, &TVoxelRangeCustomization<NumericType>::OnEndSliderMovement)
				.UndeterminedString(INVTEXT("Multiple Values"))
				.Font(IDetailLayoutBuilder::GetDetailFont())
				.AllowSpin(true)
				.IsEnabled_Lambda([this]
				{
					return MinValueHandle ? !MinValueHandle->IsEditConst() : false;
				})
				.TypeInterface(TypeInterface)
			]
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			.Padding(2.f, 0.f)
			.AutoWidth()
			[
				SNew(STextBlock)
				.Font(IDetailLayoutBuilder::GetDetailFont())
				.Text(INVTEXT("Max"))
			]
			+ SHorizontalBox::Slot()
			.Padding(FMargin(0.f, 0.f, 3.f, 0.f))
			.VAlign(VAlign_Center)
			[
				SNew(SNumericEntryBox<NumericType>)
				.Value(this, &TVoxelRangeCustomization<NumericType>::OnGetValue, false)
				.MinValue_Lambda([this]
				{
					if (bClampToMinMaxLimits)
					{
						return OnGetValue(true);
					}

					return MinAllowedValue;
				})
				.MinSliderValue_Lambda([this]
				{
					if (bClampToMinMaxLimits)
					{
						return OnGetValue(true);
					}

					return MinAllowedSliderValue;
				})
				.MaxValue(MaxAllowedValue)
				.MaxSliderValue(MaxAllowedSliderValue)
				.OnValueCommitted(this, &TVoxelRangeCustomization<NumericType>::OnValueCommitted, false)
				.OnValueChanged(this, &TVoxelRangeCustomization<NumericType>::OnValueChanged, false)
				.OnBeginSliderMovement(this, &TVoxelRangeCustomization<NumericType>::OnBeginSliderMovement)
				.OnEndSliderMovement(this, &TVoxelRangeCustomization<NumericType>::OnEndSliderMovement)
				.UndeterminedString(INVTEXT("Multiple Values"))
				.Font(IDetailLayoutBuilder::GetDetailFont())
				.AllowSpin(true)
				.IsEnabled_Lambda([this]
				{
					return MaxValueHandle ? !MaxValueHandle->IsEditConst() : false;
				})
				.TypeInterface(TypeInterface)
			]
		];
	}

	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override
	{

	}

private:
	void OnValueCommitted(NumericType NewValue, ETextCommit::Type CommitType, const bool bMin)
	{
		SetValue(NewValue, bMin, EPropertyValueSetFlags::DefaultFlags);
	}

	void OnValueChanged(NumericType NewValue, const bool bMin)
	{
		SetValue(NewValue, bMin, EPropertyValueSetFlags::InteractiveChange);
	}

	void OnBeginSliderMovement()
	{
		GEditor->BeginTransaction(INVTEXT("Set Range Property"));
	}

	void OnEndSliderMovement(NumericType)
	{
		GEditor->EndTransaction();
	}

	void SetValue(const NumericType NewValue, const bool bMin, const EPropertyValueSetFlags::Type Flags)
	{
		const TSharedPtr<IPropertyHandle> Handle = bMin ? MinValueHandle : MaxValueHandle;
		const TSharedPtr<IPropertyHandle> OtherHandle = bMin ? MaxValueHandle : MinValueHandle;

		const TOptional<NumericType> OtherValue = OnGetValue(!bMin);
		bool bOutOfRange = false;
		if (OtherValue.IsSet())
		{
			if (bMin &&
				NewValue > OtherValue.GetValue())
			{
				bOutOfRange = true;
			}
			else if (
				!bMin &&
				NewValue < OtherValue.GetValue())
			{
				bOutOfRange = true;
			}
		}

		if (!bOutOfRange ||
			bAllowInvertedRange)
		{
			if (Flags == EPropertyValueSetFlags::InteractiveChange)
			{
				SetHandleValue(Handle, NewValue, Flags);

				if (OtherValue.IsSet())
				{
					SetHandleValue(OtherHandle, OtherValue.GetValue(), Flags);
				}
			}
			else
			{
				if (OtherValue.IsSet())
				{
					SetHandleValue(OtherHandle, OtherValue.GetValue(), Flags);
				}

				SetHandleValue(Handle, NewValue, Flags);
			}
		}
		else if (!bClampToMinMaxLimits)
		{
			SetHandleValue(OtherHandle, NewValue, Flags);
			SetHandleValue(Handle, NewValue, Flags);
		}
	}

	void SetHandleValue(const TSharedPtr<IPropertyHandle>& Handle, const NumericType NewValue, const EPropertyValueSetFlags::Type Flags)
	{
		const int32 NumObjects = Handle->GetNumPerObjectValues();
		const FString ValueString = LexToString(NewValue);
		for (int32 Index = 0; Index < NumObjects; Index++)
		{
			ensure(Handle->SetPerObjectValue(Index, ValueString, Flags) == FPropertyAccess::Success);
		}
	}

	void PrepareSettings(const TSharedRef<IPropertyHandle>& StructPropertyHandle)
	{
		MinValueHandle = StructPropertyHandle->GetChildHandle(TEXT("Min"));
		MaxValueHandle = StructPropertyHandle->GetChildHandle(TEXT("Max"));
		check(MinValueHandle.IsValid());
		check(MaxValueHandle.IsValid());

		const FProperty* Property = StructPropertyHandle->GetProperty();
		check(Property);

		const FString& MetaUIMinString = Property->GetMetaData(TEXT("UIMin"));
		const FString& MetaUIMaxString = Property->GetMetaData(TEXT("UIMax"));
		const FString& MetaClampMinString = Property->GetMetaData(TEXT("ClampMin"));
		const FString& MetaClampMaxString = Property->GetMetaData(TEXT("ClampMax"));
		const FString& UIMinString = MetaUIMinString.Len() ? MetaUIMinString : MetaClampMinString;
		const FString& UIMaxString = MetaUIMaxString.Len() ? MetaUIMaxString : MetaClampMaxString;
		const FString& MetaUnits = Property->GetMetaData(TEXT("Units"));

		NumericType ClampMin = std::numeric_limits<NumericType>::lowest();
		NumericType ClampMax = std::numeric_limits<NumericType>::max();
		TTypeFromString<NumericType>::FromString(ClampMin, *MetaClampMinString);
		TTypeFromString<NumericType>::FromString(ClampMax, *MetaClampMaxString);

		NumericType UIMin = std::numeric_limits<NumericType>::lowest();
		NumericType UIMax = std::numeric_limits<NumericType>::max();
		TTypeFromString<NumericType>::FromString(UIMin, *UIMinString);
		TTypeFromString<NumericType>::FromString(UIMax, *UIMaxString);

		const NumericType ActualUIMin = FMath::Max(UIMin, ClampMin);
		const NumericType ActualUIMax = FMath::Min(UIMax, ClampMax);

		MinAllowedValue = MetaClampMinString.Len() ? ClampMin : TOptional<NumericType>();
		MaxAllowedValue = MetaClampMaxString.Len() ? ClampMax : TOptional<NumericType>();
		MinAllowedSliderValue = (UIMinString.Len()) ? ActualUIMin : TOptional<NumericType>();
		MaxAllowedSliderValue = (UIMaxString.Len()) ? ActualUIMax : TOptional<NumericType>();

		bAllowInvertedRange = Property->HasMetaData(TEXT("AllowInvertedRange"));
		bClampToMinMaxLimits = Property->HasMetaData(TEXT("ClampToMinMaxLimits"));

		TOptional<EUnit> PropertyUnits = EUnit::Unspecified;
		if (!MetaUnits.IsEmpty())
		{
			PropertyUnits = FUnitConversion::UnitFromString(*MetaUnits);
		}

		TypeInterface = MakeVoxelShared<TNumericUnitTypeInterface<NumericType>>(PropertyUnits.GetValue());
	}

	TOptional<NumericType> OnGetValue(const bool bMin) const
	{
		const TSharedPtr<IPropertyHandle> Handle = (bMin ? MinValueHandle : MaxValueHandle);
		const int32 NumObjects = Handle->GetNumPerObjectValues();

		FString ValueStr;
		Handle->GetPerObjectValue(0, ValueStr);

		NumericType Result;
		LexFromString(Result, *ValueStr);

		for (int32 Index = 1; Index < NumObjects; Index++)
		{
			ValueStr.Reset();
			Handle->GetPerObjectValue(Index, ValueStr);

			NumericType Value;
			LexFromString(Value, *ValueStr);

			if (Result != Value)
			{
				return {};
			}
		}

		return Result;
	}

private:
	TSharedPtr<TNumericUnitTypeInterface<NumericType>> TypeInterface;

	TSharedPtr<IPropertyHandle> MinValueHandle;
	TSharedPtr<IPropertyHandle> MaxValueHandle;

	TOptional<NumericType> MinAllowedValue;
	TOptional<NumericType> MaxAllowedValue;
	TOptional<NumericType> MinAllowedSliderValue;
	TOptional<NumericType> MaxAllowedSliderValue;

	bool bAllowInvertedRange = false;
	bool bClampToMinMaxLimits = false;
};

using FVoxelRangeCustomizationInt32 = TVoxelRangeCustomization<int32>;
using FVoxelRangeCustomizationFloat = TVoxelRangeCustomization<float>;

DEFINE_VOXEL_STRUCT_LAYOUT(FVoxelInt32Range, FVoxelRangeCustomizationInt32);
DEFINE_VOXEL_STRUCT_LAYOUT(FVoxelFloatRange, FVoxelRangeCustomizationFloat);